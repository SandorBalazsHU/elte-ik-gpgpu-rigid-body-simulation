// Use cl::vector instead of STL version
#define __NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

#include "Simulation.h"

//Initialize OpenCL API
bool Simulation::InitCL() {
	try {
		//Initialize OpenCL API
		cl::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		// Try to get the sharing platform!
		bool create_context_success = false;
		for (auto platform : platforms) {
			// Next, create an OpenCL context on the platform.  Attempt to create a GPU-based context.
			cl_context_properties contextProperties[] = {
				#ifdef _WIN32
					CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(),
					CL_GL_CONTEXT_KHR,   (cl_context_properties)wglGetCurrentContext(),
					CL_WGL_HDC_KHR,      (cl_context_properties)wglGetCurrentDC(),
				#elif defined( __GNUC__)
					CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(),
					CL_GL_CONTEXT_KHR,   (cl_context_properties)glXGetCurrentContext(),
					CL_GLX_DISPLAY_KHR,  (cl_context_properties)glXGetCurrentDisplay(),
				#endif
				0
			};

			// Create Context
			try {
				context = cl::Context(CL_DEVICE_TYPE_GPU, contextProperties);
				create_context_success = true;
				break;
			} catch (cl::Error error) {}
		}

		if (!create_context_success) throw cl::Error(CL_INVALID_CONTEXT, "Failed to create CL/GL shared context");

		// Create Command Queue
		cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		std::cout << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
		commandQueue = cl::CommandQueue(context, devices[0]);

		// Read source file, load the kernel
		std::ifstream sourceFile("Simulation.cl");
		std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
		cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));

		// Make program of the source code in the context
		program = cl::Program(context, source);
		try {
			program.build(devices);
		}
		catch (cl::Error error) {
			std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
			throw error;
		}

		// Make kernel
		kernel = cl::Kernel(program, "update");

		CLvelocities = cl::Buffer(context, CL_MEM_READ_WRITE, numberOfBallsArray * sizeof(glm::vec3));
		CLpositions = cl::Buffer(context, CL_MEM_READ_WRITE, numberOfBallsArray * sizeof(glm::vec3));
		CLcollisionCheck = cl::Buffer(context, CL_MEM_READ_WRITE, numberOfBallsArray * sizeof(bool));
	}
	catch (cl::Error error)	{
		std::cerr << error.what() << std::endl;
		return false;
	}
	return true;
}

void Simulation::CalculateCL() {
	// Run Kernel
	try {
		//Write the current vectors to GPU
		commandQueue.enqueueWriteBuffer(CLpositions, CL_TRUE, 0, numberOfBalls * sizeof(glm::vec3), positions);
		commandQueue.enqueueWriteBuffer(CLvelocities, CL_TRUE, 0, numberOfBalls * sizeof(glm::vec3), velocities);
		commandQueue.enqueueWriteBuffer(CLcollisionCheck, CL_TRUE, 0, numberOfBalls * sizeof(bool), collisionCheck);

		kernel.setArg(0, CLpositions);
		kernel.setArg(1, CLvelocities);
		kernel.setArg(2, CLcollisionCheck);
		//Set the current variables
		kernel.setArg(3, numberOfBalls);
		kernel.setArg(4, boxSize);

		//RUN THE KERNEL
		cl::NDRange global(numberOfBalls);

		// interaction & integration
		commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);

		// Wait for all computations to finish
		commandQueue.finish();
	}
	catch (cl::Error error) {
		std::cerr << error.what() << std::endl;
		exit(1);
	}
}
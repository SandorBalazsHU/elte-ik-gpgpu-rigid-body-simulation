/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
*/

#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "imgui\imgui.h"
#include "imgui\imgui_impl_sdl_gl3.h"

#include "Simulation.h"

void exitProgram() {
	SDL_Quit();
	std::cout << currentDateTime() << "- Program terminated correctly" << std::endl;
}

SDL_atomic_t frames;

Uint32 fps_timer_callback(Uint32 interval, void* data)
{
	const float f = SDL_AtomicGet(&frames);
	const float iv = interval * 0.001f;

	/* Note: the thread safety of printf is ambiguous across platforms */
	printf("%.2f\tfps\n", f / iv);

	/* Reset frame counter */
	SDL_AtomicSet(&frames, 0);

	return interval;
}

int main(int argc, char* args[])
{
	//Logging to files.
	//freopen("simulation.log", "w", stdout);
	//freopen("error.log", "w", stderr);

	//Exit Callback
	atexit(exitProgram);

	//SDL Init
	if (SDL_Init(SDL_INIT_VIDEO) == -1)	{
		std::cerr << currentDateTime() << "- [SDL_Init ERROR]: " << SDL_GetError() << std::endl;
		return 1;
	}

	//OpenGL Configuration
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);		//Color buffer size
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);			//Red 8 bit
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);			//Green 8 bit
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);			//Blue 8 bit
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);			//Alpha 8 bit
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);		//Depth Buffer on
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);			//Depth Buffer 24
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);  //Anti aliasing 4

	//Creating Window
	SDL_Window* window = 0;
	window = SDL_CreateWindow("Rigid Body Simulation", 100, 100, 640, 480,
	SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	if (window == 0) {
		std::cerr << currentDateTime() << "- [WINDOW CREATION ERROR]: " << SDL_GetError() << std::endl;
		return 1;
	}

	//imgui initialisation
	ImGui_ImplSdlGL3_Init(window);

	//Create OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == 0) {
		std::cerr << currentDateTime() << "- [OpenGL context creation ERROR]: " << SDL_GetError() << std::endl;
		return 1;
	}

	//VSINC ON
	SDL_GL_SetSwapInterval(0);

	//GLEW start
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		std::cerr << currentDateTime() << "- [GLEW start error]" << std::endl;
		return 1;
	}

	//Check OpenGL version
	int glVersion[2] = { -1, -1 };
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
	std::cout << currentDateTime() <<  "- Running OpenGL " << glVersion[0] << "." << glVersion[1] << std::endl;

	if (glVersion[0] == -1 && glVersion[1] == -1) {
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		std::cerr << currentDateTime() << "- [OGL context error]" << std::endl;
		return 1;
	}

	//Base variables for the rander loop
	bool quit = false;
	int fpsPlotCounter = 0;

	Simulation simulation;

	//OpenCL Initialisation
	if (!simulation.InitCL()) {
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		std::cerr << currentDateTime() << "- [OpenCL INIT ERROR]" << std::endl;
		return 1;
	}

	//Simulation Initialisation
	if (!simulation.Init()) {
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		std::cerr << currentDateTime() << "- [Simulation ERROR]" << std::endl;
		return 1;
	}

	SDL_AddTimer(1000, fps_timer_callback, NULL);
	//https://wiki.libsdl.org/SDL_atomic_t

	const float SCREEN_FPS = 55.0f;
	const float SCREEN_TICKS_PER_FRAME = 1000.0f / SCREEN_FPS;
	//https://lazyfoo.net/tutorials/SDL/25_capping_frame_rate/index.php

	//The render loop
	while (!quit)
	{
		//The FPS Counter
		fpsPlotCounter++;
		if (fpsPlotCounter == 100) fpsPlotCounter = 0;
		const Uint32 time = SDL_GetTicks();
		SDL_Event event;
		//Event processing loop
		while (SDL_PollEvent(&event))
		{
			//ImGUI event handling
			ImGui_ImplSdlGL3_ProcessEvent(&event);
			bool is_mouse_captured = ImGui::GetIO().WantCaptureMouse;
			bool is_keyboard_captured = ImGui::GetIO().WantCaptureKeyboard;

			switch (event.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
					if (!is_keyboard_captured) simulation.KeyboardDown(event.key);
					break;
				case SDL_KEYUP:
					if (!is_keyboard_captured) simulation.KeyboardUp(event.key);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (!is_mouse_captured) simulation.MouseDown(event.button);
					break;
				case SDL_MOUSEBUTTONUP:
					if (!is_mouse_captured) simulation.MouseUp(event.button);
					break;
				case SDL_MOUSEWHEEL:
					if (!is_mouse_captured) simulation.MouseWheel(event.wheel);
					break;
				case SDL_MOUSEMOTION:
					if (!is_mouse_captured) simulation.MouseMove(event.motion);
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						simulation.Resize(event.window.data1, event.window.data2);
					}
					break;
			}

		}

		//Rendering
		ImGui_ImplSdlGL3_NewFrame(window);
		simulation.Update();
		simulation.Render();
		ImGui::Render();
		SDL_GL_SwapWindow(window);

		//FPS Counter
		const Uint32 last_time = SDL_GetTicks();

		SDL_AtomicAdd(&frames, 1);

		float frameTicks = last_time - time;
		if (frameTicks < SCREEN_TICKS_PER_FRAME)
		{
			//Wait remaining time
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
		}

		//FPS Counter
		const Uint32 last_time2 = SDL_GetTicks();
		std::stringstream window_title;
		float fps = 1000.0f / (last_time2 - time);
		window_title << "Rigid Body Simulation. FPS: " << fps;
		simulation.fps[fpsPlotCounter] = fps;
		SDL_SetWindowTitle(window, window_title.str().c_str());
	}

	//Cleanup
	simulation.Clean();
	ImGui_ImplSdlGL3_Shutdown();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	return 0;
}
# GPU accelerated rigid body simulation with OpenGL and OpenCL.

[![Rigid Body Simulation](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/Img01.jpg)](https://www.youtube.com/watch?v=ky2dFqLJdGQ)
(Click the image for the video)
(Video URL: https://youtu.be/ky2dFqLJdGQ)

## The main features:
* Real time simulation with OpenGL (SDL2) wiev.
* Real time Ball to Wall, Ball to Box, Ball to Ball collision and refelction.
* GPU Acceleration with OpenCL.
* Live switch between the CPU and GPU implementation.
* GUI and live parametering
* Live camera interaction (Sphere camera rotation)
* Live mouse interaction.

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/Img05.jpg)

## The envirement:
* The code is platform free and it use free multiplatform libs, but i developed and tested in Winddows.
* This is a Visual studio project (Made with Visual Studio 2019).
* The project need SDL2 OpemCL GLEW IMGUI for compiling.
* This zip file contains the nececcery dependecies. You can mount the dependencies in a T drive with the script in the zip file.
* (https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/dependencies.zip)
* The Visual studio project configured for this dependecy pack and the T virtual Drive.

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/Img02.jpg)

## The GUI:
* You can rotate the camera with the mouse, when you hold the left mouse button.
* You can move the small box with the mouse, when you hold the right mouse button.
* You can zoom in and out with the mouse wheel.
* The main GUI in the left top corner. (The window is replaceable and resizable.)
* Here you can see the FPS meter.
* ---- All setting is live ----
* Then you can set the gravity vector.
* Then you can set the air resistance.
* Then you can set the simulation box size.
* Then you can set the balls number. [1-1500]
* Then you can apply random XZ and Y ball start position. (Works after reset)
* <b>The most importent is the RUN switch. Here you can START/STOP the simulation.</b>
* Then you can switch on/off the ball to ball collision.
* Then you can reinitialise the simulation. (RESET)
* Then you can switch on/off the small box.

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/Img03.jpg)

## The virtual folders:
* I made virual folders in Visual Studio for the better transparency.
* I used CamelCased names and English comments.

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/Img07.jpg)

## The Theoretical background:
I stored the balls positions in float3 and their velocities in an another float3 vector.
The new position is the position vector and the velocity vectors sum.
I modelld the gravity vith an another float3 vector and i subtract the velocitiy vector with the gravity vector.
I add an air resistance vector when slow down.

```
    glm::vec3 positions[numberOfBalls];
    glm::vec3 velocities[numberOfBalls];
    float resistance;

    velocities[i] = velocities[i] - gravity;
    velocities[i] = velocities[i] * resistance;
    positions[i]  = positions[i] + velocities[i];
    if (ballCollisionRun) ballCollision(i);
    if(barrierIsOn) barrierCollision(i);
    wallCollision(i);
```
#### The processing loop
![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/loop.jpg)

### The Ball to Wall collision:
![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/mat3.jpg)

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/mat4.jpg)

## The ball to Ball Collision (The first way, i used this for GPU):

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/collision.gif)

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/mat1.jpg)

## The ball to Ball Collision (The second way, i used this for CPU):

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/mat2.jpg)


### Used sources:
* Ball to wall reflection: https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
* Ball to wall collision (Way 1): http://exploratoria.github.io/exhibits/mechanics/elastic-collisions-in-3d/
* Ball to wall collision (Way 2): https://en.wikipedia.org/wiki/Elastic_collision
* Phisical simulations: https://www.khanacademy.org/science/physics/linear-momentum/elastic-and-inelastic-collisions/a/what-are-elastic-and-inelastic-collisions

## Issues
* When you set 1500 balls the GPU ball to ball collision sometimes becomes unstable (In this case you can switch to CPU and Back.)
* With 1500 balls the average FPS is 20-30.

![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/Img04.jpg)
![](https://github.com/SandorBalazsHU/elte-ik-gpgpu-rigid-body-simulation/blob/master/Images/Img06.jpg)

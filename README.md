# Basic OpenGL with GLFW: Instanced Rendering

Calling draw separately for each wastes a ton of time.
Using a similar strategy to the spritebatching example, we can buffer our world matrices, and use them to render hundreds of objects in a single draw call.
This example uses a skybox from the skybox tutorial, but it isn't required for the instancing part.

# Setup

You will need to have CMake installed on your computer, and properly added to your path.
In order to setup, run the following in a shell, then open the project in your preferred editor.
Windows setup has been configured for use with Visual Studio.

Windows:
```
cd path/to/folder
setup.cmd
```
Linux:
```
cd path/to/folder
./setup
```

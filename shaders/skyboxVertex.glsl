/*
Title: Instanced Rendering
File Name: skyboxVertex.glsl
Copyright ? 2016
Author: David Erbelding
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#version 400 core

// Vertex attribute for position
layout(location = 0) in vec3 in_position;

// Uniform for camera view
uniform mat4 cameraView;

// We send the position out to the fragment shader to help read from the texture.
out vec3 position;

void main(void)
{
	// output the transformed vector
	vec4 p = cameraView * vec4(in_position, 1);

	// Instead of outputting p, we will swizzle the output to send x y w w.
	// The gpu will end up dividing z by w to get depth between 0 and 1.
	// By making z = w, we make the result of that division 1, the maximum possible depth.
	// This makes it so that the skybox renders only where there is absolutely nothing else.
	gl_Position = p.xyww;
	
	// Output the position to the fragment shader.
	position = in_position;
}
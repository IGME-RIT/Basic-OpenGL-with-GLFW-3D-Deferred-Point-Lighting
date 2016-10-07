/*
Title: Instanced Rendering
File Name: pointLightVert.glsl
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

struct pointLight
{
	vec3 position;
	float radius;
	vec4 attenuation;
	vec4 color;
};

// Vertex attribute for position
layout(location = 0) in vec3 in_vertex;

uniform mat4 cameraView;

uniform pointLight in_light;

out pointLight light;
out vec3 worldPosition;

void main(void)
{
	// Pass the light data forward to the fragment step
	light.position = vec3(cameraView * vec4(in_light.position, 1));
	light.radius = in_light.radius;
	light.attenuation = in_light.attenuation;
	light.color = in_light.color;

	// Send The world position in screen space
	gl_Position = cameraView * vec4(in_light.position + (in_vertex * in_light.radius), 1);

	// Get the position of the vertex in screen space.
	worldPosition = vec3(gl_Position);
}
/*
Title: Instanced Rendering
File Name: vertex.glsl
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
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;

// This is really the only change here, we put the world Matrix at location 4, and say it's a mat4
// In reality, it's taking up locations 5, 6, and 7 as well, because each location is 4 floats.
layout(location = 4) in mat4 in_worldMat;


uniform mat4 cameraView;

out vec3 position;
out vec2 uv;
out mat3 tbn;

void main(void)
{

	// transform the vector
	// also pass the world position of the surface forward to the fragment shader
	vec4 worldPosition = (in_worldMat) * vec4(in_position, 1);
	position = vec3(worldPosition);
	vec4 viewPosition = cameraView * worldPosition;

	// output the transformed vector
	gl_Position = viewPosition;

	// We have a little extra work here.
	// Not only do we have to multiply the normal by the world matrix, we also have to multiply the tangent
	vec3 normal = mat3(in_worldMat) * in_normal;
	vec3 tangent = mat3(in_worldMat) * in_tangent;

	// The third vector we need is a bitangent, or a vector perpendicular to both the normal and tangent.
	// This can be easily accomplished with a cross product.
	vec3 bitangent = normalize(cross(tangent, normal));

	// Finally, we combine them into a matrix.
	// Conveniently, a matrix constructed this way will rotate any multiplied vectors from texture space to world space.
	tbn = mat3(tangent, bitangent, normal);

	uv = in_uv;
}
/*
Title: Deferred Point Lighting
File Name: diffuseNormalFrag.glsl
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

in vec3 position;
in vec2 uv;
in mat3 tbn;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 normal;

void main(void)
{	
	// calculate normal from normal map
	// change range of pixel from [0 to 1] to [-1 to 1]
	vec3 texnorm = normalize(vec3(texture(normalMap, uv)) * 2.0 - 1.0);
	vec3 norm = tbn * texnorm;

	
	// finally, sample from the texuture and apply the light.
	color = texture(diffuseMap, uv);
	
	// change range of pixel from [-1 to 1] back to [0 to 1]
	// so that it can be drawn in RGB to our texture buffer
	normal = vec4(norm * .5 + .5, 1);
}
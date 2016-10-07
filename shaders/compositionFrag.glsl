/*
Title: 2D Normal Mapping
File Name: fragment.glsl
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

uniform sampler2D texColor;
uniform sampler2D texLight;
uniform sampler2D texNormal;
uniform sampler2D texDepth;

void main(void)
{
	vec3 sunDir = vec3(1, 0, 0);
	vec4 sunColor = vec4(1);
	vec4 ambient = vec4(.1, .1, .3, 1);

	// Multiply color and light to get our final value!
	vec4 color = texelFetch(texColor, ivec2(gl_FragCoord), 0);
	vec3 normal = vec3(texelFetch(texNormal, ivec2(gl_FragCoord), 0) * 2 - 1);
	float depth = texelFetch(texDepth, ivec2(gl_FragCoord), 0).x;
	vec4 light = texelFetch(texLight, ivec2(gl_FragCoord), 0);

	if(depth < .99999)
	{
		float ndotl = clamp(dot(sunDir, normalize(normal)), 0, 1);
		color *= clamp(light + (sunColor * ndotl) + ambient, 0, 1);
	}

	gl_FragColor = color;
}
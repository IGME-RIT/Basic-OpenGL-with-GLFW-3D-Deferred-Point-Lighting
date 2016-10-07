/*
Title: 2D Normal Mapping
File Name: pointLightFrag.glsl
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

in pointLight light;
in vec3 worldPosition;

uniform sampler2D texNormal;
uniform sampler2D texDepth;
uniform mat4 viewRotation;

void main(void)
{
	// Read normal and depth values from the geometry buffer.
	vec4 normal4 = texelFetch(texNormal, ivec2(gl_FragCoord), 0) * 2 - 1;
	vec3 normal = vec3(viewRotation * normal4);


	float depth = texelFetch(texDepth, ivec2(gl_FragCoord), 0).x;

	vec3 viewRay = vec3(worldPosition.xy / worldPosition.z, 1);

	// Calculate our projection constants (you should of course do this in the app code, I'm just showing how to do it)
	float ProjectionA = 100 / (100 - .1);
	float ProjectionB = (-100 * .1) / (100 - .1);

	// Sample the depth and convert to linear view space Z (assume it gets sampled as
	// a floating point value of the range [0,1])
	float linearDepth = ProjectionB / (depth - ProjectionA);

	// The position of the pixel in view space
	vec3 positionVS = viewRay * linearDepth;


	vec3 surfaceToLight = light.position - positionVS;

	float ndotl = clamp(dot(normalize(surfaceToLight), normalize(normal)), 0, 1);
	
	float d = clamp(length(surfaceToLight) / light.radius, 0, 1);
	float attenuation = (1 / (light.attenuation.x * d * d + light.attenuation.y * d + light.attenuation.z)) - light.attenuation.w;

	gl_FragColor = light.color * ndotl * attenuation;
}
/*
Title: Deferred Point Lighting
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
in vec3 screenPosition;

uniform sampler2D texNormal;
uniform sampler2D texDepth;
uniform mat4 viewRotation;
uniform float projectionA;
uniform float projectionB;

void main(void)
{
	// First, we read normal and depth values from the geometry buffer.
	vec4 normal4 = texelFetch(texNormal, ivec2(gl_FragCoord), 0) * 2 - 1;
	float depth = texelFetch(texDepth, ivec2(gl_FragCoord), 0).x;
	
	// Rotate the normals into view space.
	vec3 normal = vec3(viewRotation * normal4);



	// Next we need to calculate the world position of our object using the depth buffer.
	// Here we get a view directional vector by dividing the screenposition by it's z value (the depth)
	vec3 viewRay = screenPosition / screenPosition.z;


	// Convert the depth to linear view space. The is the opposite operation that projection does.
	float linearDepth = projectionB / (depth - projectionA);


	// The position of the pixel in view space will be the view direction multiplied by the linearized depth.
	vec3 positionVS = viewRay * linearDepth;



	// Now that we have the position of the light and the surface, calculate lighting the same way as done previously.

	// Calculate light angle.
	vec3 surfaceToLight = light.position - positionVS;

	// Get diffuse value.
	float ndotl = clamp(dot(normalize(surfaceToLight), normalize(normal)), 0, 1);
	
	// Calclate distance and attenuation.
	float d = clamp(length(surfaceToLight) / light.radius, 0, 1);
	float attenuation = (1 / (light.attenuation.x * d * d + light.attenuation.y * d + light.attenuation.z)) - light.attenuation.w;

	// Write final color value.
	gl_FragColor = light.color * ndotl * attenuation;
}
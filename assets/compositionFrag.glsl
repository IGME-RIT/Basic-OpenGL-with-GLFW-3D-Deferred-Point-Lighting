/*
Title: Deferred Point Lighting
File Name: compositionFrag.glsl
Copyright ? 2016, 2019
Author: David Erbelding, Niko Procopi
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
	// get the size of the screenSize
	// size of colorScreen, lightScreen, and
	// the current image being rendered should be the same
	vec2 screenSize = textureSize(texColor, 0);
	
	// convert it to int
	ivec2 screenSizeInt = ivec2(screenSize);
	
	// if we are in the bottom 25% of the screen
	if
	  (
		(gl_FragCoord.x < screenSize.x * 0.25) && 
		(gl_FragCoord.y < screenSize.y * 0.25)
	  )
	{
		// get the uv coordinates of this screen-space
		ivec2 uv = ivec2(gl_FragCoord); 
		
		// the range of UV will be 0 - 0.25, so multiply
		// by 4 to get 0 - 1.0, to draw full image in the 
		// corner
		uv.x *= 4;
		uv.y *= 4;
		
		// get a color from the textured circles
		gl_FragColor = texelFetch(texDepth, ivec2(uv), 0);
	}
		
	// if we are in the left-hand side of the screen,
	// not in the bottom corner of the screen,
	// but between 0.25 and 0.50 of the height
	else if
	  (
		(gl_FragCoord.x < screenSize.x * 0.25) && 
		(gl_FragCoord.y < screenSize.y * 0.5)
	  )
	{
		// get the uv coordinates again
		ivec2 uv = ivec2(gl_FragCoord);

		// the uv.x will range 0 to 0.25
		// the uv.y will range 0.25 to 0.50
		
		// same as before
		uv.x *= 4;
		
		// subtract 1/4th the size, 
		// to get range from 0 to 0.25
		uv.y -= screenSizeInt.y / 4;
		
		// multiply by 4 to get 0 to 1.0
		uv.y *= 4;
	
		// get the color of the screen from the light screenshot
		gl_FragColor = texelFetch(texNormal, ivec2(uv), 0);
	}
	
	else if
	  (
		(gl_FragCoord.x < screenSize.x * 0.25) && 
		(gl_FragCoord.y < screenSize.y * 0.75)
	  )
	{
		// get the uv coordinates again
		ivec2 uv = ivec2(gl_FragCoord);

		// the uv.x will range 0 to 0.25
		// the uv.y will range 0.25 to 0.50
		
		// same as before
		uv.x *= 4;
		
		// subtract 1/4th the size, 
		// to get range from 0 to 0.25
		uv.y -= screenSizeInt.y / 2;
		
		// multiply by 4 to get 0 to 1.0
		uv.y *= 4;
	
		// get the color of the screen from the light screenshot
		gl_FragColor = texelFetch(texLight, ivec2(uv), 0);
	}
	
	else if
	  (
		(gl_FragCoord.x < screenSize.x * 0.25) && 
		(gl_FragCoord.y < screenSize.y * 1.0)
	  )
	{
		// get the uv coordinates again
		ivec2 uv = ivec2(gl_FragCoord);

		// the uv.x will range 0 to 0.25
		// the uv.y will range 0.25 to 0.50
		
		// same as before
		uv.x *= 4;
		
		// subtract 1/4th the size, 
		// to get range from 0 to 0.25
		uv.y -= 3 * screenSizeInt.y / 4;
		
		// multiply by 4 to get 0 to 1.0
		uv.y *= 4;
	
		// get the color of the screen from the light screenshot
		gl_FragColor = texelFetch(texColor, ivec2(uv), 0);
	}
	
	else
	{
		float depth = texelFetch(texDepth, ivec2(gl_FragCoord), 0).x;
		vec4 color = texelFetch(texColor, ivec2(gl_FragCoord), 0);

		// Don't apply shading to the skybox.
		if(depth < .99999)
		{
			vec3 sunDir = vec3(1, 0, 0);
			vec4 sunColor = vec4(1);
			vec4 ambient = vec4(.1, .1, .3, 1);

			// Multiply color and light to get our final value!
			vec3 normal = vec3(texelFetch(texNormal, ivec2(gl_FragCoord), 0) * 2 - 1);
			vec4 light = texelFetch(texLight, ivec2(gl_FragCoord), 0);
		
			float ndotl = clamp(dot(sunDir, normalize(normal)), 0, 1);
			color *= clamp(light + (sunColor * ndotl) + ambient, 0, 1);
		}

		gl_FragColor = color;
	}
}
Documentation Author: Niko Procopi 2019

This tutorial was designed for Visual Studio 2017 / 2019
If the solution does not compile, retarget the solution
to a different version of the Windows SDK. If you do not
have any version of the Windows SDK, it can be installed
from the Visual Studio Installer Tool

Welcome to the 3D Deferred Rendering Tutorial for Point Lights!
Prerequesites: 2D Deferred Rendering, 3D object loading, skybox, shadow mapping

3D Deferred Rendering is very similar to 2D Deferred Rendering,
we use geometry in the shape of a light to allow the rasterizer to
only calculate lighting on pixels that are within the radius of each light.
Also, we add a skybox, but everyone who has gotten this far in the 
tutorials should already know how to do that.

For 3D Deferred Rendering, we have the same necessary 3 shader programs:
1. Render scene with no lights, 
2. Render scene with only the pixels of the lights
3. Draw the final image, which combines the two, plus some debug images

We add one more shader program to draw a skybox, the code for this
is absolutely identical to the prerequisite skybox tutorial.

1. Rendering with no lights
Rendering the scene with no lights is almost the same as 2D:
In the Vertex Shader, rather than just multiplying position by the world
matrix, we multiply by cameraView (which is projection * view), as well, 
to handle the 3D camera. We also calculate the TBN matrix for normal mapping.
In the fragment shader, we texture the scene with color textures, and save 
the rendered scene to a texture. We need to render the scene with normal 
maps, but we do it a little differently this time. We unpack the normal
and multiply by TBN, just like usual:
	// change range of pixel from [0 to 1] to [-1 to 1]
	vec3 texnorm = normalize(vec3(texture(normalMap, uv)) * 2.0 - 1.0);
	vec3 norm = tbn * texnorm;
But after that, we need to render to a texture, we need the pixels to
be within the range of [0 to 1] to fit into RGB, so we re-compress the pixel:
	normal = vec4(norm * .5 + .5, 1);
	
We will also get the depth buffer from this render pass. We don't create
a depth buffer with a fragment shader, we get it from OpenGL. This is the 
same technique that we used to render the depth map in the prerequisite
shadow mapping tutorial. The ONLY thing this depth buffer will be used for,
is to determine if a pixel is part of the skybox or not 
(depth < 0.999 -> not skybox)
This will assure that we do not apply lighting to the skybox

2. Rendering scene with only the pixels of the lights
Rendering the pixels of the lights will be similar to the 2D counterpart,
some parts are easier and some parts are harder. This time, rather than having
a square of geometry, that we had to cut into a circle, we have a icosphere of geometry,
which is already in the shape of a 3D point light. We treat this like any other object
in the vertex shader, while also taking the color and radius of the light into 
consideration; we take that data from the vertices, and use them to construct a light
structure. It would be better not to put these into the vertex buffer, it would be
best to use a uniform buffer, but that can be changed later. We then use the data
(color, radius, etc) from the vertex buffer to make a light structure "out pointLight light;"
In the fragment shader, we take that data in from the vertex shader "in pointLight light;",
and we also take in the position of the screen that the light is at.
We have a uniform to take in the screenshot of the scene rendered with normal map textures,
just like the 2D tutorial, and we take in the depth buffer, which is only used to check to
see if a pixel is part of the skybox. We also take in "viewRotation" as a uniform, which
has the direction the camera faces, withoutt the position of the camera (just like Skybox),
we use this to compare to the normals, so that we know if a normal faces the camera or not.
First we grab the normal from the screenshot, converting range from [0 to 1] to [-1 to 1]
	vec4 normal4 = texelFetch(texNormal, ivec2(gl_FragCoord), 0) * 2 - 1;
Then we apply the rotation of our camera
	vec3 normal = vec3(viewRotation * normal4);
After that, we need to know the 3D position of each pixel that was in the first renderpass.
One way to do this is to export a texture that holds the 3D position of all pixels, but that
is NOT how we will do that in this tutorial. Here, we use the (X, Y) screen position, 
combined with the depth buffer, to get the position of each pixel (that was in the first
renderpass). First we get the directional vector to the pixel
	vec3 viewRay = screenPosition / screenPosition.z;
Then we linearlize the depth buffer, we convert it to a range of 0-1, just like the shadow
mapping prerequisite.
	float linearDepth = projectionB / (depth - projectionA);
Then we get the 3D position of the pixel by combining both variables:
	vec3 positionVS = viewRay * linearDepth;
After that, we get our direction from the pixel to the light
	vec3 surfaceToLight = light.position - positionVS;
And the rest of the lighting calculations are exactly the same as previous tutorials

3. Final image
We draw the debug images (25% of the screen) exactly the same way
as we did it in the last tutorial. For the other 75% of the screen,
we check depth, to see if the pixel is part of the skybox. 
depth < 0.999 -> not skybox
If the pixel is in the skybox, just export the color that it takes in,
if the pixel is not in the skybox, apply lighting from the previous 
render pass, and add a directional light (sunlight) to all pixels.

Congratulations, you're done

How to Improve:	
Try 3D Deferred Rendering for Spot Lights
Try to pass the lights to the 2nd renderpass with a uniform buffer,
	rather than putting color, radius, (etc) in the vertex buffer.
	If you do this, the light structure can go straight to fragment shader
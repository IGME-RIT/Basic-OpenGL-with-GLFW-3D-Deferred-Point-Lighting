/*
Title: Deferred Point Lighting
File Name: main.cpp
Copyright � 2016
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


#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "FreeImage.h"
#include <vector>
#include "../header/mesh.h"
#include "../header/fpsController.h"
#include "../header/transform3d.h"
#include "../header/material.h"
#include "../header/texture.h"
#include "../header/cubeMap.h"
#include "../header/pointLightRenderer.h"
#include <iostream>




// Store the current dimensions of the viewport.
glm::vec2 viewportDimensions = glm::vec2(800, 600);
glm::vec2 mousePosition = glm::vec2();

// The texture we will be rendering to. It will match the dimensions of the screen.
Texture* screenColor;
Texture* screenNormal;
Texture* screenDepth;
Texture* screenLighting;

// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
    viewportDimensions = glm::vec2(width, height);

    // Resize the fullscreen texture.
    screenColor->Resize(width, height, GL_RGBA, GL_UNSIGNED_BYTE);
    screenNormal->Resize(width, height, GL_RGBA, GL_UNSIGNED_BYTE);
    screenDepth->Resize(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    screenLighting->Resize(width, height, GL_RGBA, GL_UNSIGNED_BYTE);
}

// This will get called when the mouse moves.
void mouseMoveCallback(GLFWwindow *window, GLdouble mouseX, GLdouble mouseY)
{
    mousePosition = glm::vec2(mouseX, mouseY);
}


int main(int argc, char **argv)
{

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	// Initialize GLFW
	glfwInit();

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(viewportDimensions.x, viewportDimensions.y, "Lights", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set window callbacks
	glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);

	// Initialize glew
	glewInit();


    // Similarly to how this was done in 2 dimensions, we will need 3 textures for color, normals, and lighting:
    // The sample type doesn't really matter, because we'll be using texelfetch.
    screenColor = new Texture(viewportDimensions.x, viewportDimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
    screenNormal = new Texture(viewportDimensions.x, viewportDimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
    screenLighting = new Texture(viewportDimensions.x, viewportDimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);

    // We will also need a depth texture:
    // Instead of storing 4 separate 8 bit channels of color, this stores a single channel for depth
    // It's used to calculate the 3D world position of our pixel in the lighting step
    screenDepth = new Texture(viewportDimensions.x, viewportDimensions.y, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST);


    // Create and bind the framebuffer for our geometry data.
    // This is the first buffer we render to, and contains the textures and normals of all the sprites
    GLuint geometryFrameBuffer;
    glGenFramebuffers(1, &geometryFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, geometryFrameBuffer);


    // Attach both the color and normal textures to the sprite frame buffer.
    // The sprite buffer will render to both of them in parallel.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColor->GetGLTexture(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, screenNormal->GetGLTexture(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, screenDepth->GetGLTexture(), 0);


    // Create and bind the framebuffer for our light data.
    // This is the second buffer we render to. It will contain all of the lighting information.
    GLuint lightFrameBuffer;
    glGenFramebuffers(1, &lightFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, lightFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenLighting->GetGLTexture(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);




    // The mesh loading code has changed slightly, we now have to do some extra math to take advantage of our normal maps.
    // Here we pass in true to calculate tangents.
    Mesh* model = new Mesh("../assets/ironbuckler.obj", true);
    Mesh* cube = new Mesh("../assets/cube.obj", true);


	// Create Shaders
    Shader* vertexShader = new Shader("../shaders/vertex.glsl", GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader("../shaders/diffuseNormalFrag.glsl", GL_FRAGMENT_SHADER);

    // Create A Shader Program
    ShaderProgram* shaderProgram = new ShaderProgram();
    shaderProgram->AttachShader(vertexShader);
    shaderProgram->AttachShader(fragmentShader);

    // Create a material using a texture for our model
    Material* diffuseNormalMat = new Material(shaderProgram);
    diffuseNormalMat->SetTexture("diffuseMap", new Texture("../assets/iron_buckler_diffuse.png", GL_LINEAR));
    diffuseNormalMat->SetTexture("normalMap", new Texture("../assets/iron_buckler_normal.png", GL_LINEAR));


    Shader* skyboxVertexShader = new Shader("../shaders/skyboxvertex.glsl", GL_VERTEX_SHADER);
    Shader* skyboxfragmentShader = new Shader("../shaders/skyboxfragment.glsl", GL_FRAGMENT_SHADER);

    // Create A Shader Program for the skybox
    ShaderProgram* skyboxShaderProgram = new ShaderProgram();
    skyboxShaderProgram->AttachShader(skyboxVertexShader);
    skyboxShaderProgram->AttachShader(skyboxfragmentShader);

    // Create material for skybox
    Material* skyMat = new Material(skyboxShaderProgram);
    std::vector<char*> faceFilePaths;
    faceFilePaths.push_back("../assets/skyboxLeft.png");
    faceFilePaths.push_back("../assets/skyboxRight.png");
    faceFilePaths.push_back("../assets/skyboxBottom.png");
    faceFilePaths.push_back("../assets/skyboxTop.png");
    faceFilePaths.push_back("../assets/skyboxBack.png");
    faceFilePaths.push_back("../assets/skyboxFront.png");

    // The cube map class just saves time by holding all the previous cube map loading code
    CubeMap* sky = new CubeMap(faceFilePaths);
    skyMat->SetCubeMap("cubeMap", sky);

    ShaderProgram* pointLightProgram = new ShaderProgram();
    pointLightProgram->AttachShader(new Shader("../shaders/pointLightVert.glsl", GL_VERTEX_SHADER));
    pointLightProgram->AttachShader(new Shader("../shaders/pointLightFrag.glsl", GL_FRAGMENT_SHADER));
    Material* pointLightMat = new Material(pointLightProgram);
    pointLightMat->SetTexture("texNormal", screenNormal);
    pointLightMat->SetTexture("texDepth", screenDepth);


    PointLightRenderer* pointLightRenderer = new PointLightRenderer();

    // Create the material that will render the color and light to the screen
    ShaderProgram* compositionProgram = new ShaderProgram();
    compositionProgram->AttachShader(new Shader("../shaders/fullScreenVert.glsl", GL_VERTEX_SHADER));
    compositionProgram->AttachShader(new Shader("../shaders/compositionFrag.glsl", GL_FRAGMENT_SHADER));
    Material* compositionMat = new Material(compositionProgram);
    compositionMat->SetTexture("texColor", screenColor);
    compositionMat->SetTexture("texNormal", screenNormal);
    compositionMat->SetTexture("texLight", screenLighting);
    compositionMat->SetTexture("texDepth", screenDepth);


    // The transform being used to draw our second shape.
    std::vector<Transform3D> transforms;
    for (int i = 0; i < 64; i++)
    {
        Transform3D transform;
        transform.SetPosition(glm::vec3((i % 4) * 2, (i / 4 % 4) * 2, (i / 16 % 4) * 2));
        transform.RotateX(1.5);
        transforms.push_back(transform);
    }

    std::vector<PointLight> lights;
    for (int i = 0; i < 27; i++)
    {
        PointLight l = PointLight(
            glm::vec3((i / 1 % 3) * 2 + 1, (i / 3 % 3) * 2 + 1, (i / 9 % 3) * 2 + 1), 6,
            glm::vec4(1, 1, 0, .5f),
            glm::vec4(i / 27.f, (i % 9) / 9.f, (i % 3) / 3.f, 1));
        lights.push_back(l);
    }

    // Make a first person controller for the camera.
    FPSController controller = FPSController();



    // Print instructions to the console.
    std::cout << "Use WASD to move, and the mouse to look around." << std::endl;
    std::cout << "Press escape or alt-f4 to exit." << std::endl;


    float frames = 0;
    float secCounter = 0;

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Exit when escape is pressed.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        // Calculate delta time and frame rate
        float dt = glfwGetTime();
        frames++;
        secCounter += dt;
        if (secCounter > 1.f)
        {
            std::string title = "Lights FPS: " + std::to_string(frames);
            glfwSetWindowTitle(window, title.c_str());
            secCounter = 0;
            frames = 0;
        }
        glfwSetTime(0);
        

        // Update the player controller
        controller.Update(window, viewportDimensions, mousePosition, dt);
        

        std::vector<glm::mat4> matrices;

        // rotate cube transform and get a matrix for it
        for (int i = 0; i < transforms.size(); i++)
        {
            transforms[i].RotateY(dt);
            matrices.push_back(transforms[i].GetMatrix());
        }


        // View matrix.
        glm::mat4 view = controller.GetTransform().GetInverseMatrix();
        // Projection matrix.
        glm::mat4 projection = glm::perspective(.75f, viewportDimensions.x / viewportDimensions.y, .1f, 100.f);
        // Compose view and projection.
        glm::mat4 viewProjection = projection * view;

        ///////////////////////////////
        // Start Rendering           /
        /////////////////////////////

        glBindFramebuffer(GL_FRAMEBUFFER, geometryFrameBuffer);
        GLenum geometryBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, geometryBuffers);

        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0, 0.0, 0.0, 0.0);


        // Set the camera and world matrices to the shader
        // The string names correspond directly to the uniform names within the shader.
        diffuseNormalMat->SetMatrix("cameraView", viewProjection);


        // Bind the material and draw the model
        diffuseNormalMat->Bind();

        // Instead of just drawing one, we pass in a vector of matrices (this function is where the instancing really happens)
        model->DrawInstanced(matrices);

        diffuseNormalMat->Unbind();

        /////////////////////////
        // Skybox              /
        ///////////////////////


        glm::mat4 viewRotation = projection * glm::mat4(glm::mat3(view));
        skyMat->SetMatrix("cameraView", viewRotation);
        glDepthFunc(GL_LEQUAL);
        skyMat->Bind();
        cube->Draw();
        skyMat->Unbind();
        // Set the depth test back to the default setting.
        glDepthFunc(GL_LESS);

        ////////////////////////
        // Lighting           /
        //////////////////////

        // Set up the frame buffer

        glBindFramebuffer(GL_FRAMEBUFFER, lightFrameBuffer);
        GLenum lightBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, lightBuffers);

        // Let the light renderer take care of the rest
        pointLightMat->SetMatrix("cameraView", viewProjection);
        pointLightMat->SetMatrix("viewRotation", viewRotation);
        // 100 and .1 are the near and far plane.
        // These values are used to calculate the world position of a pixel from its depth value.
        pointLightMat->SetFloat("projectionA", 100 / (100 - .1)); 
        pointLightMat->SetFloat("projectionB", (-100 * .1) / (100 - .1));
        pointLightRenderer->RenderLights(lights, pointLightMat);

        ////////////////////////
        // Composition        /
        //////////////////////

        // Now, combine the sprite colors with the lights
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        // Clear it.
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 0.0);

        // Bind the material to combine them
        compositionMat->Bind();

        // Draw three "vertices" as a triangle.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Unbind
        compositionMat->Unbind();





		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();
	}

    // Delete mesh objects
    delete model;
    delete cube;
    delete pointLightRenderer;

    // Free memory used by materials and all sub objects
    delete diffuseNormalMat;
    delete skyMat;
    delete pointLightMat;
    delete compositionMat;

    glDeleteFramebuffers(1, &geometryFrameBuffer);
    glDeleteFramebuffers(1, &lightFrameBuffer);

	// Free GLFW memory.
	glfwTerminate();

	// End of Program.
	return 0;
}

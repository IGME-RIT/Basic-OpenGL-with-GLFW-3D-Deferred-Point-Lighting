/*
Title: Instanced Rendering
File Name: pointLightRenderer.h
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

#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "../header/material.h"

//struct for vertex with uv
struct PointLight
{
    glm::vec3 m_position;
    float m_radius;
    glm::vec4 m_attenuation;
    glm::vec4 m_color;

    // Makes a 2d vertex with uc and color data.
    PointLight(glm::vec3 position, float radius, glm::vec4 attenuation, glm::vec4 color) {
        m_position = position;
        m_radius = radius;
        m_attenuation = attenuation;
        m_color = color;
    }
};


class PointLightRenderer
{
public:

    PointLightRenderer();
    ~PointLightRenderer();
    
    void RenderLights(std::vector<PointLight> lights, Material* pointLightMaterial);

private:

    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_instanceBuffer;

    std::vector<glm::vec3> m_vertices;
    std::vector<unsigned int> m_indices;

    Material* m_lightMat;

};
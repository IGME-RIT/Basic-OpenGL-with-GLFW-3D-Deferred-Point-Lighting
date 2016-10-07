/*
Title: Instanced Rendering
File Name: cubeMap.cpp
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

#include "../header/cubeMap.h"


//filePaths.push_back("../assets/skyboxLeft.png");
//filePaths.push_back("../assets/skyboxRight.png");
//filePaths.push_back("../assets/skyboxBottom.png");
//filePaths.push_back("../assets/skyboxTop.png");
//filePaths.push_back("../assets/skyboxBack.png");
//filePaths.push_back("../assets/skyboxFront.png");

CubeMap::CubeMap(std::vector<char*> filePaths)
{
    // Create an OpenGL texture.
    glGenTextures(1, &m_cubeMap);

    // Bind our texture as a cube map.
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);

    // Fill our openGL side texture object.
    for (GLuint i = 0; i < filePaths.size(); i++)
    {
        // Load the face and convet it to 32 bit.
        FIBITMAP* bitmap = FreeImage_ConvertTo32Bits(FreeImage_Load(FreeImage_GetFileType(filePaths[i]), filePaths[i]));

        // Load the image into OpenGL memory.
        // GL_TEXTURE_CUBE_MAP_POSITIVE_X indicates the side of the skybox. Incrementing that value gives us the constant used by each side.
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap),
            0, GL_BGRA, GL_UNSIGNED_BYTE, static_cast<void*>(FreeImage_GetBits(bitmap)));

        // We can unload the image now.
        FreeImage_Unload(bitmap);
    }

    // Set sampler parameters on our cube map.
    // These make sure the texture doesn't look pixelated.
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These prevent artifacts from appearing near the edges.
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Unbind
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap()
{
    glDeleteTextures(1, &m_cubeMap);
}

void CubeMap::IncRefCount()
{
    m_refCount++;
}

void CubeMap::DecRefCount()
{
    m_refCount--;
    if (m_refCount == 0)
    {
        delete this;
    }
}

GLuint CubeMap::GetGLCubeMap()
{
    return m_cubeMap;
}

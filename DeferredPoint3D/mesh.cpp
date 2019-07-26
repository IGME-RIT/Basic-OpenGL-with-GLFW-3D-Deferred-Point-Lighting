/*
Title: Deferred Point Lighting
File Name: mesh.cpp
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

#include "mesh.h"



Mesh::Mesh(std::vector<Vertex3dUVNormal> vertices, std::vector<unsigned int> indices)
{
	m_vertices = vertices;
	m_indices = indices;
	// Create the shape by setting up buffers

	// Set up vertex buffer
    glGenBuffers(1, &m_instanceBuffer);

	// Set up vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex3dUVNormal), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set up index buffer
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Mesh(std::string filePath, bool calcTangents)
{

    // before we do anything, lets first check if the file even exists:
    std::ifstream file(filePath);

    if (!file.good())
    {
        // If we encounter an error, print a message and return.
        std::cout << "Can't read file: " << filePath << std::endl;
        return;
    }

    // These are temporary, and will contain our vertex data while we read from the file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;


    // Now we have to process the string . . .
    std::string line;

    // Loop over every line in the file, storing it in the string 'line'
    while (std::getline(file, line))
    {
        /*

        obj files have a ton of features, but we'll only be using the core set here

        =================================================
        Lines starting with just 'v' are vertex positions. They might look like this:
        v 1.0 -2.5345 3.141
        The positions are stored as floating point values seperated by spaces
        =================================================
        vt is for uvs aka texture coordinates:
        vt 0.12 0.87
        Remember they only have an x and y value
        =================================================
        vn is for our normals:
        vn -0.473 0.1201 0.7778
        =================================================
        f indicates faces, they are the most complex and look something like this:
        f 100/1/1 101/1/1 102/3/2 103/3/2

        each set of values  here ex 100/1/1, is a vertex
        the first (100) is the index of the vertex position in the list of vertices as they appear in the file
        the second (1) is the index of the uv coordinates in the list of uvs the same way
        and the third (1) is the index of our normals in the corresponding list of normals
        
        You'll notice that there are 4 of these groupings.
        That is because those 4 vertices form a quad.
        Some of them will also appear in groups of 3, as tris, so we'll have to account for both cases.

        Also, since obj files are ordered differently than how we use them, we have to reorganize them.
        */

        
        // Let's get started:


        // check if it's a vertex position
        // strncmp checks if the first n characters of these strings match (n is 2 here)
        if (strncmp("v ", &line[0], 2) == 0) 
        {
            // strtok takes in a string and a delimiter, storing the string internally.
            // it also returns a pointer to the first character of the first word (split by the character given, " ").
            strtok(&line[0], " "); 
            // every time after the first, strtok returns the next word (splitting with the given character) until it runs out of words.
            float x = std::stof(strtok(NULL, " "));
            float y = std::stof(strtok(NULL, " "));
            float z = std::stof(strtok(NULL, " "));
            vertices.push_back(glm::vec3(x, y, z)); // make a vector from the given values and store it.
        }
        // texture coordinates
        else if (strncmp("vt", &line[0], 2) == 0)
        {
            // same as above, but only 2 floats per value
            strtok(&line[0], " ");
            float u = std::stof(strtok(NULL, " "));
            float v = std::stof(strtok(NULL, " "));
            uvs.push_back(glm::vec2(u, v));
        }
        // vertex normals
        else if (strncmp("vn", &line[0], 2) == 0)
        {
            // one more time for normals!
            strtok(&line[0], " ");
            float x = std::stof(strtok(NULL, " "));
            float y = std::stof(strtok(NULL, " "));
            float z = std::stof(strtok(NULL, " "));
            normals.push_back(glm::vec3(x, y, z));
        }
        // faces (these should be last in the file, so we can just interpret them immediately)
        else if (strncmp("f", &line[0], 1) == 0)
        {
            // keep track of the indices from our vector/uv/normal buffer for this face
            std::vector<unsigned int> faceIndices;

            // this will store the vertices as we read over them.
            char* token = strtok(&line[0], " ");

            // loop over the vertices until we get NULL (what strtok returns at the end of the line)
            while ((token = strtok(0, "/")) != NULL)
            {
                // split up index data (important, obj file indexing starts at 1, so we have to subtract 1 here or bad things will happen)
                int i = std::stoi(token) - 1;
                glm::vec3 vp = vertices[i];

                token = strtok(0, "/");
                int j = std::stoi(token) - 1;
                glm::vec2 vt = uvs[j];

                token = strtok(0, " ");
                int k = std::stoi(token) - 1;
                glm::vec3 vn = normals[k];

                // Unfortunately obj files store vertex data in seperate groups.
                // We could use the data that way, but we would repeat tons of vertices, and be unable to use an index buffer.
                // Instead we're going to compare vertices to avoid redundant values.

                // does this vertex exist already?
                bool newVertex = false;

                // loop over all existing vertices
                for (int i = 0; i < m_vertices.size(); i++)
                {
                    Vertex3dUVNormal other = m_vertices[i];
                    // if match found...
                    if (vp == other.m_position &&
                        vt == other.m_texCoord &&
                        vn == other.m_normal)
                    {
                        //...reuse the index for this face and stop iterating
                        faceIndices.push_back(i);
                        newVertex = true;
                        break;
                    }
                }

                // if a new vertex, create and add it to the collection
                if (!newVertex)
                {
                    // the index for this vertex will be at the end of the collection
                    faceIndices.push_back(m_vertices.size());
                    m_vertices.push_back(Vertex3dUVNormal(vp, vt, vn, glm::vec3()));
                }
            }

            // now that our face is using the final indices, we need to add it to our index buffer

            // add the first 3 indices of the face to our index collection to form a triangle
            for (int i = 0; i < 3; i++)
            {
                m_indices.push_back(faceIndices[i]);
            }
            // the face is a quad, add 3 more vertices to form a triangle
            if (faceIndices.size() == 4)
            {
                m_indices.push_back(faceIndices[0]);
                m_indices.push_back(faceIndices[2]);
                m_indices.push_back(faceIndices[3]);
            }


            // we're done here
        }
        // other line, just print it out for debug purposes
        else 
        {
            std::cout << line << std::endl;
        }
    }


    // After all of that nonsense close the file
    file.close();

    // If we said to calculate tangents, do that now
    if (calcTangents)
    {
        CalculateTangents();
    }
    // create buffers for opengl just like normal

    // Set up vertex buffer
    glGenBuffers(1, &m_instanceBuffer);

	// Set up vertex buffer
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex3dUVNormal), &m_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Set up index buffer
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
	// Clear buffers for the shape object when done using them.
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
    glDeleteBuffers(1, &m_instanceBuffer);
}



void Mesh::Draw()
{
    
    // Bind the vertex buffer and set the Vertex Attribute.
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3dUVNormal), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3dUVNormal), (void*)sizeof(glm::vec3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex3dUVNormal), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex3dUVNormal), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Enable vertex attribute
	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // Bind element array buffer and draw all indices in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Disable vertex attribute and unbind index buffer.
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

void Mesh::DrawInstanced(std::vector<glm::mat4> matrices)
{
    // First, we bind the vertex buffer and set the Vertex Attributes just like we normally would
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    // These take up the first 3 attribute slots
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3dUVNormal), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3dUVNormal), (void*)sizeof(glm::vec3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex3dUVNormal), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex3dUVNormal), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    // This is where things get interesting, first we want to buffer our matrix data for openGL
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), matrices.data(), GL_STATIC_DRAW);

    // Next, we tell openGL how that data is layed out.
    // Unfortunately, glVertexAttribPointer doesn't accept sizes greater than 4, so we have to do it in 4 sets of 4. This is basically unavoidable.
    // (On a more postive note, we can still use it as a matrix in the shader.)
    // Note: We aren't using the same buffer as before, but we still start at the 4th attribute location.
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(0));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 4));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 8));
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 12));

    // This leaves a problem though. If we just had the above code, we would end up using a different matrix for each vertex.
    // In order to get around that problem, we use glVertexAttribDivisor
    // By default, this value is 0, which is what makes the value advance for each vertex.
    // By setting a value of 1, it will advance for each instance of the mesh that we render.
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Enable vertex attribute, all 8 of them
    for (int i = 0; i < 8; i++)
    {
        glEnableVertexAttribArray(i);
    }

    // Bind index buffer buffer and draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    // This call is just like the glDrawElements in the non instanced draw function, but
    // we also pass in the number of instances we want to draw.
    glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0, matrices.size());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Disable vertex attributes.
    for (int i = 0; i < 8; i++)
    {
        glDisableVertexAttribArray(i);
    }

    // Set divisors back to default to prevent errors showing up later.
    glVertexAttribDivisor(4, 0);
    glVertexAttribDivisor(5, 0);
    glVertexAttribDivisor(6, 0);
    glVertexAttribDivisor(7, 0);

}


void Mesh::CalculateTangents()
{
    // Tangents are calculated per face, so we loop over our vertices one face at a time...
    for (unsigned int i = 0; i < m_indices.size(); i += 3)
    {
        Vertex3dUVNormal& v0 = m_vertices[m_indices[i]];
        Vertex3dUVNormal& v1 = m_vertices[m_indices[i + 1]];
        Vertex3dUVNormal& v2 = m_vertices[m_indices[i + 2]];

        // Subtract to get the vector between our first vertex, and the other two
        glm::vec3 edge1 = v1.m_position - v0.m_position;
        glm::vec3 edge2 = v2.m_position - v0.m_position;

        // calculate corresponding vectors in texture space
        glm::vec2 tex1 = glm::vec2(v1.m_texCoord.x - v0.m_texCoord.x, v2.m_texCoord.x - v0.m_texCoord.x);
        glm::vec2 tex2 = glm::vec2(v1.m_texCoord.y - v0.m_texCoord.y, v2.m_texCoord.y - v0.m_texCoord.y);

        // calculate the inverse of the determinant of those two vectors as a matrix?
        float f = 1.0f / (tex1.x * tex2.y - tex1.y * tex2.x);

        glm::vec3 tangent;

        // scale the components of our vectors to get a tangent vector
        tangent.x = f * (tex2.y * edge1.x - tex2.x * edge2.x);
        tangent.y = f * (tex2.y * edge1.y - tex2.x * edge2.y);
        tangent.z = f * (tex2.y * edge1.z - tex2.x * edge2.z);

        v0.m_tangent += tangent;
        v1.m_tangent += tangent;
        v2.m_tangent += tangent;
    }

    for (unsigned int i = 0; i < m_vertices.size(); i++)
    {
        m_vertices[i].m_tangent = glm::normalize(m_vertices[i].m_tangent);
    }
}
#pragma once
#include <array>
// Struct for the data a vertex contains ( used in the mesh.cpp ) 
struct Vertex
{
	float pos[3];
	float tex[2];
	float nor[3];
	
	Vertex() = default;
	Vertex(const std::array<float, 3>& position, const std::array<float, 2>& textureCoords, const std::array<float, 3>& normal)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
			nor[i] = normal[i];
		}

		tex[0] = textureCoords[0];
		tex[1] = textureCoords[1];
	}
	// Assignment-operator 
	Vertex& operator=(const Vertex& vertex)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = vertex.pos[i];
			nor[i] = vertex.nor[i];

		}

		tex[0] = vertex.tex[0];
		tex[1] = vertex.tex[1];

		return *this;
	}
};
// Struct that contains the data of a face ( three vertices ) ( used in the mesh.cpp ) 
struct Face
{
	Vertex vertices[3];

	Face() = default;
	Face(Vertex v1, Vertex v2, Vertex v3)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
	}
};
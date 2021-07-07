#pragma once
#include <array>

struct Vertex
{
	float pos[3];
	float tex[2];
	float nor[3];
	float tan[3];
	float btan[3];

	Vertex() = default;
	Vertex(const std::array<float, 3>& position, const std::array<float, 2>& textureCoords, const std::array<float, 3>& normal, std::array<float, 3> tangent = { 0,0,0 }, std::array<float, 3> bitangent = { 0,0,0 })
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
			nor[i] = normal[i];
			tan[i] = tangent[i];
			btan[i] = bitangent[i];
		}

		tex[0] = textureCoords[0];
		tex[1] = textureCoords[1];
	}

	Vertex& operator=(const Vertex& vertex)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = vertex.pos[i];
			nor[i] = vertex.nor[i];
			tan[i] = vertex.tan[i];
			btan[i] = vertex.btan[i];
		}

		tex[0] = vertex.tex[0];
		tex[1] = vertex.tex[1];

		return *this;
	}

	void setTangent(float tangent[3]) { tan[0] = tangent[0]; tan[1] = tangent[1]; tan[2] = tangent[2]; };
	void setBiTangent(float bitangent[3]) { btan[0] = bitangent[0]; btan[1] = bitangent[1]; btan[2] = bitangent[2]; };
};

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
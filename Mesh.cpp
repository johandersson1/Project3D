#include "Mesh.h"
#include <fstream>

bool Mesh::LoadModel(std::string name)
{
	std::vector<std::array<float, 3>> v;
	std::vector<std::array<float, 2>> vt;
	std::vector<std::array<float, 3>> vn;
	std::vector<std::array<std::array<int, 3>, 3>> f;

	std::ifstream reader;

	reader.open("Models/" + name + "/" + name + ".obj", std::ios::beg);

	if (!reader.is_open())
		return false;

	int counter = 0;
	std::string line;
	while (std::getline(reader, line))
	{
		std::string text;
		reader >> text;
		if (text == "v")
		{
			std::array<float, 3> vertex;

			reader >> vertex[0];
			reader >> vertex[1];
			reader >> vertex[2];

			v.push_back(vertex);
		}

		if (text == "vt")
		{
			std::array<float, 2> texCoord;

			reader >> texCoord[0];
			reader >> texCoord[1];

			texCoord[1] = 1 - texCoord[1];

			vt.push_back(texCoord);
		}

		if (text == "vn")
		{
			std::array<float, 3> normal;

			reader >> normal[0];
			reader >> normal[1];
			reader >> normal[2];

			vn.push_back(normal);
		}

		if (text == "f")
		{
			std::array<std::array<int, 3>, 3> face;

			std::string temp;

			for (int i = 0; i < 3; ++i)
			{
				reader >> temp;

				std::string substr;

				face[0][i] = std::stoi(temp.substr(0, temp.find("/")));

				substr = temp.substr(temp.find("/") + 1, temp.length());
				face[1][i] = std::stoi(substr.substr(0, substr.find("/")));
				face[2][i] = std::stoi(substr.substr(substr.find("/") + 1, substr.length()));
			}

			f.push_back(face);
		}
	}

	for (int i = 0; i < f.size(); ++i)
	{
		std::array<float, 3> pos;
		std::array<float, 2> tex;
		std::array<float, 3> nor;
		std::array<Vertex, 3> vertices;

		for (int j = 0; j < 3; ++j)
		{
			int index = f[i][0][j] - 1;

			pos[0] = v[index][0];
			pos[1] = v[index][1];
			pos[2] = v[index][2];

			index = f[i][1][j] - 1;

			tex[0] = vt[index][0];
			tex[1] = vt[index][1];

			index = f[i][2][j] - 1;

			nor[0] = vn[index][0];
			nor[1] = vn[index][1];
			nor[2] = vn[index][2];

			positions.push_back({ pos[0],pos[1],pos[2] });
			vertices[j] = Vertex(pos, tex, nor);
			vertexCount++;
		}

		Face face = Face(vertices[0], vertices[1], vertices[2]);
		faces.push_back(face);
	}

	return true;
}

bool Mesh::LoadMaterial(ID3D11Device* device, std::string name)
{
	std::ifstream reader;

	reader.open("Models/" + name + "/" + name + ".mtl", std::ios::beg);

	if (!reader.is_open())
		return false;

	int counter = 0;
	std::string line;
	while (std::getline(reader, line))
	{
		std::string text;
		reader >> text;

		if (text == "Kd")
		{
			XMFLOAT4 diffuse;

			reader >> diffuse.x;
			reader >> diffuse.y;
			reader >> diffuse.z;
			diffuse.w = 0;

			material.data.diffuse = diffuse;
		}

		if (text == "Ka")
		{
			XMFLOAT4 ambient;

			reader >> ambient.x;
			reader >> ambient.y;
			reader >> ambient.z;
			ambient.w = 0;

			material.data.ambient = ambient;
		}

		if (text == "Ks")
		{
			XMFLOAT4 specular;

			reader >> specular.x;
			reader >> specular.y;
			reader >> specular.z;
			specular.w = 0;

			material.data.specular = specular;
		}

		if (text == "Ns")
		{
			reader >> material.data.specularPower;
		}

		if (text == "map_Kd")
		{
			std::string diffuse;
			reader >> diffuse;
			//material.diffuseTexture = Texture(device, "Models/" + name + "/" + diffuse);
			material.diffuseTexures.push_back(Texture(device, "Models/" + name + "/" + diffuse));
		}

	}

	return true;
}
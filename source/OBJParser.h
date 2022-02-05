/*=============================================================================*/
// Copyright 2021 Elite Engine 2.0
// Authors: Thomas Goussaert
/*=============================================================================*/
// EOBJParser.h: most basic OBJParser!
/*=============================================================================*/

#include <string>
#include <fstream>
#include <vector>
#include "Mesh.h"

static bool ParseOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	std::ifstream file(filename);
	if (!file)
		return false;

	std::vector<FVector3> positions;
	std::vector<FVector3> normals;
	std::vector<FVector2> UVs;

	vertices.clear();
	indices.clear();

	std::string sCommand;
	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;
			positions.push_back(FVector3(x, y, -z));
		}
		else if (sCommand == "vt")
		{
			// Vertex TexCoord
			float u, v;
			file >> u >> v;
			UVs.push_back(FVector2(u, 1 - v));
		}
		else if (sCommand == "vn")
		{
			// Vertex Normal
			float x, y, z;
			file >> x >> y >> z;
			normals.push_back(FVector3(x, y, z));
		}
		else if (sCommand == "f")
		{
			//if a face is read:
			//construct the 3 vertices, add them to the vertex array
			//add three indices to the index array
			//add the material index as attibute to the attribute array
			//
			// Faces or triangles
			Vertex vertex{};
			size_t iPosition, iTexCoord, iNormal;
			for (size_t iFace = 0; iFace < 3; iFace++)
			{
				// OBJ format uses 1-based arrays
				file >> iPosition;
				vertex.position = positions[iPosition - 1];

				if ('/' == file.peek())//is next in buffer ==  '/' ?
				{
					file.ignore();//read and ignore one element ('/')

					if ('/' != file.peek())
					{
						// Optional texture coordinate
						file >> iTexCoord;
						vertex.uv = UVs[iTexCoord - 1];
					}

					if ('/' == file.peek())
					{
						file.ignore();

						// Optional vertex normal
						file >> iNormal;
						vertex.normal = normals[iNormal - 1];
					}
				}

				vertices.push_back(vertex);
				indices.push_back(uint32_t(vertices.size()) - 1);
			}
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');
	}

	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		uint32_t index0 = indices[i];
		uint32_t index1 = indices[i + 1];
		uint32_t index2 = indices[i + 2];

		const FVector3& p0 = vertices[index0].position;
		const FVector3& p1 = vertices[index1].position;
		const FVector3& p2 = vertices[index2].position;
		const FVector2& uv0 = vertices[index0].uv;
		const FVector2& uv1 = vertices[index1].uv;
		const FVector2& uv2 = vertices[index2].uv;

		const FVector3 edge0 = p1 - p0;
		const FVector3 edge1 = p2 - p0;

		const FVector2 diffX = FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const FVector2 diffY = FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
		float r = 1.f / Cross(diffX, diffY);

		FVector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		vertices[index0].tangent += tangent;
		vertices[index1].tangent += tangent;
		vertices[index2].tangent += tangent;
	}
	for (auto& v : vertices)
	{
		v.tangent = GetNormalized(Reject(v.tangent, v.normal));
	}

	return true;
}
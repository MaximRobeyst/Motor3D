#pragma once
#include "Component.h"
#include "Mesh.h"

class TerrainComponent : public IComponent
{
public:
	TerrainComponent(int width = 8, int height = 8, std::string m_HeightMapFile = "Resources/ireland-heightmap_64.raw");
	~TerrainComponent();

	void Start() override;
	void Render() override;

private:
	void ParseHeightMap();
	void CreateGrid();

	int m_NrOfRows{ 64 };
	int m_NrOfColumns{ 64 };
	int m_NrOfVertices{};

	std::vector<unsigned short> m_VecHeightValues{};
	std::vector<Vertex> m_VertexArr{};
	std::vector<uint32_t> m_IndexArr{};

	std::string m_HeightMapFile{};
	Mesh* m_pMesh{};
};


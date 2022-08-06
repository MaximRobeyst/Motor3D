#pragma once
#include "Component.h"
#include "Mesh.h"

class MeshComponent;
class TerrainComponent : public IComponent
{
public:
	TerrainComponent(int width = 8, int height = 8, std::string m_HeightMapFile = "Resources/ireland-heightmap_64.raw");
	~TerrainComponent();

	void Start() override;
	void Render() override;

	void RegisterMembers() override;

	void RenderGUI() override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>&);
	virtual void Deserialize(const rapidjson::Value&);

private:
	void ParseHeightMap();
	void CreateGrid();

	void Remesh();

	int m_NrOfRows{ 64 };
	int m_NrOfColumns{ 64 };
	int m_NrOfVertices{};

	float m_Height{ 1.f };

	std::vector<unsigned short> m_VecHeightValues{};
	std::vector<Vertex> m_VertexArr{};
	std::vector<uint32_t> m_IndexArr{};

	std::string m_HeightMapFile{};

	TransformComponent* m_pTransform{};
	MeshComponent* m_pMeshComponent{};
};


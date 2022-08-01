#include "TerrainComponent.h"

#include "Factory.h"
#include "Material.h"
#include "Scene.h"
#include "MaterialManager.h"
#include "Texture.h"
#include "TransformComponent.h"

#include <fstream>
#include <vector>
#include <imgui.h>

const Creator<IComponent, TerrainComponent> m_TerrainCreator{};

TerrainComponent::TerrainComponent(int width, int height, std::string heightMapFile)
	: m_NrOfRows{width}
	, m_NrOfColumns{height}
	, m_NrOfVertices{m_NrOfRows * m_NrOfColumns}
	, m_HeightMapFile{heightMapFile}
{
	ParseHeightMap();
	CreateGrid();
}

TerrainComponent::~TerrainComponent()
{
	delete m_pMesh;
}

void TerrainComponent::Start()
{
	m_pTransform = m_pGameobject->GetComponent<TransformComponent>();
}

void TerrainComponent::Render()
{
	m_pMesh->SetWorldMatrix(m_pTransform->GetWorldMatrix());
	m_pMesh->Render(MyEngine::GetSingleton()->GetDeviceContext(), m_pGameobject->GetScene()->GetCamera());
}

void TerrainComponent::RenderGUI()
{
	if (ImGui::InputFloat("height", &m_Height))
		Remesh();

	if (ImGui::InputInt("Rows", &m_NrOfRows))
		Remesh();

	if (ImGui::InputInt("Colums", &m_NrOfColumns))
		Remesh();
}

void TerrainComponent::ParseHeightMap()
{
	m_VecHeightValues.resize(m_NrOfVertices, 0);

	std::ifstream inFile{};
	inFile.open(m_HeightMapFile.c_str(), std::ios_base::binary);
	if (!inFile)
	{
		return;
	}

	inFile.read(reinterpret_cast<char*>(&m_VecHeightValues[0]), static_cast<std::streamsize>(m_NrOfVertices * sizeof(unsigned short)));
	inFile.close();
}

void TerrainComponent::CreateGrid()
{
	int nrOfRows = m_NrOfRows, nrOfColums = m_NrOfColumns;
	float width = 20.f, depth = 20.f;
	float cellWidth = width / nrOfColums;
	float cellDepth = depth / nrOfRows;
	float cellXPos = -width / 2.f;
	float cellzPos = depth / 2.f;

	m_VertexArr.resize(m_NrOfVertices);
	for (int row = 0; row < nrOfRows; ++row)
	{
		cellXPos = -width / 2.f;
		for (int col = 0; col < nrOfColums; ++col)
		{
			int vertexId = row * nrOfColums + col;
			m_VertexArr[vertexId].position.x = cellXPos;
			m_VertexArr[vertexId].position.y = (m_VecHeightValues[vertexId] / 65535.f) * m_Height;
			m_VertexArr[vertexId].position.z = cellzPos;

			m_VertexArr[vertexId].uv.x = static_cast<float>(col) /  static_cast<float>(nrOfColums - 1);
			m_VertexArr[vertexId].uv.y = static_cast<float>(row) / static_cast<float>(nrOfRows - 1);

			cellXPos += cellWidth;
		}
		cellzPos -= cellDepth;
	}

	int nrQuadsRow = nrOfRows - 1;
	int nrQuadsColumn = nrOfColums - 1;

	m_IndexArr.reserve(nrQuadsRow * nrQuadsColumn * 6);
	for (int row = 0; row < nrQuadsRow; ++row)
	{
		for (int col = 0; col < nrQuadsColumn; ++col)
		{
			int a = row * nrOfColums + col;
			int b = a + 1;
			int c = a + nrOfColums;
			int d = c + 1;

			m_IndexArr.push_back(a);
			m_IndexArr.push_back(d);
			m_IndexArr.push_back(c);

			m_IndexArr.push_back(a);
			m_IndexArr.push_back(b);
			m_IndexArr.push_back(d);
		}
	}

	// Set mesh from the mesh component

	m_pMesh = new Mesh(MyEngine::GetSingleton()->GetDevice(), MyEngine::GetSingleton()->GetWindowHandle(), m_VertexArr, m_IndexArr, "Resources/terrain.obj", 0, MaterialManager::GetInstance()->GetMaterial("default"));
	m_pMesh->GetMaterial("default")->SetDiffuseMap(new Texture(MyEngine::GetSingleton()->GetDevice(), "Resources/ireland_map.png"));
}

void TerrainComponent::Remesh()
{
	m_IndexArr.clear();
	m_IndexArr.clear();

	m_NrOfVertices = m_NrOfRows * m_NrOfColumns;

	ParseHeightMap();
	CreateGrid();
}

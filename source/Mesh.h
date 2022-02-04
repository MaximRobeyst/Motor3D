#pragma once
#include <vector>

#include "DataTypes.h"
#include "RGBColor.h"

#include "MyEngine.h"

class Texture;
class Camera;
class Material;

struct Vertex
{
	FVector3 position;
	FVector2 uv;
	FVector3 normal;
	FVector3 tangent;
	//RGBColor color;
};

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, HWND hWnd, const std::string& filePath);				// Constructor
	Mesh(ID3D11Device* pDevice, HWND hWnd, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);				// Constructor
	~Mesh();				// Destructor

	// Copy/move constructors and assignment operators
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) noexcept = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other)	noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera);

private:
	// Private member functions		

	// Datamembers
	ID3D11InputLayout* m_pVertexLayout{ nullptr };
	ID3D11Buffer* m_pVertexBuffer{ nullptr };
	ID3D11Buffer* m_pIndexBuffer{ nullptr };
	Texture* m_pTexture{ nullptr };

	Material* m_pMatrial{ nullptr };
	FMatrix4 m_WorldMatrix{};

	uint32_t m_AmountIndices{};
};


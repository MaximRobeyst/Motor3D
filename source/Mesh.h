#pragma once
#include <vector>

#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)

#include "RGBColor.h"

#include "MyEngine.h"
#include <map>

class Texture;
class Camera;
class Material;

struct Vertex
{
	FVector3 position;
	FVector3 normal;
	FVector3 tangent;
	FVector2 uv;
	//RGBColor color;
};

class Mesh final
{
public:
	Mesh(ID3D11Device* pDevice, HWND hWnd, const std::string& filePath, Material* pMaterial);				// Constructor
	Mesh(ID3D11Device* pDevice, HWND hWnd, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& filePath, int submeshId, Material* pMaterial);				// Constructor
	~Mesh();				// Destructor

	// Copy/move constructors and assignment operators
	Mesh(const Mesh& other);
	Mesh(Mesh&& other) noexcept = delete;
	Mesh& operator=(const Mesh& other) ;
	Mesh& operator=(Mesh&& other)	noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera);

	FMatrix4 GetWorldMatrix() const;
	void SetWorldMatrix(const FMatrix4& worldMatrix);

	//void AddMaterial(const std::string& materialName, Material* pMaterial);
	void SetMaterial(const std::string& name, Material* m_pMaterial);
	Material* GetMaterial(const std::string& name) const;
	Mesh* GetSubMesh(int index);

	std::string GetFilename();
	int GetSubmeshID();

private:
	// Private member functions		
	void Initialize(ID3D11Device* pDevice, HWND hWnd, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

	// Datamembers
	ID3D11InputLayout* m_pVertexLayout{ nullptr };
	ID3D11Buffer* m_pVertexBuffer{ nullptr };
	ID3D11Buffer* m_pIndexBuffer{ nullptr };

	std::string m_Filename;
	std::vector<Mesh*> m_pSubmeshes{};
	int m_SubmeshId;

	Material* m_pMaterial{ nullptr };

	FMatrix4 m_WorldMatrix{};

	uint32_t m_AmountIndices{};
};


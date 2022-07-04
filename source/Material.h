#ifndef MATERIAL_HEADER
#define MATERIAL_HEADER

// Include Files
#include "MyEngine.h"
#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)

#include "Mesh.h"

#include <memory>
#include <stringbuffer.h>
#include <prettywriter.h>
#undef max
#undef min
#include <document.h>

class GameObject;
class Scene;
class Texture;

// Material Class						
// Turn this into abstract class
class Material
{
public:
	Material(ID3D11Device* pDevice, const std::string& assertFile, const std::string& name = "grid_material");				// Constructor
	virtual ~Material();				// Destructor

	// Copy/move constructors and assignment operators
	Material(const Material& other);
	Material(Material&& other) noexcept = delete;
	Material& operator=(const Material& other) = delete;
	Material& operator=(Material&& other)	noexcept  = delete;
	// Member functions						
	
	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;

	ID3DX11EffectMatrixVariable* GetMatWorldViewProjMatrix() const;

	void SetDiffuseMap(Texture* pTexture);
	
	void SetName(const std::string& name);
	std::string GetName() const;

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assertFile);

	void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>& writer);
	static Material* Deserialize(Scene* pScene, const rapidjson::Value& value);

protected:
	// Private member functions								
	

	// Datamembers								
	ID3DX11Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pTechnique{ nullptr };

	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };

	Texture* m_pTexture;
	std::string m_Name;
	std::string m_AssertFile;

};
#endif

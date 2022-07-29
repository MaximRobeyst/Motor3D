#pragma once
#include "Component.h"

class MeshComponent : public IComponent
{
public:
	MeshComponent(Mesh* pMesh = nullptr);
	~MeshComponent();

	void Start() override;
	void Render() override;
	void Update() override;

	void RenderGUI() override;

	virtual void Serialize(rapidjson::PrettyWriter< rapidjson::StringBuffer>&);
	virtual void Deserialize(const rapidjson::Value&);
private:
	void SetMesh(const std::string& meshpath);

	Mesh* m_pMesh{};
	TransformComponent* m_pTransform;
};

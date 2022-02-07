#pragma once

#include "resource.h"
#include <windows.h>
#include <vector>
#include "Light.h"

class Mesh;
class Camera;
class Material;
class LitMaterial;

class MyApplication
{
public:
	// Constructor(s) and destructor
	MyApplication();				
	virtual ~MyApplication();	

	// Disabling default copy/move constructor and assignment operator
	MyApplication(const MyApplication& yRef)			= delete;
	MyApplication(MyApplication&& yRef)					= delete;
	MyApplication& operator=(const MyApplication& yRef) = delete;
	MyApplication& operator=(MyApplication&& yRef)		= delete;

	//-------------------------------------------------
	// Own methods								
	//-------------------------------------------------
	void Initialize();

	void Paint();
	void Update(float dt);

	void Unitialize();

	void Activate(bool isActive);
	void LeftMouseButtonAction(int x, int y, bool isUp);
	void RightMouseButtonAction(int x, int y, bool isUp);

	void KeyUp(WPARAM wparam);
	void KeyDown(WPARAM wparam);
private:
	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------
	std::vector<POINT> m_PointsVec{};

	LitMaterial* m_pLitMaterial{ nullptr };
	Material* m_pUnLitMaterial{ nullptr };
	float m_Rotation{};

	std::vector<PointLight> m_pLights;

	//Mesh* m_pMesh{nullptr};
	Camera* m_pCamera{nullptr};

	std::vector<Mesh*> m_pMeshes{};
};
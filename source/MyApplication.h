#pragma once

#include "resource.h"
#include <windows.h>
#include <vector>
#include "Light.h"
#include <memory>

class Mesh;
class Camera;
class Material;
class LitMaterial;
class Scene;

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

	// The application will be even smaller the only thing we will really need is a scene the application will
	// call the functions of these scenes and wont really do anything else besides except for UI functionallity
	// later on
	Scene* m_pScene;

	LitMaterial* m_pLitMaterial{ nullptr };
	Material* m_pUnLitMaterial{ nullptr };
	float m_Rotation{};

	std::vector<PointLight> m_pLights;

	//Mesh* m_pMesh{nullptr};
	Camera* m_pCamera{nullptr};

	std::vector<Mesh*> m_pMeshes;
};
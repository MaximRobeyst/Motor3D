#pragma once

#include "resource.h"
#include <windows.h>
#include <vector>
#include "Light.h"
#include <memory>
#include <string>


class Mesh;
class Camera;
class Material;
class LitMaterial;
class Scene;

typedef int ImGuiTreeNodeFlags;

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

	void Start();
	void Render();
#ifdef _DEBUG
	void RenderGUI();
#endif
	void Update(float dt);

	void Unitialize();

	void Activate(bool isActive);
	void LeftMouseButtonAction(int x, int y, bool isUp);
	void RightMouseButtonAction(int x, int y, bool isUp);

	void KeyUp(WPARAM wparam);
	void KeyDown(WPARAM wparam);

#ifdef  _DEBUG
	Camera* GetCamera() const;
#endif //  _DEBUG

private:
#ifdef  _DEBUG
	void ApplicationFiles(const std::string& path);
	void DialogueFolder(int i, const std::string& path, ImGuiTreeNodeFlags node_flags, int& node_clicked, bool test_drag_and_drop);
#endif //  _DEBUG

	void SaveFile();
	void LoadFile();

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

#ifdef  _DEBUG
	Camera* m_pCamera{ nullptr };
#endif //  _DEBUG


	std::vector<Mesh*> m_pMeshes;
};
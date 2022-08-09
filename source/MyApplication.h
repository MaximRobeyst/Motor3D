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
class EditorWindow;

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

	virtual void Initialize() {};
	virtual void Start() {};
	virtual void Render() {};
	virtual void Update() {};

	void BaseInitialize();
	void BaseStart();
	void BaseRender();
#ifdef _DEBUG
	void RenderGUI();
#endif
	void BaseUpdate();

	Scene* GetScene();

	void Activate(bool isActive);
	void LeftMouseButtonAction();
	void RightMouseButtonAction();

	void KeyUp(WPARAM wparam);
	void KeyDown(WPARAM wparam);

#ifdef  _DEBUG
	EditorWindow* GetWindowByName(std::string name);
	void AddWindowEditor(EditorWindow* pWindow);
	Camera* GetCamera() const;
#endif //  _DEBUG

protected:
	Scene* m_pScene;
	std::vector<Mesh*> m_pMeshes;

#ifdef  _DEBUG
	Camera* m_pCamera{ nullptr };
	std::vector<EditorWindow*> m_pEditorWindows{};
#endif //  _DEBUG

private:
#ifdef  _DEBUG
	void ApplicationFiles(const std::string& path);
	void DialogueFolder(int i, const std::string& path, ImGuiTreeNodeFlags node_flags, int& node_clicked, bool test_drag_and_drop);
#endif //  _DEBUG
};
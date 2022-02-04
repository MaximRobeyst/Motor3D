#pragma once

#include "resource.h"
#include <windows.h>
#include <vector>

class Mesh;
class Camera;

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

	Mesh* m_pMesh;
	Camera* m_pCamera;
};
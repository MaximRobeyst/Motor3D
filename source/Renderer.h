#pragma once
#include "MyEngine.h"

class Renderer
{
public:
	Renderer(HWND hwnd, UINT width, UINT height);
	virtual ~Renderer() {};

	virtual void Initialize() = 0;

	HWND GetWindowHandle() const;
	UINT GetWindowHeight() const;
	UINT GetWindowWidth() const;

protected:
	HWND m_hWnd{};
	UINT m_Width{};
	UINT m_Height{};

};


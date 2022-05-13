#include "Renderer.h"

Renderer::Renderer(HWND hwnd, UINT width, UINT height)
	: m_hWnd{hwnd}
	, m_Width{width}
	, m_Height{height}
{
}

HWND Renderer::GetWindowHandle() const
{
	return m_hWnd;
}

UINT Renderer::GetWindowHeight() const
{
	return m_Height;
}

UINT Renderer::GetWindowWidth() const
{
	return m_Width;
}

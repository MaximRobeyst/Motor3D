#pragma once
#include "Renderer.h"
#include "DX11Renderer.h"

class ServiceLocator
{
public:
	static Renderer* GetRenderer();
	static DX11Renderer* GetDX11Renderer();
	static void ProvideRenderer(Renderer* pRenderService);

private:
	static Renderer* m_pRenderService;
};


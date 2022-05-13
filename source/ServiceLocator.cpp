#include "ServiceLocator.h"

Renderer* ServiceLocator::m_pRenderService{};

Renderer* ServiceLocator::GetRenderer() 
{ 
	return m_pRenderService; 
}

DX11Renderer* ServiceLocator::GetDX11Renderer()
{
	return static_cast<DX11Renderer*>(m_pRenderService);
}

void ServiceLocator::ProvideRenderer(Renderer* pRenderService)
{
	m_pRenderService = pRenderService;

}

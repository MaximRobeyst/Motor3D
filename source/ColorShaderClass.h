#pragma once
#include <d3d11.h>
#include "DataTypes.h"

class VertexShader;
class PixelShader;
class Camera;

// ColorShaderClass Class									
class ColorShaderClass final
{
public:
	struct VS_BUFFER
	{
		FMatrix4 mWorldViewProj;
	};

	ColorShaderClass();				// Constructor
	~ColorShaderClass();				// Destructor

	// Copy/move constructors and assignment operators
	ColorShaderClass(const ColorShaderClass& other) = delete;
	ColorShaderClass(ColorShaderClass&& other) noexcept = delete;
	ColorShaderClass& operator=(const ColorShaderClass& other) = delete;
	ColorShaderClass& operator=(ColorShaderClass&& other)	noexcept = delete;

	// Member functions						
	bool Initialize(ID3D11Device* pDevice, HWND hWnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera);

private:
	// Private member functions								


	// Datamembers								
	VertexShader* m_pVertexShader;
	PixelShader* m_pPixelShader;


};



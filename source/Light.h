#pragma once
#pragma warning (push, 0)
#include "DataTypes.h"
#include <DirectXMath.h>
#pragma warning(pop)

struct DirectionalLight
{
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 color;
	float intensity;
};

struct PointLight 
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
	float intensity;
};
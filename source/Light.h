#pragma once
#pragma warning (push, 0)
#include "DataTypes.h"
#pragma warning(pop)

struct DirectionalLight
{
	FVector3 direction;
	FVector3 color;
	float intensity;
};

struct PointLight 
{
	FVector3 position;
	FVector3 color;
	float intensity;
};
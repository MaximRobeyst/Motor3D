#pragma once
#include "DataTypes.h"

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
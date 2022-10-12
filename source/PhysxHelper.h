#pragma once
#include "PhysXManager.h"

#include <DirectXMath.h>
#include <wincodec.h>

namespace PhysxHelper
{
	inline physx::PxVec3 ToPxVec3(const DirectX::XMFLOAT3& vec)
	{
		return { vec.x, vec.y, vec.z };
	}

	inline physx::PxExtendedVec3 ToPxExtendedVec3(const DirectX::XMFLOAT3& vec)
	{
		return { vec.x, vec.y, vec.z };
	}

	inline physx::PxVec2 ToPxVec2(const DirectX::XMFLOAT2& vec)
	{
		return { vec.x, vec.y };
	}

	inline physx::PxVec4 ToPxVec4(const DirectX::XMFLOAT4& vec)
	{
		return { vec.x, vec.y, vec.z, vec.w };
	}

	inline physx::PxQuat ToPxQuat(const DirectX::XMFLOAT4& vec)
	{
		return { vec.x, vec.y, vec.z, vec.w };
	}
	inline DirectX::XMFLOAT3 ToXMFLOAT3(const physx::PxVec3& vec)
	{
		return { vec.x, vec.y, vec.z };
	}

	inline DirectX::XMFLOAT3 ToXMFLOAT3(const physx::PxExtendedVec3& vec)
	{
		return { static_cast<float>(vec.x), static_cast<float>(vec.y), static_cast<float>(vec.z) };
	}

	inline DirectX::XMFLOAT4 ColorToXMFLOAT4(physx::PxU32 color)
	{
		float r = static_cast<UCHAR>(color >> 16) / 255.f;
		float g = static_cast<UCHAR>(color >> 8) / 255.f;
		float b = static_cast<UCHAR>(color) / 255.f;

		return { r,g,b,1.f };
	}
};


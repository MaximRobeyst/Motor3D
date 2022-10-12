#pragma once
#include <foundation/PxErrorCallback.h>
#include <string>

// Convert an UTF8 string to a wide Unicode String

class PhysxErrorCallback final : public physx::PxErrorCallback
{
public:
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
};

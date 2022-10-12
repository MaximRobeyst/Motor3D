#include "PhysxErrorCallback.h"
#include <sstream>
#include "Logger.h"

#pragma warning(push)
#pragma warning(disable: 26812)
void PhysxErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	std::wstringstream ss;
	ss << L"PHYSX CALLBACK REPORT:" << std::endl;
	ss << "Error Code: " << code << std::endl;
	ss << "Message: " << message << std::endl;
	ss << "File: " << file << " (line: " << line << ")";

	switch (code)
	{
	case physx::PxErrorCode::eDEBUG_INFO:
		Logger::GetInstance()->LogDebug("PHYSX Debug Info @ {} (line {})\n\t{}");
		break;
	case physx::PxErrorCode::ePERF_WARNING:
	case physx::PxErrorCode::eDEBUG_WARNING:
		Logger::GetInstance()->LogWarning("PHYSX Warning @ {} (line {})\n\t{}");
		break;
	default:
		break;
	}
}

#pragma warning(pop)
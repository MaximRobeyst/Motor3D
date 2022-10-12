#pragma once
#include <PxPhysicsAPI.h>
#include <PxFiltering.h>

inline physx::PxFilterFlags OverlordSimulationFilterShader(
	physx::PxFilterObjectAttributes attribute0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attribute1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* pConstantBlock, physx::PxU32 blockSize)
{
	//If word3 is not set use default filter_shader (means one of the actors is not a Overlord RigidBody)
	if (filterData0.word3 == 0 || filterData1.word3 == 0)
	{
		return PxDefaultSimulationFilterShader(attribute0, filterData0, attribute1, filterData1, pairFlags, pConstantBlock, blockSize);
	}

	if ((filterData0.word0 & filterData1.word1) == filterData0.word0 || (filterData1.word0 & filterData0.word1) == filterData1.word0)
	{
		return physx::PxFilterFlag::eSUPPRESS;
	}

	if ((attribute0 & physx::PxFilterObjectFlag::eTRIGGER) != 0 || (attribute1 & physx::PxFilterObjectFlag::eTRIGGER) != 0)
	{
		//pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
		pairFlags |= physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	if ((filterData0.word0 & filterData1.word0) != 0)
	{
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		//pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
	}

	pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;

	return physx::PxFilterFlag::eDEFAULT;
}
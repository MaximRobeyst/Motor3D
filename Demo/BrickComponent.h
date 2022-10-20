#pragma once
#include <Component.h>

class BrickComponent : public IComponent
{
public:
	void OnTriggerExit(GameObject* pOther) override;
};


#include "BrickComponent.h"


void BrickComponent::OnTriggerExit(GameObject* )
{
	//GameObject::Destoy(GetGameObject());
	GetGameObject()->SetEnabled(false);
}

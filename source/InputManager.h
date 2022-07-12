#pragma once
#include <vector>
#include <map>

#include "Command.h"
#include "EngineCommand.h"

class InputManager
{
public:
	InputManager();
	~InputManager();

	void HandleInput();

private:
	std::map<int ,Command*> m_pCommands{};
#ifdef _DEBUG
	std::map<int, EngineCommand*> m_pEngineCommands{};
#endif
};


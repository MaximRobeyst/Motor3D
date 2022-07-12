#pragma once
class EngineCommand
{
public:
	virtual ~EngineCommand() {};

	virtual void Execute() = 0;
	virtual void Undo() = 0;
};


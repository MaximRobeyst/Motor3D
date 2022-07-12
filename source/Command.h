#pragma once
class Command
{
public:
	~Command() {};
	virtual void Execute() = 0;
};


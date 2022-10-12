#pragma once
#include <MyApplication.h>

class DemoApplication : public MyApplication
{
public:
	void Initialize() override;

private:
	void LoadPongScene();
	void LoadBaseScene();
};


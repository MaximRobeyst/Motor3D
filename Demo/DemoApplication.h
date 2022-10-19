#pragma once
#include <MyApplication.h>

class DemoApplication : public MyApplication
{
public:
	void Initialize() override;
	void Update() override;

private:
	void LoadPongScene();
	void LoadBaseScene();
};


#include "GameTime.h"

GameTime& GameTime::GetInstance()
{
    static GameTime instance{};
    return instance;
}

float GameTime::GetElapsed() const
{
    return m_ElapsedTime;
}

void GameTime::SetElapsed(float elapsedTime)
{
    m_ElapsedTime = elapsedTime;
}

int GameTime::GetFPS() const
{
    return static_cast<int>(1.f / m_ElapsedTime);
}
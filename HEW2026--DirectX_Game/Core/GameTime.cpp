#include "GameTime.h"
#include "Config.h"

void GameTime::Init()
{
	m_hitstopFrame = 0;
	m_isHitstop = false;
}

void GameTime::Update()
{
	// 先にヒットストップ中かを決めてから1減らす(逆だと1フレーム短くなる)
	m_isHitstop = (m_hitstopFrame > 0);
	if (m_hitstopFrame > 0)
	{
		m_hitstopFrame--;
	}
}

void GameTime::StartHitstop(float second)
{
	// 秒 → フレーム数(四捨五入)
	m_hitstopFrame = (int)(second * FPS + 0.5f);
}

float GameTime::GetScale()
{
	// ヒットストップ中はゲーム内の時間を遅くする
	if (m_isHitstop)
	{
		return HITSTOP_SCALE;
	}
	return 1.0f;
}

bool GameTime::IsHitstop()
{
	return m_isHitstop;
}

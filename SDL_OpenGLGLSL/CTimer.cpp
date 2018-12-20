#include "CTimer.h"

CTimer::CTimer():
	m_fFPS(0.0f),
	m_fTargetFPS(0.0f),
	m_iFrameTick(0)
{
}

CTimer::CTimer(float fps):
	m_fFPS(0.0f),
	m_fTargetFPS(fps),
	m_iFrameTick(0)
{

}


CTimer::~CTimer()
{
	
}

void CTimer::Start()
{
	m_iStartTicks = SDL_GetTicks();
}

float CTimer::End()
{
	unsigned int ticks = SDL_GetTicks() - m_iStartTicks;
	m_iFrameTick = ticks;
	
	AverageFPS();

	if (m_fTargetFPS != 0.0f)
	{
		if (1000.0f / m_fTargetFPS > m_iFrameTick)
		{
			SDL_Delay(m_fTargetFPS - m_iFrameTick);
			m_fFPS = m_fTargetFPS;
			m_iFrameTick = 1000.0f / m_fTargetFPS;
		}
	}

	return m_fFPS;
}

unsigned int CTimer::GetTimeSpan()
{
	return m_iFrameTick;
}

void CTimer::AverageFPS()
{
	static const int SAMPLECOUNT = 100;
	static float samples[SAMPLECOUNT];
	static int currentSample = 0;

	samples[currentSample++ % SAMPLECOUNT] = m_iFrameTick;

	int count = currentSample < SAMPLECOUNT ? currentSample : SAMPLECOUNT;
	float averageFrameTime = 0.0f;

	for (int i = 0; i < count; i++)
	{
		averageFrameTime += samples[i];
	}

	averageFrameTime /= count;
	m_fFPS = averageFrameTime > 0.0f ? 1000.0 / averageFrameTime : 60.0f;


}
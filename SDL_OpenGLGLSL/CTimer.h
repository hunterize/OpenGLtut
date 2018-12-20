#pragma once

#include <SDL.h>

class CTimer
{
public:
	CTimer();
	CTimer(float fps);
	~CTimer();

	void Start();
	//return fps
	float End();

	unsigned int GetTimeSpan();

private:

	void AverageFPS();

	float m_fFPS;
	float m_fTargetFPS;
	unsigned int m_iFrameTick;

	unsigned int m_iStartTicks;

};


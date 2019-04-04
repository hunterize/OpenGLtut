#pragma once

#include <SDL.h>
#include <iostream>

class CTimer
{
public:
	CTimer();
	CTimer(int fps);
	~CTimer();

	void Start();
	//return fps
	float End();

	bool IsReadyForNextFrame();

	unsigned int GetTimeSpan();

private:

	void AverageFPS();

	float m_fFPS;
	unsigned int m_iTargetFPS;
	unsigned int m_iFrameTick;

	unsigned int m_iStartTicks;
	unsigned int m_iCurrentTicks;

};


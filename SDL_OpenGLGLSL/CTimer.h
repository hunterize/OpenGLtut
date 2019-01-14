#pragma once

#include <SDL.h>
#include <iostream>

class CTimer
{
public:
	CTimer();
	CTimer(float fps);
	~CTimer();

	void Start();
	//return fps
	float End();

	bool IsReadyForDraw();

	unsigned int GetTimeSpan();

private:

	void AverageFPS();

	float m_fFPS;
	float m_fTargetFPS;
	unsigned int m_iFrameTick;

	unsigned int m_iStartTicks;
	unsigned int m_iCurrentTicks;

};


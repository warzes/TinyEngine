#include "stdafx.h"
#include "EngineTimestamp.h"
//-----------------------------------------------------------------------------
double EngineTimestamp::GetTime()
{
	return glfwGetTime();
}
//-----------------------------------------------------------------------------
void EngineTimestamp::Update()
{
	const double curTime = EngineTimestamp::GetTime();
	ElapsedTime = curTime - PreviousFrameTimestamp;
	PreviousFrameTimestamp = curTime;
}
//-----------------------------------------------------------------------------
void EngineTimestamp::UpdateAverageFrameTime()
{
	FrameTimeSum += ElapsedTime;
	NumberOfAccumulatedFrames += 1;

	if( FrameTimeSum > AverageTimeUpdateInterval && NumberOfAccumulatedFrames > 0 )
	{
		AverageFrameTime = FrameTimeSum / double(NumberOfAccumulatedFrames);
		NumberOfAccumulatedFrames = 0;
		FrameTimeSum = 0.0;
	}
}
//-----------------------------------------------------------------------------
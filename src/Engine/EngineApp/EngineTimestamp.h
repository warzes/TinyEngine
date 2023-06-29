#pragma once

class EngineTimestamp final
{
public:
	static double GetTime();

	void Update();
	void UpdateAverageFrameTime();

	// timestamp in seconds for the previous frame
	double PreviousFrameTimestamp = 0.0;
	double ElapsedTime = 0.0;
	double FrameTimeSum = 0.0;
	int NumberOfAccumulatedFrames = 0;
	double AverageFrameTime = 0.0;
	double AverageTimeUpdateInterval = 0.5;
};
#pragma once

#include <chrono>

class FrameTimer
{
public:
	FrameTimer();
	float Mark();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};


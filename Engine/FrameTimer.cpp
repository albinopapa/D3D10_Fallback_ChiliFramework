#include "FrameTimer.h"

using timer = std::chrono::high_resolution_clock;

FrameTimer::FrameTimer() 
	: 
	start( timer::now() )
{}

float FrameTimer::Mark()
{
	const auto stop = timer::now();
	const auto elapse = std::chrono::duration<float>( stop - start ).count();
	start = stop;
	return elapse;
}

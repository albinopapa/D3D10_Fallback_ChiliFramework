#include "FrameTimer.h"
#include <utility>
#include <numeric>

StopWatch::StopWatch()
{
	Start();
}
void StopWatch::Start()
{
	start = hires_clock::now();
	isStopped = false;
}
void StopWatch::Stop()
{
	end = hires_clock::now();
	isStopped = true;
}
float StopWatch::Lap()
{
	const float curTime = Elapsed();
	const float totTime = std::accumulate( lap_times.begin(), lap_times.end(), 0.f );
	const float lapTime = curTime - totTime;

	if( !isStopped )
	{
		lap_times.push_back( lapTime );
	}

	return lapTime;
}
void StopWatch::Reset()
{
	isStopped = true;
	end = start;
}
const std::vector<float>& StopWatch::LapTimes()
{
	return lap_times;
}
float StopWatch::Elapsed()const
{
	if( isStopped )
	{
		return hires_dur( end - start ).count();
	}
	else
	{
		return hires_dur( hires_clock::now() - start ).count();
	}
}

float FrameTimer::Mark()
{
	stop_watch.Stop();
	const auto elapse = stop_watch.Elapsed();
	stop_watch.Start();
	return elapse;
}

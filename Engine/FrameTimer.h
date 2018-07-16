#pragma once

#include <algorithm>
#include <chrono>
#include <memory>
#include <vector>

using hires_clock = std::chrono::high_resolution_clock;
using hires_tp = std::chrono::time_point<hires_clock>;
using hires_dur = std::chrono::duration<float>;

class StopWatch
{
public:
	StopWatch();
	void Start();
	void Stop();
	float Lap();
	void Reset();

	const std::vector<float>& LapTimes();
	float Elapsed()const;

private:
	hires_tp start, end;
	std::vector<float> lap_times;	
	bool isStopped = true;
};

class FrameTimer
{
public:
	float Mark();

private:
	StopWatch stop_watch;
};

class EventTimerBase
{
public:
	virtual void Step( ) = 0;
	virtual bool Loops()const = 0;
	virtual void Reset() = 0;
	virtual bool HasExpired()const = 0;
};

template<class CallBack>
class EventTimer : public EventTimerBase
{
public:
	EventTimer() = default;
	EventTimer( float StartTime, CallBack _callback, bool _loops = false )
		:
		start( StartTime ),
		counter( StartTime ),
		loops( _loops ),
		callback( _callback )
	{}

	void Start()
	{
		timer.Start();
	}
	void Step()override
	{
		counter -= timer.Lap();
		if( counter <= 0.f )
		{
			timer.Reset();
			counter = start;
			expired = true;
			callback();
		}
	}
	float CurrentTime()const
	{
		return timer.Elapsed();
	}
	bool Loops()const override
	{
		return loops;
	}
	void Reset()override
	{
		expired = false;
	}
	bool HasExpired()const override
	{
		return counter <= 0.f;
	}

private:
	StopWatch timer;
	float start, counter;
	CallBack callback;
	bool loops = false, expired = false;
};

class EventManager
{
public:
	void AddEventTimer( std::unique_ptr<EventTimerBase> _timer )
	{
		timers.push_back( std::move( _timer ) );
	}

	void Update( float dt )
	{
		std::for_each( timers.begin(), timers.end(),
			[ dt ]( const std::unique_ptr<EventTimerBase>& _timer )
			{
				_timer->Step();
			} );
		RemoveExpired();
	}

	void RemoveAllTimers()
	{
		timers.clear();
	}
private:
	void RemoveExpired()
	{
		auto remit = std::remove_if( timers.begin(), timers.end(), 
			[]( const std::unique_ptr<EventTimerBase>& _timer )
			{
				return ( !_timer->Loops() ) && _timer->HasExpired();
			} );

		timers.erase( remit, timers.end() );
	}

private:
	std::vector<std::unique_ptr<EventTimerBase>> timers;
};
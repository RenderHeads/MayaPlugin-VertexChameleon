//
// Vertex Chameleon Maya Plugin Source Code
// Copyright (C) 2007-2014 RenderHeads Ltd.
//
// This source is available for distribution and/or modification
// only under the terms of the MIT license.  All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the MIT license
// for more details.
//

#define MNoVersionString
#include "MayaPCH.h"
#include "Timer.h"
#include <sys/timeb.h>

#ifdef WIN32

Timer::Timer()
{
	frequency = 0;
	performance_timer_start = 0;
	resolution = 0;
	mm_timer_start = 0;
	mm_timer_elapsed = 0;
	performance_timer_elapsed = 0;
	performance_timer = 0;
	reset();
}

void Timer::reset()													// Initialize Our Timer (Get It Ready)
{
	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &frequency))
	{
		// No Performance Counter Available
		performance_timer	= FALSE;					// Set Performance Timer To FALSE
		mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
		resolution		= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
		frequency			= 1000;						// Set Our Timer Frequency To 1000
		mm_timer_elapsed	= mm_timer_start;		// Set The Elapsed Time To The Current Time
	}
	else
	{
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &performance_timer_start);
		performance_timer			= TRUE;				// Set Performance Timer To TRUE
		// Calculate The Timer Resolution Using The Timer Frequency
		resolution				= (float) (((double)1.0f)/((double)frequency));
		// Set The Elapsed Time To The Current Time
		performance_timer_elapsed	= performance_timer_start;
	}
}

float Timer::getTime() const						// Get Time In Milliseconds
{
	__int64 time;									// time Will Hold A 64 Bit Integer
	if (performance_timer)							// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( time - performance_timer_start) * resolution)*1000.0f;
		//return (int) ((time - performance_timer_start) * resolution * 1000.0f);
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( timeGetTime() - mm_timer_start) * resolution)*1000.0f;
		//return (int) ((timeGetTime() - mm_timer_start) * resolution * 1000.0f);
	}
}

#else

Timer::Timer() 
{
	_timer = new timeb;
	ftime((timeb*)_timer);
}

Timer::~Timer()
{
	if (_timer) delete (timeb*)_timer;
}

void Timer::reset() 
{
	ftime((timeb*)_timer);     
}

float Timer::getTime() const 
{
	timeb now;
	ftime(&now);

	int ds = (int)now.time - (int)((timeb*)_timer)->time;
	int dms = (int)now.millitm - (int)((timeb*)_timer)->millitm;

	return (float)(ds * 1000 + dms);
}

#endif
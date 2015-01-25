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

#ifndef TIMER_H
#define TIMER_H

#ifdef WIN32

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
// Ctime.h
class Timer
{
private:
	__int64       frequency;									// Timer Frequency
	float         resolution;									// Timer Resolution
	unsigned long mm_timer_start;								// Multimedia Timer Start Value
	unsigned long mm_timer_elapsed;								// Multimedia Timer Elapsed Time
	bool		  performance_timer;							// Using The Performance Timer?
	__int64       performance_timer_start;						// Performance Timer Start Value
	__int64       performance_timer_elapsed;					// Performance Timer Elapsed Time

public:
	Timer();
	float getTime() const;
	void reset();
};

#else

class Timer
{
public:
	void reset();
	float getTime() const;

	Timer();
	~Timer();

private:
	void *_timer;
};

#endif

#endif


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

#ifndef MAYAUTILITY_H
#define MAYAUTILITY_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>

struct TimerResult
{
	MString name;
	float time;
};

void		SetOptionVar(const MString& varName, const std::string& val);

MString		Convert(MStringArray& strings);

bool		hasArg(const MArgDatabase& argData,const char* shortName,const char* longName);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,int& result);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,unsigned int& result);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,double& result);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,float& result);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,float2& result);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,float3& result);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,double3& result);
bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,MString& result);

inline double
RandomSignedUnit()
{
	return (((double)rand() / (double)RAND_MAX) - 0.5) * 2.0;
}

inline MColor
Lerp(const MColor& src, const MColor& dst, float t)
{
	MColor result;
	result.r = src.r + (dst.r - src.r) * t;
	result.g = src.g + (dst.g - src.g) * t;
	result.b = src.b + (dst.b - src.b) * t;
	result.a = src.a + (dst.a - src.a) * t;
	return result;
}

inline unsigned int
Min(unsigned int value1, unsigned int value2)
{
#ifdef WIN32
	return min(value1, value2);
#else
	return std::min(value1, value2);
#endif
}

inline int
Min(int value1, int value2)
{
#ifdef WIN32
	return min(value1, value2);
#else
	return std::min(value1, value2);
#endif
}

inline double
Min(double value1, double value2)
{
#ifdef WIN32
	return min(value1, value2);
#else
	return std::min(value1, value2);
#endif
}

inline float
Min(float value1, float value2)
{
#ifdef WIN32
	return min(value1, value2);
#else
	return std::min(value1, value2);
#endif
}

inline double
Max(double value1, double value2)
{
#ifdef WIN32
	return max(value1, value2);
#else
	return std::max(value1, value2);
#endif
}

inline float
Max(float value1, float value2)
{
#ifdef WIN32
	return max(value1, value2);
#else
	return std::max(value1, value2);
#endif
}

inline unsigned int
ClampUInt(unsigned int low, unsigned int high, unsigned int val)
{
#ifdef WIN32
	return min(max(low, val), high);
#else
	return std::min(std::max(low, val), high);
#endif
}

inline int
ClampInt(int low, int high, int val)
{
#ifdef WIN32
	return min(max(low, val), high);
#else
	return std::min(std::max(low, val), high);
#endif
}

inline float
ClampFloat(float low, float high, float val)
{
#ifdef WIN32
	return min(max(low, val), high);
#else
	return std::min(std::max(low, val), high);
#endif
}

inline double
ClampDouble(double low, double high, double val)
{
#ifdef WIN32
	return min(max(low, val), high);
#else
	return std::min(std::max(low, val), high);
#endif
}

// Macros
//
#define MCheckStatus(status,message)	\
	if( MStatus::kSuccess != status )	\
{									\
	status.perror(message);			\
	return status;					\
}

#endif
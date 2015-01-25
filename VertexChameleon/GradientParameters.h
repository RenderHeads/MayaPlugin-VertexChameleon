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
#ifndef GRADIENTPARAMETERS_H
#define GRADIENTPARAMETERS_H

#include <float.h>
#include "ColorUtils.h"

enum GradientType
{
	GT_None,
	GT_AXIS_X,
	GT_AXIS_Y,
	GT_AXIS_Z,
	GT_SPHERICAL,
};
enum RepeatMode
{
	REPEAT_CLAMP,
	REPEAT_WRAP,
	REPEAT_MIRROR,
};

class GradientParameters
{
public:
	GradientParameters()
	{
		type = GT_None;
		repeatMode = REPEAT_CLAMP;
		colour1.setMax();
		colour2.setMin();
		isReverse = false;
		numRepeats = 1.0f;
		offset = 0.0f;

		useWorldBounds = false;
		worldMin[0] = worldMin[1] = worldMin[2] = -FLT_MAX;
		worldMax[0] = worldMax[1] = worldMax[2] = FLT_MAX;

		isOverrideCenter = false;
		isOverrideRadius = false;
		centerWorldPosition[0] = 0.0f;
		centerWorldPosition[1] = 0.0f;
		centerWorldPosition[2] = 0.0f;
		radius = 1.0f;
	}

	GradientType	type;
	RepeatMode		repeatMode;
	ColRGB			colour1, colour2;
	float			numRepeats;
	float			offset;
	float			radius;
	bool			isReverse;

	// linear
	bool			useWorldBounds;
	float3			worldMin, worldMax;

	// spherical
	bool			isOverrideCenter;
	bool			isOverrideRadius;
	float3			centerWorldPosition;

	MString			rampNodeName;
};

#endif

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
#include "MayaUtility.h"
#include "ColorUtils.h"
#include "Utility.h"

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,ColRGB& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		double valR, valG, valB, valA;
		status=argData.getFlagArgument(longName, 0, valR);
		status=argData.getFlagArgument(longName, 1, valG);
		status=argData.getFlagArgument(longName, 2, valB);
		status=argData.getFlagArgument(longName, 3, valA);
		if (status==MS::kSuccess)
		{
			result.r = (float)valR;
			result.g = (float)valG;
			result.b = (float)valB;
			result.a = (float)valA;
			return true;
		}
	}
	return false;
}

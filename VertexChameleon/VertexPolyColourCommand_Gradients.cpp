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
#include <assert.h>
#include <float.h>
#include "NodeSearch.h"
#include "Utility.h"
#include "Random.h"
#include "PolyColourNode.h"
#include "LayerData.h"
#include "MeshData.h"
#include "VertexColourJob.h"
#include "GradientParameters.h"
#include "VertexPolyColourCommand.h"

using namespace std;

void
VertexPolyColourCommand::CreateGradientColours()
{
	MStatus status;

	bool multiRamp = (m_gradientParams.rampNodeName.length() != 0);
	MObject rampObject;
	MFnDependencyNode* rampDepNode = NULL;
	MRampAttribute* ramp = NULL;
	MRampAttribute* rampAlpha = NULL;
	if (multiRamp)
	{
		MayaUtility::NodeSearch search(m_gradientParams.rampNodeName.asChar(), MFn::kRampShader);
		if (!search.getNextNode(rampObject))
			return;

		rampDepNode = new MFnDependencyNode(rampObject, &status);
		ramp = new MRampAttribute(rampObject, rampDepNode->attribute("color"), &status);
		rampAlpha = new MRampAttribute(rampObject, rampDepNode->attribute("transparency"), &status);
	}

	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_gradientParams.type == GT_SPHERICAL)
			CreateSphericalGradientColours(job, ramp, rampAlpha);
		else
			CreateAxisGradientColours(job, ramp, rampAlpha);
	}

	if (rampAlpha != NULL)
	{
		delete rampAlpha;
		rampAlpha = NULL;
	}
	if (ramp != NULL)
	{
		delete ramp;
		ramp = NULL;
	}
	if (rampDepNode != NULL)
	{
		delete rampDepNode;
		rampDepNode = NULL;
	}
}

void
VertexPolyColourCommand::CreateAxisGradientColours(VertexColourJob& job, MRampAttribute* rampColor, MRampAttribute* rampAlpha)
{
	MStatus status;

	// find min and max in axis
	float3 lowAxis, hiAxis;
	lowAxis[0] = FLT_MAX;
	lowAxis[1] = FLT_MAX;
	lowAxis[2] = FLT_MAX;
	hiAxis[0] = -FLT_MAX;
	hiAxis[1] = -FLT_MAX;
	hiAxis[2] = -FLT_MAX;

	unsigned int i = 0;
	unsigned int numPoints = job.points.length();

	if (m_gradientParams.useWorldBounds)
	{
		lowAxis[0] = m_gradientParams.worldMin[0];
		lowAxis[1] = m_gradientParams.worldMin[1];
		lowAxis[2] = m_gradientParams.worldMin[2];
		hiAxis[0] = m_gradientParams.worldMax[0];
		hiAxis[1] = m_gradientParams.worldMax[1];
		hiAxis[2] = m_gradientParams.worldMax[2];
	}
	else
	{
		for (i = 0; i < numPoints; i++)
		{
			MPoint pt = job.points[i];
			lowAxis[0] = Min(lowAxis[0], (float)pt.x);
			lowAxis[1] = Min(lowAxis[1], (float)pt.y);
			lowAxis[2] = Min(lowAxis[2], (float)pt.z);
			hiAxis[0] = Max(hiAxis[0], (float)pt.x);
			hiAxis[1] = Max(hiAxis[1], (float)pt.y);
			hiAxis[2] = Max(hiAxis[2], (float)pt.z);
		}
	}

	int axisIndex = (int)m_gradientParams.type - 1;
	// for each vertex position, evaluate colour
	for (i = 0; i< numPoints; i++)
	{
		MPoint pt = job.points[i];

		float t = ((float)pt[axisIndex] - lowAxis[axisIndex]) / (hiAxis[axisIndex] - lowAxis[axisIndex]);
		if (m_gradientParams.isReverse)
		{
			t = 1.0f - t;
		}
		t *= m_gradientParams.numRepeats;
		//t -= m_gradientParams.offset;
		if (m_gradientParams.repeatMode == REPEAT_CLAMP)
		{
			t = Max(Min(t, 1.0f), 0.0f);
		}
		if (m_gradientParams.repeatMode == REPEAT_WRAP)
		{
			t = fmodf(t, 1.0f);
		}
		if (m_gradientParams.repeatMode == REPEAT_MIRROR)
		{
			t = fabsf(fmodf(t+1.0f, 2.0f) - 1.0f);
		}

		if (rampColor)
		{
#ifndef MAYA60

			MColor color;
			rampColor->getColorAtPosition(t, color, &status);
			MColor alpha;
			rampAlpha->getColorAtPosition(t, alpha, &status);
			color.a = alpha.r;
			job.dstColors[i] = color;
#endif
		}
		else
		{
			ColRGB dst = m_gradientParams.colour1;
			dst.lerp(m_gradientParams.colour2, t);
			job.dstColors[i] = dst.get();
		}
	}

}

void
VertexPolyColourCommand::CreateSphericalGradientColours(VertexColourJob& job, MRampAttribute* rampColor, MRampAttribute* rampAlpha)
{
	MStatus status;

	unsigned int numVerts = job.points.length();

	// Get the center point
	MPoint startPoint;
	if (!m_gradientParams.isOverrideCenter)
	{
		// get center from this selection
		float3 lowAxis, hiAxis;
		lowAxis[0] = FLT_MAX;
		lowAxis[1] = FLT_MAX;
		lowAxis[2] = FLT_MAX;
		hiAxis[0] = -FLT_MAX;
		hiAxis[1] = -FLT_MAX;
		hiAxis[2] = -FLT_MAX;

		for (unsigned int i = 0; i < numVerts; i++)
		{
			MPoint pt = job.points[i];
			lowAxis[0] = Min(lowAxis[0], (float)pt.x);
			lowAxis[1] = Min(lowAxis[1], (float)pt.y);
			lowAxis[2] = Min(lowAxis[2], (float)pt.z);
			hiAxis[0] = Max(hiAxis[0], (float)pt.x);
			hiAxis[1] = Max(hiAxis[1], (float)pt.y);
			hiAxis[2] = Max(hiAxis[2], (float)pt.z);
		}

		startPoint.x = (lowAxis[0] + hiAxis[0]) * 0.5f;
		startPoint.y = (lowAxis[1] + hiAxis[1]) * 0.5f;
		startPoint.z = (lowAxis[2] + hiAxis[2]) * 0.5f;
		startPoint.w = 1.0;
	}
	else
	{
		startPoint.x = m_gradientParams.centerWorldPosition[0];
		startPoint.y = m_gradientParams.centerWorldPosition[1];
		startPoint.z = m_gradientParams.centerWorldPosition[2];
		startPoint.w = 1.0;
	}

	// find radius 
	float farthestDist = 0.0f;
	if (!m_gradientParams.isOverrideRadius)
	{
		for (unsigned int i = 0; i < numVerts; i++)
		{
			MPoint pt = job.points[i];
			farthestDist = max(farthestDist, (float)pt.distanceTo(startPoint));
		}
	}
	else
	{
		farthestDist = m_gradientParams.radius;
	}

	// for each vertex position, evaluate colour
	for (unsigned int i = 0; i < numVerts; i++)
	{
		MPoint pt = job.points[i];
		float d = (float)pt.distanceTo(startPoint);

		float t = d / farthestDist;
		if (m_gradientParams.isReverse)
		{
			t = 1.0f - t;
		}
		t *= m_gradientParams.numRepeats;
		t -= m_gradientParams.offset;
		if (m_gradientParams.repeatMode == REPEAT_CLAMP)
		{
			t = Max(Min(t, 1.0f), 0.0f);
		}
		if (m_gradientParams.repeatMode == REPEAT_WRAP)
		{
			t = fmodf(t, 1.0f);
		}
		if (m_gradientParams.repeatMode == REPEAT_MIRROR)
		{
			t = fabsf(fmodf(t+1.0f, 2.0f) - 1.0f);
		}

		if (rampColor)
		{
#ifndef MAYA60
			MColor color;
			rampColor->getColorAtPosition(t, color, &status);
			MColor alpha;
			rampAlpha->getColorAtPosition(t, alpha, &status);
			color.a = alpha.r;
			job.dstColors[i] = color;
#endif
		}
		else
		{
			ColRGB dst = m_gradientParams.colour1;
			dst.lerp(m_gradientParams.colour2, t);
			job.dstColors[i] = dst.get();
		}
	}
}

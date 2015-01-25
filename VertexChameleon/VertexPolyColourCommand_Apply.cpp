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
#include "Utility.h"
#include "Random.h"
#include "Blend.h"
#include "PolyColourNode.h"
#include "LayerData.h"
#include "MeshData.h"
#include "VertexColourJob.h"
#include "GradientParameters.h"
#include "VertexPolyColourCommand.h"

using namespace std;

void
VertexPolyColourCommand::ConvertFacetise()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		ConvertFacetise(job);
	}
}

void
VertexPolyColourCommand::ConvertFacetise(VertexColourJob& job)
{
	unsigned int numVerticeFaces = job.vfFaceList.length();

	if (job.primitiveStart.length() != numVerticeFaces)
		return;

	unsigned int startIndex = 0;
	unsigned int numSamples = 0;
	MColor avCol(0.0f, 0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < numVerticeFaces; i++)
	{
		int faceIndex = job.vfFaceList[i];
		int vertIndex = job.vfVertList[i];
		int newFace = job.primitiveStart[i];
		MColor newCol = job.dstColors[i];

		if (newFace)
		{
			if (numSamples > 0)
			{
				avCol /= (float)numSamples;
				for (unsigned int j = startIndex; j < i; j++)
				{
					job.dstColors[j] = avCol;
				}
			}
			
			startIndex = i;
			avCol = newCol;
			numSamples = 1;
		}
		else
		{
			avCol += newCol;
			numSamples++;
		}
	}

	// Handle the last face
	if (numSamples > 0)
	{
		avCol /= (float)numSamples;
		for (unsigned int j = startIndex; j < numVerticeFaces; j++)
		{
			job.dstColors[j] = avCol;
		}
	}
}

void
VertexPolyColourCommand::ConvertPositionsToColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		ConvertPositionsToColours(job);
	}
}


void
VertexPolyColourCommand::ConvertPositionsToColours(VertexColourJob& job)
{
	unsigned int numColours = job.dstColors.length();

	if (job.points.length() != numColours)
		return;

	// find min and max in axis
	float3 lowAxis, hiAxis;
	lowAxis[0] = FLT_MAX;
	lowAxis[1] = FLT_MAX;
	lowAxis[2] = FLT_MAX;
	hiAxis[0] = -FLT_MAX;
	hiAxis[1] = -FLT_MAX;
	hiAxis[2] = -FLT_MAX;

	unsigned int numPoints = job.points.length();
	unsigned int i = 0;
	for (i = 0; i < numPoints; i++)
	{
		const MPoint& pt = job.points[i];
		lowAxis[0] = Min(lowAxis[0], (float)pt.x);
		lowAxis[1] = Min(lowAxis[1], (float)pt.y);
		lowAxis[2] = Min(lowAxis[2], (float)pt.z);
		hiAxis[0] = Max(hiAxis[0], (float)pt.x);
		hiAxis[1] = Max(hiAxis[1], (float)pt.y);
		hiAxis[2] = Max(hiAxis[2], (float)pt.z);
	}

	for (unsigned int i = 0; i < numColours; i++)
	{	
		float x = ((float)job.points[i].x - lowAxis[0]) / (hiAxis[0] - lowAxis[0]);
		float y = ((float)job.points[i].y - lowAxis[1]) / (hiAxis[1] - lowAxis[1]);
		float z = ((float)job.points[i].z - lowAxis[2]) / (hiAxis[2] - lowAxis[2]);

		job.dstColors[i].r = x;
		job.dstColors[i].g = y;
		job.dstColors[i].b = z;
		job.dstColors[i].a = 1.0f;
	}
}

void
VertexPolyColourCommand::ConvertUVsToColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		ConvertUVsToColours(job);
	}
}


void
VertexPolyColourCommand::ConvertUVsToColours(VertexColourJob& job)
{
	unsigned int numColours = job.dstColors.length();

	if (job.uvs.length() != numColours)
		return;

	for (unsigned int i = 0; i < numColours; i++)
	{	
		job.dstColors[i].r = job.uvs[i].x;
		job.dstColors[i].g = job.uvs[i].y;
		job.dstColors[i].b = 0.0;
		job.dstColors[i].a = 1.0f;
	}
}

void
VertexPolyColourCommand::ConvertNormalsToColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		ConvertNormalsToColours(job);
	}
}

void
VertexPolyColourCommand::ConvertNormalsToColours(VertexColourJob& job)
{
	MSpace::Space space = MSpace::kObject;
	if (m_space == "World" || m_space == "world")
		space = MSpace::kWorld;
	if (job.m_componentMode == CM_Vertex)
	{
		int numVertices = job.vertexIndices.length();
		for (int i = 0; i < numVertices; i++)
		{
			int index = job.vertexIndices[i];

			MVector normal;
			job.meshData->mesh->getVertexNormal(index, normal, space);
			
			job.dstColors[i].r = 0.5f + (float)normal.x * 0.5f;
			job.dstColors[i].g = 0.5f + (float)normal.y * 0.5f;
			job.dstColors[i].b = 0.5f + (float)normal.z * 0.5f;
			job.dstColors[i].a = 1.0f;
		}
	}
	else
	{
		int numVerticeFaces = job.vfFaceList.length();
		for (int i = 0; i < numVerticeFaces; i++)
		{
			int faceIndex = job.vfFaceList[i];
			int vertIndex = job.vfVertList[i];

			MVector normal;
			job.meshData->mesh->getFaceVertexNormal(faceIndex, vertIndex, normal, space);
			/*MVector facenormal;
			job.meshData->mesh->getPolygonNormal(faceIndex, facenormal, space);

			float angle = (float)(normal.angle(facenormal)) / (3.141592654f * 0.5f);*/

			job.dstColors[i].r = 0.5f + (float)normal.x * 0.5f;
			job.dstColors[i].g = 0.5f + (float)normal.y * 0.5f;
			job.dstColors[i].b = 0.5f + (float)normal.z * 0.5f;
			job.dstColors[i].a = 1.0f;

			/*job.dstColors[i].r = angle;
			job.dstColors[i].g = angle;
			job.dstColors[i].b = angle;
			job.dstColors[i].a = 1.0f;*/
		}
	}
}

void
VertexPolyColourCommand::NormaliseRGB()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		unsigned int numColors = job.dstColors.length();
		for (unsigned int i = 0; i < numColors; i++)
		{
			ColRGB rgb;
			rgb.set(job.dstColors[i]);
			rgb.NormaliseRGB();
			job.dstColors[i] = rgb.get();
		}
	}
}

void
VertexPolyColourCommand::ChannelCopy()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		MFloatArray channelValues;
		ExtractColourChannel(job.srcColors, m_channelSrc, channelValues);
		ReplaceColourChannel(job.dstColors, m_channelDst, channelValues);
	}
}

void
VertexPolyColourCommand::ChannelSwap()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		MFloatArray srcValues, dstValues;
		ExtractColourChannel(job.dstColors, m_channelSrc, srcValues);
		ExtractColourChannel(job.dstColors, m_channelDst, dstValues);

		ReplaceColourChannel(job.dstColors, m_channelDst, srcValues);
		ReplaceColourChannel(job.dstColors, m_channelSrc, dstValues);
	}
}

void
VertexPolyColourCommand::DoQueries()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_isMinMaxRGBA)
		{
			FindMinRGBA(job.srcColors, m_minRGBA);
			FindMaxRGBA(job.srcColors, m_maxRGBA);
		}
		if (m_isAverageRGBA)
			FindAverageRGBA(job.srcColors, m_averageR, m_averageG, m_averageB, m_averageA, m_numTotalColours);
		if (m_isUniqueColours)
			FindUniqueColours(job.srcColors, m_uniqueColours);
	}

	if (m_isMinMaxRGBA)
	{
		appendToResult((double)m_minRGBA.r);
		appendToResult((double)m_minRGBA.g);
		appendToResult((double)m_minRGBA.b);
		appendToResult((double)m_minRGBA.a);
		appendToResult((double)m_maxRGBA.r);
		appendToResult((double)m_maxRGBA.g);
		appendToResult((double)m_maxRGBA.b);
		appendToResult((double)m_maxRGBA.a);
	}
	if (m_isAverageRGBA)
	{
		if (m_numTotalColours != 0)
		{
			m_averageR /= (double) m_numTotalColours;
			m_averageG /= (double) m_numTotalColours;
			m_averageB /= (double) m_numTotalColours;
			m_averageA /= (double) m_numTotalColours;
		}
		else
		{
			m_averageR = m_averageG = m_averageB = 0.0f;
			m_averageA = 1.0f;
		}
		appendToResult(m_averageR);
		appendToResult(m_averageG);
		appendToResult(m_averageB);
		appendToResult(m_averageA);
	}
	if (m_isUniqueColours)
	{
		appendToResult((int)m_uniqueColours.length());
	}
};

void
VertexPolyColourCommand::DoQuerySamplePointAverage()
{
	MPoint samplePoint(MDistance::uiToInternal(m_samplePoint[0]),
						MDistance::uiToInternal(m_samplePoint[1]),
						MDistance::uiToInternal(m_samplePoint[2]),
						1.0f);
	
	unsigned int numSamples = m_numSamples;
	size_t i, j, k, l;

	{
		// If we have less points to sample than the number of samples we need, 
		// decrease the number of samples
		unsigned int totalPoints = 0;
		size_t numJobs = m_jobs.size();
		for (i = 0; i < numJobs; i++)
		{
			VertexColourJob& job = *m_jobs[i];
			totalPoints += job.points.length();
		}
		numSamples = Min(numSamples, totalPoints);
	}

	// Initialise N results
	double* distances = new double[numSamples];
	ColRGB* colours = new ColRGB[numSamples];
	for (i = 0; i < numSamples; i++)
	{
		distances[i] = FLT_MAX;
		colours[i].setMin();
	}

	size_t numJobs = m_jobs.size();
	for (i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		size_t numPoints = job.points.length();
		for (j = 0; j < numPoints; j++)
		{
			const MPoint& p = job.points[(unsigned int)j];
			double distance = p.distanceTo(samplePoint);
			
			for (k = 0; k < numSamples; k++)
			{
				if (distance < distances[k])
				{
					// Shift results up
					for (l = (numSamples-1); l > k; l--)
					{
						distances[l] = distances[l-1];
						colours[l] = colours[l-1];
					}

					distances[k] = distance;
					colours[k].set(job.srcColors[(unsigned int)j]);
					break;
				}
			}
		}
	}

	//double maxDistance = -FLT_MAX;
	//double minDistance = FLT_MAX;
	double distanceTotal = 0.0f;
	for (k = 0; k < numSamples; k++)
	{
		//maxDistance = Max(distances[k], maxDistance);
		//minDistance = Min(distances[k], minDistance);
		//distances[k] = MDistance::internalToUI(distances[k]);
		distances[k] = 1.0 / distances[k];
		distanceTotal += distances[k];
	}

	ColRGB averageCol(0.0f, 0.0f, 0.0f, 0.0f);
	for (k = 0; k < numSamples; k++)
	{
		//double d = (distances[k] - minDistance) / (maxDistance - minDistance);
		float weight = (float)(distances[k] / distanceTotal);
		averageCol += colours[k] * weight;
	}

	delete [] distances;
	delete [] colours;

	appendToResult(averageCol.r);
	appendToResult(averageCol.g);
	appendToResult(averageCol.b);
	appendToResult(averageCol.a);
}

void
VertexPolyColourCommand::AdjustBrightnessContrast()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_contrastR != 0.0f || m_contrastG != 0.0f || m_contrastB != 0.0f || m_contrastA != 0.0f || m_brightness != 0.0f)
			DoContrastRGBA(job.dstColors, m_contrastR, m_contrastG, m_contrastB, m_contrastA, m_brightness);
		if (m_contrastH != 0.0f || m_contrastS != 0.0f || m_contrastV != 0.0f)
			DoContrastHSV(job.dstColors, m_contrastH, m_contrastS, m_contrastV);
	}
}

void
VertexPolyColourCommand::AdjustPosterize()
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;
	float h = 0.0f;
	float s = 0.0f;
	float v = 0.0f;
	if (m_posterizeMaskHSV&HUE)
		h = m_posterizeAmount;
	if (m_posterizeMaskHSV&SAT)
		s = m_posterizeAmount;
	if (m_posterizeMaskHSV&VAL)
		v = m_posterizeAmount;
	if (m_posterizeMaskRGBA&RED)
		r = m_posterizeAmount;
	if (m_posterizeMaskRGBA&GRN)
		g = m_posterizeAmount;
	if (m_posterizeMaskRGBA&BLU)
		b = m_posterizeAmount;
	if (m_posterizeMaskRGBA&ALP)
		a = m_posterizeAmount;

	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_posterizeMaskHSV != 0)
			DoQuantiseHSV(job.dstColors, h, s, v);
		if (m_posterizeMaskRGBA != 0)
			DoQuantiseRGBA(job.dstColors, r, g, b, a);
	}
}

void
VertexPolyColourCommand::AdjustClamp()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_maskClamp & RED)
			DoClampR(job.dstColors, m_clampR[0], m_clampR[1]);
		if (m_maskClamp & GRN)
			DoClampG(job.dstColors, m_clampG[0], m_clampG[1]);
		if (m_maskClamp & BLU)
			DoClampB(job.dstColors, m_clampB[0], m_clampB[1]);
		if (m_maskClamp & ALP)
			DoClampA(job.dstColors, m_clampA[0], m_clampA[1]);
		if (m_maskClamp & HUE || m_maskClamp & SAT || m_maskClamp & VAL)
			DoClampHSV(job.dstColors, m_clampH[0], m_clampH[1], m_clampS[0], m_clampS[1], m_clampV[0], m_clampV[1]);
	}
}

void
VertexPolyColourCommand::AdjustSaturate()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_maskClamp & RED)
			DoSaturateR(job.dstColors, m_clampR[0], m_clampR[1]);
		if (m_maskClamp & GRN)
			DoSaturateG(job.dstColors, m_clampG[0], m_clampG[1]);
		if (m_maskClamp & BLU)
			DoSaturateB(job.dstColors, m_clampB[0], m_clampB[1]);
		if (m_maskClamp & ALP)
			DoSaturateA(job.dstColors, m_clampA[0], m_clampA[1]);
		if (m_maskClamp & HUE || m_maskClamp & SAT || m_maskClamp & VAL)
			DoSaturateHSV(job.dstColors, m_clampH[0], m_clampH[1], m_clampS[0], m_clampS[1], m_clampV[0], m_clampV[1]);
	}
}

void
VertexPolyColourCommand::ApplyRandomComponents()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (job.components == MObject::kNullObj)
		{
			if (m_maskRGB!=0)
			{
				ColRGB randomRGB;
				randomRGB.r = gRand.RandomUnit();
				randomRGB.g = gRand.RandomUnit();
				randomRGB.b = gRand.RandomUnit();
				randomRGB.a = gRand.RandomUnit();
				BlendPartialRGBA(randomRGB, m_colRGB, job.dstColors);
			}
			if (m_maskHSV!=0)
			{
				ColHSV randomHSV;
				randomHSV.h = gRand.RandomUnit() * 360.0f;
				randomHSV.s = gRand.RandomUnit();
				randomHSV.v = gRand.RandomUnit();
				randomHSV.a = 0.0f;
				BlendPartialHSV(randomHSV, m_colHSV, job.dstColors);
			}
		}
		else
		{
			if (job.components.apiType() == MFn::kMeshPolygonComponent)
			{
				if (m_maskRGB!=0)
				{
					BlendPrimitiveRandomPartialRGBA(m_colRGB, job.dstColors, job.primitiveStart);
				}
				if (m_maskHSV!=0)
				{
					BlendPrimitiveRandomPartialHSV(m_colHSV, job.dstColors, job.primitiveStart);
				}
			}
			else
			{
				if (m_maskRGB!=0)
				{
					BlendRandomPartialRGBA(m_maskRGB, m_colRGB, job.dstColors);
				}
				if (m_maskHSV!=0)
				{
					BlendRandomPartialHSV(m_maskHSV, m_colHSV, job.dstColors);
				}
			}
		}
	}
}

void
VertexPolyColourCommand::AdjustRandomiseComponents()
{
	ColRGB zeroRGB;
	ColHSV zeroHSV;
	zeroRGB.setMin();
	zeroHSV.setMin();

	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (job.components == MObject::kNullObj)
		{
			if (m_maskRGB!=0)
			{
				ColRGB randomRGB;
				randomRGB.r = m_colRGB.r * gRand.RandomSignedUnit();
				randomRGB.g = m_colRGB.g * gRand.RandomSignedUnit();
				randomRGB.b = m_colRGB.b * gRand.RandomSignedUnit();
				randomRGB.a = m_colRGB.a * gRand.RandomSignedUnit();
				AddRGBA(randomRGB, job.dstColors);
			}
			if (m_maskHSV!=0)
			{
				ColHSV randomHSV;
				randomHSV.h = m_colHSV.h * gRand.RandomSignedUnit();
				randomHSV.s = m_colHSV.s * gRand.RandomSignedUnit();
				randomHSV.v = m_colHSV.v * gRand.RandomSignedUnit();
				randomHSV.a = 0.0f;
				AddHSVA(randomHSV, job.dstColors);
			}
		}
		else
		{
			if (job.components.apiType() == MFn::kMeshPolygonComponent)
			{
				if (m_maskRGB!=0)
				{
					AddPrimitiveRandomRGBA(m_colRGB, job.dstColors, job.primitiveStart);
				}
				if (m_maskHSV!=0)
				{
					AddPrimitiveRandomHSV(m_colHSV, job.dstColors, job.primitiveStart);
				}
			}
			else
			{
				if (m_maskRGB!=0)
					AddRandomRGBA(zeroRGB, m_colRGB, job.dstColors);
				if (m_maskHSV!=0)
					AddRandomHSVA(zeroHSV, m_colHSV, job.dstColors);
			}
		}
	}
}

void
VertexPolyColourCommand::AdjustLevels()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		DoLevels(job.dstColors, m_levels);
	}
}

void
VertexPolyColourCommand::AdjustRGBAColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		AddRGBA(m_colRGB, job.dstColors);
	}
}

void
VertexPolyColourCommand::AdjustHSVAColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		AddHSVA(m_colHSV, job.dstColors);
	}
}

void
VertexPolyColourCommand::ReplaceRGBAColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_maskRGB == RGBA)
			ReplaceRGBA(m_colRGB, job.dstColors);
		else
			ReplacePartialRGBA(m_colRGB, m_maskRGB, job.dstColors);
	}
}

void
VertexPolyColourCommand::ReplaceHSVAColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (m_maskHSV == HSVA)
			ReplaceHSVA(m_colHSV, job.dstColors);
		else
			ReplacePartialHSVA(m_colHSV, m_maskHSV, job.dstColors);
	}
}

void
VertexPolyColourCommand::ReplaceColoursFromArray()
{
	unsigned int totalColours = 0;
	size_t numJobs = m_jobs.size();
	size_t i = 0;
	
	for (i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];
		totalColours += job.dstColors.length();
	}

	if (m_colorArray.length() != totalColours * 4)
	{
		displayWarning("Number of colours didn't match up");
		return;
	}

	unsigned int colourIndex = 0;
	for (i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		unsigned int num=job.dstColors.length();
		for (unsigned int j=0; j<num; j++)
		{
			job.dstColors[j].r = (float)m_colorArray[colourIndex + 0];
			job.dstColors[j].g = (float)m_colorArray[colourIndex + 1];
			job.dstColors[j].b = (float)m_colorArray[colourIndex + 2];
			job.dstColors[j].a = (float)m_colorArray[colourIndex + 3];
			
			colourIndex += 4;
		}
	}
}

void
VertexPolyColourCommand::BlendColours()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		::BlendColours(m_blendMode, job.srcColors, job.dstColors, m_blendAmount);
	}
}

void
VertexPolyColourCommand::DoMasterBlend()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		BlendReplace(job.srcColors, job.dstColors, m_masterBlendAmount);
	}
}

void
VertexPolyColourCommand::ApplyChannelMask()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		ApplyChannelMask(job);
	}
}

void
VertexPolyColourCommand::ApplyChannelMask(VertexColourJob& job)
{
	assert(job.srcColors.length() == job.dstColors.length());

	unsigned int num = job.srcColors.length();
	for (unsigned int i = 0; i < num; i++)
	{
		if (0 == (m_channelMaskRGBA&RED))
			job.dstColors[i].r = job.srcColors[i].r;
		if (0 == (m_channelMaskRGBA&GRN))
			job.dstColors[i].g = job.srcColors[i].g;
		if (0 == (m_channelMaskRGBA&BLU))
			job.dstColors[i].b = job.srcColors[i].b;
		if (0 == (m_channelMaskRGBA&ALP))
			job.dstColors[i].a = job.srcColors[i].a;
	}
}

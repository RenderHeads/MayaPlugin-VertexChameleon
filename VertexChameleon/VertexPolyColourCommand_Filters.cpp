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
#include <float.h>
#include "MeshData.h"
#include "VertexPolyColourCommand.h"

void
VertexPolyColourCommand::DoFilters()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		switch (m_operation)
		{
		case OP_FilterBlur:
			DoFilterBlur(job);
			break;
		case OP_FilterSharpen:
			DoFilterSharpen(job);
			break;
		case OP_FilterErode:
			DoFilterErode(job);
			break;
		case OP_FilterDilate:
			DoFilterDilate(job);
			break;
		case OP_FilterCurvature:
			DoCurvature(job);
			break;
		case OP_FilterInvert:
			if (m_invertMaskRGBA&RED)
				DoInvertR(job.dstColors);
			if (m_invertMaskRGBA&GRN)
				DoInvertG(job.dstColors);
			if (m_invertMaskRGBA&BLU)
				DoInvertB(job.dstColors);
			if (m_invertMaskRGBA&ALP)
				DoInvertA(job.dstColors);
			if (m_invertMaskHSV&HUE)
				DoInvertH(job.dstColors);
			if (m_invertMaskHSV&SAT)
				DoInvertS(job.dstColors);
			if (m_invertMaskHSV&VAL)
				DoInvertV(job.dstColors);
			break;
		case OP_Sample3DTexture:
			DoSample3DTexture(job);
			break;
		}
	}
}

void
VertexPolyColourCommand::DoCurvature(VertexColourJob& job)
{
	MStatus status;

	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	MItMeshVertex vertexIter2(job.meshData->dagPath, job.components);

	// for each vertex, look at connected vertices
	// average them
	// dotproduct from current
	
	double minDist = DBL_MAX;
	double maxDist = -DBL_MAX;
	{
		int numVerts = vertexIter.count();
		int prevIndex = 0;
		for (int i = 0; i < numVerts; i++) 
		{
			int idx = job.vertexIndices[i];
			vertexIter.setIndex(idx, prevIndex);
			MVector normal;
			vertexIter.getNormal(normal, MSpace::kObject);
			normal.normalize();

			double totalA = 0.0;
			MVector total = MVector::zero;
			MIntArray connectedVerts;
			vertexIter.getConnectedVertices(connectedVerts);
			unsigned numSamples = connectedVerts.length();
			for (unsigned int j=0; j<numSamples; j++)
			{
				vertexIter2.setIndex(connectedVerts[j], prevIndex);
				MVector normal2;
				vertexIter2.getNormal(normal2, MSpace::kObject);
				total += normal2;

				normal2.normalize();

				double a = (normal.x * normal2.x + normal.y * normal2.y + normal.z  * normal2.z);
				//double a = normal2.angle(normal);
				totalA += a;

			}

			totalA /= (double)numSamples;

			if (totalA < minDist)
				minDist = totalA;
			if (totalA > maxDist)
				maxDist = totalA;
			/*
			// Get average of neighbour normals
			total /= (double)numSamples;

			total.normalize();
			// compare vertex normal with average of neighbours
			double a = total.angle(normal);

			if (a < minDist)
				minDist = a;
			if (a > maxDist)
				maxDist = a;*/
		}
	}

	{
		
		int numVerts = vertexIter.count();
		int prevIndex = 0;
		for (int i = 0; i < numVerts; i++) 
		{
			int idx = job.vertexIndices[i];
			vertexIter.setIndex(idx, prevIndex);
			MVector normal;
			vertexIter.getNormal(normal, MSpace::kObject);
			normal.normalize();

			double totalA = 0.0;
			MVector total = MVector::zero;
			MIntArray connectedVerts;
			vertexIter.getConnectedVertices(connectedVerts);
			unsigned numSamples = connectedVerts.length();
			for (unsigned int j=0; j<numSamples; j++)
			{
				vertexIter2.setIndex(connectedVerts[j], prevIndex);
				MVector normal2;
				vertexIter2.getNormal(normal2, MSpace::kObject);
				total += normal2;

				normal2.normalize();
				//double a = normal2.angle(normal);
				double a = (normal.x * normal2.x + normal.y * normal2.y + normal.z  * normal2.z);
				totalA += a;
			}

			totalA /= (double)numSamples;

			/*
			total /= (double)numSamples;

			total.normalize();
			double a = total.angle(normal);*/

			float avgDist = (float)((totalA - minDist) / (maxDist-minDist));
			job.dstColors[i].r = avgDist;
			job.dstColors[i].g = avgDist;
			job.dstColors[i].b = avgDist;
		}
	}
}

void
VertexPolyColourCommand::DoDensity(VertexColourJob& job)
{
	MStatus status;

	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	MItMeshVertex vertexIter2(job.meshData->dagPath, job.components);

	/*
	// Find min and max distances between points
	double minDist = DBL_MAX;
	double maxDist = -DBL_MAX;
	{
		int numVerts = vertexIter.count();
		int prevIndex = 0;
		for (int i = 0; i < numVerts; i++) 
		{
			vertexIter.setIndex(i, prevIndex);
			MPoint p1 = vertexIter.position(MSpace::kWorld);
			for (int j = i+1; j < numVerts; j++) 
			{
				vertexIter2.setIndex(j, prevIndex);
				MPoint p2 = vertexIter2.position(MSpace::kWorld);

				double d = p1.distanceTo(p2);
				if (d < minDist)
					minDist = d;
				if (d > maxDist)
					maxDist = d;
			}
		}
	}*/
	
	double minDist = DBL_MAX;
	double maxDist = -DBL_MAX;
	{
		int numVerts = vertexIter.count();
		int prevIndex = 0;
		for (int i = 0; i < numVerts; i++) 
		{
			int idx = job.vertexIndices[i];
			vertexIter.setIndex(idx, prevIndex);
			MPoint p1 = vertexIter.position(MSpace::kWorld);

			double totalDist = 0.0;
			MIntArray connectedVerts;
			vertexIter.getConnectedVertices(connectedVerts);
			unsigned numSamples = connectedVerts.length();
			for (unsigned int j=0; j<numSamples; j++)
			{
				vertexIter2.setIndex(connectedVerts[j], prevIndex);
				MPoint p2 = vertexIter2.position(MSpace::kWorld);
				totalDist += p1.distanceTo(p2);
			}

			totalDist /= (double)numSamples;

			if (totalDist < minDist)
				minDist = totalDist;
			if (totalDist > maxDist)
				maxDist = totalDist;
		}
	}

	{
		
		
		int numVerts = vertexIter.count();
		int prevIndex = 0;
		for (int i = 0; i < numVerts; i++)
		{
			int idx = job.vertexIndices[i];
			vertexIter.setIndex(idx, prevIndex);
			double totalDist = 0.0;
			MPoint p1 = vertexIter.position(MSpace::kWorld);

			MIntArray connectedVerts;
			vertexIter.getConnectedVertices(connectedVerts);
			unsigned numSamples = connectedVerts.length();
			for (unsigned int j=0; j<numSamples; j++)
			{
				vertexIter2.setIndex(connectedVerts[j], prevIndex);
				MPoint p2 = vertexIter2.position(MSpace::kWorld);
				double ld = p1.distanceTo(p2);
				totalDist += ld;
			}

			/*
			for (int j = 0; j < numVerts; j++) 
			{
				if (i != j)
				{
					vertexIter2.setIndex(j, prevIndex);
					MPoint p2 = vertexIter2.position(MSpace::kWorld);

					double ld = (p1.distanceTo(p2) - minDist)/maxDist;
					ld = 1.0 - ld;

					totalDist += ld * ld;

					double d = p1.distanceTo(p2);
					if (d > maxD)
						maxD = d;
				}
			}*/

			// min and max can be scaled 

			double ad = (float)(totalDist / (double)numSamples);
			float avgDist = (float)((ad - minDist) / (maxDist-minDist));
			//float avgDist = (float)(totalDist / (double)numSamples);
			job.dstColors[i].r = avgDist;
			job.dstColors[i].g = avgDist;
			job.dstColors[i].b = avgDist;
		}
	}
}


void
VertexPolyColourCommand::DoSample3DTexture(VertexColourJob& job)
{
	MStatus status;

	unsigned int numPoints = job.points.length();

	MFloatPointArray points;
	MFloatVectorArray resultColors, resultAlphas;
	points.setLength(numPoints);

	unsigned int i;
	for (i = 0; i < numPoints; i++)
	{
		MPoint p = job.points[i];
		MFloatPoint fp((float)p.x * m_noiseScale[0], (float)p.y * m_noiseScale[1], (float)p.z *m_noiseScale[2]);
		fp.x += m_3DTextureOffset[0];
		fp.y += m_3DTextureOffset[1];
		fp.z += m_3DTextureOffset[2];
		points[i] = fp;
	}
	
	MFloatMatrix cameraMat;
	status = MRenderUtil::sampleShadingNetwork(m_3DTextureName + ".outColor", points.length(), false, false, cameraMat, &points, NULL, NULL, NULL, &points, NULL, NULL, NULL, resultColors, resultAlphas);

	unsigned int numColors = resultColors.length();
	for (i = 0; i < numColors; i++)
	{
		job.dstColors[i].r = resultColors[i].x;
		job.dstColors[i].g = resultColors[i].y;
		job.dstColors[i].b = resultColors[i].z;
	}
}

/*


void
VertexPolyColourCommand::DoFilterBlur(VertexColourJob& job)
{
	MStatus status;

	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	MItMeshVertex vertexIter2(job.meshData->dagPath);

	int idx = 0;
	for ( ; !vertexIter.isDone(); vertexIter.next() ) 
	{
		ColRGB col,totalCol,endCol;
		MColor mcol;
#if MAYA_API_VERSION >= 700
		vertexIter.getColor(mcol, &job.meshData->readColorSetName);
#else
		vertexIter.getColor(mcol);
#endif
		col.set(mcol);
		totalCol=col;
		MIntArray connectedVerts;
		vertexIter.getConnectedVertices(connectedVerts);
		unsigned numSamples = connectedVerts.length();
		for (unsigned int i=0; i<numSamples; i++)
		{
			int prevIndex;
			vertexIter2.setIndex(connectedVerts[i],prevIndex);
			MColor newmcol;
#if MAYA_API_VERSION >= 700
			vertexIter2.getColor(newmcol, &job.meshData->readColorSetName);
#else
			vertexIter2.getColor(newmcol);
#endif
			totalCol+=newmcol;
		}

		endCol=totalCol/((float)numSamples+1.0f);

		job.dstColors[idx] = endCol.get();
		idx++;
	}
}
*/

/*
void
IsVertexBlah(MItMeshVertex& vertIter, const MDagPath& dagPath)
{
	bool result = false;

	MIntArray faceList;
	vertIter.getConnectedFaces(faceList);

	MItMeshFaceVertex fvIter(dagPath);
	for (int i=0; i< faceList.length(); i++)
	{
		int faceIndex = faceList[i];
		int localVertexIndex = 0;
		int prevFaceId = 0;
		int prevVertId = 0;

		fvIter.setIndex(faceIndex, localVertexIndex, prevFaceId, prevVertId);
	}

	MColor color;
	fvIter.getColor(color, job.meshData->readColorSetName);

	return result;
}
*/

void
VertexPolyColourCommand::DoFilterBlur(VertexColourJob& job)
{
	MStatus status;

	MItMeshFaceVertex vfIter(job.meshData->dagPath, job.components);
	MItMeshVertex vertexIter(job.meshData->dagPath);

	int idx = 0;
	for ( ; !vfIter.isDone(); vfIter.next() ) 
	{
		MColor a;
#if MAYA_API_VERSION >= 700
		vfIter.getColor(a, &job.meshData->readColorSetName);
#else
		vfIter.getColor(a);
#endif

		int vertexIndex = vfIter.vertId();
		int prevVertexIndex = 0;
		vertexIter.setIndex(vertexIndex, prevVertexIndex);

		bool vertexFace = false;
		float threshold = 0.1f;
		MColorArray faceColors;
#if MAYA_API_VERSION >= 700
		vertexIter.getColors(faceColors, &job.meshData->readColorSetName);
#else
		vertexIter.getColors(faceColors);
#endif
		for (unsigned int i=0; i<faceColors.length(); i++)
		{
			MColor b = faceColors[i];

			if ((float)fabs((float)(b.r - a.r)) > threshold ||
				fabs(b.g - a.g) > threshold ||
				fabs(b.b - a.b) > threshold ||
				fabs(b.a - a.a) > threshold)
			{
				vertexFace = true;
				break;
			}
		}

		unsigned int numSamples = 1;
		ColRGB totalCol(a);
		ColRGB endCol;

		if (!vertexFace)
		{
			MIntArray connectedVerts;
			vertexIter.getConnectedVertices(connectedVerts);
			for (unsigned int i=0; i<connectedVerts.length(); i++)
			{
				int prevIndex;
				vertexIter.setIndex(connectedVerts[i], prevIndex);
#if MAYA_API_VERSION >= 700
				vertexIter.getColor(a, &job.meshData->readColorSetName);
#else
				vertexIter.getColor(a);
#endif

				bool vertexFace = false;
				float threshold = 0.1f;
				MColorArray faceColors;
#if MAYA_API_VERSION >= 700
				vertexIter.getColors(faceColors, &job.meshData->readColorSetName);
#else
				vertexIter.getColors(faceColors);
#endif
				for (unsigned int i=0; i<faceColors.length(); i++)
				{
					MColor b = faceColors[i];

					if (fabs(b.r - a.r) > threshold ||
						fabs(b.g - a.g) > threshold ||
						fabs(b.b - a.b) > threshold ||
						fabs(b.a - a.a) > threshold)
					{
						vertexFace = true;
						break;
					}
				}

				if (!vertexFace)
				{
					totalCol+=a;
					numSamples++;
				}
			}
		}
		/*else
		{
 			MItMeshPolygon faceIter(job.meshData->dagPath);
			int prevFaceIndex = 0;
			int faceIndex = vfIter.faceId();
			faceIter.setIndex(faceIndex, prevFaceIndex);

			unsigned int numFaceVertices = faceIter.polygonVertexCount();
			MItMeshFaceVertex vfIter2(job.meshData->dagPath);
			for (unsigned int i=0; i<numFaceVertices; i++)
			{
				int localVertexIndex = i;
				int prevFaceId = 0;
				int prevVertId = 0;
				vfIter2.setIndex(faceIndex, localVertexIndex, prevFaceId, prevVertId);

				MColor a;
#if MAYA_API_VERSION >= 700
				vfIter2.getColor(a, &job.meshData->readColorSetName);
#else
				vfIter2.getColor(a);
#endif

				totalCol+=a;
				numSamples++;
			}
		}*/

		endCol=totalCol/((float)numSamples);
		job.dstColors[idx] = endCol.get();
		idx++;
	}
}

void
VertexPolyColourCommand::DoFilterSharpen(VertexColourJob& job)
{
	MStatus status;

	//MColorArray cols;
	//MIntArray verts;

	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	MItMeshVertex vertexIter2(job.meshData->dagPath);
	int idx = 0;
	for ( ; !vertexIter.isDone(); vertexIter.next() ) 
	{
		ColRGB col,totalCol,endCol;
		MColor mcol;
#if MAYA_API_VERSION >= 700
		vertexIter.getColor(mcol, &job.meshData->readColorSetName);
#else
		vertexIter.getColor(mcol);
#endif
		col.set(mcol);
		totalCol.setMin();
		MIntArray connectedVerts;
		vertexIter.getConnectedVertices(connectedVerts);
		unsigned numSamples = connectedVerts.length();
		for (unsigned int i=0; i<numSamples; i++)
		{
			int prevIndex;
			vertexIter2.setIndex(connectedVerts[i],prevIndex);
			MColor newmcol;
#if MAYA_API_VERSION >= 700
			vertexIter2.getColor(newmcol, &job.meshData->readColorSetName);
#else
			vertexIter2.getColor(newmcol);
#endif
			totalCol+=newmcol;
		}

		endCol = (col + col) - ((totalCol) / ((float)numSamples));
		endCol.selfClamp();
		//float t=m_filterStrength;
		//endCol=col*(1.0f-t)+endCol*t;

		//cols.append(endCol.get());
		//verts.append(vertexIter.index());

		job.dstColors[idx] = endCol.get();
		idx++;
	}

	/*MDGModifier* modifier = NULL;
	if (m_isUndoable)
	{
		modifier = new MDGModifier;
		m_undos.push_back(modifier);
	}
	mesh.setVertexColors(cols, verts, modifier);*/
}

void
VertexPolyColourCommand::DoFilterErode(VertexColourJob& job)
{
	MStatus status;

	//MColorArray cols;
	//MIntArray verts;

	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	MItMeshVertex vertexIter2(job.meshData->dagPath);
	int idx = 0;
	for ( ; !vertexIter.isDone(); vertexIter.next() ) 
	{
		ColRGB col,endCol;
		MColor mcol;
#if MAYA_API_VERSION >= 700
		vertexIter.getColor(mcol, &job.meshData->readColorSetName);
#else
		vertexIter.getColor(mcol);
#endif
		col.set(mcol);
		endCol=col;
		MIntArray connectedVerts;
		vertexIter.getConnectedVertices(connectedVerts);
		unsigned numSamples = connectedVerts.length();
		for (unsigned int i=0; i<numSamples; i++)
		{
			int prevIndex;
			vertexIter2.setIndex(connectedVerts[i],prevIndex);
			MColor newmcol;
#if MAYA_API_VERSION >= 700
			vertexIter2.getColor(newmcol, &job.meshData->readColorSetName);
#else
			vertexIter2.getColor(newmcol);
#endif
			endCol.getMinRGBA(newmcol);
		}

		//float t=m_filterStrength;
		//endCol=col*(1.0f-t)+endCol*t;

		//cols.append(endCol.get());
		//verts.append(vertexIter.index());
		job.dstColors[idx] = endCol.get();
		idx++;
	}

	/*MDGModifier* modifier = NULL;
	if (m_isUndoable)
	{
		modifier = new MDGModifier;
		m_undos.push_back(modifier);
	}
	mesh.setVertexColors(cols, verts, modifier);*/
}

void
VertexPolyColourCommand::DoFilterDilate(VertexColourJob& job)
{
	MStatus status;

	//MColorArray cols;
	//MIntArray verts;

	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	MItMeshVertex vertexIter2(job.meshData->dagPath);
	int idx = 0;
	for ( ; !vertexIter.isDone(); vertexIter.next() ) 
	{
		ColRGB col,endCol;
		MColor mcol;
#if MAYA_API_VERSION >= 700
		vertexIter.getColor(mcol, &job.meshData->readColorSetName);
#else
		vertexIter.getColor(mcol);
#endif
		col.set(mcol);
		endCol=col;
		MIntArray connectedVerts;
		vertexIter.getConnectedVertices(connectedVerts);
		unsigned numSamples = connectedVerts.length();
		for (unsigned int i=0; i<numSamples; i++)
		{
			int prevIndex;
			vertexIter2.setIndex(connectedVerts[i],prevIndex);
			MColor newmcol;
#if MAYA_API_VERSION >= 700
			vertexIter2.getColor(newmcol, &job.meshData->readColorSetName);
#else
			vertexIter2.getColor(newmcol);
#endif
			endCol.getMaxRGBA(newmcol);
		}

		//float t=m_filterStrength;
		//endCol=col*(1.0f-t)+endCol*t;

		//cols.append(endCol.get());
		//verts.append(vertexIter.index());
		job.dstColors[idx] = endCol.get();
		idx++;
	}

	/*MDGModifier* modifier = NULL;
	if (m_isUndoable)
	{
		modifier = new MDGModifier;
		m_undos.push_back(modifier);
	}
	mesh.setVertexColors(cols, verts, modifier);*/
}

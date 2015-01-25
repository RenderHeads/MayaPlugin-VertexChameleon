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
#include "MeshData.h"
#include "VertexPolyColourCommand.h"

using namespace std;

void
VertexPolyColourCommand::GetColoursFromEdges(VertexColourJob& job)
{
	switch (m_forcedComponentMode)
	{
	case CM_Vertex:
		GetEdgeColoursFromVertices(job);
		job.m_componentMode = CM_Vertex;
		break;
	case CM_VertexFace:
	case CM_None:
	default:
		GetEdgeColoursFromVertexFaces(job);
		job.m_componentMode = CM_VertexFace;
		break;
	}
}

void
VertexPolyColourCommand::GetEdgeColoursFromVertexFaces(VertexColourJob& job)
{
	// count number of vertex-faces
	int totalVF = 0;
	{
		MItMeshEdge edgeIter(job.meshData->dagPath, job.components);
		for ( ; !edgeIter.isDone(); edgeIter.next() ) 
		{
			int numFaces=0; 
			edgeIter.numConnectedFaces(numFaces);
			totalVF += numFaces * 2;
		}
	}

	// allocate arrays
	job.vertexIndices.setLength(totalVF);
	job.vfVertList.setLength(totalVF);
	job.vfFaceList.setLength(totalVF);
	job.srcColors.setLength(totalVF);

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;
#endif

	// gather 
	MIntArray faces;
	int idx=0;
	int prevFace = 0;
	int prevFaceId = 0;
	int prevVertId = 0;
	int prevIndex = 0;
	MItMeshVertex vertexIter(job.meshData->dagPath);
	MItMeshEdge edgeIter(job.meshData->dagPath, job.components);
	for ( ; !edgeIter.isDone(); edgeIter.next() ) 
	{
		int2 edgeVerts;
		job.meshData->mesh->getEdgeVertices(edgeIter.index(), edgeVerts);
		int numFaces = edgeIter.getConnectedFaces(faces);
		for (int j =0; j < 2; j++)
		{
			vertexIter.setIndex(edgeVerts[j], prevIndex);
			for (int i = 0 ; i < numFaces; i++)
			{
				// get geometry indices
				job.vertexIndices[idx] = edgeVerts[j];
				job.vfVertList[idx] = edgeVerts[j];
				job.vfFaceList[idx] = faces[i];
#if MAYA_API_VERSION >= 700
				vertexIter.getColor(job.srcColors[idx], faces[i], readColorSet);
#else
				vertexIter.getColor(job.srcColors[idx], faces[i]);
#endif

				idx++;
			}
		}
	}
}

// We know this function works, but it is very slow
// It's just left here incase we find a bug with the new faster version above
void
VertexPolyColourCommand::GetEdgeColoursFromVertexFacesSLOW(VertexColourJob& job)
{
	MColorArray	srcFaceVertColors;

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;

	job.meshData->mesh->getFaceVertexColors(srcFaceVertColors, readColorSet);
#else	
	job.meshData->mesh->getFaceVertexColors(srcFaceVertColors);
#endif

	MItMeshFaceVertex faceVertIter(job.meshData->dagPath.node());
	int prevFace,prevVert;
	int idx=0;

	MItMeshEdge edgeIter(job.meshData->dagPath, job.components);
	for ( ; !edgeIter.isDone(); edgeIter.next() ) 
	{
		// get edge vertex indices
		int edgeID = edgeIter.index();
		int2 edgeVerts;
		job.meshData->mesh->getEdgeVertices(edgeID, edgeVerts);

		// get the indices for the faces that contain both edge vertices
		MIntArray faces;
		int numFaces=edgeIter.getConnectedFaces(faces);
#ifdef _DEBUG
		assert(numFaces==1 || numFaces==2);
#endif

		// get the vertex-face
		for (int i = 0; i < numFaces; i++)
		{
			int numFaceVerts = job.meshData->mesh->polygonVertexCount(faces[i]);

			for (int j = 0; j < numFaceVerts; j++)
			{
				faceVertIter.setIndex(faces[i], j, prevFace, prevVert);
				int vert=faceVertIter.vertId();

				// if this vertex-face is one of the edge vertices
				if (vert == edgeVerts[0] || vert == edgeVerts[1])
				{
					int face = faces[i];
					int vertidx = j;

					job.vertexIndices.append(idx);
					job.vfVertList.append(vert);
					job.vfFaceList.append(face);

					int colorIndex;
					job.meshData->mesh->getFaceVertexColorIndex(face, vertidx, colorIndex);
					job.srcColors.append(srcFaceVertColors[colorIndex]);
					idx++;
				}
			}
		}
	}
}


void
VertexPolyColourCommand::GetEdgeColoursFromVertices(VertexColourJob& job)
{
	// Get geometry indices
	MItMeshEdge edgeIter(job.meshData->dagPath, job.components);
	int totalVerts = edgeIter.count() * 2;
	job.vertexIndices.setLength(totalVerts);
	int idx = 0;
	for ( ; !edgeIter.isDone(); edgeIter.next() ) 
	{
		int edgeID = edgeIter.index();
		int2 edgeVerts;
		job.meshData->mesh->getEdgeVertices(edgeID, edgeVerts);

		job.vertexIndices[idx] = edgeVerts[0];
		job.vertexIndices[idx+1] = edgeVerts[1];
		idx += 2;
	}

	// NOTE: edges will add 4x the number of vertices
	// TODO: perhaps convert edges to something else first

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;
#endif

	// Get colours
	MItMeshVertex vertexIter(job.meshData->dagPath);
	int prevIndex = 0;
	job.srcColors.setLength(totalVerts);
	for (int i = 0; i < totalVerts; i++)
	{
		vertexIter.setIndex(job.vertexIndices[i], prevIndex);
#if MAYA_API_VERSION >= 700
		vertexIter.getColor(job.srcColors[i], readColorSet);
#else
		vertexIter.getColor(job.srcColors[i]);
#endif
	}
}

void
VertexPolyColourCommand::GetColoursFromFaces(VertexColourJob& job)
{
	switch (m_forcedComponentMode)
	{
	case CM_Vertex:
		GetFaceColoursFromVertices(job);
		job.m_componentMode = CM_Vertex;
		break;
	case CM_VertexFace:
	case CM_None:
	default:
		GetFaceColoursFromVertexFaces(job);
		job.m_componentMode = CM_VertexFace;
		break;
	}
}

void
VertexPolyColourCommand::GetFaceColoursFromVertices(VertexColourJob& job)
{
	int totalVerts = 0;
	MItMeshPolygon polyIter(job.meshData->dagPath, job.components);
	for ( ; !polyIter.isDone(); polyIter.next() ) 
	{
		totalVerts += polyIter.polygonVertexCount();
	}
	job.vertexIndices.setLength(totalVerts);
	job.srcColors.setLength(totalVerts);
	job.primitiveStart.setLength(totalVerts);

	// Get geometry indices
	int idx = 0;
	int i = 0;
	polyIter.reset();
	for ( ; !polyIter.isDone(); polyIter.next() ) 
	{
		int numVerts = polyIter.polygonVertexCount();

		job.primitiveStart[idx] = 1;
		for (i = 1; i < numVerts; i++)
		{
			job.primitiveStart[idx+i] = 0;
		}

		for (i = 0; i < numVerts; i++)
		{
			job.vertexIndices[idx] = polyIter.vertexIndex(i);
			//polyIter.getColor(job.srcColors[idx], i);
			idx++;
		}
	}

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;
#endif

	// Get colours
	MItMeshVertex vertexIter(job.meshData->dagPath);
	int prevIndex = 0;
	for (i = 0; i < totalVerts; i++)
	{
		vertexIter.setIndex(job.vertexIndices[i], prevIndex);
#if MAYA_API_VERSION >= 700
		vertexIter.getColor(job.srcColors[i], readColorSet);
#else
		vertexIter.getColor(job.srcColors[i]);
#endif
	}
}

void
VertexPolyColourCommand::GetFaceColoursFromVertexFaces(VertexColourJob& job)
{
	MItMeshPolygon polyIter(job.meshData->dagPath, job.components);

	// count vertex faces
	int totalVF = 0;
	{
		polyIter.reset();
		for ( ; !polyIter.isDone(); polyIter.next() ) 
		{
			totalVF += polyIter.polygonVertexCount();
		}
	}

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;
#endif

	// allocate arrays
	job.vertexIndices.setLength(totalVF);
	job.vfVertList.setLength(totalVF);
	job.vfFaceList.setLength(totalVF);
	job.srcColors.setLength(totalVF);
	job.primitiveStart.setLength(totalVF);

	// gather
	int idx=0;
	polyIter.reset();
	for ( ; !polyIter.isDone(); polyIter.next() ) 
	{
		int face = polyIter.index();
		unsigned int vertCount = polyIter.polygonVertexCount();
		unsigned int j = 0;

		job.primitiveStart[idx] = 1;
		for (j = 1; j < vertCount; j++)
		{
			job.primitiveStart[idx+j] = 0;
		}

#if MAYA_API_VERSION >= 700
		MColorArray faceColors;
		polyIter.getColors(faceColors, readColorSet);
#endif
		for (j = 0; j < vertCount; j++)
		{
			int vertidx=j;

			job.vertexIndices[idx] = idx;
			job.vfVertList[idx] = polyIter.vertexIndex(j);
			job.vfFaceList[idx] = face;
			// WARNING: this function doesn't let you specify the colorset
			// need to tell Autodesk about this and find a workaround
#if MAYA_API_VERSION >= 700
			job.srcColors[idx] = faceColors[j];
#else
			polyIter.getColor(job.srcColors[idx], j);
#endif

			idx++;
		}
	}
}

void
VertexPolyColourCommand::GetColoursFromVertexFaces(VertexColourJob& job)
{
	switch (m_forcedComponentMode)
	{
	case CM_VertexFace:
	case CM_Vertex:
	case CM_None:
	default:
		GetVertexFaceColoursFromVertexFaces(job);
		job.m_componentMode = CM_VertexFace;
		break;
	}
}

void
VertexPolyColourCommand::GetVertexFaceColoursFromVertexFaces(VertexColourJob& job)
{
	MStatus status;

	MColorArray	srcFaceVertColors;
	
#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;

	job.meshData->mesh->getFaceVertexColors(srcFaceVertColors, readColorSet);
#else	
	job.meshData->mesh->getFaceVertexColors(srcFaceVertColors);
#endif

	// count number of vertex faces
	int totalVF = 0;
	MFnDoubleIndexedComponent comp(job.components);
	totalVF = comp.elementCount();

	// allocate arrays
	job.vertexIndices.setLength(totalVF);
	job.vfVertList.setLength(totalVF);
	job.vfFaceList.setLength(totalVF);
	job.srcColors.setLength(totalVF);

	int idx=0;
	//MItMeshFaceVertex facevertIter(job.dagPath, job.components);
	//facevertIter.reset();
	MItMeshFaceVertex facevertIter(job.meshData->dagPath, job.components);
	for ( ; !facevertIter.isDone(); facevertIter.next() ) 
	{
		int face=facevertIter.faceId();
		int vert=facevertIter.vertId();
		int vertidx=facevertIter.faceVertId();
		
		// Get geometry indices
		job.vertexIndices[idx] = idx;
		job.vfVertList[idx] = vert;
		job.vfFaceList[idx] = face;

		//facevertIter.getColor(job.srcColors[idx]);    // For some reason this is slower than the code below..weird

		// Get colours
		int idx2;
		job.meshData->mesh->getFaceVertexColorIndex(face, vertidx, idx2);
		job.srcColors[idx] = srcFaceVertColors[idx2];
		idx++;
	}
}

void
VertexPolyColourCommand::GetColoursFromVertices(VertexColourJob& job)
{
	switch (m_forcedComponentMode)
	{
	case CM_VertexFace:
		GetVertexFaceColoursFromVertices(job);
		job.m_componentMode = CM_VertexFace;
		break;
	case CM_Vertex:
	case CM_None:
	default:
		GetVertexColoursFromVertices(job);
		job.m_componentMode = CM_Vertex;
		break;
	}
}

void
VertexPolyColourCommand::GetVertexFaceColoursFromVertices(VertexColourJob& job)
{
	// count number of vertex-faces
	int totalVF = 0;
	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	for ( ; !vertexIter.isDone(); vertexIter.next() ) 
	{
		int numFaces;
		vertexIter.numConnectedFaces(numFaces);
		totalVF += numFaces;
	}

	job.vertexIndices.setLength(totalVF);
	job.vfVertList.setLength(totalVF);
	job.vfFaceList.setLength(totalVF);
	job.srcColors.setLength(totalVF);

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;
#endif

	// gather
	MIntArray faces;
	int idx=0;
	vertexIter.reset();
	for ( ; !vertexIter.isDone(); vertexIter.next() ) 
	{
		int index = vertexIter.index();
		vertexIter.getConnectedFaces(faces);

		int numFaces = faces.length();
		for (int i = 0; i < numFaces; i++)
		{
			job.vertexIndices[idx] = index;
			job.vfVertList[idx] = index;
			job.vfFaceList[idx] = faces[i];
#if MAYA_API_VERSION >= 700
			vertexIter.getColor(job.srcColors[idx], faces[i], readColorSet);
#else
			vertexIter.getColor(job.srcColors[idx], faces[i]);
#endif
			idx++;
		}
	}
}


// This is the slow version, but we know it works.. 
// Once we know the new fast method is stable we can get rid of this one.
void
VertexPolyColourCommand::GetVertexFaceColoursFromVerticesSLOW(VertexColourJob& job)
{
	MStatus status;
	MColorArray		srcFaceVertColors;
	
#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;

	status=job.meshData->mesh->getFaceVertexColors(srcFaceVertColors, readColorSet);
#else	
	status=job.meshData->mesh->getFaceVertexColors(srcFaceVertColors);
#endif

	MItMeshFaceVertex facevertIter(job.meshData->dagPath.node(),&status);

	int idx=0;
	MItMeshVertex vertexIter(job.meshData->dagPath, job.components);
	for ( ; !vertexIter.isDone(); vertexIter.next() ) 
	{
		int vertexIndex=vertexIter.index();

		MIntArray faces;
		int numFaces;
		status=vertexIter.getConnectedFaces(faces);
		status=vertexIter.numConnectedFaces(numFaces);

		// get the vertex-face
		for (int i=0; i<numFaces; i++)
		{
			int numFaceVerts=job.meshData->mesh->polygonVertexCount(faces[i],&status);

			for (int j=0; j<numFaceVerts; j++)
			{
				int prevFace,prevVert;
				status = facevertIter.setIndex(faces[i],j,prevFace,prevVert);
				int	vert=facevertIter.vertId(&status);

				if (vert==vertexIndex)
				{
					int face=faces[i];
					int vertidx=j;

					// Get geometry indices
					job.vertexIndices.append(idx);
					job.vfVertList.append(vert);
					job.vfFaceList.append(face);

					// Get colours
					int colorIndex;
					status=job.meshData->mesh->getFaceVertexColorIndex(face,vertidx,colorIndex);
					MColor col=srcFaceVertColors[colorIndex];
					job.srcColors.append(col);
					idx++;
				}
			}
		}
	}
}

void
VertexPolyColourCommand::GetVertexColoursFromVertices(VertexColourJob& job)
{
	// Get geometry indices
	MStatus status;
	MFnSingleIndexedComponent singleIndices(job.components);
	status=singleIndices.getElements(job.vertexIndices);

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;
#endif
	// Get colours
	MItMeshVertex vertexIter(job.meshData->dagPath);
	int prevIndex = 0;
	unsigned int numVerts = job.vertexIndices.length();
	job.srcColors.setLength(numVerts);
	for (unsigned int i = 0; i < numVerts; i++)
	{
		vertexIter.setIndex(job.vertexIndices[i], prevIndex);
#if MAYA_API_VERSION >= 700
		vertexIter.getColor(job.srcColors[i], readColorSet);
#else
		vertexIter.getColor(job.srcColors[i]);
#endif
	}

	/*
	MColorArray allVertexColours;
	job.mesh->getVertexColors(allVertexColours);

	unsigned int numVerts = job.vertexIndices.length();
	job.srcColors.setLength(numVerts);
	for (unsigned int i = 0; i < numVerts; i++)
	{
	int idx = job.vertexIndices[i];
	job.srcColors[i] = allVertexColours[idx];
	}*/
}

void
VertexPolyColourCommand::GetObjectColours(VertexColourJob& job)
{
	switch (m_forcedComponentMode)
	{
	case CM_VertexFace:
		GetObjectVertexFaceColours(job);
		job.m_componentMode = CM_VertexFace;
		break;
	case CM_Vertex:
	case CM_None:
	default:
		GetObjectVertexColours(job);
		job.m_componentMode = CM_Vertex;
		break;
	}
}

void
VertexPolyColourCommand::GetObjectVertexColours(VertexColourJob& job)
{
	// TODO: perhaps this could be precalculated, and just cropped with a memcopy

	// Get geometry indices
	int numVerts = job.meshData->mesh->numVertices();
	job.vertexIndices.setLength(numVerts);
	for (int i = 0; i < numVerts; i++)
	{
		job.vertexIndices[i] = i;
	}

	// Get colours
#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;

	job.meshData->mesh->getVertexColors(job.srcColors, readColorSet);
#else
	job.meshData->mesh->getVertexColors(job.srcColors);
#endif

#ifdef _DEBUG
	assert(job.srcColors.length() == job.vertexIndices.length());
#endif
	//MString txt = "ReadColorSet: " + job.meshData->mesh->currentColorSetName();
	//OutputText(txt);
}

void
VertexPolyColourCommand::GetObjectVertexFaceColours(VertexColourJob& job)
{
	//MColorArray	srcFaceVertColors;
	//job.mesh->getFaceVertexColors(srcFaceVertColors);

	job.vertexIndices.setLength(job.meshData->mesh->numFaceVertices());
	job.vfVertList.setLength(job.meshData->mesh->numFaceVertices());
	job.vfFaceList.setLength(job.meshData->mesh->numFaceVertices());
	//job.srcColors.setLength(job.mesh->numFaceVertices());

	int idx = 0;
	MItMeshFaceVertex facevertIter(job.meshData->dagPath.node());
	for ( ; !facevertIter.isDone(); facevertIter.next() ) 
	{
		// Get geometry indices
		job.vertexIndices[idx] = idx;
		job.vfVertList[idx] = facevertIter.vertId();
		job.vfFaceList[idx] = facevertIter.faceId();
		
		// Get colours
		//facevertIter.getColor(job.srcColors[idx]);

		/*
		// Get colours
		int colorIndex;
		int vertidx = facevertIter.faceVertId();
		job.mesh->getFaceVertexColorIndex(face, vertidx, colorIndex);
		job.srcColors.append(srcFaceVertColors[colorIndex]);*/

		idx++;
	}

	if (m_isVerbose)
	{
		MString txt = "ReadColorSet: ";
		txt += job.meshData->readColorSetName;
		OutputText(txt);
	}

#if MAYA_API_VERSION >= 700
	MString* readColorSet = NULL;
	if (job.meshData->readColorSetName.length()>0)
		readColorSet = &job.meshData->readColorSetName;

	job.meshData->mesh->getFaceVertexColors(job.srcColors, readColorSet);
#else
	job.meshData->mesh->getFaceVertexColors(job.srcColors);
#endif

#ifdef _DEBUG
	assert(job.meshData->mesh->numFaceVertices() == idx);
	assert(job.srcColors.length() == job.vertexIndices.length());
#endif
}

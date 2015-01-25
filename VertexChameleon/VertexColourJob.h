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

#ifndef VERTEXCOLOURJOB_H
#define VERTEXCOLOURJOB_H

enum ComponentMode
{
	CM_None,
	CM_Vertex,
	CM_VertexFace,
	CM_Object,

	CM_MAX,
};

class MeshData;

class VertexColourJob
{
public:
	VertexColourJob(MeshData* mesh)
	{
		m_componentMode = CM_None;
		meshData = mesh;
	}
	virtual ~VertexColourJob()
	{
		meshData = NULL;
	}

	MObject			components;
	MeshData*		meshData;

	ComponentMode	m_componentMode;
	MColorArray		srcColors, dstColors;	// Selection source and dest colours
	MIntArray		vertexIndices;			// Selection vertex indices
	MIntArray		vfFaceList, vfVertList;	// Selection vertex-face indices
	MIntArray		primitiveStart;			// Used to indicate the start of a new primitive (eg face / edge)
	MPointArray		points;					// Vertex or Vertex-face positions in WORLD space
	MFloatVectorArray		uvs;					// Vertex or Vertex-face uvs
};

#endif

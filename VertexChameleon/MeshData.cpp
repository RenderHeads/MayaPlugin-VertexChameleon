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
#include "LayerData.h"
#include "MeshData.h"

MeshData::MeshData(const MDagPath& path)
{
	hasLayers = false;
	activeLayer = NULL;
	dagPath = path;
	layerViewMode = "Final Comp";
	mesh = NULL;

	BuildMesh();
}

void
MeshData::BuildMesh()
{
	delete mesh;
	mesh = NULL;

	mesh = new MFnMesh(dagPath);
#if MAYA_API_VERSION >= 700
	mesh->getCurrentColorSetName(currentColorSetName);
	readColorSetName = currentColorSetName;
#endif

	int nv = mesh->numVertices();
	nv++;
}

MeshData::~MeshData()
{
	delete mesh;
	mesh = NULL;

	DeleteLayersData();

	jobs.clear();
}

void
MeshData::DeleteLayersData()
{
	std::vector<LayerData*>::reverse_iterator riter;
	for (riter = layers.rbegin(); riter != layers.rend(); ++riter)
	{
		delete (*riter);
	}
	layers.clear();
}
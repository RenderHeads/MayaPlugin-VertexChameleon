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

#ifndef MESHDATA_H
#define MESHDATA_H

#include <iostream>
#include <vector>

using namespace std;

class LayerData;
class VertexColourJob;

class MeshData
{
public:
	MeshData(const MDagPath& path);
	virtual ~MeshData();

	void			DeleteLayersData();
	void			BuildMesh();

	MDagPath		dagPath;
	MFnMesh*		mesh;
	MString			currentColorSetName, readColorSetName, writeColorSetName;

	bool					hasLayers;
	LayerData*				activeLayer;
	std::vector<LayerData*> layers;
	MString					layerViewMode;

	std::vector<VertexColourJob*> jobs;

};

#endif

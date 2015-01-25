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
#include "VertexColourPlugin.h"

MayaPlugin* VCPlugin = NULL;

MStatus
initializePlugin(MObject obj)
{
	// Enable run-time memory check for debug builds.
/*#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif*/

	MayaPluginParams params;
	params.authorString = "RenderHeads Ltd";
	params.productName = "Vertex Chameleon 1.x";
	params.productShortName = "VCP";
	params.versionString = "1.8.2";

	VCPlugin = new VertexColourPlugin(params);
	VCPlugin->Load(obj);

	return MStatus::kSuccess;
}

MStatus
uninitializePlugin(MObject obj)
{
	if (VCPlugin != NULL)
	{
		VCPlugin->Unload(obj);
		delete VCPlugin;
		VCPlugin = NULL;
	}

	return MStatus::kSuccess;
}
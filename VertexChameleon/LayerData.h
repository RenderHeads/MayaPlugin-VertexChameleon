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

#ifndef LAYERDATA_H
#define LAYERDATA_H

#include "Blend.h"

class LayerData
{
public:
	LayerData()
	{
		visible = true;
		blendMode = BM_Replace;
		blendAmount = 1.0f;
	}
	MString		name;
	bool		visible;
	BlendMode	blendMode;
	float		blendAmount;
};

#endif

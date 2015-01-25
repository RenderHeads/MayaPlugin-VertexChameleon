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

#ifndef BLEND_H
#define BLEND_H

enum BlendMode
{
	BM_Replace,
	BM_Add,
	BM_Subtract,
	BM_Multiply,
	BM_Screen,
	BM_Overlay,
	BM_Lighten,
	BM_Darken,
	//BM_Soft,
	//BM_Hard,
	BM_Dodge,
	BM_Burn,
	BM_Diff,
	//BM_IDiff,
	//BM_Exclusion,

	BM_HueReplace,
	BM_SatReplace,
	BM_ColourReplace,
	BM_ValueReplace,
};


BlendMode
getBlendMode(const MString& name);

void
BlendAlpha(const MColorArray& srcColors, MColorArray& dstColors, const MColorArray& alphaColors);
void
BlendReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendMultiply(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendAdd(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendSubtract(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendDarken(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendLighten(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendDodge(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendBurn(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendScreen(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendOverlay(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
/*void
BlendSoftLight(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendHardLight(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);

void
BlendDiff(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendIDiff(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendExclusion(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
*/
void
BlendDifference(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendHueReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendSatReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendColourReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);
void
BlendValueReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);

void
BlendColours(BlendMode mode, const MColorArray& srcColors, MColorArray& dstColors, float blendAmount);

#endif

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
#include "MayaUtility.h"
#include "Random.h"
#include "ColorUtils.h"
#include "Blend.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BlendMode
getBlendMode(const MString& name)
{
	if (0==STRICMP(name.asChar(), "replace"))
		return BM_Replace;
	if (0==STRICMP(name.asChar(), "add"))
		return BM_Add;
	if (0==STRICMP(name.asChar(), "subtract"))
		return BM_Subtract;
	if (0==STRICMP(name.asChar(), "multiply"))
		return BM_Multiply;
	if (0==STRICMP(name.asChar(), "screen"))
		return BM_Screen;
	if (0==STRICMP(name.asChar(), "overlay"))
		return BM_Overlay;
	if ((0==STRICMP(name.asChar(), "lighten only")) || (0==STRICMP(name.asChar(), "lighten")))
		return BM_Lighten;
	if ((0==STRICMP(name.asChar(), "darken only")) || (0==STRICMP(name.asChar(), "darken")))
		return BM_Darken;

/*	if (0==STRICMP(name.asChar(), "soft"))
		return BM_Soft;
	if (0==STRICMP(name.asChar(), "hard"))
		return BM_Hard;
*/
	if (0==STRICMP(name.asChar(), "dodge"))
		return BM_Dodge;
	if (0==STRICMP(name.asChar(), "burn"))
		return BM_Burn;

	if (0==STRICMP(name.asChar(), "difference"))
		return BM_Diff;
	/*if (0==STRICMP(name.asChar(), "inv difference"))
		return BM_IDiff;
	if (0==STRICMP(name.asChar(), "exclusion"))
		return BM_Exclusion;*/

	if (0==STRICMP(name.asChar(), "hue replace"))
		return BM_HueReplace;
	if (0==STRICMP(name.asChar(), "saturation replace"))
		return BM_SatReplace;
	if (0==STRICMP(name.asChar(), "colour replace"))
		return BM_ColourReplace;
	if (0==STRICMP(name.asChar(), "value replace"))
		return BM_ValueReplace;

	return BM_Replace;
}


void
BlendReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		const MColor& dst = dstColors[i];
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendAlpha(const MColorArray& srcColors, MColorArray& dstColors, const MColorArray& alphaColors)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		float a = alphaColors[i].a;
		dstColors[i] = Lerp(dst, src, a);
		dstColors[i].a = Max(dst.a, src.a);
	}
}

void
BlendMultiply(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r *= src.r;
		dst.g *= src.g;
		dst.b *= src.b;
		//dst.a *= src.a;
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendAdd(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r += src.r;
		dst.g += src.g;
		dst.b += src.b;
		//dst.a += src.a;
		dst.r = Min(dst.r, 1.0f);
		dst.g = Min(dst.g, 1.0f);
		dst.b = Min(dst.b, 1.0f);
		//dst.a = Min(dst.a, 1.0f);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendSubtract(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r = src.r - dst.r;
		dst.g = src.g - dst.g;
		dst.b = src.b - dst.b;
		//dst.a = src.a - dst.a;
		dst.r = Max(dst.r, 0.0f);
		dst.g = Max(dst.g, 0.0f);
		dst.b = Max(dst.b, 0.0f);
		//dst.a = Max(dst.a, 0.0f);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendDarken(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r = Min(src.r,dst.r);
		dst.g = Min(src.g,dst.g);
		dst.b = Min(src.b,dst.b);
		//dst.a = Min(src.a,dst.a);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendLighten(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r = Max(src.r,dst.r);
		dst.g = Max(src.g,dst.g);
		dst.b = Max(src.b,dst.b);
		//dst.a = Max(src.a,dst.a);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendBurn(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r = 1.0f - (1.0f - src.r) / dst.r;
		dst.g = 1.0f - (1.0f - src.g) / dst.g;
		dst.b = 1.0f - (1.0f - src.b) / dst.b;
		dst.r = Max(dst.r, 0.0f);
		dst.g = Max(dst.g, 0.0f);
		dst.b = Max(dst.b, 0.0f);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendDodge(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r = src.r / (1.0f - dst.r);
		dst.g = src.g / (1.0f - dst.g);
		dst.b = src.b / (1.0f - dst.b);
		dst.r = Min(dst.r, 1.0f);
		dst.g = Min(dst.g, 1.0f);
		dst.b = Min(dst.b, 1.0f);
		dst.a = Min(dst.a, 1.0f);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendDifference(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r = fabsf(src.r - dst.r);
		dst.g = fabsf(src.g - dst.g);
		dst.b = fabsf(src.b - dst.b);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendHueReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		ColRGB rgbSrc, rgbDst;
		rgbSrc.set(srcColors[i]);
		rgbDst.set(dstColors[i]);
		ColHSV hsvSrc = RGB2HSV(rgbSrc);
		ColHSV hsvDst = RGB2HSV(rgbDst);
		hsvSrc.h = hsvDst.h;

		ColRGB finalRgb = HSV2RGB(hsvSrc);
		rgbSrc.lerp(finalRgb, blendAmount);

		dstColors[i] = rgbSrc.get();
	}
}

void
BlendSatReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		ColRGB rgbSrc, rgbDst;
		rgbSrc.set(srcColors[i]);
		rgbDst.set(dstColors[i]);
		ColHSV hsvSrc = RGB2HSV(rgbSrc);
		ColHSV hsvDst = RGB2HSV(rgbDst);
		hsvSrc.s = hsvDst.s;

		ColRGB finalRgb = HSV2RGB(hsvSrc);
		rgbSrc.lerp(finalRgb, blendAmount);

		dstColors[i] = rgbSrc.get();
	}
}

void
BlendColourReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		ColRGB rgbSrc, rgbDst;
		rgbSrc.set(srcColors[i]);
		rgbDst.set(dstColors[i]);
		ColHSV hsvSrc = RGB2HSV(rgbSrc);
		ColHSV hsvDst = RGB2HSV(rgbDst);
		hsvSrc.h = hsvDst.h;
		hsvSrc.s = hsvDst.s;

		ColRGB finalRgb = HSV2RGB(hsvSrc);
		rgbSrc.lerp(finalRgb, blendAmount);

		dstColors[i] = rgbSrc.get();
	}
}


void
BlendValueReplace(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		ColRGB rgbSrc, rgbDst;
		rgbSrc.set(srcColors[i]);
		rgbDst.set(dstColors[i]);
		ColHSV hsvSrc = RGB2HSV(rgbSrc);
		ColHSV hsvDst = RGB2HSV(rgbDst);
		hsvSrc.v = hsvDst.v;

		ColRGB finalRgb = HSV2RGB(hsvSrc);
		rgbSrc.lerp(finalRgb, blendAmount);

		dstColors[i] = rgbSrc.get();
	}
}

void
BlendScreen(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];
		dst.r = 1.0f - (1.0f - src.r) * (1.0f - dst.r);
		dst.g = 1.0f - (1.0f - src.g) * (1.0f - dst.g);
		dst.b = 1.0f - (1.0f - src.b) * (1.0f - dst.b);
		dst.r = Min(dst.r, 1.0f);
		dst.g = Min(dst.g, 1.0f);
		dst.b = Min(dst.b, 1.0f);
		dst.a = Min(dst.a, 1.0f);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

float
DotProduct(const MFloatVector& a, const MFloatVector& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

MFloatVector
Mul(const MFloatVector& a, const MFloatVector& b)
{
	return MFloatVector(a.x * b.x, a.y * b.y, a.z * b.z);
}

void
BlendOverlay(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	MFloatVector lumCoeff(0.25f, 0.65f, 0.1f);

	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		MColor src = srcColors[i];
		MColor dst = dstColors[i];

		MFloatVector base(src.r, src.g, src.b);
		MFloatVector blend(dst.r, dst.g, dst.b);

		float L = Min(1.0f, Max(0.0f, 10.0f * (DotProduct(lumCoeff, base) - 0.45f)));
		MFloatVector result1 = Mul(base, blend) * 2.0f;
		MFloatVector result2 = MFloatVector::one - 2.0f * Mul((MFloatVector::one - blend), (MFloatVector::one - base));

		dst.r = Min(result2.x, 1.0f);
		dst.g = Min(result2.y, 1.0f);
		dst.b = Min(result2.z, 1.0f);
		src.r = Min(result1.x, 1.0f);
		src.g = Min(result1.y, 1.0f);
		src.b = Min(result1.z, 1.0f);
		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}

void
BlendSoftLight(const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	unsigned int num = srcColors.length();
	for (unsigned int i = 0 ; i < num; i++)
	{
		const MColor& src = srcColors[i];
		MColor dst = dstColors[i];

		dst.r = (2.0f * src.r * dst.r) + (src.r * src.r) - 2.0f * src.r * src.r * dst.r;
		dst.g = (2.0f * src.g * dst.g) + (src.g * src.g) - 2.0f * src.g * src.g * dst.g;
		dst.b = (2.0f * src.b * dst.b) + (src.b * src.b) - 2.0f * src.b * src.b * dst.b;

		dstColors[i] = Lerp(src, dst, blendAmount);
	}
}


void
BlendColours(BlendMode mode, const MColorArray& srcColors, MColorArray& dstColors, float blendAmount)
{
	if (blendAmount != 0.0f)
	{
		switch (mode)
		{
		case BM_Multiply:
			BlendMultiply(srcColors, dstColors, blendAmount);
			break;
		case BM_Add:
			BlendAdd(srcColors, dstColors, blendAmount);
			break;
		case BM_Subtract:
			BlendSubtract(srcColors, dstColors, blendAmount);
			break;
		case BM_Darken:
			BlendDarken(srcColors, dstColors, blendAmount);
			break;
		case BM_Lighten:
			BlendLighten(srcColors, dstColors, blendAmount);
			break;
		case BM_Dodge:
			BlendDodge(srcColors, dstColors, blendAmount);
			break;
		case BM_Burn:
			BlendBurn(srcColors, dstColors, blendAmount);
			break;
		case BM_Screen:
			BlendScreen(srcColors, dstColors, blendAmount);
			break;
		case BM_Overlay:
			BlendOverlay(srcColors, dstColors, blendAmount);
			break;
		case BM_Diff:
			BlendDifference(srcColors, dstColors, blendAmount);
			break;
		case BM_HueReplace:
			BlendHueReplace(srcColors, dstColors, blendAmount);
			break;
		case BM_SatReplace:
			BlendSatReplace(srcColors, dstColors, blendAmount);
			break;
		case BM_ColourReplace:
			BlendColourReplace(srcColors, dstColors, blendAmount);
			break;
		case BM_ValueReplace:
			BlendValueReplace(srcColors, dstColors, blendAmount);
			break;
		default:
		case BM_Replace:
			if (blendAmount != 1.0f)
				BlendReplace(srcColors, dstColors, blendAmount);
			break;
		}
	}
	else
	{
		dstColors = srcColors;
	}
}

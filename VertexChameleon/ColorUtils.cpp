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
#include "Random.h"
#include "ColorUtils.h"

ColorChannelMask
getColorChannel(const char* name)
{
	if (0==STRICMP(name, "red"))
		return RED;
	if (0==STRICMP(name, "green"))
		return GRN;
	if (0==STRICMP(name, "blue"))
		return BLU;
	if (0==STRICMP(name, "hue"))
		return HUE;
	if (0==STRICMP(name, "saturation"))
		return SAT;
	if (0==STRICMP(name, "value"))
		return VAL;
	if (0==STRICMP(name, "alpha"))
		return ALP;

	return (ColorChannelMask)0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
FixUninitialisedColors(MColorArray& colors)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		MColor& col=colors[i];
		if (col.r<0.0f) col.r=0.0f;
		if (col.g<0.0f) col.g=0.0f;
		if (col.b<0.0f) col.b=0.0f;
		if (col.a<0.0f) col.a=1.0f;
	}
}

void
DoCopy(MIntArray& verts,MColorArray& srcColors,MColorArray& dstColors)
{
	for (unsigned int i=0; i<verts.length(); i++)
	{
		int idx=verts[i];
		dstColors.set( srcColors[idx] ,i);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
ReplaceHSVA(const ColHSV& hsv,MColorArray& colors)
{
	ColRGB rgb;
	rgb=HSV2RGB(hsv);
	ReplaceRGBA(rgb,colors);
}

void
ReplaceRGBA(const ColRGB& rgb,MColorArray& colors)
{
	MColor col;
	col.r=rgb.r;
	col.g=rgb.g;
	col.b=rgb.b;
	col.a=rgb.a;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i] = col;
	}
}

void
ReplaceRandomHSVA(const ColHSV& colHSV,const ColHSV& randomHSV,MColorArray& colors)
{
	MColor col;
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		hsv=colHSV;
		
		hsv.h+=randomHSV.h*gRand.RandomSignedUnit();
		hsv.s+=randomHSV.s*gRand.RandomSignedUnit();
		hsv.v+=randomHSV.v*gRand.RandomSignedUnit();
		hsv.a+=randomHSV.a*gRand.RandomSignedUnit();
		//hsv.selfClamp();
		rgb=HSV2RGB(hsv);
		rgb.selfClamp();
		col.r=rgb.r;
		col.g=rgb.g;
		col.b=rgb.b;
		col.a=rgb.a;

		colors.set(col,i);
	}
}

void
ReplaceRandomRGBA(const ColRGB& colRGB,const ColRGB& randomCol,MColorArray& colors)
{
	MColor col;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		col.r+=colRGB.r+randomCol.r*gRand.RandomSignedUnit();
		col.g+=colRGB.g+randomCol.g*gRand.RandomSignedUnit();
		col.b+=colRGB.b+randomCol.b*gRand.RandomSignedUnit();
		col.a+=colRGB.a+randomCol.a*gRand.RandomSignedUnit();
		colors.set(col,i);
	}
}

void
ReplaceRandomFreqHSVA(const ColHSV& colHSV,const ColHSV& randomHSV,float randFreq,MColorArray& colors)
{
	MColor col;
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		hsv=colHSV;

		hsv.h+=randomHSV.h*gRand.RandomSignedUnit();
		hsv.s+=randomHSV.s*gRand.RandomSignedUnit();
		hsv.v+=randomHSV.v*gRand.RandomSignedUnit();
		hsv.a+=randomHSV.a*gRand.RandomSignedUnit();
		//hsv.selfClamp();
		rgb=HSV2RGB(hsv);
		rgb.selfClamp();
		col.r=rgb.r;
		col.g=rgb.g;
		col.b=rgb.b;
		col.a=rgb.a;

		colors.set(col,i);
	}
}

void
ReplacePartialHSVA(const ColHSV& col,unsigned int mask,MColorArray& colors)
{
	if (mask&HUE)
		ReplaceH(col.h,colors);
	if (mask&SAT)
		ReplaceS(col.s,colors);
	if (mask&VAL)
		ReplaceV(col.v,colors);
	if (mask&ALP)
		ReplaceA(col.a,colors);

	/*
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);

		for (unsigned int j=0; j<4; j++)
		{
			if (mask&(1<<j))
				hsv.comp[j]=col.comp[j];
		}

		rgb=HSV2RGB(hsv);

		colors.set(rgb.get(),i);
	}*/
}
void
ReplacePartialRGBA(const ColRGB& col,unsigned int mask,MColorArray& colors)
{
	// TODO: profile this
	if (mask&RED)
		ReplaceR(col.r,colors);
	if (mask&GRN)
		ReplaceG(col.g,colors);
	if (mask&BLU)
		ReplaceB(col.b,colors);
	if (mask&ALP)
		ReplaceA(col.a,colors);

	/*
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);

		for (unsigned int j=0; j<4; j++)
		{
			if (mask&(1<<j))
				rgb.comp[j]=col.comp[j];
		}

		colors.set(rgb.get(),i);
		//colors[i] = rgb.get();
	}*/
}
void
ReplaceH(float value,MColorArray& colors)
{
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.h=value;
		rgb=HSV2RGB(hsv);
		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}
void
ReplaceS(float value,MColorArray& colors)
{
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.s=value;
		rgb=HSV2RGB(hsv);
		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}
void
ReplaceV(float value,MColorArray& colors)
{
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.v=value;
		rgb=HSV2RGB(hsv);
		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}
void
ReplaceR(float value,MColorArray& colors)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].r=value;
	}
}
void
ReplaceG(float value,MColorArray& colors)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].g=value;
	}
}
void
ReplaceB(float value,MColorArray& colors)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].b=value;
	}
}
void
ReplaceA(float value,MColorArray& colors)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].a=value;
	}
}
void
ReplaceRandomPartialHSVA(const ColHSV& col,const ColHSV& randomCol,unsigned int mask,MColorArray& colors)
{
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);

		float randVal=gRand.RandomUnit();
		if (mask&HUE)
			hsv.h = col.h + randomCol.h * randVal;

		randVal=gRand.RandomUnit();
		if (mask&SAT)
			hsv.s = col.s + randomCol.s * randVal;

		randVal=gRand.RandomUnit();
		if (mask&VAL)
			hsv.v = col.v + randomCol.v * randVal;

		randVal=gRand.RandomUnit();
		if (mask&ALP)
			hsv.a = col.a + randomCol.a * randVal;

		hsv.selfClamp();
		rgb=HSV2RGB(hsv);

		colors.set(rgb.get(),i);
	}
}

void
ReplaceRandomPartialRGBA(const ColRGB& col,const ColRGB& randomCol,unsigned int mask,MColorArray& colors)
{
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);

		float randVal=gRand.RandomUnit();
		if (mask&RED)
			rgb.r = col.r + randomCol.r * randVal;

		randVal=gRand.RandomUnit();
		if (mask&GRN)
			rgb.g = col.g + randomCol.g * randVal;

		randVal=gRand.RandomUnit();
		if (mask&BLU)
			rgb.b = col.b + randomCol.b * randVal;

		randVal=gRand.RandomUnit();
		if (mask&ALP)
			rgb.a = col.a + randomCol.a * randVal;

		rgb.selfClamp();

		colors.set(rgb.get(),i);
	}
}

void
BlendPartialRGBA(const ColRGB& randCol, const ColRGB& blendAmount, MColorArray& colors)
{
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		rgb.lerp(randCol, blendAmount.r, blendAmount.g, blendAmount.b, blendAmount.a);
		colors.set(rgb.get(),i);
	}
}

void
BlendPartialHSV(const ColHSV& randCol, const ColHSV& blendAmount, MColorArray& colors)
{
	ColHSV hsv;
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.lerp(randCol, blendAmount.h, blendAmount.s, blendAmount.v, 0.0f);
		rgb=HSV2RGB(hsv);

		colors.set(rgb.get(),i);
	}
}

void
BlendRandomPartialRGBA(unsigned int mask, const ColRGB& blendAmount, MColorArray& colors)
{
	ColRGB rgb;
	ColRGB randCol;
	randCol.setMin();
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
	
		if (mask&RED)
			randCol.r = gRand.RandomUnit();
		if (mask&GRN)
			randCol.g = gRand.RandomUnit();
		if (mask&BLU)
			randCol.b = gRand.RandomUnit();
		if (mask&ALP)
			randCol.a = gRand.RandomUnit();

		rgb.lerp(randCol, blendAmount.r, blendAmount.g, blendAmount.b, blendAmount.a);

		colors.set(rgb.get(),i);
	}
}


void
BlendRandomPartialHSV(unsigned int mask, const ColHSV& blendAmount, MColorArray& colors)
{
	ColHSV hsv;
	ColRGB rgb;
	ColHSV randCol;
	randCol.setMin();
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);

		if (mask&HUE)
			randCol.h = gRand.RandomUnit() * 360.0f;
		if (mask&SAT)
			randCol.s = gRand.RandomUnit();
		if (mask&VAL)
			randCol.v = gRand.RandomUnit();

		hsv.lerp(randCol, blendAmount.h, blendAmount.s, blendAmount.v, 0.0f);
		rgb=HSV2RGB(hsv);

		colors.set(rgb.get(),i);
	}
}

void
BlendPrimitiveRandomPartialRGBA(const ColRGB& blendAmount, MColorArray& colors, MIntArray& primitiveStart)
{
	ColRGB rgb;
	ColRGB randCol;
	randCol.r = gRand.RandomUnit();
	randCol.g = gRand.RandomUnit();
	randCol.b = gRand.RandomUnit();
	randCol.a = gRand.RandomUnit();
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (primitiveStart[i] == 1)
		{
			randCol.r = gRand.RandomUnit();
			randCol.g = gRand.RandomUnit();
			randCol.b = gRand.RandomUnit();
			randCol.a = gRand.RandomUnit();
		}

		rgb.set(colors[i]);
		rgb.lerp(randCol, blendAmount.r, blendAmount.g, blendAmount.b, blendAmount.a);

		colors.set(rgb.get(),i);
	}
}

void
BlendPrimitiveRandomPartialHSV(const ColHSV& blendAmount, MColorArray& colors, MIntArray& primitiveStart)
{
	ColHSV hsv;
	ColRGB rgb;
	ColHSV randCol;
	randCol.h = gRand.RandomUnit() * 360.0f;
	randCol.s = gRand.RandomUnit();
	randCol.v = gRand.RandomUnit();
	randCol.a = 0.0f;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (primitiveStart[i] == 1)
		{
			randCol.h = gRand.RandomUnit() * 360.0f;
			randCol.s = gRand.RandomUnit();
			randCol.v = gRand.RandomUnit();
		}

		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.lerp(randCol, blendAmount.h, blendAmount.s, blendAmount.v, 0.0f);
		rgb=HSV2RGB(hsv);

		colors.set(rgb.get(),i);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
AddHSVA(const ColHSV& col,MColorArray& colors)
{
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv+=col;
		hsv.selfClamp();
		rgb=HSV2RGB(hsv);
		rgb.selfClamp();

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

void
AddRGBA(const ColRGB& col,MColorArray& colors)
{
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		rgb+=col;
		rgb.selfClamp();

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

void
AddRandomHSVA(const ColHSV& col,const ColHSV& randomCol,MColorArray& colors)
{
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.h+=col.h+randomCol.h*gRand.RandomSignedUnit();
		hsv.s+=col.s+randomCol.s*gRand.RandomSignedUnit();
		hsv.v+=col.v+randomCol.v*gRand.RandomSignedUnit();
		hsv.a+=col.a+randomCol.a*gRand.RandomSignedUnit();
		hsv.selfClamp();
		rgb=HSV2RGB(hsv);
		rgb.selfClamp();

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}
void
AddRandomRGBA(const ColRGB& col,const ColRGB& randomCol,MColorArray& colors)
{
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		rgb.r+=col.r+randomCol.r*gRand.RandomSignedUnit();
		rgb.g+=col.g+randomCol.g*gRand.RandomSignedUnit();
		rgb.b+=col.b+randomCol.b*gRand.RandomSignedUnit();
		rgb.a+=col.a+randomCol.a*gRand.RandomSignedUnit();
		rgb.selfClamp();

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

void
ReplacePrimitiveRandomPartialRGBA(const ColRGB& col, const ColRGB& randomCol, unsigned int mask, MColorArray& colors, MIntArray& primitiveStart)
{
	ColRGB randomRGB;
	randomRGB.setMin();

	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (primitiveStart[i] == 1)
		{
			randomRGB.r = randomCol.r * gRand.RandomUnit();
			randomRGB.g = randomCol.g * gRand.RandomUnit();
			randomRGB.b = randomCol.b * gRand.RandomUnit();
			randomRGB.a = randomCol.a * gRand.RandomUnit();
		}
		rgb.set(colors[i]);
		
		if (mask&RED)
			rgb.r = col.r + randomRGB.r;

		if (mask&GRN)
			rgb.g = col.g + randomRGB.g;

		if (mask&BLU)
			rgb.b = col.b + randomRGB.b;

		if (mask&ALP)
			rgb.a = col.a + randomRGB.a;

		rgb.selfClamp();
		colors[i] = rgb.get();
	}
}

void
ReplacePrimitiveRandomPartialHSV(const ColHSV& col, const ColHSV& randomCol, unsigned int mask, MColorArray& colors, MIntArray& primitiveStart)
{
	ColHSV randomHSV;
	randomHSV.setMin();

	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (primitiveStart[i] == 1)
		{
			randomHSV.h = randomCol.h * gRand.RandomUnit();
			randomHSV.s = randomCol.s * gRand.RandomUnit();
			randomHSV.v = randomCol.v * gRand.RandomUnit();
		}
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);

		if (mask&HUE)
			hsv.h = col.h + randomHSV.h;

		if (mask&SAT)
			hsv.s = col.s + randomHSV.s;

		if (mask&VAL)
			hsv.v = col.v + randomHSV.v;

		hsv.selfClamp();
		rgb=HSV2RGB(hsv);
		rgb.selfClamp();

		colors[i] = rgb.get();
	}
}

void
AddPrimitiveRandomRGBA(const ColRGB& randomCol, MColorArray& colors, MIntArray& primitiveStart)
{
	ColRGB randomRGB;
	randomRGB.setMin();

	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (primitiveStart[i] == 1)
		{
			randomRGB.r = randomCol.r * gRand.RandomSignedUnit();
			randomRGB.g = randomCol.g * gRand.RandomSignedUnit();
			randomRGB.b = randomCol.b * gRand.RandomSignedUnit();
			randomRGB.a = randomCol.a * gRand.RandomSignedUnit();
		}
		rgb.set(colors[i]);
		rgb.r+=randomRGB.r;
		rgb.g+=randomRGB.g;
		rgb.b+=randomRGB.b;
		rgb.a+=randomRGB.a;
		rgb.selfClamp();

		colors[i] = rgb.get();
	}
}

void
AddPrimitiveRandomHSV(const ColHSV& randomCol, MColorArray& colors, MIntArray& primitiveStart)
{
	ColHSV randomHSV;
	randomHSV.setMin();

	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (primitiveStart[i] == 1)
		{
			randomHSV.h = randomCol.h * gRand.RandomSignedUnit();
			randomHSV.s = randomCol.s * gRand.RandomSignedUnit();
			randomHSV.v = randomCol.v * gRand.RandomSignedUnit();
			randomHSV.a = 0.0f;
		}
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.h+=randomHSV.h;
		hsv.s+=randomHSV.s;
		hsv.v+=randomHSV.v;
		hsv.selfClamp();
		rgb=HSV2RGB(hsv);
		rgb.selfClamp();

		colors[i] = rgb.get();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
DoContrast(MColorArray& colors,float contrast, float brightness)
{
	ColRGB rgb;

	float scaledContrast=powf(20.f,contrast);
	float add=0.5f+brightness;

	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);

		rgb.r=(rgb.r-0.5f)*scaledContrast+add;
		rgb.g=(rgb.g-0.5f)*scaledContrast+add;
		rgb.b=(rgb.b-0.5f)*scaledContrast+add;
		rgb.selfClamp();

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

void
DoContrastRGBA(MColorArray& colors, float contrastR, float contrastG, float contrastB, float contrastA, float brightness)
{
	ColRGB rgb;

	float scaledContrastR=powf(20.f,contrastR);
	float scaledContrastG=powf(20.f,contrastG);
	float scaledContrastB=powf(20.f,contrastB);
	float scaledContrastA=powf(20.f,contrastA);
	float add=0.5f+brightness;

	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);

		rgb.r=(rgb.r-0.5f)*scaledContrastR+add;
		rgb.g=(rgb.g-0.5f)*scaledContrastG+add;
		rgb.b=(rgb.b-0.5f)*scaledContrastB+add;
		rgb.a=(rgb.a-0.5f)*scaledContrastA + 0.5f;
		rgb.selfClamp();

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

void
DoContrastHSV(MColorArray& colors, float contrastH, float contrastS, float contrastV)
{
	ColRGB rgb;
	ColHSV hsv;

	float scaledContrastH=powf(20.f,contrastH);
	float scaledContrastS=powf(20.f,contrastS);
	float scaledContrastV=powf(20.f,contrastV);
	//float add=0.5f;

	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{		
		rgb.set(colors[i]);
		hsv = RGB2HSV(rgb);
		hsv.h /= 360.0f;

		hsv.h=(hsv.h-0.5f)*scaledContrastH + 0.5f;
		hsv.s=(hsv.s-0.5f)*scaledContrastS + 0.5f;
		hsv.v=(hsv.v-0.5f)*scaledContrastV + 0.5f;

		hsv.h *= 360.0f;
		hsv.selfClamp();

		rgb = HSV2RGB(hsv);
		colors[i] = rgb.get();
	}
}

void
DoContrastRandom(MColorArray& colors,float contrast,float random)
{
	ColRGB rgb;

	float brightness=0.0f;
	float add=0.5f+brightness;

	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);

		float scaledContrast=powf(20.f,contrast+random*gRand.RandomSignedUnit());
		rgb.r=(rgb.r-0.5f)*scaledContrast+add;
		rgb.g=(rgb.g-0.5f)*scaledContrast+add;
		rgb.b=(rgb.b-0.5f)*scaledContrast+add;
		rgb.selfClamp();

		colors[i] = rgb.get();
	}
}

void
DoQuantiseRGBA(MColorArray& colors, float quantiseR, float quantiseG, float quantiseB, float quantiseA)
{
	ColRGB rgb;

	float range = 256;
	quantiseR = range-(quantiseR * range);
	quantiseG = range-(quantiseG * range);
	quantiseB = range-(quantiseB * range);
	quantiseA = range-(quantiseA * range);

	// prevent divide by zero
	quantiseR = Max(quantiseR, 0.0001f);
	quantiseG = Max(quantiseG, 0.0001f);
	quantiseB = Max(quantiseB, 0.0001f);
	quantiseA = Max(quantiseA, 0.0001f);

	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);

		rgb.r=((int)(rgb.r * quantiseR)) / quantiseR;
		rgb.g=((int)(rgb.g * quantiseG)) / quantiseG;
		rgb.b=((int)(rgb.b * quantiseB)) / quantiseB;
		rgb.a=((int)(rgb.a * quantiseA)) / quantiseA;
		rgb.selfClamp();

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

void
DoQuantiseHSV(MColorArray& colors, float quantiseH, float quantiseS, float quantiseV)
{
	ColRGB rgb;
	ColHSV hsv;

	float range = 256;
	quantiseH = range-(quantiseH * range);
	quantiseS = range-(quantiseS * range);
	quantiseV = range-(quantiseV * range);

	// prevent divide by zero
	quantiseH = Max(quantiseH, 0.0001f);
	quantiseS = Max(quantiseS, 0.0001f);
	quantiseV = Max(quantiseV, 0.0001f);

	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv = RGB2HSV(rgb);
		hsv.h /= 360.0f;

		hsv.h=((int)(hsv.h * quantiseH)) / quantiseH;
		hsv.s=((int)(hsv.s * quantiseS)) / quantiseS;
		hsv.v=((int)(hsv.v * quantiseV)) / quantiseV;

		hsv.h *= 360.0f;
		rgb = HSV2RGB(hsv);

		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
DoLevels(MColorArray& colors,const LevelsParams& p)
{
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		rgb.r=DoLevels(rgb.r,p);
		rgb.g=DoLevels(rgb.g,p);
		rgb.b=DoLevels(rgb.b,p);
		rgb.selfClamp();
		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
DoPartialHSVAGrab(const MColorArray& colors,unsigned int mask,MDoubleArray& result)
{
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);

		if (mask&HUE)
			result.append( (double) hsv.h );
		if (mask&SAT)
			result.append( (double) hsv.s );
		if (mask&VAL)
			result.append( (double) hsv.v );
		if (mask&ALP)
			result.append( (double) hsv.a );
	}
}
void
DoPartialRGBAGrab(const MColorArray& colors,unsigned int mask,MDoubleArray& result)
{
	ColRGB rgb;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);

		if (mask&RED)
			result.append( (double) rgb.r );
		if (mask&GRN)
			result.append( (double) rgb.g );
		if (mask&BLU)
			result.append( (double) rgb.b );
		if (mask&ALP)
			result.append( (double) rgb.a );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
FindMinAlpha(const MColorArray& colors,float& min)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		float alpha=colors[i].a;
		if (alpha<min)
			min=alpha;
	}
}
void
FindMaxAlpha(const MColorArray& colors,float& max)
{	
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		float alpha=colors[i].a;
		if (alpha>max)
			max=alpha;
	}
}
void
FindMinRGBA(const MColorArray& colors,ColRGB& min)
{	
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		min.getMin(colors[i]);
	}
}
void
FindMaxRGBA(const MColorArray& colors,ColRGB& max)
{	
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		max.getMax(colors[i]);
	}
}
void
FindMinHSV(const MColorArray& colors,ColHSV& min)
{	
	ColRGB curColRGB;
	ColHSV curColHSV;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		curColRGB.set(colors[i]);
		curColHSV=RGB2HSV(curColRGB);
		min.getMin(curColHSV);
	}
}
void
FindMaxHSV(const MColorArray& colors,ColHSV& max)
{	
	ColRGB curColRGB;
	ColHSV curColHSV;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		curColRGB.r=colors[i].r;
		curColRGB.g=colors[i].g;
		curColRGB.b=colors[i].b;
		curColHSV=RGB2HSV(curColRGB);
		max.getMax(curColHSV);
	}
}
void
FindAverageRGBA(const MColorArray& colors,double& r, double& g, double& b, double& a, unsigned long& count)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		r += colors[i].r;
		g += colors[i].g;
		b += colors[i].b;
		a += colors[i].a;
	}
	count+=num;
}
void
FindUniqueColours(const MColorArray& colors, MColorArray& uniqueColours)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		bool match = false;
		unsigned long numUnique = uniqueColours.length();
		for (unsigned long j=0; j<numUnique; j++)
		{
			if (uniqueColours[j] == colors[i])
			{
				match = true;
				break;
			}
		}
		if (!match)
		{
			uniqueColours.append(colors[i]);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
DoInvertH(MColorArray& colors)
{	
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.h=360.0f-hsv.h;
		rgb=HSV2RGB(hsv);
		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}
void
DoInvertS(MColorArray& colors)
{	
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.s=1.0f-hsv.s;
		rgb=HSV2RGB(hsv);
		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}
void
DoInvertV(MColorArray& colors)
{	
	ColRGB rgb;
	ColHSV hsv;
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		rgb.set(colors[i]);
		hsv=RGB2HSV(rgb);
		hsv.v=1.0f-hsv.v;
		rgb=HSV2RGB(hsv);
		//colors.set(rgb.get(),i);
		colors[i] = rgb.get();
	}
}
void
DoInvertA(MColorArray& colors)
{	
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].a=1.0f-colors[i].a;
	}
}
void
DoInvertR(MColorArray& colors)
{	
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].r=1.0f-colors[i].r;
	}
}
void
DoInvertG(MColorArray& colors)
{	
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].g=1.0f-colors[i].g;
	}
}
void
DoInvertB(MColorArray& colors)
{	
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].b=1.0f-colors[i].b;
	}
}

void
DoClampR(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].r = Max(Min(colors[i].r, high), low);
	}
}
void
DoClampG(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].g = Max(Min(colors[i].g, high), low);
	}
}
void
DoClampB(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].b = Max(Min(colors[i].b, high), low);
	}
}
void
DoClampA(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		colors[i].a = Max(Min(colors[i].a, high), low);
	}
}
void
DoClampHSV(MColorArray& colors, float lowH, float highH, float lowS, float highS, float lowV, float highV)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		ColRGB rgb;
		rgb.set(colors[i]);
		ColHSV hsv = RGB2HSV(rgb);
		hsv.h = Max(Min(hsv.h, highH), lowH);
		hsv.s = Max(Min(hsv.s, highS), lowS);
		hsv.v = Max(Min(hsv.v, highV), lowV);
		rgb = HSV2RGB(hsv);
		colors[i] = rgb.get();
	}
}

void
DoSaturateR(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (colors[i].r < low)
			colors[i].r = 0.0f;
		else
			if (colors[i].r > high)
				colors[i].r = 1.0f;
	}
}
void
DoSaturateG(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (colors[i].g < low)
			colors[i].g = 0.0f;
		else
			if (colors[i].g > high)
				colors[i].g = 1.0f;
	}
}
void
DoSaturateB(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (colors[i].b < low)
			colors[i].b = 0.0f;
		else
			if (colors[i].b > high)
				colors[i].b = 1.0f;
	}
}
void
DoSaturateA(MColorArray& colors, float low, float high)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		if (colors[i].a < low)
			colors[i].a = 0.0f;
		else
			if (colors[i].a > high)
				colors[i].a = 1.0f;
	}
}
void
DoSaturateHSV(MColorArray& colors, float lowH, float highH, float lowS, float highS, float lowV, float highV)
{
	unsigned int num=colors.length();
	for (unsigned int i=0; i<num; i++)
	{
		ColRGB rgb;
		rgb.set(colors[i]);
		ColHSV hsv = RGB2HSV(rgb);

		if (hsv.h < lowH)
			hsv.h = 0.0f;
		else
			if (hsv.h > highH)
				hsv.h = 1.0f;

		if (hsv.s < lowS)
			hsv.s = 0.0f;
		else
			if (hsv.s > highS)
				hsv.s = 1.0f;

		if (hsv.v < lowV)
			hsv.v = 0.0f;
		else
			if (hsv.v > highV)
				hsv.v = 1.0f;

		rgb = HSV2RGB(hsv);
		colors[i] = rgb.get();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void
ExtractColourChannel(const MColorArray& colors, ColorChannelMask channel, MFloatArray& values)
{
	unsigned int numColors = colors.length();
	values.setLength(numColors);

	unsigned int i = 0;

	switch(channel)
	{
	case RED:
		for (i = 0; i < numColors; i++)
			values[i] = colors[i].r;
		break;
	case GRN:
		for (i = 0; i < numColors; i++)
			values[i] = colors[i].g;
		break;
	case BLU:
		for (i = 0; i < numColors; i++)
			values[i] = colors[i].b;
		break;
	case ALP:
		for (i = 0; i < numColors; i++)
			values[i] = colors[i].a;
		break;
	case HUE:
		for (i = 0; i < numColors; i++)
		{
			ColRGB rgb;
			rgb.set(colors[i]);
			ColHSV hsv = RGB2HSV(rgb);
			values[i] = hsv.h / 360.0f;
		}
		break;
	case SAT:
		for (i = 0; i < numColors; i++)
		{
			ColRGB rgb;
			rgb.set(colors[i]);
			ColHSV hsv = RGB2HSV(rgb);
			values[i] = hsv.s;
		}
		break;
	case VAL:
		for (i = 0; i < numColors; i++)
		{
			ColRGB rgb;
			rgb.set(colors[i]);
			ColHSV hsv = RGB2HSV(rgb);
			values[i] = hsv.v;
		}
		break;
	}
}

void
ReplaceColourChannel(MColorArray& colors, ColorChannelMask channel, const MFloatArray& values)
{
	unsigned int numColors = colors.length();

	unsigned int i = 0;

	switch(channel)
	{
	case RED:
		for (i = 0; i < numColors; i++)
			colors[i].r = values[i];
		break;
	case GRN:
		for (i = 0; i < numColors; i++)
			colors[i].g = values[i];
		break;
	case BLU:
		for (i = 0; i < numColors; i++)
			colors[i].b = values[i];
		break;
	case ALP:
		for (i = 0; i < numColors; i++)
			colors[i].a = values[i];
		break;
	case HUE:
		for (i = 0; i < numColors; i++)
		{
			ColRGB rgb;
			rgb.set(colors[i]);
			ColHSV hsv = RGB2HSV(rgb);
			hsv.h = values[i] * 360.0f;
			hsv.selfClamp();
			rgb = HSV2RGB(hsv);
			colors[i] = rgb.get();
		}
		break;
	case SAT:
		for (i = 0; i < numColors; i++)
		{
			ColRGB rgb;
			rgb.set(colors[i]);
			ColHSV hsv = RGB2HSV(rgb);
			hsv.s = values[i];
			hsv.selfClamp();
			rgb = HSV2RGB(hsv);
			colors[i] = rgb.get();
		}
		break;
	case VAL:
		for (i = 0; i < numColors; i++)
		{
			ColRGB rgb;
			rgb.set(colors[i]);
			ColHSV hsv = RGB2HSV(rgb);
			hsv.v = values[i];
			hsv.selfClamp();
			rgb = HSV2RGB(hsv);
			colors[i] = rgb.get();
		}
		break;
	}
}

#if MAYA_API_VERSION >= 700
bool
MeshHasColourSet(const MFnMesh& mesh, const MString& setName)
{
	MStringArray colorSets;
	mesh.getColorSetNames(colorSets);
	for (unsigned int i = 0; i < colorSets.length(); i++)
	{
		if (colorSets[i] == setName)
			return true;
	}

	return false;
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

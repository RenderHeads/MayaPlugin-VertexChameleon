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

#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <math.h>
#include "MayaUtility.h"
#include "Blend.h"


#ifdef WIN32
#define STRICMP _stricmp
#else
#define STRICMP strcasecmp
#endif


enum ColorChannelMask
{
	RED=0x01,
	GRN=0x02,
	BLU=0x04,
	ALP=0x08,
	RGB=RED|GRN|BLU,
	RGBA=RGB|ALP,
	HUE=0x10,
	SAT=0x20,
	VAL=0x40,
	HSV=HUE|SAT|VAL,
	HSVA=HSV|ALP,
};

ColorChannelMask	getColorChannel(const char* name);

struct ColRGB
{
	union
	{
		float comp[4];
		struct {	float r,g,b,a;	};
	};
	ColRGB()
	{

	}
	ColRGB(float rr,float gg,float bb,float aa=1.0f)
	{
		r=rr;
		g=gg;
		b=bb;
		a=aa;
	}
	ColRGB(const MColor& mcol)
	{
		r=mcol.r;
		g=mcol.g;
		b=mcol.b;
		a=mcol.a;
	}
	ColRGB& operator += (const ColRGB& src)
	{
		r+=src.r;
		g+=src.g;
		b+=src.b;
		a+=src.a;
		return *this;
	}
	ColRGB& operator += (const MColor& src)
	{
		r+=src.r;
		g+=src.g;
		b+=src.b;
		a+=src.a;
		return *this;
	}

	ColRGB operator / (const float& val) const
	{
		float recip=1.0f/val;
		return ColRGB(r*recip,g*recip,b*recip,a*recip);
	}
	ColRGB operator * (const float& val) const
	{
		return ColRGB(r*val,g*val,b*val,a*val);
	}
	ColRGB operator + (const ColRGB& c) const
	{
		return ColRGB(r+c.r, g+c.g, b+c.b, a+c.a);
	}
	ColRGB operator - (const ColRGB& c) const
	{
		return ColRGB(r-c.r, g-c.g, b-c.b, a-c.a);
	}
	ColRGB& operator=(const ColRGB& src)
	{
		if (this != &src) 
		{
			r = src.r;
			g = src.g;
			b = src.b;
			a = src.a;
		}
		return *this;
	}
	void	setMin()
	{
		r=g=b=a=0.0f;
	}
	void	setMax()
	{
		r=g=b=a=1.0f;
	}
	void	set(const MColor& col)
	{
		r=col.r;
		g=col.g;
		b=col.b;
		a=col.a;
	}
	MColor get() const
	{
		return MColor(r,g,b,a);
	}
	void	getMin(const MColor& col)
	{
		r=Min(r,col.r);
		g=Min(g,col.g);
		b=Min(b,col.b);
		a=Min(a,col.a);
	}
	void	getMinRGB(const MColor& col)
	{
		float total=col.r+col.g+col.b;
		float total2=r+g+b;
		if (total<total2)
		{
			r=col.r;
			g=col.g;
			b=col.b;
		}
	}
	void	getMaxRGB(const MColor& col)
	{
		float total=col.r+col.g+col.b;
		float total2=r+g+b;
		if (total>total2)
		{
			r=col.r;
			g=col.g;
			b=col.b;
		}
	}
	void	getMinRGBA(const MColor& col)
	{
		float total=col.r+col.g+col.b;
		float total2=r+g+b;
		if (total<total2)
		{
			r=col.r;
			g=col.g;
			b=col.b;
		}
		a=Min(a,col.a);
	}
	void	getMaxRGBA(const MColor& col)
	{
		float total=col.r+col.g+col.b;
		float total2=r+g+b;
		if (total>total2)
		{
			r=col.r;
			g=col.g;
			b=col.b;
		}
		a=Max(a, col.a);
	}
	void	getMax(const MColor& col)
	{
		r=Max(r,col.r);
		g=Max(g,col.g);
		b=Max(b,col.b);
		a=Max(a,col.a);
	}
	void	selfClamp()
	{
		r=Min(r,1.0f);
		r=Max(r,0.0f);
		g=Min(g,1.0f);
		g=Max(g,0.0f);
		b=Min(b,1.0f);
		b=Max(b,0.0f);
		a=Min(a,1.0f);
		a=Max(a,0.0f);
	}
	void lerp(const ColRGB& dst, float t)
	{
		r = r + (dst.r - r) * t;
		g = g + (dst.g - g) * t;
		b = b + (dst.b - b) * t;
		a = a + (dst.a - a) * t;
	}
	void lerp(const ColRGB& dst, float tr, float tg, float tb, float ta)
	{
		r = r + (dst.r - r) * tr;
		g = g + (dst.g - g) * tg;
		b = b + (dst.b - b) * tb;
		a = a + (dst.a - a) * ta;
	}
	float GetLengthRGB()
	{
		return sqrtf((r * r) + (g * g) + (b * b));
	}
	void NormaliseRGB()
	{
		float l = GetLengthRGB();
		if (l != 0.0f)
		{
			l = 1.0f / l;
			r *= l;
			g *= l;
			b *= l;
		}
	}
};

struct ColHSV
{
	union
	{
		float comp[4];
		struct {	float h,s,v,a;	};
	};

	ColHSV& operator=(const ColHSV& src)
	{
		if (this != &src) 
		{
			h = src.h;
			s = src.s;
			v = src.v;
			a = src.a;
		}
		return *this;
	}
	ColHSV& operator += (const ColHSV& src)
	{
		h+=src.h;
		s+=src.s;
		v+=src.v;
		a+=src.a;
		return *this;
	}

	void	setMin()
	{
		h=s=v=a=0.0f;
	}
	void	setMax()
	{
		h=360.0f;
		s=v=a=1.0f;
	}
	void	getMin(const ColHSV& col)
	{
		h=Min(h,col.h);
		s=Min(s,col.s);
		v=Min(v,col.v);
		a=Min(a,col.a);
	}
	void	getMax(const ColHSV& col)
	{
		h=Max(h,col.h);
		s=Max(s,col.s);
		v=Max(v,col.v);
		a=Max(a,col.a);
	}
	void lerp(const ColHSV& dst, float th, float ts, float tv, float ta)
	{
		h = h + (dst.h - h) * th;
		s = s + (dst.s - s) * ts;
		v = v + (dst.v - v) * tv;
		a = a + (dst.a - a) * ta;
	}
	void	selfClamp()
	{
		if (h<0.0f)
		{
			while (h<0.0f) h+=360.0f;
			h=360.0f-h;
		}
		
		// this clamping was removed as it made weird behaviour when using randomness in hue when hue was 360..
		// this HSV functions take into account 0 < hue > 360 so we shouldn't need to clamp it
			//h=fmodf(h,360.0f);
			//h=Min(h,360.0f);
			//h=Max(h,0.0f);

		a=Min(a,1.0f);
		a=Max(a,0.0f);

		// safeval stops the s/v turning the color completely black or white
		// this means we don't loose our hue information
		static const float safeval=0.001f;
		static const float safemax=1.0f-safeval;
		static const float safemin=0.0f+safeval;
		s=Min(s,safemax);
		s=Max(s,safemin);
		v=Min(v,safemax);
		v=Max(v,safemin);
	}
	void	selfClampRelative()
	{
		h=Min(h,360.0f);
		h=Max(h,-360.0f);
		s=Min(s,1.0f);
		s=Max(s,-1.0f);
		v=Min(v,1.0f);
		v=Max(v,-1.0f);
		a=Min(a,1.0f);
		a=Max(a,-1.0f);
	}
};

struct LevelsParams
{
	float inBlack,inWhite;
	float inGamma;
	float outBlack,outWhite;
};
/*
class ArrayRGBA
{
public:
	ArrayRGBA(unsigned int num)
	{
		m_numColors=0;
		m_colors=NULL;
	}
	~ArrayRGBA()
	{

	}
private:
	unsigned int	m_numColors;
	ColRGB*			m_colors;
};*/

inline float
DoLevels(float val,const LevelsParams& p)
{
	val=powf( (val - p.inBlack) / (p.inWhite-p.inBlack) , p.inGamma);
	val=val*(p.outWhite-p.outBlack) + p.outBlack;
	return val;
}

// returns a random number between -1.0 and 1.0
/*inline float
OLD_RandomSignedUnit()
{
	return float((rand()%20000)-10000)*0.0001f;
}*/

inline ColRGB
HSV2RGB(const ColHSV& inhsv)
{
	ColHSV c1=inhsv;
	ColRGB c2,sat;

	// We could have a faster unsafe version without these checks
	while (c1.h < 0.0f)
		c1.h += 360.0f;
	while (c1.h > 360.0f)
		c1.h -= 360.0f;

	if (c1.h < 120.0f)
	{
		sat.r = (120.0f - c1.h) / 60.0f;
		sat.g = c1.h / 60.0f;
		sat.b = 0.0f;
	} 
	else if (c1.h < 240.0f)
	{
		sat.r = 0.0f;
		sat.g = (240.0f - c1.h) / 60.0f;
		sat.b = (c1.h - 120.0f) / 60.0f;
	}
	else
	{
		sat.r = (c1.h - 240.0f) / 60.0f;
		sat.g = 0.0f;
		sat.b = (360.0f - c1.h) / 60.0f;
	}
	sat.r = Min(sat.r,1.0f);
	sat.g = Min(sat.g,1.0f);
	sat.b = Min(sat.b,1.0f);

	c2.r = (1.0f - c1.s + c1.s * sat.r) * c1.v;
	c2.g = (1.0f - c1.s + c1.s * sat.g) * c1.v;
	c2.b = (1.0f - c1.s + c1.s * sat.b) * c1.v;
	c2.a = c1.a;

	return(c2);
}

inline ColHSV
RGB2HSV(const ColRGB& c1)
{
	float themin,themax,delta;
	ColHSV c2;

	themin = Min(c1.r,Min(c1.g,c1.b));
	themax = Max(c1.r,Max(c1.g,c1.b));
	delta = themax - themin;
	c2.v = themax;
	c2.s = 0.0f;
	if (themax > 0.0f)
		c2.s = delta / themax;
	c2.h = 0.0f;
	if (delta > 0.0f) {
		if (themax == c1.r && themax != c1.g)
			c2.h += (c1.g - c1.b) / delta;
		if (themax == c1.g && themax != c1.b)
			c2.h += (2.0f + (c1.b - c1.r) / delta);
		if (themax == c1.b && themax != c1.r)
			c2.h += (4.0f + (c1.r - c1.g) / delta);
		c2.h *= 60.0f;
	}
	c2.a=c1.a;
	return(c2);
}

class MIntArray;
class MColorArray;
class MDoubleArray;

// fix maya's uninitialised colors
void	FixUninitialisedColors(MColorArray& colors);

// find min/max
void	FindMinAlpha(const MColorArray& colors,float& min);
void	FindMaxAlpha(const MColorArray& colors,float& max);
void	FindMinRGBA(const MColorArray& colors,ColRGB& min);
void	FindMaxRGBA(const MColorArray& colors,ColRGB& max);
void	FindMinHSV(const MColorArray& colors,ColHSV& min);
void	FindMaxHSV(const MColorArray& colors,ColHSV& max);
void	FindAverageRGBA(const MColorArray& colors,double& r, double& g, double& b, double& a, unsigned long& count);
void	FindUniqueColours(const MColorArray& colors, MColorArray& uniqueColours);

// return colors
void	DoPartialHSVAGrab(const MColorArray& colors,unsigned int mask,MDoubleArray& result);
void	DoPartialRGBAGrab(const MColorArray& colors,unsigned int mask,MDoubleArray& result);

// replace colors
void	ReplaceHSVA(const ColHSV& hsv,MColorArray& colors);
void	ReplaceRGBA(const ColRGB& rgb,MColorArray& colors);
void	ReplacePartialHSVA(const ColHSV& col,unsigned int mask,MColorArray& colors);
void	ReplacePartialRGBA(const ColRGB& col,unsigned int mask,MColorArray& colors);
void	ReplaceH(float value,MColorArray& colors);
void	ReplaceS(float value,MColorArray& colors);
void	ReplaceV(float value,MColorArray& colors);
void	ReplaceR(float value,MColorArray& colors);
void	ReplaceG(float value,MColorArray& colors);
void	ReplaceB(float value,MColorArray& colors);
void	ReplaceA(float value,MColorArray& colors);

// replace random
void	ReplaceRandomHSVA(const ColHSV& col,const ColHSV& randomCol,MColorArray& colors);
void	ReplaceRandomRGBA(const ColRGB& col,const ColRGB& randomCol,MColorArray& colors);
void	ReplaceRandomPartialHSVA(const ColHSV& col,const ColHSV& randomCol,unsigned int mask,MColorArray& colors);
void	ReplaceRandomPartialRGBA(const ColRGB& col,const ColRGB& randomCol,unsigned int mask,MColorArray& colors);
void	ReplacePrimitiveRandomPartialRGBA(const ColRGB& col, const ColRGB& randomCol, unsigned int mask, MColorArray& colors, MIntArray& primitiveStart);
void	ReplacePrimitiveRandomPartialHSV(const ColHSV& col, const ColHSV& randomCol, unsigned int mask, MColorArray& colors, MIntArray& primitiveStart);
void	BlendPartialRGBA(const ColRGB& randCol, const ColRGB& blendAmount, MColorArray& colors);
void	BlendPartialHSV(const ColHSV& randCol, const ColHSV& blendAmount, MColorArray& colors);
void	BlendRandomPartialRGBA(unsigned int mask, const ColRGB& blendAmount, MColorArray& colors);
void	BlendRandomPartialHSV(unsigned int mask, const ColHSV& blendAmount, MColorArray& colors);
void	BlendPrimitiveRandomPartialRGBA(const ColRGB& blendAmount, MColorArray& colors, MIntArray& primitiveStart);
void	BlendPrimitiveRandomPartialHSV(const ColHSV& blendAmount, MColorArray& colors, MIntArray& primitiveStart);

// add/subtract color
void	AddHSVA(const ColHSV& col,MColorArray& colors);
void	AddRGBA(const ColRGB& col,MColorArray& colors);

// add/subtract random
void	AddRandomHSVA(const ColHSV& col,const ColHSV& randomCol,MColorArray& colors);
void	AddRandomRGBA(const ColRGB& col,const ColRGB& randomCol,MColorArray& colors);
void	AddPrimitiveRandomRGBA(const ColRGB& randomCol, MColorArray& colors, MIntArray& primitiveStart);
void	AddPrimitiveRandomHSV(const ColHSV& randomCol, MColorArray& colors, MIntArray& primitiveStart);

// change contrast
void	DoContrast(MColorArray& colors,float contrast, float brightness);
void	DoContrastRandom(MColorArray& colors,float contrast,float random);
void	DoContrastRGBA(MColorArray& colors, float contrastR, float contrastG, float contrastB, float contrastA, float brightness);
void	DoContrastHSV(MColorArray& colors, float contrastH, float contrastS, float contrastV);

// Quantise
void	DoQuantiseRGBA(MColorArray& colors, float quantiseR, float quantiseG, float quantiseB, float quantiseA);
void	DoQuantiseHSV(MColorArray& colors, float quantiseH, float quantiseS, float quantiseV);

// change levels
void	DoLevels(MColorArray& colors,const LevelsParams& p);

// invert
void	DoInvertH(MColorArray& colors);
void	DoInvertS(MColorArray& colors);
void	DoInvertV(MColorArray& colors);
void	DoInvertA(MColorArray& colors);
void	DoInvertR(MColorArray& colors);
void	DoInvertG(MColorArray& colors);
void	DoInvertB(MColorArray& colors);

// clamp
void	DoClampR(MColorArray& colors, float low, float high);
void	DoClampG(MColorArray& colors, float low, float high);
void	DoClampB(MColorArray& colors, float low, float high);
void	DoClampA(MColorArray& colors, float low, float high);
void	DoClampHSV(MColorArray& colors, float lowH, float highH, float lowS, float highS, float lowV, float highV);

// saturate
void	DoSaturateR(MColorArray& colors, float low, float high);
void	DoSaturateG(MColorArray& colors, float low, float high);
void	DoSaturateB(MColorArray& colors, float low, float high);
void	DoSaturateA(MColorArray& colors, float low, float high);
void	DoSaturateHSV(MColorArray& colors, float lowH, float highH, float lowS, float highS, float lowV, float highV);

void	ExtractColourChannel(const MColorArray& colors, ColorChannelMask channel, MFloatArray& values);
void	ReplaceColourChannel(MColorArray& colors, ColorChannelMask channel, const MFloatArray& values);

#if MAYA_API_VERSION >= 700
bool	MeshHasColourSet(const MFnMesh& mesh, const MString& setName);
#endif

#endif

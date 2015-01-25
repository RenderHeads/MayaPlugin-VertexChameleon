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
#include <sys/timeb.h>
#include "MayaUtility.h"
#include "Utility.h"
#include "Random.h"
#include "MeshData.h"
#include "VertexPolyColourCommand.h"

VertexPolyColourCommand::VertexPolyColourCommand()
{
	m_savedObjectSelectionMask = NULL;
	m_savedComponentSelectionMask = NULL;

	// General
	m_isUndoable = true;
	m_isVerbose = false;
	m_forcedComponentMode = CM_None;
	m_operation = OP_PassThru;
	m_readSourceColours = true;
	m_isReadColoursFromNode = false;
	m_isReadColoursFromNodeDest = false;
	m_isReadDestColoursFromNode = false;
	m_isReadDestColoursFromNodeSource = false;
	m_isNodeWriteDest = false;
	m_applyEnabled = true;
	m_compositeEnabled = true;
	m_blendingEnabled = false;
	m_blendAmount = 1.0f;
	m_blendMode = BM_Replace;
	m_masterBlendAmount = 1.0f;
	m_forceViewportRefresh = false;
	m_channelMaskRGBA = RGBA;
	m_getComponentPositions = false;
	m_getComponentUVs = false;
	m_isRestoreSelection = false;

	// 3D Texture
	m_noiseScale[0] = 1.0f;
	m_noiseScale[1] = 1.0f;
	m_noiseScale[2] = 1.0f;
	m_3DTextureOffset[0] = 0.0f;
	m_3DTextureOffset[1] = 0.0f;
	m_3DTextureOffset[2] = 0.0f;

	// Random
	//_randomComponentMode = RC_None;
	m_randomiseRGB[0] = 0.0f;
	m_randomiseRGB[1] = 0.0f;
	m_randomiseRGB[2] = 0.0f;
	m_randomiseHSV[0] = 0.0f;
	m_randomiseHSV[1] = 0.0f;
	m_randomiseHSV[2] = 0.0f;
	m_randomiseAlpha = 0.0f;

	// Invert
	m_invertMaskRGBA = 0;
	m_invertMaskHSV = 0;

	// Posterize
	m_posterizeMaskRGBA = 0;
	m_posterizeMaskHSV = 0;
	m_posterizeAmount = 0.0f;

	// Paint
	m_isPaint = false;
	m_paintVertexIndex = 0;

	// Apply
	m_maskHSV=0;
	m_maskRGB=0;
	m_colRGB.setMin();
	m_colHSV.setMin();

	// Channel Copy/Swap
	m_channelSrc = (ColorChannelMask)0;
	m_channelDst = (ColorChannelMask)0;

	// Clamp
	m_maskClamp=0;
	m_clampR[0] = 0.0f;
	m_clampR[1] = 1.0f;
	m_clampG[0] = 0.0f;
	m_clampG[1] = 1.0f;
	m_clampB[0] = 0.0f;
	m_clampB[1] = 1.0f;
	m_clampA[0] = 0.0f;
	m_clampA[1] = 1.0f;
	m_clampH[0] = 0.0f;
	m_clampH[1] = 360.0f;
	m_clampS[0] = 0.0f;
	m_clampS[1] = 1.0f;
	m_clampV[0] = 0.0f;
	m_clampV[1] = 1.0f;

	// Adjust brightness + contrast
	m_brightness=0.0f;
	m_contrastR=0.0f;
	m_contrastG=0.0f;
	m_contrastB=0.0f;
	m_contrastA=0.0f;
	m_contrastH=0.0f;
	m_contrastS=0.0f;
	m_contrastV=0.0f;

	// Adjust levels
	m_levels.inBlack=0.0f;
	m_levels.inWhite=1.0f;
	m_levels.inGamma=1.0f;
	m_levels.outBlack=0.0f;
	m_levels.outWhite=1.0f;

	// Facetise
	m_space = "object";
	m_type = "normal";

	// Query
	m_isMinMaxRGBA = m_isAverageRGBA = m_isUniqueColours = false;
	m_minRGBA.setMax();
	m_maxRGBA.setMin();
	m_averageR = m_averageG = m_averageB = m_averageA = 0.0;
	m_numTotalColours=0;
	m_numUniqueColours=0;
	m_numSamples = 3;
	m_samplePoint[0] = m_samplePoint[1] = m_samplePoint[2] = 0.0f;
}

// declare the parameter syntax for this command
MSyntax
VertexPolyColourCommand::newSyntax()
{
	MStatus status;
	MSyntax syntax;

	// General flags
	syntax.addFlag("-nun","-noUndo");
	syntax.addFlag("-vb","-verbose");
	syntax.addFlag("-fc","-forceComponent", MSyntax::kLong);
	syntax.addFlag("-op","-operation", MSyntax::kLong);
	syntax.addFlag("-nds","-nodeSource");
	syntax.addFlag("-ndx","-nodeSource2");
	syntax.addFlag("-ndd","-nodeDest");
	syntax.addFlag("-ndz","-nodeDestSource");
	syntax.addFlag("-nwd","-nodeWriteDest");
	syntax.addFlag("-cmk","-channelMask", MSyntax::kString);
	syntax.addFlag("-frc","-forceReadColourSet", MSyntax::kString);
	syntax.addFlag("-fwc","-forceWriteColourSet", MSyntax::kString);
	
	// Apply RGBA & HSVA channels
	syntax.addFlag("-r","-colorR",MSyntax::kDouble);
	syntax.addFlag("-g","-colorG",MSyntax::kDouble);
	syntax.addFlag("-b","-colorB",MSyntax::kDouble);
	syntax.addFlag("-h","-colorH",MSyntax::kDouble);
	syntax.addFlag("-s","-colorS",MSyntax::kDouble);
	syntax.addFlag("-v","-colorV",MSyntax::kDouble);
	syntax.addFlag("-a","-colorA",MSyntax::kDouble);
	syntax.addFlag("-car","-colorArray");

	// Adjust brightness + contrast
	syntax.addFlag("-c","-contrast",MSyntax::kDouble);
	syntax.addFlag("-cor","-contrastR",MSyntax::kDouble);
	syntax.addFlag("-cog","-contrastG",MSyntax::kDouble);
	syntax.addFlag("-cob","-contrastB",MSyntax::kDouble);
	syntax.addFlag("-coa","-contrastA",MSyntax::kDouble);
	syntax.addFlag("-coh","-contrastH",MSyntax::kDouble);
	syntax.addFlag("-cos","-contrastS",MSyntax::kDouble);
	syntax.addFlag("-cov","-contrastV",MSyntax::kDouble);
	syntax.addFlag("-bri","-brightness",MSyntax::kDouble);

	// Adjust Levels
	syntax.addFlag("-lev","-levels",MSyntax::kDouble,MSyntax::kDouble,MSyntax::kDouble,MSyntax::kDouble,MSyntax::kDouble);

	// Random
	syntax.addFlag("-rnr","-randomRGB",  MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-rnh","-randomHSV", MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-rna","-randomAlpha", MSyntax::kDouble);

	// Filters
	syntax.addFlag("-fs","-filterStrength",MSyntax::kDouble);

	// Invert
	syntax.addFlag("-inv","-invert",MSyntax::kString);
	
	// Posterize
	syntax.addFlag("-pos","-posterize",MSyntax::kString);
	syntax.addFlag("-poa","-posterizeAmount",MSyntax::kDouble);

	// Painting
	syntax.addFlag("-pa","-paint");
	syntax.addFlag("-pao","-object", MSyntax::kString);
	syntax.addFlag("-pai","-vertexIndex", MSyntax::kLong);

	// Gradient
	syntax.addFlag("-grt","-gradientType",MSyntax::kString);
	syntax.addFlag("-grr","-gradientRepeat",MSyntax::kString);
	syntax.addFlag("-grs","-gradientRepeats",MSyntax::kDouble);
	syntax.addFlag("-gro","-gradientOffset",MSyntax::kDouble);
	syntax.addFlag("-grv","-gradientReverse");
	syntax.addFlag("-gwm","-gradientWorldBoundsMin", MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-gwx","-gradientWorldBoundsMax", MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-gor","-gradientOverrideRadius");
	syntax.addFlag("-goc","-gradientOverrideCenter");
	syntax.addFlag("-grq","-gradientRadius", MSyntax::kDouble);
	syntax.addFlag("-grp","-gradientStartPos",MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-gr1","-gradCol1",MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-gr2","-gradCol2",MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-grn","-gradientRampNode",MSyntax::kString);
	
	// Channel Copy/Swap
	syntax.addFlag("-chs","-channelSrc",MSyntax::kString);
	syntax.addFlag("-chd","-channelDst",MSyntax::kString);

	// Clamping
	syntax.addFlag("-clr","-clampR",MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-clg","-clampG",MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-clb","-clampB",MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-cla","-clampA",MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-clh","-clampH",MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-cls","-clampS",MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-clv","-clampV",MSyntax::kDouble, MSyntax::kDouble);

	// Blending
	syntax.addFlag("-bl","-blend",MSyntax::kDouble);	// % blend, default to 100%
	syntax.addFlag("-blm","-blendMode",MSyntax::kString);
	syntax.addFlag("-mab","-masterBlend",MSyntax::kDouble);

	// Query
	syntax.addFlag("-mmr","-minmaxRGBA");
	syntax.addFlag("-arg","-averageRGBA");
	syntax.addFlag("-unc","-uniqueColours");
	syntax.addFlag("-poi","-samplePoint", MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag("-nsa","-numSamples", MSyntax::kLong);

	// 3D Texture
	syntax.addFlag("-t3n","-texture3DNode",MSyntax::kString);
	syntax.addFlag("-nsx","-noiseScaleX",MSyntax::kDouble);
	syntax.addFlag("-nsy","-noiseScaleY",MSyntax::kDouble);
	syntax.addFlag("-nsz","-noiseScaleZ",MSyntax::kDouble);
	syntax.addFlag("-t3o","-texture3DOffset",MSyntax::kDouble,MSyntax::kDouble, MSyntax::kDouble);

	// Normals->RGB
	syntax.addFlag("-spa","-space",MSyntax::kString);
	syntax.addFlag("-typ","-type",MSyntax::kString);

	syntax.useSelectionAsDefault(false);
	syntax.enableQuery(false);
	syntax.enableEdit(false);
	//status = syntax.setObjectType(MSyntax::kStringObjects, 0, 1);

	return syntax;
}

const char*
VertexPolyColourCommand::ParseArguments(const MArgList& args)
{
	static const char* error_noArgs = "No parameters specified. -help for available flags.";

	MArgDatabase argData(syntax(), args);

	m_isVerbose = argData.isFlagSet("-verbose");
	m_isUndoable = !argData.isFlagSet("-noUndo");

	int operation = 0;
	if (getArgValue(argData, "-op", "-operation", operation))
	{
		if (operation >= 0 && operation < OP_MAX)
			m_operation = (Operation)operation;
		else
		{
			return "Invalid operation";
		}
	}

	int forceComponent = 0;
	if (getArgValue(argData, "-fc", "-forceComponent", forceComponent))
	{
		if (forceComponent >= 0 && forceComponent < CM_MAX)
			m_forcedComponentMode = (ComponentMode)forceComponent;
		else
		{
			return "Invalid forced component type";
		}
	}

	getArgValue(argData, "-frc","-forceReadColourSet", m_forceReadColourSetName);
	getArgValue(argData, "-fwc","-forceWriteColourSet", m_forceWriteColourSetName);

	switch (m_operation)
	{
	case OP_ApplyRGBA:
		m_forceViewportRefresh = true;
		ParseChannelMaskArguments(args, argData);
		ParseRGBAChannelsArguments(args, argData);
		ParseBlendArguments(args, argData);
		//ParsePaintArguments(args, argData);
		ParseMasterBlendArguments(args, argData);
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		break;
	case OP_ApplyHSVA:
		m_forceViewportRefresh = true;
		ParseChannelMaskArguments(args, argData);
		ParseHSVAChannelsArguments(args, argData);
		ParseBlendArguments(args, argData);
		//ParsePaintArguments(args, argData);
		ParseMasterBlendArguments(args, argData);
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		break;
	case OP_ApplyColourArray:
		if (argData.isFlagSet("-colorArray"))
		{
			unsigned int flagIndex = args.flagIndex("-car", "-colorArray");
			if (flagIndex != MArgList::kInvalidArgIndex)
			{
				flagIndex++;
				if (args.length() > flagIndex)
				{
					args.get(flagIndex, m_colorArray);
				}
			}
		}
		m_forcedComponentMode = CM_Vertex;
		break;
	case OP_ApplyChannelCopy:
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		ParseChannelCopySwapArguments(args, argData);
		m_isReadDestColoursFromNode = argData.isFlagSet("-nodeDest");
		//m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		m_isReadColoursFromNodeDest = argData.isFlagSet("-nodeSource2");
		m_isReadDestColoursFromNodeSource = argData.isFlagSet("-nodeDestSource");
		break;
	case OP_ApplyChannelSwap:
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		ParseChannelCopySwapArguments(args, argData);
		break;
	case OP_ApplyRandomComponents:
		m_forceViewportRefresh = true;
		m_blendingEnabled = true;
		ParseChannelMaskArguments(args, argData);
		ParseMasterBlendArguments(args, argData);
		ParseBlendArguments(args, argData);
		//ParseRandomiseArguments(args, argData);
		ParseRGBAChannelsArguments(args, argData);
		ParseHSVAChannelsArguments(args, argData);
		//m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		m_isNodeWriteDest = argData.isFlagSet("-nodeWriteDest");
		if (m_isNodeWriteDest)
		{
			m_forceViewportRefresh = false;
		}
		break;
	case OP_AdjustRGBA:
		m_blendingEnabled = false;
		m_forceViewportRefresh = true;
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		ParseChannelMaskArguments(args, argData);
		ParseRGBAChannelsArguments(args, argData);
		break;
	case OP_AdjustHSVA:
		m_blendingEnabled = false;
		m_forceViewportRefresh = true;
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		ParseChannelMaskArguments(args, argData);
		ParseHSVAChannelsArguments(args, argData);
		break;
	case OP_AdjustLevels:
		m_blendingEnabled = false;
		m_forceViewportRefresh = true;
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		ParseChannelMaskArguments(args, argData);
		ParseLevelsArguments(args, argData);
		break;
	case OP_AdjustBrightnessContrast:
		m_forceViewportRefresh = true;
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		ParseChannelMaskArguments(args, argData);
		ParseBCArguments(args, argData);
		//ParsePaintArguments(args, argData);
		break;
	case OP_AdjustPosterize:
		ParseChannelMaskArguments(args, argData);
		ParsePosterizeArguments(args, argData);
		m_blendingEnabled = false;
		m_forceViewportRefresh = true;
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		break;
	case OP_AdjustClamp:
		ParseChannelMaskArguments(args, argData);
		ParseClampArguments(args, argData);
		m_blendingEnabled = false;
		break;
	case OP_AdjustSaturate:
		ParseChannelMaskArguments(args, argData);
		ParseClampArguments(args, argData);
		m_blendingEnabled = false;
		break;
	case OP_AdjustRandomiseComponents:
		//m_forceViewportRefresh = true;
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		ParseMasterBlendArguments(args, argData);
		//ParseRandomiseArguments(args, argData);
		ParseRGBAChannelsArguments(args, argData);
		ParseHSVAChannelsArguments(args, argData);
		//m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		m_isNodeWriteDest = argData.isFlagSet("-nodeWriteDest");
		if (m_isNodeWriteDest)
		{
			m_forceViewportRefresh = false;
		}
		break;
	case OP_Query:
		ParseQueryArguments(args, argData);
		m_applyEnabled = false;
		m_compositeEnabled = false;
		break;
	case OP_QuerySamplePointAverage:
		getArgValue(argData, "-poi","-samplePoint", m_samplePoint);
		getArgValue(argData, "-nsa","-numSamples", m_numSamples);
		m_numSamples = ClampUInt((unsigned int)1, (unsigned int)1000, m_numSamples);
		m_getComponentPositions = true;
		m_forcedComponentMode = CM_Vertex;
		m_applyEnabled = false;
		m_compositeEnabled = false;
		break;
	case OP_NodeWrite:
		m_applyEnabled = false;
		m_compositeEnabled = false;
		break;
	case OP_NodeRemove:
		m_applyEnabled = false;
		m_compositeEnabled = false;
		break;
	case OP_DestNodeWrite:
		m_applyEnabled = false;
		m_compositeEnabled = false;
		break;
	case OP_FilterBlur:
		ParseChannelMaskArguments(args, argData);
		ParseBlendArguments(args, argData);
		//m_forceViewportRefresh = true;
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_forcedComponentMode = CM_VertexFace;
		ParseMasterBlendArguments(args, argData);
		//m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		//m_isReadDestColoursFromNode = argData.isFlagSet("-nodeDest");
		m_isNodeWriteDest = argData.isFlagSet("-nodeWriteDest");
		if (m_isNodeWriteDest)
		{
			m_forceViewportRefresh = false;
		}
		break;
	case OP_FilterSharpen:
	case OP_FilterErode:
	case OP_FilterDilate:
	case OP_FilterCurvature:
		//ParseFilterStrength(args, argData);
		//ParsePaintArguments(args, argData);
		ParseChannelMaskArguments(args, argData);
		ParseBlendArguments(args, argData);
		//m_forceViewportRefresh = true;
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_forcedComponentMode = CM_Vertex;
		ParseMasterBlendArguments(args, argData);
		//m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		//m_isReadDestColoursFromNode = argData.isFlagSet("-nodeDest");
		m_isNodeWriteDest = argData.isFlagSet("-nodeWriteDest");
		if (m_isNodeWriteDest)
		{
			m_forceViewportRefresh = false;
		}
		break;
		/*ParseFilterStrength(args, argData);
		ParsePaintArguments(args, argData);
		m_readSourceColours = false;
		m_applyEnabled = false;
		m_blendingEnabled = false;
		break;*/
	case OP_FilterInvert:
		ParseChannelMaskArguments(args, argData);
		ParseInvertArguments(args, argData);
		ParseBlendArguments(args, argData);
		m_forceViewportRefresh = true;
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		break;
	case OP_ApplyGradient:
		ParseChannelMaskArguments(args, argData);
		ParseGradientArguments(args, argData);
		ParseBlendArguments(args, argData);
		ParseMasterBlendArguments(args, argData);
		m_getComponentPositions = true;
		m_forceViewportRefresh = true;
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		break;
	case OP_ConvNormalsToColour:
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		getArgValue(argData, "-spa","-space", m_space);
		break;
	case OP_ConvPositionsToColour:
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		getArgValue(argData, "-spa","-space", m_space);
		m_getComponentPositions = true;
		break;
	case OP_NormaliseRGB:
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		break;
	case OP_Facetise:
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		m_forcedComponentMode = CM_VertexFace;
		break;
	case OP_ConvUVsToColour:
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_blendingEnabled = false;
		ParseChannelMaskArguments(args, argData);
		m_getComponentUVs = true;
		break;
	case OP_Sample3DTexture:
		m_readSourceColours = true;
		m_applyEnabled = true;
		m_blendingEnabled = true;
		//m_forcedComponentMode = CM_Vertex;
		m_getComponentPositions = true;
		ParseChannelMaskArguments(args, argData);
		ParseBlendArguments(args, argData);
		ParseMasterBlendArguments(args, argData);
		Parse3DTextureArguments(args, argData);
		//m_forceViewportRefresh = true;
		//m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		m_isNodeWriteDest = argData.isFlagSet("-nodeWriteDest");
		if (m_isNodeWriteDest)
		{
			m_forceViewportRefresh = false;
		}
		break;
	case OP_BlendNodeColours:
		m_forceViewportRefresh = true;
		m_applyEnabled = true;
		//m_forcedComponentMode = CM_Vertex;
		ParseMasterBlendArguments(args, argData);
		m_isReadColoursFromNode = argData.isFlagSet("-nodeSource");
		m_isReadDestColoursFromNode = argData.isFlagSet("-nodeDest");
		break;
	case OP_UpdateLayerComposite:
		m_applyEnabled = false;
		m_readSourceColours = false;
		m_forceViewportRefresh = false;
		m_forcedComponentMode = CM_VertexFace;
		//m_compositeEnabled = false;
		break;
	default:
	case OP_PassThru:
		break;
	}

	return NULL;
}

void
VertexPolyColourCommand::ParseChannelMaskArguments(const MArgList& args, MArgDatabase& argData)
{
	MString channels;
	if (getArgValue(argData, "-cmk","-channelMask", channels))
	{
		m_channelMaskRGBA = 0;
		if ((channels.index('R') >= 0) || (channels.index('r') >= 0))
			m_channelMaskRGBA|=RED;
		if ((channels.index('G') >= 0) || (channels.index('g') >= 0))
			m_channelMaskRGBA|=GRN;
		if ((channels.index('B') >= 0) || (channels.index('b') >= 0))
			m_channelMaskRGBA|=BLU;
		if ((channels.index('A') >= 0) || (channels.index('a') >= 0))
			m_channelMaskRGBA|=ALP;
	}
}

void
VertexPolyColourCommand::ParseChannelCopySwapArguments(const MArgList& args, MArgDatabase& argData)
{
	MString channelName;
	if (getArgValue(argData,"-chs","-channelSrc", channelName))
	{
		m_channelSrc = getColorChannel(channelName.asChar());
	}
	if (getArgValue(argData,"-chd","-channelDst", channelName))
	{
		m_channelDst = getColorChannel(channelName.asChar());
	}
}

void
VertexPolyColourCommand::ParseClampArguments(const MArgList& args, MArgDatabase& argData)
{
	if (getArgValue(argData, "-clr", "-clampR", m_clampR))
		m_maskClamp|=RED;
	if (getArgValue(argData, "-clg", "-clampG", m_clampG))
		m_maskClamp|=GRN;
	if (getArgValue(argData, "-clb", "-clampB", m_clampB))
		m_maskClamp|=BLU;
	if (getArgValue(argData, "-cla", "-clampA", m_clampA))
		m_maskClamp|=ALP;
	if (getArgValue(argData, "-clh", "-clampH", m_clampH))
		m_maskClamp|=HUE;
	if (getArgValue(argData, "-cls", "-clampS", m_clampS))
		m_maskClamp|=SAT;
	if (getArgValue(argData, "-clv", "-clampV", m_clampV))
		m_maskClamp|=VAL;
}

void
VertexPolyColourCommand::Parse3DTextureArguments(const MArgList& args, MArgDatabase& argData)
{
	getArgValue(argData, "-nsx", "-noiseScaleX", m_noiseScale[0]);
	getArgValue(argData, "-nsy", "-noiseScaleY", m_noiseScale[1]);
	getArgValue(argData, "-nsz", "-noiseScaleZ", m_noiseScale[2]);

	getArgValue(argData, "-t3o", "-texture3DOffset", m_3DTextureOffset);
	getArgValue(argData, "-t3n", "-texture3DNode", m_3DTextureName);
}

void
VertexPolyColourCommand::ParseRandomiseArguments(const MArgList& args, MArgDatabase& argData)
{
	getArgValue(argData, "-rnr", "-randomRGB", m_randomiseRGB);
	getArgValue(argData, "-rna", "-randomAlpha", m_randomiseAlpha);
	getArgValue(argData, "-rnh", "-randomHSV", m_randomiseHSV);
}

void
VertexPolyColourCommand::ParseMasterBlendArguments(const MArgList& args, MArgDatabase& argData)
{
	getArgValue(argData,"-mas","-masterBlend", m_masterBlendAmount);
}

void
VertexPolyColourCommand::ParsePaintArguments(const MArgList& args, MArgDatabase& argData)
{
	m_isPaint = argData.isFlagSet("-paint");
	if (m_isPaint)
	{
		if (!getArgValue(argData,"-pao","-object", m_paintObjectName))
			m_isPaint = false;
		if (!getArgValue(argData,"-pai","-vertexIndex", m_paintVertexIndex))
			m_isPaint = false;
	}
}

void
VertexPolyColourCommand::ParseBCArguments(const MArgList& args, MArgDatabase& argData)
{
	if (getArgValue(argData, "-c", "-contrast", m_contrastR))
	{
		m_contrastG = m_contrastR;
		m_contrastB = m_contrastR;
		m_contrastA = 0.0;
	}
	getArgValue(argData, "-cor", "-contrastR", m_contrastR);
	getArgValue(argData, "-cog", "-contrastG", m_contrastG);
	getArgValue(argData, "-cob", "-contrastB", m_contrastB);
	getArgValue(argData, "-coa", "-contrastA", m_contrastA);
	getArgValue(argData, "-coh", "-contrastH", m_contrastH);
	getArgValue(argData, "-cos", "-contrastS", m_contrastS);
	getArgValue(argData, "-cov", "-contrastV", m_contrastV);
	getArgValue(argData, "-bri", "-brightness", m_brightness);
}

void
VertexPolyColourCommand::ParseLevelsArguments(const MArgList& args, MArgDatabase& argData)
{
	if (argData.isFlagSet("-levels"))
	{
		double inblack,inwhite,ingamma,outblack,outwhite;
		argData.getFlagArgument("-levels", 0, inblack);
		argData.getFlagArgument("-levels", 1, inwhite);
		argData.getFlagArgument("-levels", 2, ingamma);
		argData.getFlagArgument("-levels", 3, outblack);
		argData.getFlagArgument("-levels", 4, outwhite);
		m_levels.inBlack=(float)inblack;
		m_levels.inWhite=(float)inwhite;
		m_levels.inGamma=(float)ingamma;
		m_levels.outBlack=(float)outblack;
		m_levels.outWhite=(float)outwhite;
	}
}

void
VertexPolyColourCommand::ParseRGBAChannelsArguments(const MArgList& args, MArgDatabase& argData)
{
	// RGB Colorspace
	if (getArgValue(argData, "-r", "-colorR", m_colRGB.r) || argData.isFlagSet("-channelR"))
		m_maskRGB|=RED;
	if (getArgValue(argData, "-g", "-colorG", m_colRGB.g) || argData.isFlagSet("-channelG"))
		m_maskRGB|=GRN;
	if (getArgValue(argData ,"-b", "-colorB", m_colRGB.b) || argData.isFlagSet("-channelB"))
		m_maskRGB|=BLU;
	if (getArgValue(argData,"-a","-colorA",m_colRGB.a) || argData.isFlagSet("-channelA"))
		m_maskRGB|=ALP;
}

void
VertexPolyColourCommand::ParseHSVAChannelsArguments(const MArgList& args, MArgDatabase& argData)
{
	// HSV Colorspace
	if (getArgValue(argData,"-h","-colorH",m_colHSV.h) || argData.isFlagSet("-channelH"))
		m_maskHSV|=HUE;
	if (getArgValue(argData,"-s","-colorS",m_colHSV.s) || argData.isFlagSet("-channelS"))
		m_maskHSV|=SAT;
	if (getArgValue(argData,"-v","-colorV",m_colHSV.v) || argData.isFlagSet("-channelV"))
		m_maskHSV|=VAL;
	if (getArgValue(argData,"-a","-colorA",m_colHSV.a) || argData.isFlagSet("-channelA"))
		m_maskHSV|=ALP;
}

void
VertexPolyColourCommand::ParseBlendArguments(const MArgList& args, MArgDatabase& argData)
{
	m_blendingEnabled = true;
	getArgValue(argData,"-bl","-blend",m_blendAmount);
	MString blendModeName;
	if (getArgValue(argData,"-blm","-blendMode", blendModeName))
	{
		m_blendMode = getBlendMode(blendModeName);
	}

	if (m_blendMode == BM_Replace && m_blendAmount == 1.0f)
		m_blendingEnabled = false;
}

void
VertexPolyColourCommand::ParseQueryArguments(const MArgList& args, MArgDatabase& argData)
{
	m_isMinMaxRGBA = argData.isFlagSet("-minmaxRGBA");
	m_isAverageRGBA = argData.isFlagSet("-averageRGBA");
	m_isUniqueColours = argData.isFlagSet("-uniqueColours");
}

void
VertexPolyColourCommand::ParseGradientArguments(const MArgList& args, MArgDatabase& argData)
{
	MString gradientType;
	if (getArgValue(argData,"-grt","-gradientType",gradientType))
	{
		if (gradientType == "X")
			m_gradientParams.type = GT_AXIS_X;
		if (gradientType == "Y")
			m_gradientParams.type = GT_AXIS_Y;
		if (gradientType == "Z")
			m_gradientParams.type = GT_AXIS_Z;
		if (gradientType == "Spherical")
			m_gradientParams.type = GT_SPHERICAL;

		getArgValue(argData,"-grn","-gradientRampNode", m_gradientParams.rampNodeName);

		if (m_gradientParams.type != GT_None)
		{
			getArgValue(argData, "-gr1","-gradCol1", m_gradientParams.colour1);
			getArgValue(argData, "-gr2","-gradCol2", m_gradientParams.colour2);
			getArgValue(argData, "-gro","-gradientOffset", m_gradientParams.offset);
			m_gradientParams.isReverse = argData.isFlagSet("-gradientReverse");

			if (m_gradientParams.type == GT_SPHERICAL)
			{
				m_gradientParams.isOverrideCenter = argData.isFlagSet("-gradientOverrideCenter");
				m_gradientParams.isOverrideRadius = argData.isFlagSet("-gradientOverrideRadius");
				getArgValue(argData, "-grp","-gradientStartPos", m_gradientParams.centerWorldPosition);
				getArgValue(argData, "-grq","-gradientRadius", m_gradientParams.radius);
			}

			if (getArgValue(argData, "-gwm","-gradientWorldBoundsMin", m_gradientParams.worldMin))
			{
				m_gradientParams.useWorldBounds = true;
			}
			if (getArgValue(argData, "-gwx","-gradientWorldBoundsMax", m_gradientParams.worldMax))
			{
				m_gradientParams.useWorldBounds = true;
			}


			getArgValue(argData, "-grs","-gradientRepeats", m_gradientParams.numRepeats);
			MString gradientRepeat;
			if (getArgValue(argData, "-grr","-gradientRepeat", gradientRepeat))
			{
				if (gradientRepeat == "Wrap")
					m_gradientParams.repeatMode = REPEAT_WRAP;
				if (gradientRepeat == "Clamp")
					m_gradientParams.repeatMode = REPEAT_CLAMP;
				if (gradientRepeat == "Mirror")
					m_gradientParams.repeatMode = REPEAT_MIRROR;
			}
		}
	}
}

void
VertexPolyColourCommand::ParseInvertArguments(const MArgList& args, MArgDatabase& argData)
{
	MString invertChannels;
	if (getArgValue(argData, "-inv","-invert", invertChannels))
	{
		if ((invertChannels.index('r') >= 0) || (invertChannels.index('R') >= 0))
			m_invertMaskRGBA|=RED;
		if ((invertChannels.index('g') >= 0) || (invertChannels.index('G') >= 0))
			m_invertMaskRGBA|=GRN;
		if ((invertChannels.index('b') >= 0) || (invertChannels.index('B') >= 0))
			m_invertMaskRGBA|=BLU;
		if ((invertChannels.index('a') >= 0) || (invertChannels.index('A') >= 0))
			m_invertMaskRGBA|=ALP;
		if ((invertChannels.index('h') >= 0) || (invertChannels.index('H') >= 0))
			m_invertMaskHSV|=HUE;
		if ((invertChannels.index('s') >= 0) || (invertChannels.index('S') >= 0))
			m_invertMaskHSV|=SAT;
		if ((invertChannels.index('v') >= 0) || (invertChannels.index('V') >= 0))
			m_invertMaskHSV|=VAL;
	}
}

void
VertexPolyColourCommand::ParsePosterizeArguments(const MArgList& args, MArgDatabase& argData)
{
	getArgValue(argData,"-poa","-posterizeAmount",m_posterizeAmount);

	MString channels;
	if (getArgValue(argData, "-pos","-posterize", channels))
	{
		if ((channels.index('r') >= 0) || (channels.index('R') >= 0))
			m_posterizeMaskRGBA|=RED;
		if ((channels.index('g') >= 0) || (channels.index('G') >= 0))
			m_posterizeMaskRGBA|=GRN;
		if ((channels.index('b') >= 0) || (channels.index('B') >= 0))
			m_posterizeMaskRGBA|=BLU;
		if ((channels.index('a') >= 0) || (channels.index('A') >= 0))
			m_posterizeMaskRGBA|=ALP;
		if ((channels.index('h') >= 0) || (channels.index('H') >= 0))
			m_posterizeMaskHSV|=HUE;
		if ((channels.index('s') >= 0) || (channels.index('S') >= 0))
			m_posterizeMaskHSV|=SAT;
		if ((channels.index('v') >= 0) || (channels.index('V') >= 0))
			m_posterizeMaskHSV|=VAL;
	}
}

VertexPolyColourCommand::~VertexPolyColourCommand()
{
	if (m_savedObjectSelectionMask != NULL)
	{
		delete m_savedObjectSelectionMask;
		m_savedObjectSelectionMask = NULL;
	}
	if (m_savedComponentSelectionMask != NULL)
	{
		delete m_savedComponentSelectionMask;
		m_savedComponentSelectionMask = NULL;
	}

	// Delete all undo info
	{
		std::vector<MDGModifier*>::reverse_iterator riter;
		for (riter = m_undos.rbegin(); riter != m_undos.rend(); ++riter)
		{
			delete (*riter);
		}
		m_undos.clear();
	}

	// Delete all jobs
	{
		std::vector<VertexColourJob*>::reverse_iterator riter;
		for (riter = m_jobs.rbegin(); riter != m_jobs.rend(); ++riter)
		{
			delete (*riter);
		}
		m_jobs.clear();
	}

	// Delete all mesh data
	{
		std::vector<MeshData*>::reverse_iterator riter;
		for (riter = m_meshes.rbegin(); riter != m_meshes.rend(); ++riter)
		{
			delete (*riter);
		}
		m_meshes.clear();
	}
}

void*
VertexPolyColourCommand::creator()
{
	return new VertexPolyColourCommand;
}

bool
VertexPolyColourCommand::isUndoable() const
{
	return true;
}

bool
VertexPolyColourCommand::hasSyntax() const
{
	return true;
}

void
VertexPolyColourCommand::OutputText(const char* text) const
{
#if MAYA60 || MAYA65
	MGlobal::displayInfo(text);
#else
	displayInfo(text);
#endif
}

void
VertexPolyColourCommand::OutputText(const MString& text) const
{
#if MAYA60 || MAYA65
	MGlobal::displayInfo(text);
#else
	displayInfo(text);
#endif
}

void
VertexPolyColourCommand::SaveSelection()
{
	// Get the current selection
	m_savedSelectionMode = MGlobal::selectionMode();
	//m_savedObjectSelectionMask = new MSelectionMask(MGlobal::objectSelectionMask());
	//m_savedComponentSelectionMask = new MSelectionMask(MGlobal::componentSelectionMask());
	MGlobal::getActiveSelectionList (m_savedSelection);
#if MAYA_API_VERSION > 600
	MGlobal::getHiliteList(m_savedHiliteList);
#endif
}

void
VertexPolyColourCommand::RestoreSelection()
{
	MStatus status;

	status = MGlobal::clearSelectionList();
	status = MGlobal::setSelectionMode(m_savedSelectionMode);
	//status = MGlobal::setObjectSelectionMask(*m_savedObjectSelectionMask);
	//status = MGlobal::setComponentSelectionMask(*m_savedComponentSelectionMask);
#if MAYA_API_VERSION > 600
	status = MGlobal::setHiliteList(m_savedHiliteList);
#endif
	status = MGlobal::setActiveSelectionList(m_savedSelection);
}

MStatus
VertexPolyColourCommand::redoIt()
{
	MStatus status = MS::kSuccess;

	std::vector<MDGModifier*>::reverse_iterator riter;
	for ( riter = m_undos.rbegin(); riter != m_undos.rend(); ++riter )
	{
		status=(*riter)->doIt();
	}

	return status;
}

MStatus
VertexPolyColourCommand::undoIt()
{
	MStatus status = MS::kSuccess;

	std::vector<MDGModifier*>::reverse_iterator riter;
	for ( riter = m_undos.rbegin(); riter != m_undos.rend(); ++riter )
	{
		status=(*riter)->undoIt();
	}

	return MS::kSuccess;
}

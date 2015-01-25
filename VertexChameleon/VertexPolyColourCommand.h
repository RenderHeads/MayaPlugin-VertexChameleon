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

#ifndef VERTEXPOLYCOLOURCOMMAND_H
#define VERTEXPOLYCOLOURCOMMAND_H

#include <iostream>
#include <vector>

#include "Timer.h"
#include "ColorUtils.h"
#include "VertexColourJob.h"
#include "GradientParameters.h"

#define TIMINGS

enum Operation
{
	// A null operation
	OP_PassThru = 0,

	OP_NodeWrite,
	OP_NodeRemove,
	OP_DestNodeWrite,
	OP_BlendNodeColours,

	// Operations that don't change colours, just return information
	OP_Query = 100,
	OP_QuerySamplePointAverage = 101,

	// Convolution filter operations
	OP_FilterBlur = 200,
	OP_FilterSharpen,
	OP_FilterErode,
	OP_FilterDilate,
	OP_FilterInvert,
	OP_FilterCurvature,

	// All operations that adjust colours
	OP_AdjustRGBA = 300,
	OP_AdjustHSVA,
	OP_AdjustColourize,
	OP_AdjustChannelMixer_TODO,
	OP_AdjustLevels,
	OP_AdjustBrightnessContrast,
	OP_AdjustPosterize,
	OP_AdjustClamp,
	OP_AdjustRandomiseComponents,
	OP_AdjustSaturate,

	// All operations that create colours
	OP_ApplyRGBA = 400,
	OP_ApplyHSVA,
	OP_ApplyGradient,
	OP_ApplyChannelCopy,
	OP_ApplyChannelSwap,
	OP_ApplyRandomComponents,
	OP_ApplyColourArray,

	OP_ConvNormalsToColour = 500,
	OP_NormaliseRGB = 501,
	OP_Facetise = 502,
	OP_ConvUVsToColour = 503,
	OP_ConvPositionsToColour = 504,

	OP_Sample3DTexture = 900,
	OP_UpdateLayerComposite,

	OP_MAX,
};

class MeshData;
class VertexColourJob;

class VertexPolyColourCommand :
	public MPxCommand
{
public:
	VertexPolyColourCommand();
	virtual     ~VertexPolyColourCommand();

	MStatus			doIt(const MArgList& args);
	MStatus			redoIt();
	MStatus			undoIt();
	bool			isUndoable() const;
	bool			hasSyntax() const;
	static void*	creator();
	static MSyntax	newSyntax();

private:

	// Initialise
	const char*		ParseArguments(const MArgList& args);
	void			ParseQueryArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseBlendArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseMasterBlendArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseChannelMaskArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseRGBAChannelsArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseHSVAChannelsArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseBCArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseLevelsArguments(const MArgList& args, MArgDatabase& argData);
	void			ParsePaintArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseGradientArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseInvertArguments(const MArgList& args, MArgDatabase& argData);
	void			ParsePosterizeArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseRandomiseArguments(const MArgList& args, MArgDatabase& argData);
	void			Parse3DTextureArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseClampArguments(const MArgList& args, MArgDatabase& argData);
	void			ParseChannelCopySwapArguments(const MArgList& args, MArgDatabase& argData);
	MStatus			Initialise(const MArgList& args);

	// Helper
	void			OutputText(const char* text) const;
	void			OutputText(const MString& text) const;
	void			SaveSelection();
	void			RestoreSelection();
#ifdef TIMINGS
	void			PrintProfileStats();
#endif

	// Pipeline
	MStatus			ProcessMeshes();
	void			CreateJobListFromPaint();
	void			CreateJobListFromSelection(MSelectionList& selection);
	void			AddJob(MDagPath& dagPath, MObject& component);
	void			PrepareMesh(VertexColourJob& job);
	void			GetSourceColours();
	void			GetSourceColours(VertexColourJob& job);
	void			BlendColours();
	void			DoMasterBlend();
	void			ApplyChannelMask();
	void			ApplyChannelMask(VertexColourJob& job);
	void			Apply();
	void			Apply(VertexColourJob& job, const MString* colorSetName);
	void			ApplyNothing(VertexColourJob& job);

	// Color Node
	bool			FindNodeOnMesh(MDagPath& dagPath,MObject& result);
	bool			ReadColoursFromNode(MDagPath& dagPath, MColorArray& colors, bool isSource);
	void			WriteColoursToNode(MDagPath& dagPath, MColorArray& colors, bool isSource);
	void			CreateNodeOnMesh(MDagPath& dagPath, MObject& ourNode);
	void			RemoveColoursNode(MDagPath& dagPath, MColorArray& colors);

	// Operations
	void			DoQueries();
	void			DoQuerySamplePointAverage();
	void			WriteColoursToNode(bool isSource);
	void			RemoveColoursNode();
	void			ReplaceRGBAColours();
	void			ReplaceHSVAColours();
	void			ReplaceColoursFromArray();
	void			AdjustRGBAColours();
	void			AdjustHSVAColours();
	void			AdjustBrightnessContrast();
	void			AdjustLevels();
	void			AdjustPosterize();
	void			AdjustClamp();
	void			AdjustSaturate();
	void			AdjustRandomiseComponents();
	void			ApplyRandomComponents();
	void			CreateGradientColours();
	void			CreateAxisGradientColours(VertexColourJob& job, MRampAttribute* rampColor, MRampAttribute* rampAlpha);
	void			CreateSphericalGradientColours(VertexColourJob& job, MRampAttribute* rampColor, MRampAttribute* rampAlpha);
	void			DoFilters();
	void			DoFilterBlur(VertexColourJob& job);
	void			DoFilterSharpen(VertexColourJob& job);
	void			DoFilterErode(VertexColourJob& job);
	void			DoFilterDilate(VertexColourJob& job);
	void			DoDensity(VertexColourJob& job);
	void			DoCurvature(VertexColourJob& job);
	void			DoSample3DTexture(VertexColourJob& job);
	void			ConvertNormalsToColours();
	void			ConvertNormalsToColours(VertexColourJob& job);
	void			ConvertUVsToColours();
	void			ConvertUVsToColours(VertexColourJob& job);
	void			ConvertPositionsToColours();
	void			ConvertPositionsToColours(VertexColourJob& job);
	void			ConvertFacetise();
	void			ConvertFacetise(VertexColourJob& job);
	void			NormaliseRGB();
	void			ChannelSwap();
	void			ChannelCopy();
#if MAYA_API_VERSION >= 700
	void			GatherLayersData();
	void			CompositeLayers();
	void			CompositeLayers(VertexColourJob& job);
#endif
	MString			GetFinalLayerName() const;

	// Component and colour extraction
	//		Object selection
	void			GetObjectColours(VertexColourJob& job);
	void			GetObjectVertexColours(VertexColourJob& job);
	void			GetObjectVertexFaceColours(VertexColourJob& job);

	//		Vertex selection
	void			GetColoursFromVertices(VertexColourJob& job);
	void			GetVertexColoursFromVertices(VertexColourJob& job);
	void			GetVertexFaceColoursFromVertices(VertexColourJob& job);
	void			GetVertexFaceColoursFromVerticesSLOW(VertexColourJob& job);

	//		Vertex-Face selection
	void			GetColoursFromVertexFaces(VertexColourJob& job);
	void			GetVertexFaceColoursFromVertexFaces(VertexColourJob& job);

	//		Face selection
	void			GetColoursFromFaces(VertexColourJob& job);
	void			GetFaceColoursFromVertices(VertexColourJob& job);
	void			GetFaceColoursFromVertexFaces(VertexColourJob& job);

	//		Edge selection
	void			GetColoursFromEdges(VertexColourJob& job);
	void			GetEdgeColoursFromVertices(VertexColourJob& job);
	void			GetEdgeColoursFromVertexFaces(VertexColourJob& job);
	void			GetEdgeColoursFromVertexFacesSLOW(VertexColourJob& job);

	// General Parameters
	bool			m_isVerbose;
	bool			m_isUndoable;
	ComponentMode	m_forcedComponentMode;
	Operation		m_operation;
	bool			m_applyEnabled;
	bool			m_compositeEnabled;
	bool			m_readSourceColours;
	bool			m_isReadColoursFromNode, m_isReadColoursFromNodeDest;
	bool			m_isReadDestColoursFromNode, m_isReadDestColoursFromNodeSource;
	bool			m_isNodeWriteDest;
	bool			m_blendingEnabled;
	float			m_blendAmount;
	BlendMode		m_blendMode;
	float			m_masterBlendAmount;
	bool			m_forceViewportRefresh;
	bool			m_getComponentPositions;
	bool			m_getComponentUVs;
	MString			m_forceReadColourSetName, m_forceWriteColourSetName;
	unsigned int	m_channelMaskRGBA;
	MDoubleArray	m_colorArray;

	// 3D Texture
	MString			m_3DTextureName;
	float3			m_noiseScale;
	float3			m_3DTextureOffset;

	// Paint
	bool			m_isPaint;
	MString			m_paintObjectName;
	unsigned int	m_paintVertexIndex;

	// Randomise Components
	//RandomComponent	m_randomComponentMode;
	//unsigned int	m_maskRandomRGBA, m_maskRandomHSV;
	float3			m_randomiseRGB, m_randomiseHSV;
	float			m_randomiseAlpha;


	// Invert
	unsigned int	m_invertMaskRGBA, m_invertMaskHSV;

	// Posterize
	unsigned int	m_posterizeMaskRGBA, m_posterizeMaskHSV;
	float			m_posterizeAmount;

	// Adjust Levels
	LevelsParams	m_levels;

	// Adjust Brightness + Contrast
	float			m_brightness;
	float			m_contrastR, m_contrastG, m_contrastB, m_contrastA;
	float			m_contrastH, m_contrastS, m_contrastV;

	// Apply RGBA
	ColRGB			m_colRGB;
	unsigned int	m_maskRGB;

	// Apply HSVA
	ColHSV			m_colHSV;
	unsigned int	m_maskHSV;

	// Channel Copy/Swap
	ColorChannelMask	m_channelSrc, m_channelDst;

	// Clamp
	float2			m_clampR, m_clampG, m_clampB, m_clampA;
	float2			m_clampH, m_clampS, m_clampV;
	unsigned int	m_maskClamp;

	// Query
	bool			m_isMinMaxRGBA, m_isAverageRGBA, m_isUniqueColours;
	ColRGB			m_minRGBA,m_maxRGBA;
	double			m_averageR, m_averageG, m_averageB, m_averageA;
	unsigned long	m_numTotalColours;
	unsigned long	m_numUniqueColours;
	MColorArray		m_uniqueColours;
	unsigned int	m_numSamples;
	double3			m_samplePoint;

	// Facetise
	MString			m_space;
	MString			m_type;

	GradientParameters	m_gradientParams;

	std::vector<VertexColourJob*>		m_jobs;
	std::vector<MeshData*>				m_meshes;

	// Save & Restore selection
	MGlobal::MSelectionMode		m_savedSelectionMode;
	MSelectionMask*				m_savedObjectSelectionMask;
	MSelectionMask*				m_savedComponentSelectionMask;
	MSelectionList				m_savedSelection;
	MSelectionList				m_savedHiliteList;

	bool						m_isRestoreSelection;

	// Profiling
	Timer		m_timer, m_masterTimer;
	float		m_parseTime, m_loadTime, m_gatherTime, m_processTime, m_blendTime, m_applyTime, m_refreshTime, m_totalTime;

	// Undo & redo stack
	std::vector<MDGModifier*> m_undos;	// The stack of undo to perform.
};

#endif

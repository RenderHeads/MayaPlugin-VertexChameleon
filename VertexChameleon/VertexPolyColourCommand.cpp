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
#include <assert.h>
#include <float.h>
#include "Utility.h"
#include "Random.h"
#include "Blend.h"
#include "PolyColourNode.h"
#include "LayerData.h"
#include "MeshData.h"
#include "VertexColourJob.h"
#include "GradientParameters.h"
#include "VertexPolyColourCommand.h"

using namespace std;

MStatus
VertexPolyColourCommand::doIt( const MArgList& args )
{
	MStatus status;

#ifdef TIMINGS
	m_parseTime = m_loadTime = m_gatherTime = m_processTime = m_blendTime = m_applyTime = m_refreshTime = m_totalTime = 0;
	m_masterTimer.reset();
	m_timer.reset();
#endif

	SaveSelection();

	status = Initialise(args);

#ifdef TIMINGS
	m_parseTime = m_timer.getTime();
#endif

	if (status == MS::kSuccess)
	{
		status = ProcessMeshes();
	}

	if (m_isRestoreSelection)
		RestoreSelection();

#ifdef TIMINGS
	PrintProfileStats();
#endif

	return status;
}

MStatus
VertexPolyColourCommand::Initialise(const MArgList& args)
{
	// Parse arguments
	const char* errorMessage = ParseArguments(args);

	if (errorMessage)
	{
		displayError(errorMessage);
		return MS::kFailure;
	}

	return MS::kSuccess;
}

MStatus
VertexPolyColourCommand::ProcessMeshes()
{
#ifdef TIMINGS
	m_timer.reset();
#endif

	if (!m_isPaint)
		CreateJobListFromSelection(m_savedSelection);
	else
	{
		m_forceViewportRefresh = false;
		CreateJobListFromPaint();
	}

#if MAYA_API_VERSION >= 700
	GatherLayersData();
#endif

#ifdef TIMINGS
	m_loadTime = m_timer.getTime();
#endif

#ifdef TIMINGS
	m_timer.reset();
#endif
	if (m_readSourceColours)
		GetSourceColours();
#ifdef TIMINGS
	m_gatherTime = m_timer.getTime();
#endif

#ifdef TIMINGS
	m_timer.reset();
#endif

	if (m_isVerbose)
	{
		MString txt("Operation: ");
		txt += (int)m_operation;
		OutputText(txt);
	}
	switch (m_operation)
	{
	case OP_ApplyRGBA:
		ReplaceRGBAColours();
		break;
	case OP_ApplyHSVA:
		ReplaceHSVAColours();
		break;
	case OP_ApplyColourArray:
		ReplaceColoursFromArray();
		break;
	case OP_AdjustRGBA:
		AdjustRGBAColours();
		break;
	case OP_ApplyChannelCopy:
		ChannelCopy();
		break;
	case OP_ApplyChannelSwap:
		ChannelSwap();
		break;
	case OP_ApplyRandomComponents:
		ApplyRandomComponents();
		break;
	case OP_AdjustHSVA:
		AdjustHSVAColours();
		break;
	case OP_AdjustBrightnessContrast:
		AdjustBrightnessContrast();
		break;
	case OP_AdjustLevels:
		AdjustLevels();
		break;
	case OP_AdjustPosterize:
		AdjustPosterize();
		break;
	case OP_AdjustClamp:
		AdjustClamp();
		break;
	case OP_AdjustSaturate:
		AdjustSaturate();
		break;
	case OP_AdjustRandomiseComponents:
		AdjustRandomiseComponents();
		break;
	case OP_NodeWrite:
		WriteColoursToNode(true);
		break;
	case OP_NodeRemove:
		RemoveColoursNode();
		break;
	case OP_DestNodeWrite:
		WriteColoursToNode(false);
		break;
	case OP_Query:
		DoQueries();
		break;
	case OP_QuerySamplePointAverage:
		DoQuerySamplePointAverage();
		break;
	case OP_ApplyGradient:
		CreateGradientColours();
		break;
	case OP_FilterBlur:
	case OP_FilterSharpen:
	case OP_FilterErode:
	case OP_FilterDilate:
	case OP_FilterCurvature:
	case OP_FilterInvert:
	case OP_Sample3DTexture:
		DoFilters();
		break;
	case OP_ConvNormalsToColour:
		ConvertNormalsToColours();
		break;
	case OP_ConvUVsToColour:
		ConvertUVsToColours();
		break;
	case OP_ConvPositionsToColour:
		ConvertPositionsToColours();
		break;
	case OP_NormaliseRGB:
		NormaliseRGB();
		break;
	case OP_Facetise:
		ConvertFacetise();
		break;
	}
#ifdef TIMINGS
	m_processTime = m_timer.getTime();
#endif

	if (m_applyEnabled)
	{
#ifdef TIMINGS
		m_timer.reset();
#endif
		if (m_blendingEnabled)
		{
			BlendColours();
		}
		if (m_masterBlendAmount != 1.0f)
		{
			DoMasterBlend();
		}
#ifdef TIMINGS
		m_blendTime = m_timer.getTime();
#endif
	}

#ifdef TIMINGS
	m_timer.reset();
#endif
	if (m_applyEnabled)
	{
		if (m_channelMaskRGBA != RGBA)
		{
			ApplyChannelMask();
		}

		if (m_isNodeWriteDest)
		{
			WriteColoursToNode(false);
		}
		else
		{
			Apply();
		}
	}

#if MAYA_API_VERSION >= 700
	if (m_compositeEnabled)
	{
		CompositeLayers();
	}
#endif

#if MAYA_API_VERSION >= 700
	// Restore colour set back
	{
		MDGModifier* modifier = NULL;
		if (m_isUndoable)
			modifier = new MDGModifier;

		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			MeshData& meshData = *m_meshes[i];
			meshData.mesh->setCurrentColorSetName(meshData.currentColorSetName, modifier);
		}

		if (modifier != NULL)
		{
			m_undos.push_back(modifier);
		}
	}
#endif

#ifdef TIMINGS
	m_applyTime = m_timer.getTime();
#endif
/*
	if (m_applyEnabled)
	{
#ifdef TIMINGS
		m_timer.reset();
#endif
		if (m_forceViewportRefresh)
			M3dView::active3dView().refresh(false, true);
#ifdef TIMINGS
		m_refreshTime = m_timer.getTime();
#endif
	}
*/
	return MS::kSuccess;
}

void
VertexPolyColourCommand::Apply()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		MString* colorSetName = NULL;
		if (job.meshData->hasLayers && job.meshData->activeLayer != NULL)
		{
			colorSetName = &job.meshData->activeLayer->name;
		}

		if (job.meshData->writeColorSetName.length() > 0)
		{
			colorSetName = &job.meshData->writeColorSetName;
		}

		Apply(job, colorSetName);
	}

	/*
	MStatus status;
	m_savedSelection.clear();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		m_savedSelection.add(job.meshData->dagPath, job.components);
		switch (job.m_componentMode)
		{
		case CM_Vertex:
			break;
		}
	}

	status = MGlobal::clearSelectionList();
	status = MGlobal::setSelectionMode(MGlobal::kSelectComponentMode);
	status = MGlobal::setActiveSelectionList(m_savedSelection);*/
}

void
VertexPolyColourCommand::ApplyNothing(VertexColourJob& job)
{
	// Added this to fix a WEIRD bug that seems to be in Maya.  This isn't a great fix but it'll have to do for now.
	// Reproducing the bug
	// 1) Create a plane
	// 2) Using layer tool, add 2 layers
	// 3) Paint the top layer red and the bottom layer green
	// 3) Set the layer tool to Final Comp so it renders that
	// 4) Set the layer tool to active layer and set the active layer to the bottom layer
	// 5) Select some vertices and paint them blue
	// The bug is that the ENTIRE object will copy the colours from the top layer
	// So it seems that if we call this function during the compositing it all works fine.

	switch (job.m_componentMode)
	{
		case CM_VertexFace:
			{
				MColorArray mca;
				MIntArray fl, vl;
				job.meshData->mesh->setFaceVertexColors(mca, fl, vl);
			}
		break;
		case CM_Vertex:
			{
				MColorArray mca;
				MIntArray vl;
				job.meshData->mesh->setVertexColors(mca, vl);
			}
		break;
	}
}

void
VertexPolyColourCommand::Apply(VertexColourJob& job, const MString* colorSetName)
{
	MStatus status;

	MDGModifier* modifier = NULL;
	MDGModifier* modifier2 = NULL;
	MDGModifier* modifier3 = NULL;
	if (m_isUndoable)
	{
		modifier = new MDGModifier;
		modifier2 = new MDGModifier;
		modifier3 = new MDGModifier;
	}

#if MAYA_API_VERSION >= 700
	if (colorSetName != NULL)
	{
		status = job.meshData->mesh->setCurrentColorSetName(*colorSetName, modifier3);
		CHECK_MSTATUS(status);
	}
#endif

	switch (job.m_componentMode)
	{
	case CM_Vertex:
		if (m_isVerbose)
		{
			MString txt = "Apply Vertices: ";
			txt += (int)job.vertexIndices.length();
			//txt += " ";
			//txt += (int)job.dstColors.length();
			//txt += " ColorSet: ";
			//txt += job.meshData->mesh->currentColorSetName();
			OutputText(txt);
		}
#ifdef _DEBUG
		assert(job.dstColors.length() == job.srcColors.length());
		assert(job.dstColors.length() == job.vertexIndices.length());
#endif
		status = job.meshData->mesh->setVertexColors(job.dstColors, job.vertexIndices, modifier);
		CHECK_MSTATUS(status);
		//job.meshData->mesh->setSomeColors(job.vertexIndices, job.dstColors);

		//job.meshData->mesh->setColor(0, tcol);
		//job.meshData->mesh->updateSurface();
		//status = job.meshData->mesh->updateSurface();
		break;
	case CM_VertexFace:
		if (m_isVerbose)
		{
			MString txt = "Apply VertexFaces: ";
			txt += (int)job.vfVertList.length();
			//txt += " ColorSet: ";
			//txt += job.meshData->mesh->currentColorSetName();
			OutputText(txt);
			
			int l1 = job.srcColors.length();
			int l2 = job.dstColors.length();
			int l3 = job.vfVertList.length();
			int l4 = job.vfFaceList.length();
			int l5 = job.vertexIndices.length();

			assert(job.dstColors.length() == job.srcColors.length());
			assert(job.vfFaceList.length() == job.vfVertList.length());
			assert(job.dstColors.length() == job.vfFaceList.length());
		}
#ifdef _DEBUG
		assert(job.dstColors.length() == job.srcColors.length());
		assert(job.vfFaceList.length() == job.vfVertList.length());
		assert(job.dstColors.length() == job.vfFaceList.length());

		/*job.dstColors.setLength(11);
		job.vfFaceList.setLength(11);
		job.vfVertList.setLength(11);*/
#endif
		status = job.meshData->mesh->setFaceVertexColors(job.dstColors, job.vfFaceList, job.vfVertList, modifier);
		/*{
			// Added this to fix a WEIRD bug that seems to be in Maya.  This isn't a great fix but it'll have to do for now.
			// Reproducing the bug
			// 1) Create a plane
			// 2) Using layer tool, add 2 layers
			// 3) Paint the top layer red and the bottom layer green
			// 3) Set the layer tool to Final Comp so it renders that
			// 4) Set the layer tool to active layer and set the active layer to the bottom layer
			// 5) Select some vertices and paint them blue
			// The bug is that the ENTIRE object will copy the colours from the top layer
			MColorArray mca;
			MIntArray fl, vl;
			status = job.meshData->mesh->setFaceVertexColors(mca, fl, vl, modifier);
		}*/
		CHECK_MSTATUS(status);
		//status = job.meshData->mesh->updateSurface();
		break;
	default:
		break;
	}

	//ApplyNothing(job);

#if MAYA_API_VERSION >= 700
	if (colorSetName != NULL)
	{
		status = job.meshData->mesh->setCurrentColorSetName(job.meshData->currentColorSetName, modifier2);
	}
#endif

	if (modifier != NULL)
	{
		m_undos.push_back(modifier2);
		m_undos.push_back(modifier);
		m_undos.push_back(modifier3);
	}

	//job.meshData->mesh->updateSurface();
}

void
VertexPolyColourCommand::CreateJobListFromPaint()
{
	MSelectionList selection;
	MString selectStr = m_paintObjectName;
	selectStr += ".vtx[";
	selectStr += (int)m_paintVertexIndex;
	selectStr += "]";
	selection.add(selectStr);

	CreateJobListFromSelection(selection);
};

void
VertexPolyColourCommand::CreateJobListFromSelection(MSelectionList& selection)
{
	// Go through the selection looking for relevant geometry and add them to the list of jobs
	MItSelectionList iter(selection);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MDagPath	dagPath;
		MObject		component;
		iter.getDagPath( dagPath, component );

		// check node type
		if (dagPath.node().hasFn(MFn::kTransform) || dagPath.node().hasFn(MFn::kPolyMesh) || dagPath.node().hasFn(MFn::kMesh))
		{
			if (component.isNull() || m_operation == OP_UpdateLayerComposite)
			{
				dagPath.extendToShape();
				AddJob(dagPath, MObject::kNullObj);
			}
			else
			{
				switch (component.apiType())
				{
				case MFn::kMeshComponent:
				case MFn::kMeshEdgeComponent:
				case MFn::kMeshPolygonComponent:
				case MFn::kMeshVertComponent:
				case MFn::kMeshVtxFaceComponent:
				case MFn::kMeshMapComponent:
					dagPath.extendToShape();
					AddJob(dagPath, component);
					break;
				default:
					break;
				}
			}
		}
	}


	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		MeshData* meshData = m_meshes[i];

		if (!MeshHasConstructionHistory(meshData->dagPath))
		{
			m_isRestoreSelection = true;
			MString meshName = meshData->mesh->fullPathName();
			//MGlobal::executeCommand(MString("select -r ") + meshName);
			//MGlobal::executeCommand("polySmooth -divisions 0");
			MGlobal::executeCommand(MString("polySmooth -divisions 0") + meshName);
			meshData->BuildMesh();
		}

		/*
		MPlug outMeshPlug, inMeshPlug;
		MObject intermediate;

		AddConstructionHistory(meshData->dagPath, outMeshPlug, inMeshPlug, intermediate, NULL);
		*/
	}

}

void
VertexPolyColourCommand::AddJob(MDagPath& dagPath, MObject& component)
{
	// Add mesh to list of unique meshes
	MeshData* meshData = NULL;
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		if (dagPath.fullPathName() == m_meshes[i]->dagPath.fullPathName())
		{
			meshData = m_meshes[i];
			break;
		}
	}
	if (meshData == NULL)
	{
		meshData = new MeshData(dagPath);
		m_meshes.push_back(meshData);
	}

#if MAYA_API_VERSION >= 700
	// If a read or write colour set is forced, check whether the mesh contains this set,
	// if not, skip the operation
	if (m_forceReadColourSetName.length() > 0)
	{
		if (MeshHasColourSet(*meshData->mesh, m_forceReadColourSetName))
			meshData->readColorSetName = m_forceReadColourSetName;
		else
			return;
	}

	if (m_forceWriteColourSetName.length() > 0)
	{
		if (MeshHasColourSet(*meshData->mesh, m_forceWriteColourSetName))
			meshData->writeColorSetName = m_forceWriteColourSetName;
		else
			return;
	}
#endif

	VertexColourJob* job = new VertexColourJob(meshData);
	job->components = component;
	m_jobs.push_back(job);
	meshData->jobs.push_back(job);
}

void
VertexPolyColourCommand::GetSourceColours()
{
	for (size_t i = 0; i < m_jobs.size(); i++)
	{
		VertexColourJob& job = *m_jobs[i];

		PrepareMesh(job);

		GetSourceColours(job);

		assert(job.srcColors.length() == job.vertexIndices.length());

		if (m_isReadColoursFromNode || m_isReadColoursFromNodeDest)
		{
			if (!ReadColoursFromNode(job.meshData->dagPath, job.srcColors, m_isReadColoursFromNode == true))
			{
				displayWarning("No source colour node found");
				FixUninitialisedColors(job.srcColors);
			}
		}
		else
		{
			FixUninitialisedColors(job.srcColors);
		}

		if (m_isReadDestColoursFromNode || m_isReadDestColoursFromNodeSource)
		{
			job.dstColors.setLength(job.srcColors.length());
			if (!ReadColoursFromNode(job.meshData->dagPath, job.dstColors, m_isReadDestColoursFromNodeSource == true))
			{
				displayWarning("No dest colour node found");
			}
		}
		else
		{
			job.dstColors = job.srcColors;
		}

		if (m_getComponentUVs && job.dstColors.length() > 0)
		{
			job.uvs.setLength(job.dstColors.length());
			if (job.m_componentMode == CM_Vertex)
			{
				assert(job.vertexIndices.length() == job.dstColors.length());

				MItMeshVertex vertexIter(job.meshData->dagPath);
				int prevIndex = 0;
				unsigned int numPoints = job.vertexIndices.length();
				for (unsigned int j = 0; j < numPoints; j++)
				{
					vertexIter.setIndex(job.vertexIndices[j], prevIndex);
					float2 uv;
					vertexIter.getUV(uv, NULL);
					job.uvs[j].x = uv[0];
					job.uvs[j].y = uv[1];
				}
			}
			if (job.m_componentMode == CM_VertexFace)
			{
				assert(job.vfFaceList.length() == job.dstColors.length());
				assert(job.vfVertList.length() == job.dstColors.length());

				MItMeshPolygon polyIter(job.meshData->dagPath);
				MItMeshFaceVertex vertexFaceIter(job.meshData->dagPath);
				int prevFace = 0;
				int prevVF = 0;
				unsigned int numPoints = job.vfFaceList.length();
				for (unsigned int j = 0; j < numPoints; j++)
				{
					int faceIndex = job.vfFaceList[j];
					int vtxIndex = job.vfVertList[j];

					polyIter.setIndex(faceIndex, prevFace);
					MIntArray vertices;
					polyIter.getVertices(vertices);
					int localVertex = -1;
					for (unsigned int k = 0; k < vertices.length(); k++)
					{
						if (vtxIndex == vertices[k])
						{
							localVertex = k;
							break;
						}
					}
					assert(localVertex >= 0);

					MStatus status = vertexFaceIter.setIndex(faceIndex, localVertex, prevFace, prevVF);

					float2 uv;
					vertexFaceIter.getUV(uv, NULL);
					job.uvs[j].x = uv[0];
					job.uvs[j].y = uv[1];

					prevFace = faceIndex;
					prevVF = localVertex;
				}
			}
		}

		if (m_getComponentPositions && job.dstColors.length() > 0)
		{
			job.points.setLength(job.dstColors.length());
			if (job.m_componentMode == CM_Vertex)
			{
				assert(job.vertexIndices.length() == job.dstColors.length());

				MItMeshVertex vertexIter(job.meshData->dagPath);
				int prevIndex = 0;
				unsigned int numPoints = job.vertexIndices.length();
				for (unsigned int j = 0; j < numPoints; j++)
				{
					vertexIter.setIndex(job.vertexIndices[j], prevIndex);
					job.points[j] = vertexIter.position(MSpace::kWorld);
				}
			}
			if (job.m_componentMode == CM_VertexFace)
			{
				assert(job.vfFaceList.length() == job.dstColors.length());
				assert(job.vfVertList.length() == job.dstColors.length());

				MItMeshPolygon polyIter(job.meshData->dagPath);
				MItMeshFaceVertex vertexFaceIter(job.meshData->dagPath);
				int prevFace = 0;
				int prevVF = 0;
				unsigned int numPoints = job.vfFaceList.length();
				for (unsigned int j = 0; j < numPoints; j++)
				{
					int faceIndex = job.vfFaceList[j];
					int vtxIndex = job.vfVertList[j];

					polyIter.setIndex(faceIndex, prevFace);
					MIntArray vertices;
					polyIter.getVertices(vertices);
					int localVertex = -1;
					for (unsigned int k = 0; k < vertices.length(); k++)
					{
						if (vtxIndex == vertices[k])
						{
							localVertex = k;
							break;
						}
					}
					assert(localVertex >= 0);

					MStatus status = vertexFaceIter.setIndex(faceIndex, localVertex, prevFace, prevVF);
					job.points[j] = vertexFaceIter.position(MSpace::kWorld);
					prevFace = faceIndex;
					prevVF = localVertex;
				}
			}
		}


		// Build an array of destination colours to write to
		//job.dstColors.setLength(job.vertexIndices.length());
		
		/*
		{
			if (!isFaceVertex)
			{
				for (unsigned int i=0; i<verts.length(); i++)
				{
					int idx = job.vertexIndices[i];
					dstColors[i] = srcColors[idx];
				}
			}
			else
			{
				job.dstColors = job.srcColors;
			}
		}*/
	}
}

void
VertexPolyColourCommand::GetSourceColours(VertexColourJob& job)
{
	if (job.components == MObject::kNullObj)
	{
		GetObjectColours(job);
	}
	else
	{
		switch (job.components.apiType())
		{
		case MFn::kMeshPolygonComponent:
			GetColoursFromFaces(job);
			break;
		case MFn::kMeshVertComponent:
			GetColoursFromVertices(job);
			break;
		case MFn::kMeshVtxFaceComponent:
			GetColoursFromVertexFaces(job);
			break;
		case MFn::kMeshEdgeComponent:
			GetColoursFromEdges(job);
			break;
			/*case MFn::kMeshMapComponent:
			break;*/
		default:
			this->displayWarning("Component selection type not supported");
			break;
		}
	}
}

void
VertexPolyColourCommand::PrepareMesh(VertexColourJob& job)
{
	MDGModifier* modifier = NULL;

	// If the mesh doesn't have any vertex colours yet, create some default ones
#if (MAYA_API_VERSION > 650)
	bool hasVertexColours = true;
	if (0 == job.meshData->mesh->numColorSets())
	{
		hasVertexColours = false;
	}
	else
	{
		// Check the color set isn't the VCP_MasterComp used by the layers system
		if (job.meshData->mesh->numColorSets() == 1 && job.meshData->hasLayers)
		{
			hasVertexColours = false;
		}
	}

	if (!hasVertexColours)
	{
		if (m_isUndoable)
			modifier = new MDGModifier;

		// Create a colour set
		MString colorSetName("colorSet1");
		job.meshData->mesh->createColorSet(colorSetName, modifier);
		job.meshData->mesh->setCurrentColorSetName(colorSetName, modifier);

		// Create default vertex colours
		MIntArray vertList;
		MColorArray colors;
		unsigned int numVerts = (unsigned int)job.meshData->mesh->numVertices();
		vertList.setLength(numVerts);
		colors.setLength(numVerts);
		unsigned int i = 0;
		for (i = 0; i < numVerts; i++)
			vertList[i] = i;
		for (i = 0; i < numVerts; i++)
			colors[i].set(MColor::kRGB, 0.37f, 0.37f, 0.37f, 1.0f);		// default rgb is default 0.37

		// Fill vertex colours
		job.meshData->mesh->setVertexColors(colors, vertList, modifier);

		// If the mesh has layer data, set the active layer
		if (job.meshData->hasLayers)
		{
			MString name("colorSet1");

			MFnDagNode attribNode(job.meshData->mesh->parent(0));
			MObject activeLayerObj = attribNode.attribute(MString("VCP_Layer_Active"));
			if (!activeLayerObj.isNull())
			{
				MPlug activeLayerPlug(attribNode.object(), activeLayerObj);
				activeLayerPlug.setValue(name);
			}

			MString command = "VCP_Layer_CreateDefaultLayerAttribs " + attribNode.fullPathName() + " colorSet1";
			MGlobal::executeCommand(command);
			MGlobal::executeCommand("VCP_Layers_UpdateUI();");
/*
			
			attribNode.addAttribute(attr);
			MObject layerBlendAmountObj = attribNode.attribute(MString("VCP_Layer_") + name + "_BlendAmount");
			if (!layerBlendAmountObj.isNull())
			{
				MPlug layerBlendAmountPlug(attribNode.object(), layerBlendAmountObj);
				layerBlendAmountPlug.setValue(100.0f);
			}

			MObject layerBlendModeObj = attribNode.attribute(MString("VCP_Layer_") + name + "_BlendMode");
			if (!layerBlendModeObj.isNull())
			{
				MPlug layerBlendModePlug(attribNode.object(), layerBlendModeObj);
				layerBlendModePlug.setValue("Replace");
			}

			MObject layerVisibleObj = attribNode.attribute(MString("VCP_Layer_") + name + "_Visible");
			if (!layerVisibleObj.isNull())
			{
				MPlug layerVisiblePlug(attribNode.object(), layerVisibleObj);
				layerVisiblePlug.setValue(true);
			}*/

			job.meshData->DeleteLayersData();
			GatherLayersData();
		}
	}
#endif

	if (modifier != NULL)
	{
		m_undos.push_back(modifier);
	}
}


#ifdef TIMINGS
void
VertexPolyColourCommand::PrintProfileStats()
{
	m_totalTime = m_masterTimer.getTime();

	bool showTimings = m_isVerbose;
	// timing stats
	if (showTimings)
	{
		static char m_text[512];
		const char* message = "PROFILE   Parse: %.2fms    Load: %.2fms    Gather: %.2fms    Process: %.2fms    Blend: %.2fms    Apply: %.2fms    Refresh: %.2fms    TotalTime: %.2fms    FPS: %.2f";

		float fps = 1.0f / (m_totalTime * 0.001f);
#ifdef WIN32
		sprintf_s(m_text, sizeof(m_text), message, m_parseTime, m_loadTime,m_gatherTime, m_processTime, m_blendTime, m_applyTime, m_refreshTime, m_totalTime, fps);
#else
		sprintf(m_text,                   message, m_parseTime, m_loadTime,m_gatherTime, m_processTime, m_blendTime, m_applyTime, m_refreshTime, m_totalTime, fps);
#endif
		OutputText(m_text);
	}
};
#endif

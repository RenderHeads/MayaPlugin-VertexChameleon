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

#if MAYA_API_VERSION >= 700

void
VertexPolyColourCommand::GatherLayersData()
{
	MString finalLayerName = GetFinalLayerName();

	size_t numMeshes = m_meshes.size();
	for (size_t i = 0; i < numMeshes; i++)
	{
		MeshData& meshData = *m_meshes[i];

		// Search for MasterComp layer
		meshData.hasLayers = false;
		{
			MStringArray colorSetNames;
			meshData.mesh->getColorSetNames(colorSetNames);
			for (unsigned int j = 0; j < colorSetNames.length(); j++)
			{
				if (colorSetNames[j] == finalLayerName)
				{
					meshData.hasLayers = true;
					break;
				}
			}
		}
		
		if (meshData.hasLayers)
		{
			// because it's a mesh node, we need to get it's attributes from it's parent transform
			MObject parentTransform = meshData.mesh->parent(0);
			if (!parentTransform.isNull())
			{
				MFnDagNode attribNode(parentTransform);
				MObject layerOrderObj = attribNode.attribute("VCP_LayerOrder");
				if (!layerOrderObj.isNull())
				{
					MString activeLayer;
					MObject activeLayerObj = attribNode.attribute(MString("VCP_Layer_Active"));
					if (!activeLayerObj.isNull())
					{
						MPlug activeLayerPlug(attribNode.object(), activeLayerObj);
						activeLayerPlug.getValue(activeLayer);
					}

					MPlug layerOrderPlug(attribNode.object(), layerOrderObj);
					MString layerOrderString;
					layerOrderPlug.getValue(layerOrderString);
					MStringArray layerNames;
					layerOrderString.split(',', layerNames);			

					unsigned int numLayers = layerNames.length();
					for (unsigned int i = 0; i < numLayers; i++)
					{
						MString name = layerNames[i];
						float blendAmount = 100.0f;
						MString blendMode = "replace";
						bool isVisible = true;

						MObject layerBlendAmountObj = attribNode.attribute(MString("VCP_Layer_") + name + "_BlendAmount");
						if (!layerBlendAmountObj.isNull())
						{
							MPlug layerBlendAmountPlug(attribNode.object(), layerBlendAmountObj);
							layerBlendAmountPlug.getValue(blendAmount);
						}

						MObject layerBlendModeObj = attribNode.attribute(MString("VCP_Layer_") + name + "_BlendMode");
						if (!layerBlendModeObj.isNull())
						{
							MPlug layerBlendModePlug(attribNode.object(), layerBlendModeObj);
							layerBlendModePlug.getValue(blendMode);
						}

						MObject layerVisibleObj = attribNode.attribute(MString("VCP_Layer_") + name + "_Visible");
						if (!layerVisibleObj.isNull())
						{
							MPlug layerVisiblePlug(attribNode.object(), layerVisibleObj);
							layerVisiblePlug.getValue(isVisible);
						}

						LayerData* layer = new LayerData;
						layer->blendAmount = blendAmount * 0.01f;
						layer->blendMode = getBlendMode(blendMode);
						layer->visible = isVisible;
						layer->name = name;
						meshData.layers.push_back(layer);

						if (layer->name == activeLayer)
						{
							MDGModifier* modifier = NULL;
							if (m_isUndoable)
								modifier = new MDGModifier;

							meshData.activeLayer = layer;
							meshData.mesh->setCurrentColorSetName(layer->name, modifier);
							meshData.readColorSetName = layer->name;

							if (modifier != NULL)
							{
								m_undos.push_back(modifier);
							}
						}
					}
				}
			}
		}
	}
}

void
VertexPolyColourCommand::CompositeLayers()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (job.meshData->hasLayers)
		{
			CompositeLayers(job);
		}
	}
}

MString
VertexPolyColourCommand::GetFinalLayerName() const
{
	MString resultName = "VCP_MasterComp";

	
#if MAYA_API_VERSION > 700
	MGlobal::getOptionVarValue("RH_VCP_Final_Layer_Name", resultName);
#else
	{
		MStringArray result;
		MGlobal::executeCommand("optionVar -q RH_VCP_Final_Layer_Name", result);
		if (result.length() == 1)
			resultName = result[0];
	}
#endif

	return resultName;
}

void
VertexPolyColourCommand::CompositeLayers(VertexColourJob& job)
{
	MStatus status;

	MString viewMode = "Final Composite";

	MString finalLayerName = GetFinalLayerName();

#if MAYA_API_VERSION > 700
	MGlobal::getOptionVarValue("RH_VCP_Layers_ViewMode", viewMode);
#else
	{
		MStringArray result;
		MGlobal::executeCommand("optionVar -q RH_VCP_Layers_ViewMode", result);
		if (result.length() == 1)
			viewMode = result[0];
	}
#endif

	if (viewMode == "Active Layer")
		return;

	if (m_isVerbose)
	{
		MString txt = "CompositeLayers: ";
		txt += job.meshData->currentColorSetName;
		OutputText(txt);
	}

	//unsigned int numColors = job.srcColors.length();
	MColorArray compColors;// = job.srcColors;

	bool written = false;
	//if (job.meshData->layerViewMode == "Final Comp")
	{
		//compColors.setLength(numColors);
		//for (unsigned int i = 0; i < numColors; i++)
		//	compColors[i].set(MColor::kRGB, 0.0f, 0.0f, 0.0f, 0.0f);

		bool firstLayer = true;
		size_t numLayers = job.meshData->layers.size();
		for (size_t j = 0; j < numLayers; j++)
		{
			const LayerData& layer = *job.meshData->layers[j];
			if (layer.visible)
			{
				job.meshData->mesh->setCurrentColorSetName(layer.name);
				job.meshData->readColorSetName = layer.name;

				GetSourceColours(job);
				FixUninitialisedColors(job.srcColors);

				if (firstLayer)
				{
					compColors = job.srcColors;
					firstLayer = false;
				}
				else
				{
					// This could a faster, but for now it's stable
					MColorArray origColors = job.srcColors;
					::BlendColours(layer.blendMode, compColors, job.srcColors, layer.blendAmount);
					::BlendAlpha(job.srcColors, compColors, origColors);
				}
				written = true;
			}
		}
		job.dstColors = compColors;
	}

	if (!written)
	{
		GetSourceColours(job);
		job.dstColors = job.srcColors;
		for (unsigned int i = 0; i < job.dstColors.length(); i++)
			job.dstColors[i].set(MColor::kRGB, 0.37f, 0.37f, 0.37f, 1.0f);
	}
	/*if (job.meshData->layerViewMode == "Active Layer")
	{
		if (job.meshData->activeLayer != NULL)
		{
			const LayerData& layer = *job.meshData->activeLayer;
			job.meshData->mesh->setCurrentColorSetName(layer.name);
			job.meshData->readColorSetName = layer.name;

			GetSourceColours(job);
			FixUninitialisedColors(job.srcColors);
			job.dstColors = job.srcColors;
		}
	}*/

	MDGModifier* modifier = NULL;
	if (m_isUndoable)
		modifier = new MDGModifier;

	status = job.meshData->mesh->setCurrentColorSetName(finalLayerName, modifier);
	CHECK_MSTATUS(status);
	Apply(job, NULL);
	//ApplyNothing(job);

	/*{
		int numSets = job.meshData->mesh->numColorSets();
		MString o;
		o += "numSets: ";
		o += numSets;
		MColorArray c1, c2;
		MString name("VCP_MasterComp");
		job.meshData->mesh->getColors(c1);
		job.meshData->mesh->getColors(c2, &name);
		o += "c1: ";
		o += c1.length();
		o += "c2: ";
		o += c2.length();
		OutputText(o);
	}*/


	status = job.meshData->mesh->setCurrentColorSetName(job.meshData->currentColorSetName, modifier);
	job.meshData->readColorSetName = job.meshData->currentColorSetName;

	if (modifier != NULL)
	{
		m_undos.push_back(modifier);
	}
}

#endif
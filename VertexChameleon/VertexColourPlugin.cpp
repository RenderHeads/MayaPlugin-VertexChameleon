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
#include "VertexPolyColourCommand.h"
#include "PolyColourNode.h"
#include "VertexColourPlugin.h"

VertexColourPlugin::VertexColourPlugin(const MayaPluginParams& params) : MayaPlugin(params)
{
	m_VertexPolyColourCreated = false;
	m_PolyColourNodeCreated = false;
}

void
VertexColourPlugin::LoadUserPlugins(MFnPlugin& plugin)
{
	MStatus status;

	// Register the command
	if (!m_VertexPolyColourCreated)
	{
		status = plugin.registerCommand("RH_VertexPolyColour", VertexPolyColourCommand::creator,VertexPolyColourCommand::newSyntax);
		if (MStatus::kSuccess != status)
		{
			status.perror("registerCommand VertexPolyColour failed");
		}
		m_VertexPolyColourCreated = true;
	}

	// Register the node
	if (!m_PolyColourNodeCreated)
	{
		status = plugin.registerNode( "RH_VCP_PolyColourNode", PolyColourNode::id, PolyColourNode::creator, PolyColourNode::initialize,MPxNode::kDependNode);
		if (MStatus::kSuccess != status)
		{
			status.perror("registerNode RH_VCP_PolyColourNode failed");
		}
		m_PolyColourNodeCreated = true;
	}

	// set the mel procs to be run when the plugin is loaded / unloaded
	status = plugin.registerUI("source RH_VertexChameleon;", "");
	if (!status)
	{
		status.perror("registerUIScripts");
	}
}

void
VertexColourPlugin::UnloadUserPlugins(MFnPlugin& plugin)
{
	MStatus status;

	// Unregister node
	if (m_PolyColourNodeCreated)
	{
		status = plugin.deregisterNode(PolyColourNode::id);
		if (MStatus::kSuccess != status)
		{
			status.perror("deregisterNode RH_VCP_PolyColourNode failed");
		}
		m_PolyColourNodeCreated = false;
	}	

	// Unregister command
	if (m_VertexPolyColourCreated)
	{
		status = plugin.deregisterCommand("RH_VertexPolyColour");
		if (MStatus::kSuccess != status)
		{
			status.perror("deregisterCommand RH_VertexPolyColour failed");
		}
		m_VertexPolyColourCreated = false;
	}
}
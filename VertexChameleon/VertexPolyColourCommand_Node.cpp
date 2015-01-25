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
#include "PolyColourNode.h"
#include "LayerData.h"
#include "MeshData.h"
#include "VertexColourJob.h"
#include "GradientParameters.h"
#include "VertexPolyColourCommand.h"

using namespace std;

void
VertexPolyColourCommand::WriteColoursToNode(bool isSource)
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		if (isSource)
		{
			WriteColoursToNode(job.meshData->dagPath, job.srcColors, isSource);
		}
		else
		{
			WriteColoursToNode(job.meshData->dagPath, job.dstColors, isSource);
		}
	}
}

void
VertexPolyColourCommand::RemoveColoursNode()
{
	size_t numJobs = m_jobs.size();
	for (size_t i = 0; i < numJobs; i++)
	{
		VertexColourJob& job = *m_jobs[i];

		RemoveColoursNode(job.meshData->dagPath, job.srcColors);
	}
}

bool
VertexPolyColourCommand::FindNodeOnMesh(MDagPath& dagPath,MObject& result)
{
	MFnDependencyNode fnDependNode( dagPath.node() );
	MPlug inMeshPlug = fnDependNode.findPlug( "outMesh" );

	// If the mesh has history..check whats connected
	MPlugArray cc;
	inMeshPlug.connectedTo( cc, true /* asDst */, true );
	for (unsigned int i=0; i<cc.length(); i++)
	{
		MObject obj=cc[i].node();
		MFnDependencyNode dn(obj);
		if (dn.typeId()==PolyColourNode::id)
		{
			result=obj;
			return true;
		}
	}
	return false;
}

void
VertexPolyColourCommand::CreateNodeOnMesh(MDagPath& dagPath, MObject& ourNode)
{
	MStatus status;

	MFnDependencyNode fnDependNode( dagPath.node() );
	MPlug plug_outMesh = fnDependNode.findPlug("outMesh");

	MDGModifier* modifier = new MDGModifier;

	// Create the node
	ourNode=modifier->createNode("RH_VCP_PolyColourNode", &status);
	if (status != MStatus::kSuccess)
		MGlobal::doErrorLogEntry("error");

	// Connect it
	MFnDependencyNode dn(ourNode, &status);
	if (status != MStatus::kSuccess)
		MGlobal::doErrorLogEntry("error");
	MString dnname = dn.name();
	MPlug plug_inMesh=dn.findPlug(PolyColourNode::m_inMesh,&status);
	MString plugName = plug_inMesh.name();
	if (status != MStatus::kSuccess)
		MGlobal::doErrorLogEntry("error");
	status=modifier->connect(plug_outMesh,plug_inMesh);
	if (status != MStatus::kSuccess)
		MGlobal::doErrorLogEntry("error");

	status=modifier->doIt();
	if (status != MStatus::kSuccess)
		MGlobal::doErrorLogEntry("error");

	if (m_isUndoable)
		m_undos.push_back(modifier);
	else
		delete modifier;
}

void
VertexPolyColourCommand::WriteColoursToNode(MDagPath& dagPath, MColorArray& colors, bool isSource)
{
	MStatus status;

	// Try to find the colour node attached to the mesh
	// If it's not found, create it
	MObject ourNode;
	if (!FindNodeOnMesh(dagPath,ourNode))
	{
		CreateNodeOnMesh(dagPath, ourNode);
	}

	// Send the selection to the node
	{
		// Pass the component list down to the node.
		// To do so, we create/retrieve the current plug
		// from the uvList attribute on the node and simply
		// set the value to our component list.
		//

		MDoubleArray dcols;
		dcols.setLength(colors.length()*4);
		int idx = 0;
		for (unsigned int i=0; i<colors.length(); i++)
		{
			dcols[idx] = (double)colors[i].r; idx++;
			dcols[idx] = (double)colors[i].g; idx++;
			dcols[idx] = (double)colors[i].b; idx++;
			dcols[idx] = (double)colors[i].a; idx++;
		}
		MFnDoubleArrayData wrapper;
		wrapper.create(dcols,&status);

		MPlug* colourPlug = NULL;
		if (isSource)
		{
			colourPlug = new MPlug(ourNode, PolyColourNode::m_colors);
		}
		else
		{
			colourPlug = new MPlug(ourNode, PolyColourNode::m_colorsDest);
		}

		// Warning, we have to do this as GCC doesn't like to pass by temporary reference
		MObject wrapperObjRef = wrapper.object();
		status = colourPlug->setValue(wrapperObjRef);
		delete colourPlug;
	}
}

void
VertexPolyColourCommand::RemoveColoursNode(MDagPath& dagPath, MColorArray& colors)
{
	MStatus status;

	MObject ourNode;
	if (FindNodeOnMesh(dagPath,ourNode))
	{
		MDGModifier* modifier = new MDGModifier;

		// Delete the node
		status=modifier->deleteNode(ourNode);
		status=modifier->doIt();

		if (m_isUndoable)
			m_undos.push_back(modifier);
		else
			delete modifier;		
	}
}

bool
VertexPolyColourCommand::ReadColoursFromNode(MDagPath& dagPath, MColorArray& colors, bool isSource)
{
	MStatus status;

	MObject ourNode;
	if (!FindNodeOnMesh(dagPath,ourNode))
		return false;

	// Pass the component list down to the node.
	// To do so, we create/retrieve the current plug
	// from the uvList attribute on the node and simply
	// set the value to our component list.
	//

	{
		MPlug* colourPlug = NULL;
		if (isSource)
		{
			colourPlug = new MPlug(ourNode, PolyColourNode::m_colors);
		}
		else
		{
			colourPlug = new MPlug(ourNode, PolyColourNode::m_colorsDest);
		}
		MObject obj;
		status = colourPlug->getValue(obj);
		delete colourPlug;
		MFnDoubleArrayData wrapper(obj);

		unsigned int numDoubles=wrapper.length();
		unsigned int numColors=numDoubles/4;

#ifdef _DEBUG
		assert(numColors == colors.length());
#endif
		if (numColors == colors.length())
		{
			int idx = 0;
			for (unsigned int i = 0; i < numColors; i++)
			{
				colors[i].r=(float)wrapper[idx]; idx++;
				colors[i].g=(float)wrapper[idx]; idx++;
				colors[i].b=(float)wrapper[idx]; idx++;
				colors[i].a=(float)wrapper[idx]; idx++;
			}
		}
	}

	return true;
}

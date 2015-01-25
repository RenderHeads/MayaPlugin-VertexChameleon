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
#include "Utility.h"
#include "Random.h"
#include "ColorUtils.h"
#include "PolyColourNode.h"

MTypeId		PolyColourNode::id( 0x00112FC0, 0);

MObject		PolyColourNode::m_inMesh;
MObject		PolyColourNode::m_colors;
MObject		PolyColourNode::m_colorsDest;
//MObject		PolyColourNode::m_verts;

/*
bool
SDPolyColorNode::setInternalValueInContext ( const  MPlug & plug , const  MDataHandle & data ,  MDGContext & ctx)
{
	bool result=MPxNode::setInternalValueInContext(plug,data,ctx);
	if (plug==m_cpList)
	{
		int a=3;
	}
	return result;
}
bool
SDPolyColorNode::setInternalValue (const MPlug& plug, const MDataHandle& data)
{
	bool result=MPxNode::setInternalValue(plug,data);
	if (plug==m_cpList)
	{
		int a=3;
	}
	return result;	
}*/

void 
PolyColourNode::postConstructor()
{
#if MAYA_API_VERSION > 800
	setDoNotWrite(true);
#endif
}

MStatus
PolyColourNode::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus status = MS::kSuccess;

	MDataHandle stateData = data.outputValue( state, &status );
	MCheckStatus( status, "ERROR getting state" );

	// Check for the HasNoEffect/PassThrough flag on the node.
	//
	// (stateData is an enumeration standard in all depend nodes)
	// 
	// (0 = Normal)
	// (1 = HasNoEffect/PassThrough)
	// (2 = Blocking)
	// ...
	//
	if( stateData.asShort() == 1 )
	{
		//MDataHandle inputMesh = data.inputValue( m_inMesh, &status );
		//MCheckStatus(status,"ERROR getting inMesh");

		//MDataHandle outputMesh = data.outputValue( m_outMesh, &status );
		//MCheckStatus(status,"ERROR getting outMesh");

		// Simply redirect the inMesh to the outMesh for the PassThrough effect
		//
		//outputMesh.set(inputMesh.asMesh());
	}
	else
	{
		// Check which output attribute we have been asked to 
		// compute. If this node doesn't know how to compute it, 
		// we must return MS::kUnknownParameter
		// 
		if (1)
		{
		}
		else
		{
			status = MS::kUnknownParameter;
		}
	}

	return status;
}

PolyColourNode::PolyColourNode()
{
}

PolyColourNode::~PolyColourNode()
{
}

void*
PolyColourNode::creator()
{
	return new PolyColourNode();
}

/*
void
PolyColourNode::setAttribDefault(MFnAttribute& attr)
{
	attr.setStorable(true);
	attr.setKeyable(false);
#if (MAYA_API_VERSION > 600)
	attr.setChannelBox(false);
#endif
}*/

MStatus
PolyColourNode::initialize()
{
	MStatus	status;
	MFnTypedAttribute attrFn;
	MFnNumericAttribute attrNum;
	MFnEnumAttribute enumFn;

	m_inMesh = attrFn.create("inMesh", "im", MFnMeshData::kMesh);
	attrFn.setStorable(false);
	attrFn.setHidden(true);
/*
	m_cpList = attrFn.create("inputComponents", "ics",MFnComponentListData::kComponentList);
	attrFn.setStorable(true);
	attrFn.setHidden(true);
	attrFn.setConnectable(false);
*/
	m_colors = attrFn.create("colors", "cols", MFnData::kDoubleArray);
	attrFn.setStorable(false);
	attrFn.setHidden(true);

	m_colorsDest = attrFn.create("colorsDest", "colsd", MFnData::kDoubleArray);
	attrFn.setStorable(false);
	attrFn.setHidden(true);

	//attrFn.setConnectable(false);

	//m_verts = attrFn.create("verts", "vts",MFnData::kIntArray);
	//attrFn.setStorable(true);
	//attrFn.setHidden(false);

	status = addAttribute( m_inMesh );
//	status = addAttribute( m_cpList );
	status = addAttribute( m_colors );
	status = addAttribute( m_colorsDest );
	//status = addAttribute( m_verts );

//	status = attributeAffects( m_inMesh, m_outMesh );
//	status = attributeAffects( m_cpList, m_outMesh );

	return status;
}

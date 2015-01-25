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

#ifndef POLYCOLOURNODE_H
#define POLYCOLOURNODE_H

#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

class PolyColourNode :
	public MPxNode
{
public:
	PolyColourNode();
	virtual ~PolyColourNode();

	virtual void 			postConstructor();
	virtual MStatus			compute( const MPlug&, MDataBlock& );
	static  void *          creator();
	static  MStatus         initialize();

	virtual MStatus			shouldSave(const MPlug& plug, bool& isSaving)
	{
		isSaving = false;
		return MS::kSuccess;
	}

//	virtual bool 			setInternalValue ( const  MPlug &, const  MDataHandle &);
//	virtual bool 	setInternalValueInContext ( const  MPlug &, const  MDataHandle &,  MDGContext &);

	static MTypeId id;

public:
	//static void				setAttribDefault(MFnAttribute& attr);

	// creation data
	static  MObject		m_inMesh;
	//static  MObject		m_outMesh;
	//static	MObject		m_cpList;
	static	MObject		m_colors;
	static	MObject		m_colorsDest;
	//static  MObject		m_verts;
};

#endif

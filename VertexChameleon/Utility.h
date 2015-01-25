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

#ifndef UTILITY_H
#define UTILITY_H

struct ColRGB;

bool		getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,ColRGB& result);


static bool
MeshHasConstructionHistory( const MDagPath& mesh)
{
	MStatus status;

	MFnDependencyNode fnDependNode( mesh.node() );

	// Assign the '.inMesh' plug from the mesh node.
	MPlug inMeshPlug = fnDependNode.findPlug( "inMesh" );

	// If the mesh has history, use it for the '.outMesh' plug.
	MPlugArray cc;
	inMeshPlug.connectedTo( cc, true /* asDst */, false );
	if ( cc.length() > 0 )
	{
		return true;
	}

	return false;
}

// **************************************************************************
//  AddConstructionHistory
// **************************************************************************
/// Adds construction history to the specified mesh, if required.
///
/// If no construction history exists, then a copy of the specifed mesh is
/// created using MFnMesh, and this new mesh is set to be an intermediate
/// node. If history already exists for the mesh, no new nodes are created.
///
/// The plugs returned represent those between which you will insert any
/// construction.
///
///   history.outMesh ->  ( your node )  -> mesh.inMesh
///
/// \note   It is assumed that you wish to insert a node immediately upstream
///         of the specified mesh. If you provide a valid (MDGModifier*) then
///         the plugs returned by this function are _disconnected_, even if
///         they were connected at the point when this was called.
///
/// \param  mesh: The mesh to which you wish to add construction history.
/// \param  outMeshPlug: The plug from which you will connect your
///                      construction node; e.g.
///                         outMeshPlug.outMesh -> yourNode.inMesh
/// \param  inMeshPlug: The plug on the mesh node to which you will connect
///                     your construction node; e.g.:
///                         yourNode.outMesh -> mesh.inMesh
/// \param  intermediate: The MObject for the intermediate node (a copy of
///                       the specified mesh) that was created to introduce
///                       construction history. This will be
///                       MObject::kNullObj if the mesh already has history.
/// \param  dg: Pointer to a MDGModifier. If non-NULL, and history already
///             exists for the mesh, this holds the undo for the disconnect
///             of the attributes from the history node to the mesh node.
// **************************************************************************

static MStatus
AddConstructionHistory( const MDagPath& mesh,
					   MPlug& outMeshPlug,
					   MPlug& inMeshPlug,
					   MObject& intermediate,
					   MDGModifier* dg )
{
	MStatus status;

	intermediate = MObject::kNullObj;

	MFnDependencyNode fnDependNode( mesh.node() );

	// Assign the '.inMesh' plug from the mesh node.
	inMeshPlug = fnDependNode.findPlug( "inMesh" );

	// If the mesh has history, use it for the '.outMesh' plug.
	MPlugArray cc;
	inMeshPlug.connectedTo( cc, true /* asDst */, false );
	if ( cc.length() > 0 )
	{
		// Assign the '.outMesh' plug from the node upstream of the mesh.
		outMeshPlug = cc[0];

		// Disconnect the plugs.
		if ( NULL != dg )
		{
			dg->disconnect( outMeshPlug, inMeshPlug );
			status = dg->doIt();
		}
	}
	else
	{
		// Need to generate construction history on the mesh
		MDagPath transform( mesh );
		transform.pop();

		MFnMesh fnMesh;
		intermediate = fnMesh.copy ( mesh.node(), transform.node(), &status );
		if ( status != MS::kSuccess ) { status.perror( "FAILED to copy mesh node" ); return status; }

		// Set the copy to be an intermediate object
		fnDependNode.setObject( intermediate );
		MPlug intermediatePlug = fnDependNode.findPlug( "intermediateObject" );
		intermediatePlug.setValue( true );

		// Assign the '.outMesh' plug from the intermediate node.
		outMeshPlug = fnDependNode.findPlug( "outMesh" );
	}

	return status;
}

#endif

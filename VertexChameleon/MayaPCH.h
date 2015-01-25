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

#ifndef MAYAPCH_H
#define MAYAPCH_H

#if MAYA85 || MAYA2008 || MAYA2009 || MAYA2010 || MAYA2011 || MAYA2012 || MAYA2013 || MAYA20135 || MAYA2014 || MAYA2015
#include <iostream>
#include <cmath>
using namespace std;
#endif

#include <maya/MPxCommand.h>
#include <maya/MStatus.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MItSelectionList.h>
#include <maya/MSelectionList.h>
#include <maya/MString.h>
#include <maya/MColor.h>
//#include <maya/M3dView.h>
#include <maya/MDistance.h>

#include <maya/MColorArray.h>
#include <maya/MIntArray.h>


#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MDGModifier.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>

#include <maya/MFnPlugin.h>

#include <maya/MIntArray.h>
#include <maya/MColorArray.h>
#include <maya/MDoubleArray.h>

#include <maya/MItCurveCV.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshFaceVertex.h>
#include <maya/MItMeshPolygon.h>

#include <maya/MFnComponentListData.h>
#include <maya/MFnSingleIndexedComponent.h>

//#include <maya/MIOStream.h>

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnComponentListData.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>
#include <maya/MStatus.h>

#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshFaceVertex.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnDoubleIndexedComponent.h>

#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>

#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnStringArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MItDependencyNodes.h>

#include <maya/MFloatPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatPoint.h>
#include <maya/MRenderUtil.h>
#include <maya/MMatrix.h>

#endif

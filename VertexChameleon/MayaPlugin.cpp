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

#include "MayaPCH.h"
#include "MayaUtility.h"
#include "MayaPlugin.h"

MayaPlugin::MayaPlugin(const MayaPluginParams& params)
{
	m_productName = params.productName;
	m_productShortName = params.productShortName;
	m_authorString = params.authorString;
	m_versionString = params.versionString;

#ifdef _DEBUG
	MGlobal::startErrorLogging();
#endif
}

MayaPlugin::~MayaPlugin()
{
#ifdef _DEBUG
	MGlobal::stopErrorLogging();
	MGlobal::closeErrorLog();
#endif
}

void
MayaPlugin::Load(MObject& pluginObject)
{
	char finalVersionStr[256];
#ifdef WIN32
	sprintf_s(finalVersionStr, sizeof(finalVersionStr), "%s, %s @ %s", m_versionString.asChar(), __DATE__, __TIME__);
#else
	sprintf(finalVersionStr, "%s, | %s @ %s", m_versionString.asChar(), __DATE__, __TIME__);
#endif

	MFnPlugin plugin(pluginObject, m_authorString.asChar(), finalVersionStr, "Any");


	LoadFeatures(plugin);
}

void
MayaPlugin::Unload(MObject& pluginObject)
{
	MStatus status;
	MFnPlugin plugin(pluginObject);

	UnloadUserPlugins(plugin);
}

void
MayaPlugin::LoadFeatures(MFnPlugin& plugin)
{
	LoadUserPlugins(plugin);
}

void
MayaPlugin::DisplayInfo(const MString& text) const
{
	MGlobal::displayInfo(m_productShortName + ": " + text);
}

void
MayaPlugin::DisplayWarning(const MString& text) const
{
	MGlobal::displayWarning(m_productShortName + ": " + text);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
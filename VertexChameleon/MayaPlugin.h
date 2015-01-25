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

#ifndef MAYAPLUGIN_H
#define MAYAPLUGIN_H

struct MayaPluginParams
{
	MString productName, productShortName;
	MString versionString;
	MString	authorString;
};

class MayaPlugin
{
public:
	MayaPlugin(const MayaPluginParams& params);
	virtual ~MayaPlugin();

	void			Load(MObject& pluginObject);
	void			Unload(MObject& pluginObject);

protected:
	void			LoadFeatures(MFnPlugin& plugin);
	virtual void	LoadUserPlugins(MFnPlugin& plugin) = 0;
	virtual void	UnloadUserPlugins(MFnPlugin& plugin) = 0;
	void			DisplayWarning(const MString& text) const;
	void			DisplayInfo(const MString& text) const;

	MString			m_authorString;
	MString			m_versionString;
	MString			m_productName, m_productShortName;

private:
	const char*		ErrorLogFilename;
};

#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
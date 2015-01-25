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
#include "NodeSearch.h"

using namespace MayaUtility;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

NodeSearch::NodeSearch(MFn::Type type,bool ignoreItermediateObjects) : m_iter(type)
{
	m_ignoreItermediateObjects=ignoreItermediateObjects;
	m_filter=type;
	m_name=NULL;
}
NodeSearch::NodeSearch(const char* name,MFn::Type type,bool ignoreItermediateObjects) : m_iter(type)
{
	m_ignoreItermediateObjects=ignoreItermediateObjects;
	m_filter=type;
	m_name=name;
}

bool
NodeSearch::getNextNode(MObject &result)
{
	while (!m_iter.isDone())
	{
#if MAYA65 || MAYA60
		MFnDagNode dagnode(m_iter.item());
		MFnDependencyNode depnode(m_iter.item());
		result=m_iter.item();
#else
		MFnDagNode dagnode(m_iter.thisNode());
		MFnDependencyNode depnode(m_iter.thisNode());
		result=m_iter.thisNode();
#endif
		m_iter.next();

		if (dagnode.isIntermediateObject() && m_ignoreItermediateObjects)
		{
		}
		else
		{
			if (m_name)
			{
				const char* tt=depnode.name().asChar();
				if (tt[0])
				{
					if (dagnode.fullPathName() == m_name)
					{
						return true;
					}
#ifdef WIN32
					if (!_stricmp(m_name,depnode.name().asChar()))
#else
					if (!strcasecmp(m_name,depnode.name().asChar()))
#endif
					{
						return true;
					}
				}
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

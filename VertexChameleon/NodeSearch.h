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

#ifndef __NODESEARCH_H__
#define __NODESEARCH_H__

//#include <iostream>
//#include <vector>

namespace MayaUtility
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class NodeSearch
{
	public:
		NodeSearch(MFn::Type type=MFn::kInvalid,bool ignoreItermediateObjects=true);
		NodeSearch(const char* name,MFn::Type type=MFn::kInvalid,bool ignoreItermediateObjects=true);
		bool				getNextNode(MObject& result);
	protected:
		const char*			m_name;
		MItDependencyNodes	m_iter;
		bool				m_ignoreItermediateObjects;
		MFn::Type			m_filter;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


}; // namespace MayaUtility

#endif


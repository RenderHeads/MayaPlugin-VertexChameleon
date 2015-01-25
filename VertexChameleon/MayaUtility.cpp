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

void
SetOptionVar(const MString& varName, const std::string& val)
{
#if MAYA_API_VERSION < 800
	MString command = "optionVar -stringValue \"";
	command += varName;
	command += "\" \"";
	command += val.c_str();
	command += "\";";
	MGlobal::executeCommand(command);
#else
	MGlobal::setOptionVarValue(varName, val.c_str());
#endif
}

MString
Convert(MStringArray& strings)
{
	MString result;
	for (unsigned int i = 0; i < strings.length(); i++)
	{
		if (i != 0)
		{
			result += " ";
		}
		result += strings[i];
	}

	return result;
}

bool
hasArg(const MArgDatabase& argData,const char* shortName,const char* longName)
{
	return (argData.isFlagSet(shortName) || argData.isFlagSet(longName));
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,unsigned int& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		unsigned int val;
		status=argData.getFlagArgument(longName, 0, val);
		if (status==MS::kSuccess)
		{
			result=val;
			return true;
		}
	}
	return false;
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,int& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		int val;
		status=argData.getFlagArgument(longName, 0, val);
		if (status==MS::kSuccess)
		{
			result=val;
			return true;
		}
	}
	return false;
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,double& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		double val;
		status=argData.getFlagArgument(longName, 0, val);
		if (status==MS::kSuccess)
		{
			result=val;
			return true;
		}
	}
	return false;
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,float& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		double val;
		status=argData.getFlagArgument(longName, 0, val);
		if (status==MS::kSuccess)
		{
			result=(float)val;
			return true;
		}
	}
	return false;
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,float2& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		double val1, val2;
		status=argData.getFlagArgument(longName, 0, val1);
		status=argData.getFlagArgument(longName, 1, val2);
		if (status==MS::kSuccess)
		{
			result[0]=(float)val1;
			result[1]=(float)val2;
			return true;
		}
	}
	return false;
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,float3& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		double val1, val2, val3;
		status=argData.getFlagArgument(longName, 0, val1);
		status=argData.getFlagArgument(longName, 1, val2);
		status=argData.getFlagArgument(longName, 2, val3);
		if (status==MS::kSuccess)
		{
			result[0]=(float)val1;
			result[1]=(float)val2;
			result[2]=(float)val3;
			return true;
		}
	}
	return false;
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,double3& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		double val1, val2, val3;
		status=argData.getFlagArgument(longName, 0, val1);
		status=argData.getFlagArgument(longName, 1, val2);
		status=argData.getFlagArgument(longName, 2, val3);
		if (status==MS::kSuccess)
		{
			result[0]=val1;
			result[1]=val2;
			result[2]=val3;
			return true;
		}
	}
	return false;
}

bool
getArgValue(const MArgDatabase& argData,const char* shortName,const char* longName,MString& result)
{
	MStatus status;
	if (hasArg(argData,shortName,longName))
	{
		MString val;
		status=argData.getFlagArgument(longName, 0, val);
		if (status==MS::kSuccess)
		{
			result=val;
			return true;
		}
	}
	return false;
}
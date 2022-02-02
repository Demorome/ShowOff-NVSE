#pragma once

#include "PluginAPI.h"

//additional type safety
struct NVSEParamInfo
{
	const char* typeStr;
	kNVSEParamType	typeID;		// ParamType
	UInt32	isOptional;	// do other bits do things?
};
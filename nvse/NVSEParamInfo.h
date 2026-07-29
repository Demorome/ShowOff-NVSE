#pragma once

#include "PluginAPI.h"

//additional type safety
struct NVSEParamInfo
{
	const char* typeStr;
	kNVSEParamType	typeID;		// ParamType
	uint32_t	isOptional;	// do other bits do things?
};
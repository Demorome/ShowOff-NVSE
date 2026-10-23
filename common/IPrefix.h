#pragma once

// 4100 - unreferenced formal parameter
// 4201 - nonstandard extension used : nameless struct/union
// 4244 - 'argument' : conversion from 'type1' to 'type2', possible loss of data
// 4324 - 'type': structure was padded due to alignment specifier
// 4505 - 'function' : unreferenced local function has been removed
// 4530 - C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma warning(disable: 4100 4201 4244 4324 4505 4530 28159)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include "common/ITypes.h"
#include "common/IDebugLog.h"
#include <Windows.h>
#include <assert.h>

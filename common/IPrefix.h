#pragma once

// 4018 - signed/unsigned mismatch
// 4244 - loss of data by assignment
// 4267 - possible loss of data (truncation)
// 4305 - truncation by assignment
// 4288 - disable warning for crap microsoft extension screwing up the scope of variables defined in for loops
// 4311 - pointer truncation
// 4312 - pointer extension
#pragma warning(disable: 4018 4244 4267 4305 4288 4312 4311)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include "common/ITypes.h"
#include "common/IDebugLog.h"
#include <Windows.h>
#include <assert.h>

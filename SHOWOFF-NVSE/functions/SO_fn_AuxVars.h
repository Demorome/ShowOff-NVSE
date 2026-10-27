#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

/* For Auxiliary StringMap Arrays (AuxStringMap, ASM, not to confuse with ASM = assembly)
 * 
 * Credits: JIPLN for AuxVars and RefMap code, which composes 99% of the code below.
 * 
 */

#define DoLaterMaybe 0

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetSize, AuxStringMapSize, "", false, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetValueType, AuxStringMapGetValueType, "", false, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetFloat, AuxStringMapGetFlt, "", false, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetRef, AuxStringMapGetRef, "", false, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetString, AuxStringMapGetStr, "", false, kParams_TwoStrings);

#if DoLaterMaybe
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetValue, AuxStringMapGetVal, , 0, 2, kParams_TwoStrings);
#endif

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetFirst, AuxStringMapFirst, "", false, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetNext, AuxStringMapNext, "", false, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetKeys, AuxStringMapKeys, "", false, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetAll, AuxStringMapGetAll, "", false, kParams_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayGetAsArray, AuxStringMapGetAsArr, "", false, kParams_OneString);

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetFromArray, AuxStringMapSetFromArr, "", false, kParams_OneString_OneArray_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetFloat, AuxStringMapSetFlt, "", false, kParams_TwoStrings_OneDouble);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetRef, AuxStringMapSetRef, "", false, kParams_TwoStrings_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetString, AuxStringMapSetStr, "", false, kParams_ThreeStrings);

#if DoLaterMaybe
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArraySetValue, AuxStringMapSetVal, "", 0, 3, kParams_JIP_OneString_OneInt_OneOptionalForm);
#endif

DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayEraseKey, AuxStringMapEraseKey, "", false, kParams_TwoStrings);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayValidateValues, AuxStringMapValidateVals, "", false, kParams_OneString);
DEFINE_COMMAND_ALT_PLUGIN(AuxStringMapArrayDestroy, AuxStringMapDestroy, "", false, kParams_OneString);

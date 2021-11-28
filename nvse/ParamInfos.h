#pragma once

#include "CommandTable.h"

// Declare Params with "static", so they are only built once (so this file can be included multiple times).

static ParamInfo kParams_OneInt[1] =
{
	{	"int", kParamType_Integer, 0 },
};

static ParamInfo kParams_TwoInts[2] =
{
	{	"int", kParamType_Integer, 0 },
	{	"int", kParamType_Integer, 0 },
};

static ParamInfo kParams_OneOptionalInt[1] =
{
	{	"int", kParamType_Integer, 1 },
};

static ParamInfo kParams_OneInt_OneOptionalInt[2] =
{
	{	"int", kParamType_Integer, 0 },
	{	"int", kParamType_Integer, 1 },
};

static ParamInfo kParams_TwoOptionalInts[2] =
{
	{	"int", kParamType_Integer, 1 },
	{	"int", kParamType_Integer, 1 },
};

static ParamInfo kParams_ThreeOptionalInts[3] =
{
	{	"int", kParamType_Integer, 1 },
	{	"int", kParamType_Integer, 1 },
	{	"int", kParamType_Integer, 1 },
};

static ParamInfo kParams_FourOptionalInts[4] =
{
	{	"int", kParamType_Integer, 1 },
	{	"int", kParamType_Integer, 1 },
	{	"int", kParamType_Integer, 1 },
	{	"int", kParamType_Integer, 1 },
};

static ParamInfo kParams_OneFloat[1] =
{
	{	"float", kParamType_Float,	0 },
};

static ParamInfo  kParams_OneFloat_OneOptionalInt[2] =
{
	{	"float", kParamType_Float, 0 },
	{	"int", kParamType_Integer, 1 },
};

static ParamInfo  kParams_OneFloat_OneInt[2] =
{
	{	"float", kParamType_Float, 0 },
	{	"int", kParamType_Integer, 0 },
};

static ParamInfo  kParams_OneOptionalFloat_OneOptionalInt[2] =
{
	{	"float", kParamType_Float, 1 },
	{	"int", kParamType_Integer, 1 },
};

static ParamInfo kParams_OneString[1] =
{
	{	"string",	kParamType_String,	0 },
};

static ParamInfo kParams_OneString_OneFloat[] =
{
	{	"string",	kParamType_String,	0 },
	{	"float",	kParamType_Float,	0 },
};

static ParamInfo kParams_OneString_OneArray[2] =
{
	{	"string",	kParamType_String,	0 },
	{	"array",	kParamType_Integer,	0 },
};

static ParamInfo kParams_OneString_OneOptionalForm[2] =
{
	{	"string",	kParamType_String,	0 },
	{	"form",	kParamType_AnyForm,	1 },
};

static ParamInfo kParams_OneString_OneArray_OneOptionalInt[3] =
{
	{	"string",	kParamType_String,	0 },
	{	"array",	kParamType_Integer,	0 },
	{	"int",		kParamType_Integer,	1 },
};

static ParamInfo kParams_TwoFloats[2] =
{
	{	"float",	kParamType_Float,	0 },
	{	"float",	kParamType_Float,	0 },
};

static ParamInfo kParams_TwoDoubles[2] =
{
	{	"double",	kParamType_Double,	0 },
	{	"double",	kParamType_Double,	0 },
};

// static ParamInfo kParams_OneSpellItem[1] =
// {
// 	{	"spell", kParamType_SpellItem, 0 }, 
// };
// 
// static ParamInfo kParams_OneMagicItem[1] =
// {
// 	{	"spell", kParamType_MagicItem, 0 }, 
// };
// 
// static ParamInfo kParams_OneMagicEffect[1] =
// {
// 	{	"magic effect", kParamType_MagicEffect, 0 }, 
// };

static ParamInfo kParams_OneObjectID[1] =
{
	{	"item", kParamType_ObjectID, 0},
};

static ParamInfo kParams_OneOptionalObjectID[1] =
{
	{	"item", kParamType_ObjectID, 1},
};

static ParamInfo kParams_OneInt_OneOptionalObjectID[2] =
{
	{	"int",	kParamType_Integer,			0	},
	{	"object",			kParamType_ObjectID,	1	},
};

static ParamInfo kParams_TwoInts_OneOptionalObjectID[3] =
{
	{	"int",	kParamType_Integer,			0	},
	{	"int",	kParamType_Integer,			0	},
	{	"object",			kParamType_ObjectID,	1	},
};

static ParamInfo kParams_OneObjectID_OneInt[2] =
{
	{	"object",		kParamType_ObjectID,	0	},
	{	"integer",	kParamType_Integer,			0	},
};

static ParamInfo kParams_OneFloat_OneOptionalObjectID[2] =
{
	{	"float",		kParamType_Float,			0	},
	{	"object",			kParamType_ObjectID,	1	},
};

static ParamInfo kParams_OneMagicItem_OneOptionalObjectID[2] =
{
   {	"magic item",	kParamType_MagicItem,		0	},
   {	"item",			kParamType_ObjectID,	1	},
};

static ParamInfo kParams_OneInventoryItem_OneOptionalObjectID[2] =
{
   {	"inv item",		kParamType_MagicItem,		0	},
   {	"target item",	kParamType_ObjectID,	1	},
};

static ParamInfo kParams_OneFormList_OneOptionalObjectID[2] =
{
   {	"form list",	kParamType_FormList,		0	},
   {	"target item",	kParamType_ObjectID,	1	},
};


static ParamInfo kParams_OneActorValue[1] =
{
	{	"actor value", kParamType_ActorValue, 0},
};

// Format String param info taken from JIP.

#define FORMAT_STRING_PARAMS 	\
	{"format string",	kParamType_String, 0}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1} 

static ParamInfo kParams_FormatString[21] =
{
	FORMAT_STRING_PARAMS
};

#define SIZEOF_FMT_STRING_PARAMS 21
#define NUM_PARAMS(paramInfoName) SIZEOF_ARRAY(paramInfoName, ParamInfo)

static ParamInfo kParams_OneActorRef[1] =
{
	{	"actor reference",	kParamType_Actor,	0	},
};

static ParamInfo kParams_OneOptionalActorRef[1] =
{
	{	"actor reference",	kParamType_Actor,	1	},
};

static ParamInfo kParams_Axis[1] =
{
	{	"axis",	kParamType_Axis,	0	},
};

static ParamInfo kParams_OneFormList[1] =
{
	{	"form list", kParamType_FormList,	0		},
};

static ParamInfo kParams_OneFormList_OneForm[2] =
{
	{	"form list", kParamType_FormList,	0		},
	{	"form", kParamType_AnyForm,	0		},
};


static ParamInfo kParams_OneFormList_OneOptionalString[2] =
{
	{	"form list", kParamType_FormList,	0		},
	{	"string",	 kParamType_String,		1		},
};

static ParamInfo kParams_OneFormList_OneOptionalString_OneOptionalInt[3] =
{
	{	"form list", kParamType_FormList,	0		},
	{	"string",	 kParamType_String,		1		},
	{	"int",		 kParamType_Integer,	1		},
};

static ParamInfo kParams_OneString_OneOptionalObjectID[2] =
{
	{	"string",		kParamType_String,		0	},
	{	"item",			kParamType_ObjectID,	1	},
};

static ParamInfo kParams_TwoStrings[2] =
{
	{	"string",	kParamType_String,	0	},
	{	"string",	kParamType_String,	0	},
};

static ParamInfo kParams_TwoStrings_OneInt[3] =
{
	{	"string",	kParamType_String,	0	},
	{	"string",	kParamType_String,	0	},
	{	"int",		kParamType_Integer,	0	},
};

static ParamInfo kParams_TwoStrings_OneOptionalInt[3] =
{
	{	"string",	kParamType_String,	0	},
	{	"string",	kParamType_String,	0	},
	{	"int",		kParamType_Integer,	1	},
};

static ParamInfo kParams_TwoStrings_OneDouble[3] =
{
	{	"string",	kParamType_String,	0	},
	{	"string",	kParamType_String,	0	},
	{	"double",	kParamType_Double,	0	},
};

static ParamInfo kParams_TwoStrings_OneForm[3] =
{
	{	"string",	kParamType_String,	0	},
	{	"string",	kParamType_String,	0	},
	{	"form",		kParamType_AnyForm,	0	},
};

static ParamInfo kParams_ThreeStrings[3] =
{
	{	"string",	kParamType_String,	0	},
	{	"string",	kParamType_String,	0	},
	{	"string",	kParamType_String,	0	},
};

static ParamInfo kParams_OneObject[1] =
{
	{	"target item",	kParamType_TESObject,	0	},
};

static ParamInfo kParams_OneOptionalObject[1] =
{
   {	"target item",	kParamType_TESObject,	1	},
};

static ParamInfo kParams_OneOptionalObject_OneOptionalInt[2] =
{
   {	"target item",	kParamType_TESObject,	1	},
	{	"int",	kParamType_Integer,	1	},
};

static ParamInfo kParams_TwoOptionalObjects[2] =
{
   {	"target item",	kParamType_TESObject,	1	},
	{	"target item",	kParamType_TESObject,	1	},
};



static ParamInfo kParams_OneString_OneOptionalObject[2] =
{
	{	"string",		kParamType_String,			0	},
	{	"item",			kParamType_TESObject,	1	},
};

static ParamInfo kParams_OneObject_OneOptionalObject[2] =
{
	{	"target item",	kParamType_TESObject,		0	},
	{	"object",		kParamType_TESObject,		1	},
};

static ParamInfo kParams_OneInt_OneOptionalObject[2] =
{
	{	"int",	kParamType_Integer,	0	},
	{	"object",	kParamType_TESObject,	1	},
};

static ParamInfo kParams_OneInt_OneOptionalForm[2] =
{
	{	"int",	kParamType_Integer,	0	},
	{	"form",	kParamType_AnyForm,	1	},
};

static ParamInfo kParams_OneForm_OneOptionalInt[2] =
{
		{	"form",	kParamType_AnyForm,	0	},
	{	"int",	kParamType_Integer,	1	},
};

static ParamInfo kParams_SetEquippedFloat[2] =
{
	{	"val", kParamType_Float, 0 },
	{	"slot", kParamType_Integer, 0 },
};

static ParamInfo kParams_FormListInteger[2] =
{
	{	"form list", kParamType_FormList,	0		},
	{	"index",	 kParamType_Integer,	0		}
};

static ParamInfo kParams_OneQuest[1] =
{
	{	"quest", kParamType_Quest, 0 },
};

static ParamInfo kParams_OneNPC[1] =
{
	{	"NPC",	kParamType_NPC,	1	},
};

static ParamInfo kParams_OneOptionalObjectRef[1] =
{
	{	"ref", kParamType_ObjectRef, 1},
};

static ParamInfo kParams_OneIntOneOptionalObjectRef[2] =
{
	{	"flag",		kParamType_Integer,	0	},
	{	"ref",		kParamType_ObjectRef,	1	},
};

static ParamInfo kParams_OneIndexOneOptionalObjectRef[2] =
{
	{	"index",		kParamType_Integer,	0	},
	{	"ref",		kParamType_ObjectRef,	1	},
};

static ParamInfo kParams_OnePackageOneIndexOneOptionalObjectRef[3] =
{
	{	"package",		kParamType_AnyForm,		0	},
	{	"index",		kParamType_Integer,		0	},
	{	"ref",			kParamType_ObjectRef,	1	},
};

static ParamInfo kParams_OneForm_OneOptionalObjectRef[2] =
{
	{	"form",	kParamType_AnyForm,		0	},
	{	"ref",	kParamType_ObjectRef,	1	},
};

static ParamInfo kParams_OneForm_OneOptionalForm[2] =
{
	{	"form",	kParamType_AnyForm,		0	},
	{	"form",	kParamType_AnyForm,	1	},
};

static ParamInfo kParams_OneForm_OneInt[2] =
{
	{	"form",	kParamType_AnyForm,	0	},
	{	"int",	kParamType_Integer, 0	},
};

static ParamInfo kParams_OneForm_OneInt_OneOptionalScript[3] =
{
	{	"form",	kParamType_AnyForm,	0	},
	{	"int",	kParamType_Integer, 0	},
	{	"script",	kParamType_AnyForm, 1	},
};

static ParamInfo kParams_OneForm[1] =
{
	{	"form",	kParamType_AnyForm,	0	},
};

static ParamInfo kParams_TwoForms[2] =
{
	{	"form",	kParamType_AnyForm,	0	},
	{	"form",	kParamType_AnyForm,	0	},
};

static ParamInfo kParams_OneForm_OneArray[2] =
{
	{	"form",	kParamType_AnyForm,	0	},
	{	"array",	kParamType_Integer,	0	},
};

static ParamInfo kParams_OneForm_OneFloat[2] =
{
	{	"form",		kParamType_AnyForm,	0	},
	{	"float",	kParamType_Float,	0	},
};

static ParamInfo kParams_OneForm_OneInt_OneString[3] =
{
	{	"form",	kParamType_AnyForm,	0	},
	{	"int",	kParamType_Integer,	0	},
	{	"array",	kParamType_String,	0	},
};


static ParamInfo kParams_OneOptionalForm[1] =
{
	{	"form",	kParamType_AnyForm,	1	},
};

static ParamInfo kParams_OneOptionalForm_OneOptionalInt[2] =
{
	{	"form",	kParamType_AnyForm,	1	},
	{	"int",kParamType_Integer,		1	},
};

static ParamInfo kParams_EquipItem[3] =
{
	{	"item",			kParamType_ObjectID,	0	},
	{	"silent",		kParamType_Integer,		1	},
	{	"lockEquip",	kParamType_Integer,		1	},
};

static ParamInfo kParams_OneFaction[1] =
{
	{	"faction",	kParamType_Faction,	0	},
};

static ParamInfo kParams_OneOptionalActorBase[1] =
{
	{	"base actor",	kParamType_ActorBase,	1	},
};

static ParamInfo kParams_OneFloat_OneOptionalActorBase[2] =
{
	{	"float",		kParamType_Float,		0	},
	{	"base actor",	kParamType_ActorBase,	1	},
};

static ParamInfo kParams_OneInt_OneOptionalActorBase[2] =
{
	{	"int",			kParamType_Integer,		0	},
	{	"base actor",	kParamType_ActorBase,	1	},
};

static ParamInfo kParams_OneRace[1] =
{
	{	"race",	kParamType_Race,	0	},
};

static ParamInfo kParams_GenericForm[4] =
{
	{	"which",			kParamType_Integer,	0	},
	{	"containingForm",	kParamType_AnyForm,	0	},
	{	"form",				kParamType_AnyForm,	0	},
	{	"index",			kParamType_Integer,	0	},
};

static ParamInfo kParams_GenericDeleteForm[3] =
{
	{	"which",			kParamType_Integer,	0	},
	{	"containingForm",	kParamType_AnyForm,	0	},
	{	"index",			kParamType_Integer,	0	},
};

static ParamInfo kParams_OneIntOneForm[2] =
{
	{	"index",	kParamType_Integer, 0	},
	{	"form",		kParamType_AnyForm,	0	},
};

// Array is passed as an int; array type otherwise requires Compiler Override.
static ParamInfo kParams_OneArrayID[1] =
{
	{	"array",	kParamType_Integer, 0	},
};
static ParamInfo kParams_TwoArrayIDs[2] =
{
	{	"array",	kParamType_Integer, 0	},
	{	"array",	kParamType_Integer, 0	},
};
static ParamInfo kParams_OneArrayID_OneDouble[2] =
{
	{	"array",	kParamType_Integer, 0	},
	{	"double",	kParamType_Double, 0	},

};


static ParamInfo kParams_OneFormlist_OneArray_OneOptionalIndex[3] =
{
	{	"form list", kParamType_FormList,	0		},
	{	"array",	 kParamType_Integer,	0		},
	{	"index",	 kParamType_Integer,	1		},
};

static ParamInfo kParams_TwoFormLists_OneOptionalIndex[3] =
{
	{	"form list", kParamType_FormList,	0		},
	{	"form list", kParamType_FormList,	0		},
	{	"index",	 kParamType_Integer,	1		},
};

static ParamInfo kParams_OneChallenge[1] =
{
	{	"challenge",	kParamType_Challenge, 0	},
};

#if 0
static ParamInfo kParams_OneBasicType[] =
{
	//{	"expression",	kNVSEParamType_BasicType,	0	},
};
#endif


static ParamInfo kParams_Event_OneForm[4] =
{
	{ "setOrRemove", kParamType_Integer, 0 },
	{ "Script", kParamType_AnyForm, 0 },
	{ "flags", kParamType_Integer, 1 },
	{ "Form", kParamType_AnyForm, 1 }

};

static ParamInfo kParams_Event[3] =
{
	{ "setOrRemove", kParamType_Integer, 0 },
	{ "Script", kParamType_AnyForm, 0 },
	{ "flags", kParamType_Integer, 1 },

};

static ParamInfo kParams_Event_OneForm_OneInt[5] =
{
	{ "setOrRemove", kParamType_Integer, 0 },
	{ "Script", kParamType_AnyForm, 0 },
	{ "flags", kParamType_Integer, 1 },
	{ "Form", kParamType_AnyForm, 1 },
	{ "Integer", kParamType_Integer, 1 },

};


//---------------JIP STUFF

static ParamInfo kParams_JIP_OneInt_OneOptionalActorBase[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneItemOrList_OneInt_OneOptionalFloat_OneOptionalInt[] =
{
	{"Item/List", kParamType_InvObjOrFormList, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_OneFormatString[] =
{
	{"String", kParamType_String, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneFloat[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_TwoInts[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_TwoForms[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneOptionalForm[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneOptionalQuest[] =
{
	{"String", kParamType_String, 0},
	{"Quest", kParamType_Quest, 1}
};

static ParamInfo kParams_JIP_OneQuest_OneFloat[] =
{
	{"Quest", kParamType_Quest, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneAIPackage_OneInt[] =
{
	{"Package", kParamType_AIPackage, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneAIPackage_TwoInts[] =
{
	{"Package", kParamType_AIPackage, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneGlobal[] =
{
	{"Global", kParamType_Global, 0}
};

static ParamInfo kParams_JIP_OneGlobal_OneFloat[] =
{
	{"Global", kParamType_Global, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneFaction[] =
{
	{"Faction", kParamType_Faction, 0}
};

static ParamInfo kParams_JIP_OneFaction_OneOptionalReputation[] =
{
	{"Faction", kParamType_Faction, 0},
	{"Reputation", kParamType_Reputation, 1}
};

static ParamInfo kParams_JIP_OptionalForm[] =
{
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneAxis_OneOptionalForm[] =
{
	{"Axis", kParamType_Axis, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_TwoDoubles_ThreeOptionalDoubles[] =
{
	{"Double", kParamType_Double, 0},
	{"Double", kParamType_Double, 0},
	{"Double", kParamType_Double, 1},
	{"Double", kParamType_Double, 1},
	{"Double", kParamType_Double, 1}
};

static ParamInfo kParams_JIP_OneCombatStyle_OneInt[] =
{
	{"Combat Style", kParamType_CombatStyle, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneCombatStyle_OneInt_OneFloat[] =
{
	{"Combat Style", kParamType_CombatStyle, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneCombatStyle_TwoInts[] =
{
	{"Combat Style", kParamType_CombatStyle, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneString_TwoOptionalInts[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_TwoInts[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneString_OneInt_OneFormatString[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneClass_OneInt[] =
{
	{"Class", kParamType_Class, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneClass_TwoInts[] =
{
	{"Class", kParamType_Class, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneSound_OneInt[] =
{
	{"Sound", kParamType_Sound, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneSound_OneInt_OneFloat[] =
{
	{"Sound", kParamType_Sound, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneSound_TwoInts[] =
{
	{"Sound", kParamType_Sound, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneOptionalSound[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Sound", kParamType_Sound, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneOptionalObjectID[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Object ID", kParamType_ObjectID, 1}
};

static ParamInfo kParams_JIP_OneWeatherID_OneInt[] =
{
	{"Weather ID", kParamType_WeatherID, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneWeatherID_OneInt_OneOptionalImageSpaceMod[] =
{
	{"Weather ID", kParamType_WeatherID, 0},
	{"Integer", kParamType_Integer, 0},
	{"Image Space Mod", kParamType_ImageSpaceModifier, 1}
};

static ParamInfo kParams_JIP_OneWeatherID_OneInt_OneString[] =
{
	{"Weather ID", kParamType_WeatherID, 0},
	{"Integer", kParamType_Integer, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_OneWeatherID[] =
{
	{"Weather ID", kParamType_WeatherID, 0}
};

static ParamInfo kParams_JIP_OneWeatherID_OneString[] =
{
	{"Weather ID", kParamType_WeatherID, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_OneWeatherID_OneInt_OneDouble[] =
{
	{"Weather ID", kParamType_WeatherID, 0},
	{"Integer", kParamType_Integer, 0},
	{"Double", kParamType_Double, 0}
};

static ParamInfo kParams_JIP_OneWeatherID_TwoInts_OneOptionalInt[] =
{
	{"Weather ID", kParamType_WeatherID, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneWeatherID_ThreeInts_OneOptionalInt[] =
{
	{"Weather ID", kParamType_WeatherID, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneString[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_OneForm_OneString[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_OneForm_OneWeatherID_OneInt_OneOptionalGlobal[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Weather ID", kParamType_WeatherID, 0},
	{"Integer", kParamType_Integer, 0},
	{"Global", kParamType_Global, 1}
};

static ParamInfo kParams_JIP_OneForm_OneWeatherID[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Weather ID", kParamType_WeatherID, 0}
};

static ParamInfo kParams_JIP_OneAxis_OneFloat[] =
{
	{"Axis", kParamType_Axis, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_ThreeInts[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneItemOrList_OneObjectRef_TwoOptionalInts[] =
{
	{"Item/List", kParamType_InvObjOrFormList, 0},
	{"Object Ref", kParamType_ObjectRef, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneInt_OneFloat_OneOptionalActorBase[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneMagicEffect[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Magic Effect", kParamType_MagicEffect, 0}
};

static ParamInfo kParams_JIP_OneOptionalQuest[] =
{
	{"Quest", kParamType_Quest, 1}
};

static ParamInfo kParams_JIP_OneString_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneOptionalInt_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 1},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneFloat_OneOptionalInt_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0},
	{"Integer", kParamType_Integer, 1},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneForm_OneOptionalInt_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 1},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_TwoStrings_OneOptionalInt_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 1},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneInt_OneOptionalForm[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneInt_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneDouble_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Double", kParamType_Double, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneFloat_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneString_OneForm_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_TwoStrings_OneOptionalForm[] =
{
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_FourInts[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneForm_TwoInts_OneFloat[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_TwoInts_OneOptionalForm[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneForm_OneOptionalForm[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneSound[] =
{
	{"Sound", kParamType_Sound, 0}
};

static ParamInfo kParams_JIP_OneSound_OneString[] =
{
	{"Sound", kParamType_Sound, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_OneWorldspace[] =
{
	{"Worldspace", kParamType_WorldSpace, 0}
};

static ParamInfo kParams_JIP_OneWorldspace_OneOptionalForm[] =
{
	{"Worldspace", kParamType_WorldSpace, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneForm_OneOptionalActorBase[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneInt_OneFloat[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneInt_OneFormatString[] =
{
	{"Integer", kParamType_Integer, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_TwoInts_OneOptionalInt[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneObjectRef_OneOptionalInt[] =
{
	{"Object Ref", kParamType_ObjectRef, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneList_TwoOptionalInts[] =
{
	{"List", kParamType_FormList, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneFaction_OneOptionalInt[] =
{
	{"Faction", kParamType_Faction, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneQuest_OneInt[] =
{
	{"Quest", kParamType_Quest, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneWorldspace_OneInt[] =
{
	{"Worldspace", kParamType_WorldSpace, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneWorldspace_TwoInts[] =
{
	{"Worldspace", kParamType_WorldSpace, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneString_OneInt_OneOptionalActorBase[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneString_OneOptionalActorBase[] =
{
	{"String", kParamType_String, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneObjectRef[] =
{
	{"Object Ref", kParamType_ObjectRef, 0}
};

static ParamInfo kParams_JIP_OneInt_ThreeFloats[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_OneEffect_TwoInts_TwoOptionalInts[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Magic Effect", kParamType_MagicEffect, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneItemOrList_TwoOptionalInts[] =
{
	{"Item/List", kParamType_InvObjOrFormList, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_TwoOptionalInts[] =
{
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneActorValue_OneFloat_OneOptionalActorBase[] =
{
	{"Actor Value", kParamType_ActorValue, 0},
	{"Float", kParamType_Float, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneActorValue_OneInt_OneOptionalActorBase[] =
{
	{"Actor Value", kParamType_ActorValue, 0},
	{"Int", kParamType_Integer, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneOptionalCell[] =
{
	{"Cell", kParamType_Cell, 1}
};

static ParamInfo kParams_JIP_OneFloat_OneFormatString[] =
{
	{"Float", kParamType_Float, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneFloat_OneInt_OneFormatString[] =
{
	{"Float", kParamType_Float, 0},
	{"Int", kParamType_Integer, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneForm_OneFormatString[] =
{
	{"Form", kParamType_AnyForm, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneInt_OneString[] =
{
	{"Integer", kParamType_Integer, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_OneForm_TwoFloats_OneFormatString[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneActor_OneTopic_TwoOptionalInts[] =
{
	{"Actor", kParamType_Actor, 0},
	{"Topic", kParamType_Topic, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneTopic[] =
{
	{"Topic", kParamType_Topic, 0}
};

static ParamInfo kParams_JIP_OneForm_ThreeFloats[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_FourOptionalInts_OneOptionalList[] =
{
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"List", kParamType_FormList, 1}
};

static ParamInfo kParams_JIP_OneInt_OneOptionalActor[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Actor", kParamType_Actor, 1}
};

static ParamInfo kParams_JIP_OneAIPackage[] =
{
	{"Package", kParamType_AIPackage, 0}
};

static ParamInfo kParams_JIP_OneObjectID_OneActorValue[] =
{
	{"Object ID", kParamType_ObjectID, 0},
	{"Actor Value", kParamType_ActorValue, 0}
};

static ParamInfo kParams_JIP_OneMessage[] =
{
	{"Message", kParamType_Message, 0}
};

static ParamInfo kParams_JIP_OneMessage_OneInt[2] =
{
	{"Message", kParamType_Message, 0},
	{"Integer", kParamType_Integer, 0},
};

static ParamInfo kParams_JIP_OneSound_OneOptionalObjectRef[] =
{
	{"Sound", kParamType_Sound, 0},
	{"Object Ref", kParamType_ObjectRef, 1}
};

static ParamInfo kParams_JIP_OneObjectRef_TwoStrings_OneOtionalInt[] =
{
	{"Object Ref", kParamType_ObjectRef, 0},
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_OneInt[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneString_OneInt_OneFloat[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneForm_OneInt[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneForm_OneAnimGroup[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Anim Group", kParamType_AnimationGroup, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneForm[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 0}
};

static ParamInfo kParams_JIP_OneActorValue_OneOptionalActorBase[] =
{
	{"Actor Value", kParamType_ActorValue, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_ThreeFloats[] =
{
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneOptionalAxis[] =
{
	{"Axis", kParamType_Axis, 1}
};

static ParamInfo kParams_JIP_OnePerk_OneInt[] =
{
	{"Perk", kParamType_Perk, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OnePerk_TwoInts[] =
{
	{"Perk", kParamType_Perk, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneList_OneInt_TwoOptionalInts[] =
{
	{"List", kParamType_FormList, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneInt_OneOptionalInt[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_OneOptionalInt[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_ThreeForms[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 0}
};

static ParamInfo kParams_JIP_TwoForms_TwoInts_OneFloat[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneDouble_OneOptionalInt[] =
{
	{"Double", kParamType_Double, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneFloat_OneOptionalForm[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_TwoForms_OneOptionalForm[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneInt_TwoOptionalInts[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_EquipData[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Float", kParamType_Float, 1},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneFloat_OneOptionalForm[] =
{
	{"Float", kParamType_Float, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_ThreeFloat[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneInt_TwoOptionalInts_OneOptionalCell[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"Cell", kParamType_Cell, 1}
};

static ParamInfo kParams_JIP_OneInt_OneForm_OneOptionalFloat_OneOptionalInt[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Float", kParamType_Float, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneForm_OneOptionalInt[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneOptionalInt[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_TwoOptionalForms[] =
{
	{"Form", kParamType_AnyForm, 1},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_TwoForms_OneInt[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_FormCondition[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneString_OneOptionalString[] =
{
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneString_OneFloat_OneOptionalString[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_TwoString_OneOptionalString[] =
{
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneString_OneInt_OneOptionalString[] =
{
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneOptionalString[] =
{
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneInt_OneOptionalObjectRef[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Object Ref", kParamType_ObjectRef, 1}
};

static ParamInfo kParams_JIP_OneActorBase[] =
{
	{"Actor Base", kParamType_ActorBase, 0}
};

static ParamInfo kParams_JIP_OneFloat_OneOptionalCell[] =
{
	{"Float", kParamType_Float, 0},
	{"Cell", kParamType_Cell, 1}
};

static ParamInfo kParams_JIP_OneActorBase_OneInt[] =
{
	{"Actor Base", kParamType_ActorBase, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneActorBase_TwoInts[] =
{
	{"Actor Base", kParamType_ActorBase, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneCell[] =
{
	{"Cell", kParamType_Cell, 0}
};

static ParamInfo kParams_JIP_OneCell_OneOptionalForm[] =
{
	{"Cell", kParamType_Cell, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneCell_OneInt[] =
{
	{"Cell", kParamType_Cell, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneCell_TwoInts[] =
{
	{"Cell", kParamType_Cell, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_TwoInts_OneOptionalActorBase[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneFaction_OneInt_OneOptionalActorBase[] =
{
	{"Faction", kParamType_Faction, 0},
	{"Integer", kParamType_Integer, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneImageSpace_OneInt[] =
{
	{"Image Space", kParamType_ImageSpace, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneImageSpace_OneInt_OneFloat[] =
{
	{"Image Space", kParamType_ImageSpace, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneIMOD_OneInt[] =
{
	{"IMOD", kParamType_ImageSpaceModifier, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneIMOD_OneInt_OneFloat[] =
{
	{"IMOD", kParamType_ImageSpaceModifier, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_TwoOptionalInts[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_TwoStrings_OneFloat[] =
{
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneGlobal_OneOptionalForm[] =
{
	{"Global", kParamType_Global, 0},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneCell_OneString[] =
{
	{"Cell", kParamType_Cell, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_TwoOptionalInts_OneOptionalObjectRef[] =
{
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"Object Ref", kParamType_ObjectRef, 1}
};

static ParamInfo kParams_JIP_OneForm_OneString_OneInt[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneMagicItem[] =
{
	{"Magic Item", kParamType_MagicItem, 0}
};

static ParamInfo kParams_JIP_FourOptionalFloats[] =
{
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1}
};

static ParamInfo kParams_JIP_OneInt_OneFloat_TwoOptionalForms[] =
{
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0},
	{"Form", kParamType_AnyForm, 1},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneQuest_OneInt_OneOptionalInt[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Quest", kParamType_Quest, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneForm_OneInt_ThreeOptionalForms[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 1},
	{"Form", kParamType_AnyForm, 1},
	{"Form", kParamType_AnyForm, 1}
};

static ParamInfo kParams_JIP_OneObjectID_OneInt_OneFormatString[] =
{
	{"Object ID", kParamType_ObjectID, 0},
	{"Integer", kParamType_Integer, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneOptionalObjectRef_TwoOptionalInts[] =
{
	{"Object Ref", kParamType_ObjectRef, 1},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneObjectRef_OneFloat[] =
{
	{"Object Ref", kParamType_ObjectRef, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneQuest_TwoInts_OneObjectRef[] =
{
	{"Quest", kParamType_Quest, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Object Ref", kParamType_ObjectRef, 0}
};

static ParamInfo kParams_JIP_OneQuest_OneInt_OneObjectRef[] =
{
	{"Quest", kParamType_Quest, 0},
	{"Integer", kParamType_Integer, 0},
	{"Object Ref", kParamType_ObjectRef, 0}
};

static ParamInfo kParams_JIP_OneQuest_OneInt_OneFormatString[] =
{
	{"Quest", kParamType_Quest, 0},
	{"Integer", kParamType_Integer, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneObjectID_OneOptionalObjectID[] =
{
	{"Object ID", kParamType_ObjectID, 0},
	{"Object ID", kParamType_ObjectID, 1}
};

static ParamInfo kParams_JIP_OneForm_OneOptionalString[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneSpellItem_OneOptionalActorBase[] =
{
	{"Spell Item", kParamType_SpellItem, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_OneMagicItem_OneOptionalActor[] =
{
	{"Magic Item", kParamType_MagicItem, 0},
	{"Actor", kParamType_Actor, 1}
};

static ParamInfo kParams_JIP_ThreeOptionalInts[] =
{
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneAIPackage_ThreeInts[] =
{
	{"Package", kParamType_AIPackage, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneDouble[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Double", kParamType_Double, 0}
};

static ParamInfo kParams_JIP_OneDouble[] =
{
	{"Double", kParamType_Double, 0}
};

static ParamInfo kParams_JIP_FourFloats_OneOptionalObjectRef[] =
{
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Object Ref", kParamType_ObjectRef, 1}
};

static ParamInfo kParams_JIP_OneActorBase_OneOptionalString[] =
{
	{"Actor Base", kParamType_ActorBase, 0},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneActorBase_OneString[] =
{
	{"Actor Base", kParamType_ActorBase, 0},
	{"String", kParamType_String, 0}
};

static ParamInfo kParams_JIP_OneActorBase_OneForm[] =
{
	{"Actor Base", kParamType_ActorBase, 0},
	{"Form", kParamType_AnyForm, 0}
};

static ParamInfo kParams_JIP_OneString_ThreeFloats_OneOptionalInt[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_OneFloat[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneString_OneAxis[] =
{
	{"String", kParamType_String, 0},
	{"Axis", kParamType_Axis, 0}
};

static ParamInfo kParams_JIP_OneString_OneAxis_OneFloat[] =
{
	{"String", kParamType_String, 0},
	{"Axis", kParamType_Axis, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneForm_ThreeFloats[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneActorValue_OneOptionalInt[] =
{
	{"Actor Value", kParamType_ActorValue, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_ThreeGlobals_OneOptionalFloat_OneOptionalInt_OneOptionalString[] =
{
	{"Global", kParamType_Global, 0},
	{"Global", kParamType_Global, 0},
	{"Global", kParamType_Global, 0},
	{"Float", kParamType_Float, 1},
	{"Integer", kParamType_Integer, 1},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneObjectRef_OneString_ThreeOptionalFloats[] =
{
	{"Object Ref", kParamType_ObjectRef, 0},
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1}
};

static ParamInfo kParams_JIP_OneObjectID_OneOptionalActorBase[] =
{
	{"Object ID", kParamType_ObjectID, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_TwoStrings_OneInt[] =
{
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneString_OneForm_ThreeOptionalFloats[] =
{
	{"String", kParamType_String, 0},
	{"Form", kParamType_AnyForm, 0},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1}
};

static ParamInfo kParams_JIP_OneEffect[] =
{
	{"Magic Effect", kParamType_MagicEffect, 0}
};

static ParamInfo kParams_JIP_OneQuest_TwoInts[] =
{
	{"Quest", kParamType_Quest, 0},
	{"Integer", kParamType_Integer, 0},
	{"Integer", kParamType_Integer, 0}
};

static ParamInfo kParams_JIP_OneFloat_ThreeOptionalFloats_OneOptionalInt[] =
{
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_OneOptionalString_OneOptionalInt[] =
{
	{"String", kParamType_String, 0},
	{"String", kParamType_String, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneObjectRef_OneFloat_OneOptionalInt[] =
{
	{"Object Ref", kParamType_ObjectRef, 0},
	{"Float", kParamType_Float, 0},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_TwoDoubles[] =
{
	{"Double", kParamType_Double, 0},
	{"Double", kParamType_Double, 0}
};

static ParamInfo kParams_JIP_OneObjectID_OneInt_OneFloat[] =
{
	{"Object ID", kParamType_ObjectID, 0},
	{"Integer", kParamType_Integer, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_TwoFloats_ThreeOptionalInts[] =
{
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneNote[] =
{
	{"Note", kParamType_Note, 0}
};

static ParamInfo kParams_JIP_OneForm_OneOptionalSpellItem[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Spell Item", kParamType_SpellItem, 1}
};

static ParamInfo kParams_JIP_OneString_TwoFloats[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneActorBase_OneOptionalActorBase[] =
{
	{"Actor Base", kParamType_ActorBase, 0},
	{"Actor Base", kParamType_ActorBase, 1}
};

static ParamInfo kParams_JIP_TwoFloats_OneOptionalObjectRef_OneOptionalInt[] =
{
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Object Ref", kParamType_ObjectRef, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_ThreeOptionalFloats_OneOptionalInt[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Float", kParamType_Float, 1},
	{"Integer", kParamType_Integer, 1}
};

static ParamInfo kParams_JIP_OneString_ThreeFloats[] =
{
	{"String", kParamType_String, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0},
	{"Float", kParamType_Float, 0}
};

static ParamInfo kParams_JIP_OneForm_OneInt_OneFormatString[] =
{
	{"Form", kParamType_AnyForm, 0},
	{"Integer", kParamType_Integer, 0},
	FORMAT_STRING_PARAMS
};

static ParamInfo kParams_JIP_OneOptionalObjectRef_OneOptionalInt_OneOptionalFloat_OneOptionalString[] =
{
	{"Object Ref", kParamType_ObjectRef, 1},
	{"Integer", kParamType_Integer, 1},
	{"Float", kParamType_Float, 1},
	{"String", kParamType_String, 1}
};

#if 0
static ParamInfo kParams_JIP_OneScript[] =
{
	{"Script", kParamType_Script, 0}
};
#endif

static ParamInfo kParams_JIP_OneOptionalInt_OneOptionalString[] =
{
	{"Integer", kParamType_Integer, 1},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_JIP_OneObjectID_OneOptionalString[] =
{
	{"Object ID", kParamType_ObjectID, 0},
	{"String", kParamType_String, 1}
};


// ShowOff stuff

static ParamInfo kParams_TwoOptionalInts_OneOptionalString[3] =
{
	{"Integer", kParamType_Integer, 1},
	{"Integer", kParamType_Integer, 1},
	{"String", kParamType_String, 1}
};

static ParamInfo kParams_TwoOptionalInts_OneOptionalContainerRef_OneOptionalList[4] =
{
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"container reference",	kParamType_Container,	1	},
	{	"form list",	kParamType_FormList,	1	},
};

static ParamInfo kParams_ThreeOptionalInts_OneOptionalContainerRef_TwoOptionalInts_OneOptionalList[7] =
{
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"container reference",	kParamType_Container,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"form list",	kParamType_FormList,	1	},
};

static ParamInfo kParams_OneOptionalContainerRef_FiveOptionalInts_OneOptionalList[7] =
{
	{	"container reference",	kParamType_Container,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"int",	kParamType_Integer,	1	},
	{	"form list",	kParamType_FormList,	1	},
};

static ParamInfo kParams_TwoFactions_OneInt_TwoOptionalInts[] =
{
	{	"faction1",	kParamType_Faction,	0	},
	{	"faction2",	kParamType_Faction,	0	},

	{	"int",kParamType_Integer,		0	},
	
	{	"int",kParamType_Integer,		1	},
	{	"int",kParamType_Integer,		1	},
};
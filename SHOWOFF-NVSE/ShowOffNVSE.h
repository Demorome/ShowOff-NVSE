#pragma once

#include <atomic>

#include "ParamInfos.h"
#include "PluginAPI.h"
#include "GameUI.h" 
#include "common/ICriticalSection.h"
#include "GameData.h"
#include "decoding.h"
#include "SOTypes.h"

extern ICriticalSection g_Lock;
extern std::atomic<bool> g_ShowFuncDebug;

//NVSE Globals
extern bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
extern bool (*ExtractFormatStringArgs)(UInt32 fmtStringPos, char* buffer, COMMAND_ARGS_EX, UInt32 maxParams, ...);  // From JIP_NVSE.H
extern NVSEArrayVarInterface* g_arrInterface;
extern NVSEArrayVar* (*CreateArray)(const NVSEArrayElement* data, UInt32 size, Script* callingScript);
extern NVSEArrayVar* (*CreateStringMap)(const char** keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
extern NVSEArrayVar* (*CreateMap)(const double* keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
extern bool (*AssignArrayResult)(NVSEArrayVar* arr, double* dest);
extern void (*SetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, const NVSEArrayElement& value);
extern void (*AppendElement)(NVSEArrayVar* arr, const NVSEArrayElement& value);
extern UInt32(*GetArraySize)(NVSEArrayVar* arr);
extern NVSEArrayVar* (*LookupArrayByID)(UInt32 id);
extern bool (*GetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, NVSEArrayElement& outElement);
extern bool (*GetArrayElements)(NVSEArrayVar* arr, NVSEArrayElement* elements, NVSEArrayElement* keys);
extern NVSEStringVarInterface* g_strInterface;
extern bool (*AssignString)(COMMAND_ARGS, const char* newValue);
extern const char* (*GetStringVar)(UInt32 stringID);
extern NVSEMessagingInterface* g_msg;
extern NVSEScriptInterface* g_scriptInterface;
extern NVSECommandTableInterface* g_commandInterface;
extern const CommandInfo* (*GetCmdByName)(const char* name);
extern bool (*FunctionCallScript)(Script* funcScript, TESObjectREFR* callingObj, TESObjectREFR* container, NVSEArrayElement* result, UInt8 numArgs, ...);
extern NVSEArrayElement EventResultPtr;
extern bool (*FunctionCallScriptAlt)(Script* funcScript, TESObjectREFR* callingObj, UInt8 numArgs, ...);
extern TESObjectREFR* (__stdcall *InventoryRefCreate)(TESObjectREFR* container, TESForm* itemForm, SInt32 countDelta, ExtraDataList* xData);

//Singletons
extern HUDMainMenu* g_HUDMainMenu;
extern TileMenu** g_tileMenuArray;
extern UInt32 g_screenWidth;
extern UInt32 g_screenHeight;
extern PlayerCharacter* g_thePlayer;
extern ActorValueOwner* g_playerAVOwner;
extern ProcessManager* g_processManager;
extern InterfaceManager* g_interfaceManager;
extern BSWin32Audio* g_bsWin32Audio;
extern DataHandler* g_dataHandler;
extern BSAudioManager* g_audioManager;
extern Sky** g_currentSky;

//-Hook Globals
extern std::atomic<bool> g_canPlayerPickpocketInCombat;
extern ActorAndItemPairs g_noEquipMap;

//-Force Pickpocketting INI globals (enabled via function)
extern std::atomic<float> g_fForcePickpocketBaseAPCost;
extern std::atomic<float> g_fForcePickpocketMinAPCost;
extern std::atomic<float> g_fForcePickpocketMaxAPCost;
extern std::atomic<float> g_fForcePickpocketPlayerAgilityMult;
extern std::atomic<float> g_fForcePickpocketPlayerSneakMult;
extern std::atomic<float> g_fForcePickpocketTargetPerceptionMult;
extern std::atomic<float> g_fForcePickpocketItemWeightMult;
extern std::atomic<float> g_fForcePickpocketItemValueMult;
extern std::atomic<float> g_fForcePickpocketPlayerStrengthMult;
extern std::atomic<float> g_fForcePickpocketTargetStrengthMult;
extern char* g_fForcePickpocketFailureMessage;

//-PreventBrokenItemRepairing (PBIR) INI globals 
extern std::atomic<bool> g_PBIR_On;
extern char* g_PBIR_FailMessage;


// Misc.
#define NUM_ARGS *((UInt8*)scriptData + *opcodeOffsetPtr)  //Probably breaks Compiler Override!! - Credits to Stewie.
#define REFR_RES *(UInt32*)result  //From JIPLN

//Below are already defined in GameForms.h - All from JIPLN
#define IS_TYPE(form, type) (*(UInt32*)form == kVtbl_##type)  
#define NOT_ID(form, type) (form->typeID != kFormType_##type) 
#define IS_ID(form, type) (form->typeID == kFormType_##type)






#pragma once

#include <atomic>
#include <memory>

#include "ParamInfos.h"
#include "NVSEParamInfoList.h"
#include "ExtractArgs_TypeSafe.h"

#include "PluginAPI.h"
#include "GameUI.h" 
#include "common/ICriticalSection.h"
#include "GameData.h"
#include "GameEffects.h"
#include "decoding.h"
#include "SOTypes.h"
#include "InventoryRef.h"

using namespace std::literals;

extern ICriticalSection g_Lock;
extern std::atomic<bool> g_ShowFuncDebug;

// NVSE Globals
extern bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
extern bool (*ExtractFormatStringArgs)(uint32_t fmtStringPos, char* buffer, COMMAND_ARGS_EX, uint32_t maxParams, ...);  // From JIP_NVSE.H
extern NVSEArrayVarInterface* g_arrInterface;
extern NVSEArrayVar* (*CreateArray)(const NVSEArrayElement* data, uint32_t size, Script* callingScript);
extern NVSEArrayVar* (*CreateStringMap)(const char** keys, const NVSEArrayElement* values, uint32_t size, Script* callingScript);
extern NVSEArrayVar* (*CreateMap)(const double* keys, const NVSEArrayElement* values, uint32_t size, Script* callingScript);
extern bool (*AssignArrayResult)(NVSEArrayVar* arr, double* dest);
extern void (*SetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, const NVSEArrayElement& value);
extern void (*AppendElement)(NVSEArrayVar* arr, const NVSEArrayElement& value);
extern uint32_t(*GetArraySize)(NVSEArrayVar* arr);
extern NVSEArrayVar* (*LookupArrayByID)(uint32_t id);
extern bool (*GetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, NVSEArrayElement& outElement);
extern bool (*GetArrayElements)(NVSEArrayVar* arr, NVSEArrayElement* elements, NVSEArrayElement* keys);
extern NVSEStringVarInterface* g_strInterface;
extern bool (*AssignString)(COMMAND_ARGS, const char* newValue);
extern const char* (*GetStringVar)(uint32_t stringID);
extern NVSEMessagingInterface* g_msg;
extern NVSEScriptInterface* g_scriptInterface;
extern NVSECommandTableInterface* g_commandInterface;
extern const CommandInfo* (*GetCmdByName)(const char* name);
extern bool (*FunctionCallScript)(Script* funcScript, TESObjectREFR* callingObj, TESObjectREFR* container, NVSEArrayElement* result, uint8_t numArgs, ...);
extern bool (*FunctionCallScriptAlt)(Script* funcScript, TESObjectREFR* callingObj, uint8_t numArgs, ...);
extern InventoryRef* (*InventoryRefGetForID)(uint32_t refID);
extern TESObjectREFR* (__stdcall *InventoryRefCreateEntry)(TESObjectREFR* container, TESForm* itemForm, int32_t countDelta, ExtraDataList* xData);
typedef InventoryRef* (*_InventoryRefCreate)(TESObjectREFR* container, const InventoryRef::Data& data, bool bValidate);
extern _InventoryRefCreate InventoryRefCreate;
extern NVSEEventManagerInterface* g_eventInterface;

extern DWORD g_mainThreadID;

// Game functions
extern bool (__cdecl *GetIsGodMode)();

//-Hook Globals
extern std::atomic<bool> g_canPlayerPickpocketInCombat;
#if 0
extern ActorAndItemPairs g_noEquipMap;
#endif
extern bool g_bNoSelfRepairingBrokenItems;
extern bool g_bNoVendorRepairingBrokenItems;
extern bool g_bResetInteriorResetsActors;
extern bool g_bAlwaysUpdateWeatherForInteriors;
extern bool g_bUseGamesettingsForFistFatigueDamage;
extern bool g_bCreaturesDealMeleeFatigueDmg;
extern bool g_bUnarmedWeaponsDealFatigueDmg;
extern bool g_bFixCaravanCurrencyRemoval;


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


extern std::vector<std::string> g_deferredPrints;

// Misc.
#define NUM_ARGS *((uint8_t*)scriptData + *opcodeOffsetPtr)  //Probably breaks Compiler Override!! - Credits to Stewie.
#define REFR_RES *(uint32_t*)result  //From JIPLN

//Below are already defined in GameForms.h - All from JIPLN
#define ADDR_ReturnTrue			0x8D0360
#define IS_TYPE(form, type) (*(uint32_t*)form == kVtbl_##type)  
#define NOT_ID(form, type) (form->typeID != kFormType_##type) 
#define IS_ID(form, type) (form->typeID == kFormType_##type)
#define IS_PROJECTILE(form) ((*(uint32_t**)form)[0x224 >> 2] == ADDR_ReturnTrue)

struct ArrayData
{
	uint32_t			size;
	std::unique_ptr<ArrayElementR[]> vals;
	std::unique_ptr<ArrayElementR[]> keys;

	ArrayData(NVSEArrayVar* srcArr, bool isPacked)
	{
		size = GetArraySize(srcArr);
		if (size)
		{
			vals = std::make_unique<ArrayElementR[]>(size);
			keys = isPacked ? nullptr : std::make_unique<ArrayElementR[]>(size);
			if (!GetArrayElements(srcArr, vals.get(), keys.get()))
				size = 0;
		}
	}
	ArrayData(NVSEArrayVar* srcArr, NVSEArrayVarInterface::ContainerTypes type) : ArrayData(
		srcArr, type == NVSEArrayVarInterface::ContainerTypes::kArrType_Array)
	{
	}
	~ArrayData() = default;
};

template <typename T, typename U>
struct decay_equiv :
	std::is_same<typename std::decay<T>::type, typename std::decay<U>::type>::type
{};




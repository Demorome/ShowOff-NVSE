#include "ShowOffNVSE.h"

#include <mutex>
#include <shared_mutex>

#include "settings.h"

#include "memory_pool.h"
#include "containers.h"
#include "internal/serialization.h"
#include "jip_nvse.h"
#include "StewieMagic.h"


// Functions (SO = ShowOff).
#include "functions/SO_fn_Misc.h"
#include "functions/SO_fn_Settings.h"
#include "functions/SO_fn_Gameplay.h"
#include "functions/SO_fn_Array.h"
#include "functions/SO_fn_AuxVars.h" 
#include "functions/SO_fn_Actors.h"
#include "functions/SO_fn_Debug.h"
#include "functions/SO_fn_Files.h"
#include "functions/SO_fn_Items.h"
#include "functions/SO_fn_Factions.h"
#include "functions/SO_fn_Topics.h"
#include "functions/SO_fn_Math.h"
#include "functions/SO_fn_Refs.h"
#include "functions/SO_fn_AuxTimer.h"

// Events
#include "Events/JohnnyEventPredefinitions.h"
#include "Events/EventFilteringInterface.h"
#include "Events/CustomEventFilters.h"
#include "Events/ShowOffEvents.h"


// Plugin Stuff
IDebugLog g_Log; // file will be open after NVSE plugin load
HMODULE	g_ShowOffHandle;
constexpr UInt32 g_PluginVersion = 182;

//***Current Max OpCode (https://geckwiki.com/index.php?title=NVSE_Opcode_Base)
const UInt32 MaxOpcode = 0x3D74;
UInt32 CurrentOpcode = 0x3C93; // starts at Opcode base for our plugin

// Allows modmakers to toggle ShowOff's debug messages for some of its functions.
#ifdef _DEBUG
std::atomic<bool> g_ShowFuncDebug = true;
#else
std::atomic<bool> g_ShowFuncDebug = false;
#endif

/*----------Globals------------------------------------------------------------------------
* It's better to include them in a .cpp file instead of a header file.
* Otherwise, compilation will fail if another .cpp file tries to include the global.
* Use for example "extern NVSEMessagingInterface* g_messagingInterface;" in main.h if you want to use the global elsewhere.
*
*===Danger of using Globals===
* Modifying globals is not thread-safe, which is unfortunate since scripts are multithreaded.
* Therefore, to modify globals after game first opens, one MUST either:
* a) lock threads to ensure thread safety,
* b) first declare the global as std::atomic<type>
* It will make the other threads sleep, so globals can be safely modified.
* Other than that, it's completely safe to check the value at any point.
*
*--Example of this:
ICriticalSection g_someLock;
void Func()
{
	ScopedLock lock(g_Lock);
	// do un-thread safe stuff like modify globals, only a single thread can be here at a time

	//will automatically leave critical section when leaving scope.
}

std::atomic<char*> doesn't really work.
*/
ICriticalSection g_Lock;

// NVSE Globals
bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
bool (*ExtractFormatStringArgs)(UInt32 fmtStringPos, char* buffer, COMMAND_ARGS_EX, UInt32 maxParams, ...);  // From JIP_NVSE.H
NVSEArrayVarInterface* g_arrInterface = nullptr;
NVSEArrayVar* (*CreateArray)(const NVSEArrayElement* data, UInt32 size, Script* callingScript);
NVSEArrayVar* (*CreateStringMap)(const char** keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
NVSEArrayVar* (*CreateMap)(const double* keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
bool (*AssignArrayResult)(NVSEArrayVar* arr, double* dest);
void (*SetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, const NVSEArrayElement& value);
void (*AppendElement)(NVSEArrayVar* arr, const NVSEArrayElement& value);
UInt32(*GetArraySize)(NVSEArrayVar* arr);
NVSEArrayVar* (*LookupArrayByID)(UInt32 id);
bool (*GetElement)(NVSEArrayVar* arr, const NVSEArrayElement& key, NVSEArrayElement& outElement);
bool (*GetArrayElements)(NVSEArrayVar* arr, NVSEArrayElement* elements, NVSEArrayElement* keys);
NVSEStringVarInterface* g_strInterface = nullptr;
bool (*AssignString)(COMMAND_ARGS, const char* newValue);
const char* (*GetStringVar)(UInt32 stringID);
NVSEMessagingInterface* g_msg = nullptr;
NVSEScriptInterface* g_scriptInterface = nullptr;
NVSECommandTableInterface* g_commandInterface = nullptr;
const CommandInfo* (*GetCmdByName)(const char* name);
bool (*FunctionCallScript)(Script* funcScript, TESObjectREFR* callingObj, TESObjectREFR* container, NVSEArrayElement* result, UInt8 numArgs, ...);
bool (*FunctionCallScriptAlt)(Script* funcScript, TESObjectREFR* callingObj, UInt8 numArgs, ...);
TESObjectREFR* (__stdcall *InventoryRefCreateEntry)(TESObjectREFR* container, TESForm* itemForm, SInt32 countDelta, ExtraDataList* xData);
_InventoryRefCreate InventoryRefCreate;
_CaptureLambdaVars CaptureLambdaVars;
_UncaptureLambdaVars UncaptureLambdaVars;
ExpressionEvaluatorUtils g_expEvalUtils;
NVSEEventManagerInterface* g_eventInterface = nullptr;

DWORD g_mainThreadID = 0;

// Singletons
HUDMainMenu* g_HUDMainMenu = nullptr;
TileMenu** g_tileMenuArray = nullptr;
UInt32 g_screenWidth = 0;
UInt32 g_screenHeight = 0;
PlayerCharacter* g_thePlayer = nullptr;
ActorValueOwner* g_playerAVOwner = nullptr;
ProcessManager* g_processManager = nullptr;
InterfaceManager* g_interfaceManager = nullptr;
BSWin32Audio* g_bsWin32Audio = nullptr;
DataHandler* g_dataHandler = nullptr;
BSAudioManager* g_audioManager = nullptr;
Sky** g_currentSky = nullptr;
RefID g_xMarkerFormID = 0x3B;
TESObjectWEAP* g_fistsWeapon = nullptr;
TimeGlobal* g_timeGlobal = nullptr;
BGSSaveLoadGame* g_BGSSaveLoadGame = nullptr;

// Game functions
bool (__cdecl* GetIsGodMode)() = nullptr;

// Hook Globals
std::atomic<bool> g_canPlayerPickpocketInCombat = false;
bool g_wroteSuperDelayedHooks = false;

std::vector<std::string> g_deferredPrints;
void HandleDeferredConsolePrints()
{
	for (auto& s : g_deferredPrints)
		Console_Print(s.c_str());

	g_deferredPrints.clear();
}

//todo: remove INI globals and make a class for them (unordered map, access value via string key)

bool g_bResetInteriorResetsActors;
bool g_bNoSelfRepairingBrokenItems;
bool g_bNoVendorRepairingBrokenItems;
bool g_bAlwaysUpdateWeatherForInteriors;
bool g_bUseGamesettingsForFistFatigueDamage;
bool g_bCreaturesDealMeleeFatigueDmg;
bool g_bUnarmedWeaponsDealFatigueDmg;
bool g_bFixCaravanCurrencyRemoval;

//-Force Pickpocketting INI globals (enabled via function)
std::atomic<float> g_fForcePickpocketBaseAPCost;
std::atomic<float> g_fForcePickpocketMinAPCost;
std::atomic<float> g_fForcePickpocketMaxAPCost;
std::atomic<float> g_fForcePickpocketPlayerAgilityMult;
std::atomic<float> g_fForcePickpocketPlayerSneakMult;
std::atomic<float> g_fForcePickpocketTargetPerceptionMult;
std::atomic<float> g_fForcePickpocketItemWeightMult;
std::atomic<float> g_fForcePickpocketItemValueMult;
std::atomic<float> g_fForcePickpocketPlayerStrengthMult;
std::atomic<float> g_fForcePickpocketTargetStrengthMult;
char* g_fForcePickpocketFailureMessage = nullptr;


// Function-defining Definitions
#define RegisterScriptCommand(name) nvse->RegisterCommand(&kCommandInfo_ ##name); //Default return type (return a number)
#define REG_CMD(name) nvse->RegisterCommand(&kCommandInfo_##name);  //Short version of RegisterScriptCommand, from JIP.
#define REG_TYPED_CMD(name, type) nvse->RegisterTypedCommand(&kCommandInfo_##name,kRetnType_##type);  //from JG
#define REG_CMD_STR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_String); //From JIPLN
#define REG_CMD_ARR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Array); //From JIPLN
#define REG_CMD_FORM(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Form);
#define REG_CMD_AMB(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Ambiguous);



// This is a message handler for nvse events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case NVSEMessagingInterface::kMessage_LoadGame:
		break;
	case NVSEMessagingInterface::kMessage_SaveGame:
		// Copied from jip_nvse.cpp
		COPY_BYTES(s_lastLoadedPath, msg->fosPath, msg->dataLen + 1);
		s_dataChangedFlags = 0;
		break;
	case NVSEMessagingInterface::kMessage_PreLoadGame:
		// Copied jip_nvse.cpp
		if (!StrEqualCS(msg->fosPath, s_lastLoadedPath))
		{
			COPY_BYTES(s_lastLoadedPath, msg->fosPath, msg->dataLen + 1);
			s_dataChangedFlags = kChangedFlag_All;
		}
		AuxTimer::RemovePendingTimers();
		AuxTimer::HandleAutoRemoveTempTimers();
		FlushJGInterfaceEvents();
		break;
	case NVSEMessagingInterface::kMessage_PostLoadGame:
		if (msg->fosLoaded)
		{
			// JIP calls DoLoadGameHousekeeping() here
			s_dataChangedFlags = 0;
		}
		break;
	case NVSEMessagingInterface::kMessage_PostLoad:
		//_MESSAGE("Received POST LOAD message with file path %s", msg->data);
		break;
	case NVSEMessagingInterface::kMessage_PostPostLoad:
		//_MESSAGE("Received POST POS LOAD message with file path %s", msg->data);
		break;
	case NVSEMessagingInterface::kMessage_ExitGame:
		//_MESSAGE("Received exit game message");
		break;
	case NVSEMessagingInterface::kMessage_ExitGame_Console:
		//_MESSAGE("Received exit game via console qqq command message");
		break;
	case NVSEMessagingInterface::kMessage_ExitToMainMenu:
		// Copied from jip_nvse.cpp
		ProcessDataChangedFlags(kChangedFlag_All);
		s_lastLoadedPath[0] = 0;
		AuxTimer::RemovePendingTimers();
		AuxTimer::HandleAutoRemoveTempTimers();
		FlushJGInterfaceEvents();
		break;
	case NVSEMessagingInterface::kMessage_Precompile:
		//_MESSAGE("Received precompile message with script at %08x", msg->data);
		break;

	case NVSEMessagingInterface::kMessage_DeleteGame:
		//_MESSAGE("Received DELETE message with file path %s", msg->data);
		break;

	case NVSEMessagingInterface::kMessage_DeferredInit:
		// From JiP's patches_game.h
		g_thePlayer = PlayerCharacter::GetSingleton();
		g_playerAVOwner = &g_thePlayer->avOwner;
		g_processManager = (ProcessManager*)0x11E0E80;
		g_bsWin32Audio = BSWin32Audio::GetSingleton();
		g_dataHandler = DataHandler::Get();
		g_audioManager = (BSAudioManager*)0x11F6EF0;
		g_currentSky = (Sky**)0x11DEA20;
		g_HUDMainMenu = *(HUDMainMenu**)0x11D96C0;  
		g_interfaceManager = *(InterfaceManager**)0x11D8A80; 
		g_tileMenuArray = *(TileMenu***)0x11F350C; 
		g_screenWidth = *(UInt32*)0x11C73E0;
		g_screenHeight = *(UInt32*)0x11C7190;
		g_fistsWeapon = *(TESObjectWEAP**)0x11CA278;
		g_BGSSaveLoadGame = *(BGSSaveLoadGame**)0x11DDF38;

		GetIsGodMode = reinterpret_cast<bool(*)()>(0x9526B0);

		// from xNVSE
		g_timeGlobal = reinterpret_cast<TimeGlobal*>(0x11F6394);

		HandleHooks::HandleDelayedGameHooks();
		HandleHooks::HandleDelayedEventHooks();

		Console_Print("ShowOff xNVSE version: %.2f", (g_PluginVersion / 100.0F));

		HandleDeferredConsolePrints();
		break;

	case NVSEMessagingInterface::kMessage_MainGameLoop:
	{
		if (!g_wroteSuperDelayedHooks) [[unlikely]]
		{
			HandleHooks::HandleSuperDelayedGameHooks();
			HandleHooks::HandleSuperDelayedEventHooks();
			g_wroteSuperDelayedHooks = true;
		}

		// Handle events
		for (const auto& EventInfo : EventsArray)
		{
			EventInfo->AddQueuedEvents();
			EventInfo->DeleteEvents();
		}
		EventHandling::HandleGameLoopEvents();

		// Handle Timers
		{
			const auto globalTimeMult = CdeclCall<double>(0x716440);

			// Copied from xNVSE
			// Calls GetPlayerTimeMultWithVATSCamShots, which applies on top of SGTM's TimeMult changes.
			// g_timeGlobal's secondsPassed only accounts for GetGlobalTimeMult, hence why we need to multiply by the VATS time mult.
			// TODO: there is another function called VATSCameraData::GetTargetTimeUpdateMult, but we aren't using it...
			const auto vatsTimeMult = ThisStdCall<double>(0x9C8CC0, reinterpret_cast<void*>(0x11F2250));
			const auto isMenuMode = CdeclCall<bool>(0x702360);

			AUX_TIMER_CS;
			AuxTimer::RemovePendingTimers();
			if (!s_auxTimerMapArraysPerm.Empty())
				s_dataChangedFlags |= kChangedFlag_AuxTimerMaps; // assume a timer will change
			AuxTimer::DoCountdown(globalTimeMult, vatsTimeMult, isMenuMode);
		}

		break;
	}
	case NVSEMessagingInterface::kMessage_RuntimeScriptError:
		//_MESSAGE("Received runtime script error message %s", msg->data);
		break;
	default:
		break;
	}
}


// Does whatever I want it to at the time.
DEFINE_COMMAND_PLUGIN(TestDemo, "", false, NULL);
bool Cmd_TestDemo_Execute(COMMAND_ARGS)
{
	UInt32 bInt = 5 || 1;
	//if (!ExtractArgsEx(EXTRACT_ARGS_EX)) return true;
	//Console_Print("[%12X]", *(UInt32*)0x8B959B);
	Console_Print("%u", bInt);
	return true;
}

extern "C"
{
	//This is required for the INI functionality; need to get the Handle.
	BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
	{
		if (fdwReason == DLL_PROCESS_ATTACH)
		{
			g_ShowOffHandle = hModule;
			DisableThreadLibraryCalls(hModule);
		}
		return TRUE;
	}

	bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
	{
		// fill out the info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "ShowOffNVSE Plugin";
		info->version = g_PluginVersion;

		// version checks
		if (nvse->nvseVersion < PACKED_NVSE_VERSION)  //fixed version check thanks to c6
		{
			char buffer[100];
			sprintf_s(buffer, 100,"NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
			MessageBoxA(nullptr, buffer, "ShowOff", MB_ICONEXCLAMATION);  //MessageBoxA usage style copied from lStewieAl.
			return false;
		}
		
		if (!nvse->isEditor)
		{
			if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
			{
				char buffer[100];
				sprintf_s(buffer, 100, "Incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion, RUNTIME_VERSION_1_4_0_525);
				MessageBoxA(nullptr, buffer, "ShowOff", MB_ICONEXCLAMATION);
				return false;
			}

			if (nvse->isNogore)
			{
				char buffer[] = "NoGore is not supported";
				MessageBoxA(nullptr, buffer, "ShowOff", MB_ICONEXCLAMATION);
				return false;
			}
		}
		else
		{
			if (nvse->editorVersion < CS_VERSION_1_4_0_518)
			{
				char buffer[100];
				sprintf_s(buffer, 100, "Incorrect editor version (got %08X, need at least %08X)", nvse->editorVersion, CS_VERSION_1_4_0_518);
				MessageBoxA(nullptr, buffer, "ShowOff", MB_ICONEXCLAMATION);
				return false;
			}
		}

		// version checks pass
		// any version compatibility checks should be done here
		return true;
	}

	bool NVSEPlugin_Load(const NVSEInterface* nvse)
	{
		PluginHandle const nvsePluginHandle = nvse->GetPluginHandle();  //from JiPLN

		// register to receive messages from NVSE
		((NVSEMessagingInterface*)nvse->QueryInterface(kInterface_Messaging))->RegisterListener(nvsePluginHandle, "NVSE", MessageHandler);

		{ 
			// set up log file
			auto* logItf = (NVSELoggingInterface*)nvse->QueryInterface(kInterface_Logging);
			std::string logPath = logItf->GetPluginLogPath();
			logPath += "ShowOffNVSE.log";
			g_Log.Open(logPath.c_str());
		}

		if (nvse->isEditor)
		{
			_MESSAGE("ShowOffNVSE Loaded successfully (Editor).\nShowOffNVSE plugin version: %u\n", g_PluginVersion);
		}
		else
		{
			_MESSAGE("ShowOffNVSE Loaded successfully (In-Game).\nShowOffNVSE plugin version: %u\n", g_PluginVersion);
		}
	
		if (!nvse->isEditor) 
		{
			g_mainThreadID = GetCurrentThreadId();

			auto const nvseData = (NVSEDataInterface*)nvse->QueryInterface(kInterface_Data);
			InventoryRefGetForID = (InventoryRef * (*)(UInt32 refID))nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
			InventoryRefCreate = (_InventoryRefCreate)nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceCreate);
			InventoryRefCreateEntry = (TESObjectREFR * (__stdcall*)(TESObjectREFR* container, TESForm *itemForm, SInt32 countDelta, ExtraDataList *xData))nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceCreateEntry);
			CaptureLambdaVars = (_CaptureLambdaVars)nvseData->GetFunc(NVSEDataInterface::kNVSEData_LambdaSaveVariableList);
			UncaptureLambdaVars = (_UncaptureLambdaVars)nvseData->GetFunc(NVSEDataInterface::kNVSEData_LambdaUnsaveVariableList);

			// From JiPLN (jip_nvse.cpp) 
			auto const serialization = (NVSESerializationInterface*)nvse->QueryInterface(kInterface_Serialization);
			WriteRecord = serialization->WriteRecord;
			WriteRecordData = serialization->WriteRecordData;
			GetNextRecordInfo = serialization->GetNextRecordInfo;
			ReadRecordData = serialization->ReadRecordData;
			ResolveRefID = serialization->ResolveRefID;
			GetSavePath = serialization->GetSavePath;
			WriteRecord8 = serialization->WriteRecord8;
			WriteRecord16 = serialization->WriteRecord16;
			WriteRecord32 = serialization->WriteRecord32;
			WriteRecord64 = serialization->WriteRecord64;
			ReadRecord8 = serialization->ReadRecord8;
			ReadRecord16 = serialization->ReadRecord16;
			ReadRecord32 = serialization->ReadRecord32;
			ReadRecord64 = serialization->ReadRecord64;
			SkipNBytes = serialization->SkipNBytes;
			serialization->SetLoadCallback(nvsePluginHandle, LoadGameCallback);
			serialization->SetSaveCallback(nvsePluginHandle, SaveGameCallback);
			
			g_scriptInterface = (NVSEScriptInterface*)nvse->QueryInterface(kInterface_Script);
			ExtractArgsEx = g_scriptInterface->ExtractArgsEx;
			ExtractFormatStringArgs = g_scriptInterface->ExtractFormatStringArgs;
			FunctionCallScript = g_scriptInterface->CallFunction;
			FunctionCallScriptAlt = g_scriptInterface->CallFunctionAlt;
			
			g_commandInterface = (NVSECommandTableInterface*)nvse->QueryInterface(kInterface_CommandTable);
			GetCmdByName = g_commandInterface->GetByName;
			
			//g_??Interface->??; shortcuts are from JiP
			g_strInterface = (NVSEStringVarInterface*)nvse->QueryInterface(kInterface_StringVar);
			GetStringVar = g_strInterface->GetString;  
			AssignString = g_strInterface->Assign;

			g_arrInterface = (NVSEArrayVarInterface*)nvse->QueryInterface(kInterface_ArrayVar);
			CreateArray = g_arrInterface->CreateArray;
			CreateMap = g_arrInterface->CreateMap;
			CreateStringMap = g_arrInterface->CreateStringMap;
			AssignArrayResult = g_arrInterface->AssignCommandResult;
			SetElement = g_arrInterface->SetElement;
			AppendElement = g_arrInterface->AppendElement;
			GetArraySize = g_arrInterface->GetArraySize;
			LookupArrayByID = g_arrInterface->LookupArrayByID;
			GetElement = g_arrInterface->GetElement;
			GetArrayElements = g_arrInterface->GetElements;

			nvse->InitExpressionEvaluatorUtils(&g_expEvalUtils);

			g_eventInterface = (NVSEEventManagerInterface*)nvse->QueryInterface(kInterface_EventManager);

#if 0
			auto johnnyGuitar = GetModuleHandle("johnnyguitar.dll");
			JGCreateEvent = (CreateScriptEvent)GetProcAddress(johnnyGuitar, "JGCreateEvent");
			JGFreeEvent = (FreeScriptEvent)GetProcAddress(johnnyGuitar, "JGFreeEvent");
			//todo: if using these imports, add JG requirement check
#endif
			
			HandleINIOptions();
			HandleHooks::HandleGameHooks();
			HandleHooks::HandleEventHooks();
			RegisterEvents();
		}

		// Register script commands
		// https://geckwiki.com/index.php?title=NVSE_Opcode_Base
		nvse->SetOpcodeBase(CurrentOpcode);

		/* ONLY COMMANDS WITH LISTED OPCODES SHOULD BE USED IN SCRIPTS */
		/* DO NO COMMENT OUT AN ALREADY-RELEASED FUNCTION; REPLACE WITH EMPTY COMMAND INSTEAD */

		//========v1.00
		/*3C93*/ REG_CMD(ModNumericGameSetting)
		/*3C94*/ REG_CMD(ModNumericINISetting)
		/*3C95*/ REG_CMD(SetPlayerCanPickpocketEquippedItems)
		/*3C96*/ REG_CMD(GetPlayerCanPickpocketEquippedItems)
		/*3C97*/ REG_CMD(GetPCHasSleepWaitOverride)
		/*3C98*/ REG_CMD(SetPCHasSleepWaitOverride)
		/*3C99*/ REG_CMD(ShowingOffDisable)  //Showing off by copying vanilla style way functions! 
		/*3C9A*/ REG_CMD(ShowingOffEnable) //best to keep undocumented to not waste people's time.
		/*3C9B*/ REG_CMD(GetNumActorsInRangeFromRef)
		/*3C9C*/ REG_CMD(GetNumCombatActorsFromActor)
		/*3C9D*/ REG_CMD(IsWeaponMelee)
		/*3C9E*/ REG_CMD(IsEquippedWeaponMelee)
		/*3C9F*/ REG_CMD(IsWeaponRanged)
		/*3CA0*/ REG_CMD(IsEquippedWeaponRanged)
		/*3CA1*/ REG_CMD(GetChallengeProgress)
		/*3CA2*/ REG_CMD(ConditionPrint) 
		/*3CA3*/ REG_CMD(MessageExAltShowoff) //Keep undocumented; don't recommend for general use, extra feature is jank.
		/*3CA4*/ REG_CMD(GetNumQueuedCornerMessages)
		/*3CA5*/ REG_CMD(GetCreatureTurningSpeed)
		/*3CA6*/ REG_CMD(SetCreatureTurningSpeed)
		/*3CA7*/ REG_CMD(GetCreatureFootWeight)
		/*3CA8*/ REG_CMD(SetCreatureFootWeight)
		/*3CA9*/ REG_CMD(SetCreatureReach)
		/*3CAA*/ REG_CMD(SetCreatureBaseScale) //JG's GetBaseScale should return the creature's base scale, so no need for a GetCreatureBaseScale.
		/*3CAB*/ REG_CMD(DumpFormList)
		/*3CAC*/ REG_CMD(ListAddArray_OLD)
		/*3CAD*/ REG_CMD(ListAddList)
		/*3CAE*/ REG_CMD(IsAnimPlayingExCond)
		/*3CAF*/ REG_CMD(GetNumCompassHostiles)

		// === TO DOCUMENT ====
		/*3CB0*/ REG_CMD(AuxStringMapArrayGetSize)
		/*3CB1*/ REG_CMD(AuxStringMapArrayGetValueType)
		/*3CB2*/ REG_CMD(AuxStringMapArrayGetFloat)
		/*3CB3*/ REG_CMD_FORM(AuxStringMapArrayGetRef)
		/*3CB4*/ REG_CMD_STR(AuxStringMapArrayGetString)
		/*3CB5*/ REG_CMD_ARR(AuxStringMapArrayGetFirst)
		/*3CB6*/ REG_CMD_ARR(AuxStringMapArrayGetNext)
		/*3CB7*/ REG_CMD_ARR(AuxStringMapArrayGetKeys)
		/*3CB8*/ REG_CMD_ARR(AuxStringMapArrayGetAll)  //todo: test this
		/*3CB9*/ REG_CMD_ARR(AuxStringMapArrayGetAsArray)
		/*3CBA*/ REG_CMD(AuxStringMapArraySetFromArray)
		/*3CBB*/ REG_CMD(AuxStringMapArraySetFloat)
		/*3CBC*/ REG_CMD(AuxStringMapArraySetRef)
		/*3CBD*/ REG_CMD(AuxStringMapArraySetString)
		/*3CBE*/ REG_CMD(AuxStringMapArrayEraseKey)
		/*3CBF*/ REG_CMD(AuxStringMapArrayValidateValues)  //todo: test
		/*3CC0*/ REG_CMD(AuxStringMapArrayDestroy)

		/*3CC1*/ REG_CMD(GetRadiationExtraData)
		/*3CC2*/ REG_CMD(SetRadiationExtraData)
		/*3CC3*/ REG_CMD(PlayerHasNightVisionActive)
		/*3CC4*/ REG_CMD(PlayerIsDrinkingPlacedWater)
		/*3CC5*/ REG_CMD(SetIsPCAMurderer)
		/*3CC6*/ REG_CMD(IsNight)
		/*3CC7*/ REG_CMD(IsLimbCrippled)
		/*3CC8*/ REG_CMD(GetNumCrippledLimbs)
		/*3CC9*/ REG_CMD(GetCrippledLimbsAsBitMask)
		/*3CCA*/ REG_CMD(GetNumBrokenEquippedItems)
		/*3CCB*/ REG_CMD(GetEquippedItemsAsBitMask)
		/*3CCC*/ REG_CMD(GetEquippedWeaponType)
		/*3CCD*/ REG_CMD(GetBaseEquippedWeight)
		/*3CCE*/ REG_CMD(GetCalculatedEquippedWeight)  //NOTE: bWeightlessWorn(Power)Armor from Stewie's is not accounted for.
		/*3CCF*/ REG_CMD(GetCalculatedMaxCarryWeight)

		//========v1.10
		/*3CD0*/ REG_CMD(GetShowOffDebugMode)
		/*3CD1*/ REG_CMD(SetShowOffDebugMode)
		/*3CD2*/ REG_CMD(UnequipItems)
		/*3CD3*/ REG_CMD_ARR(GetEquippedItems)
		/*3CD4*/ REG_CMD(IsBaseForm)
		/*3CD5*/ REG_CMD(SetRandomizerSeed)
		/*3CD6*/ REG_CMD(SetSeedUsingForm)
		/*3CD7*/ REG_CMD(GetRandomizerSeed)
		/*3CD8*/ REG_CMD(RandSeeded)
		/*3CD9*/ REG_CMD(GetRandomPercentSeeded)
		/*3CDA*/ REG_CMD(IsReferenceCloned)
		/*3CDB*/ REG_CMD(AdvanceSeed)

		//========v1.15
		/*3CDC*/ REG_CMD(IsTemporaryReference)  // KEEP UNDOCUMENTED, does not behave as I expected it to.
		/*3CDD*/ REG_CMD(GetPCHasScriptedFastTravelOverride)
		/*3CDE*/ REG_CMD(GetPCCanFastTravel)  //CheckAllowFastTravel 0x93D660
		/*3CDF*/ REG_CMD(GetWeaponHasFlag)
		/*3CE0*/ REG_CMD(GetActorHasBaseFlag)
		/*3CE1*/ REG_CMD(RemoveAllItemsShowOff)

		//========v1.17
		/*3CE2*/ REG_CMD(ForceWeaponJamAnim)
		/*3CE3*/ REG_CMD(DisableAlt)
		/*3CE4*/ REG_CMD(EnableAlt)
		/*3CE5*/ REG_CMD(GetCalculatedSkillPoints)
		/*3CE6*/ REG_CMD(GetLevelUpMenuPoints)
		/*3CE7*/ REG_CMD(ToggleQuestMessages)  // SuppressQuestMessages from JIP is probably way better implemented.

		//========v1.20
		/*3CE8*/ REG_CMD(GetCalculatedPerkPoints)
		/*3CE9*/ REG_CMD(GetLevelUpMenuCurrentPage)
		/*3CEA*/ REG_CMD(SetLevelUpMenuCurrentPage)
		/*3CEB*/ REG_CMD(ShowPerkMenu)

		//========v1.25
		/*3CEC*/ REG_CMD(ShowSkillMenu)
		/*3CED*/ REG_CMD(GetLevelUpMenuUnspentPoints)  //todo: consider making the result serialized.
		/*3CEE*/ REG_CMD(SetLevelUpMenuCanExitEarly)
		/*3CEF*/ REG_CMD(SetLevelUpMenuPoints)
		/*3CF0*/ REG_CMD(CreateFolder)
		/*3CF1*/ REG_CMD_FORM(GetExplosionRefSource)
		/*3CF2*/ REG_CMD(GetExplosionRefRadius)
		/*3CF3*/ REG_CMD(SetExplosionRefRadius)
		/*3CF4*/ REG_CMD(SetExplosionRefSource)
		/*3CF5*/ REG_CMD(GetActorValueDamage)
		/*3CF6*/ REG_CMD_ARR(GetPipboyRadioVoiceEntryData)  // todo: FIX CRASHING (when music plays and arg1 = 1?)
		/*3CF7*/ REG_CMD_ARR(GetEquippedItemRefs)
		/*3CF8*/ REG_CMD(SetNoEquipShowOff) // deprecated.
		/*3CF9*/ REG_CMD_AMB(GetNoEquipShowOff)	// can return a form (function) or a bool. Deprecated.
		/*3CFA*/ REG_CMD(SetShowOffOnCornerMessageEventHandler)

		//========v1.30
		/*3CFB*/ REG_CMD_FORM(GetIngestibleConsumeSound)
		/*3CFC*/ REG_CMD(SetIngestibleConsumeSound)
		/*3CFD*/ REG_CMD(SetFactionCombatReactionTemp) /*A merge of SetAllyTemp and SetEnemyTemp. Now undocumented since it's clearer to use the other two.*/
		/*3CFE*/ REG_CMD_FORM(GetEquippedItemRefForItem)
		/*3CFF*/ REG_CMD(SetAllyTemp)
		/*3D00*/ REG_CMD(SetEnemyTemp)
		/*3D01*/ REG_CMD_ARR(TopicInfoGetResponseStrings)
		/*3D02*/ REG_CMD(TopicInfoSetResponseStrings_OLD)

		//========v1.31
		/*3D03*/ REG_CMD(TopicInfoSetNthResponseString)

		//========v1.35
		/*3D04*/ REG_CMD(GetItemCanHaveHealth)
		/*3D05*/ REG_CMD_FORM(SayTo_GetSpeakingActor)	//doesn't seem to work...
		/*3D06*/ REG_CMD_FORM(SayTo_GetTopic)
		/*3D07*/ REG_CMD_FORM(SayTo_GetTopicInfo)
		/*3D08*/ REG_CMD_FORM(SayTo_GetQuest)		//doesn't seem to work...
		/*3D09*/ REG_CMD(FormListRemoveForm)	// known as RemoveFormFromFormList, since that's more consistent for naming.
		/*3D0A*/ REG_CMD(GetZoneRespawns)
		/*3D0B*/ REG_CMD(ClearCinematicTextQueue)
		/*3D0C*/ REG_CMD_FORM(GetCellEncounterZone)
		/*3D0D*/ REG_CMD(ShowPauseMenu)
		/*3D0E*/ REG_CMD(RemoveFormFromLeveledList)

		//========v1.40
		/*3D0F*/ REG_CMD(SetWeaponFlag)
		/*3D10*/ REG_CMD(SetOwnershipTemp)
		/*3D11*/ REG_CMD(IsActorAlt)
		/*3D12*/ REG_CMD(ApplyEasing)
		/*3D13*/ REG_CMD(Matrix_IsMatrix_OLD)
		/*3D14*/ REG_CMD_ARR(Matrix3x3_GetQuaternion_OLD)
		/*3D15*/ REG_CMD_ARR(Quaternion_GetMatrix_OLD)
		/*3D16*/ REG_CMD_ARR(Matrix_Transpose_OLD)
		/*3D17*/ REG_CMD(Matrix_Dump_OLD)
		/*3D18*/ REG_CMD_ARR(Matrix_ApplyOperationWithScalar_OLD)
		/*3D19*/ REG_CMD_ARR(Matrix_ApplyOperationWithMatrix_OLD)

		//========v1.45
		/*3D1A*/ REG_CMD_AMB(ReadFromJSONFile)
		/*3D1B*/ REG_CMD(WriteToJSONFile)
		
		//== Functions that used old array extraction method, improved vers. had to be redeclared for backwards compat.
		/*3D1C*/ REG_CMD(ListAddArray)	//todo: add bCheckForDupes arg (optional)
		/*3D1D*/ REG_CMD(Matrix_IsMatrix)
		/*3D1E*/ REG_CMD_ARR(Matrix3x3_GetQuaternion)
		/*3D1F*/ REG_CMD_ARR(Quaternion_GetMatrix)
		/*3D20*/ REG_CMD_ARR(Matrix_Transpose)
		/*3D21*/ REG_CMD(Matrix_Dump)
		/*3D22*/ REG_CMD_ARR(Matrix_ApplyOperationWithScalar)
		/*3D23*/ REG_CMD_ARR(Matrix_ApplyOperationWithMatrix)
		/*3D24*/ REG_CMD(TopicInfoSetResponseStrings)
		
		/*3D25*/	REG_CMD(GetCalculatedItemValue)

		/*3D26*/	REG_CMD(SetINIValue_Cached)
		/*3D27*/	REG_CMD(GetINIFloatOrCreate_Cached)
		/*3D28*/	REG_CMD_STR(GetINIStringOrCreate_Cached)
		/*3D29*/	REG_CMD(GetINIFloatOrDefault_Cached)
		/*3D2A*/	REG_CMD_STR(GetINIStringOrDefault_Cached)
		/*3D2B*/	REG_CMD(HasINISetting_Cached)
		/*3D2C*/	REG_CMD(ClearFileCacheShowOff)
		/*3D2D*/	REG_CMD(SaveCachedIniFile)
		/*3D2E*/	REG_CMD(ReloadIniCache)
		/*3D2F*/	REG_CMD(ResetInteriorAlt)

		//========v1.50
		/*3D30*/	REG_CMD(SetEnableParent)
		/*3D31*/	REG_CMD_FORM(GetEquippedWeapon)
		/*3D32*/	REG_CMD_FORM(GetEquippedWeaponRef)
		/*3D33*/	REG_CMD(GetCalculatedAPCost)
		/*3D34*/	REG_CMD_ARR(GetPosArray)
		/*3D35*/	REG_CMD_ARR(GetCompassTargets)
		/*3D36*/	REG_CMD(SetLiveEffectMagnitudeModifier)
		/*3D37*/	REG_CMD(IsOutsideMainThread)
		/*3D38*/	REG_CMD(GetLiveEffectBaseTrait)
		/*3D39*/	REG_CMD_STR(GetActorValueName)
		/*3D3A*/    REG_CMD(SetINIInteger_Cached)
		/*3D3B*/	REG_CMD_FORM(GetAddedItemRefShowOff)

		//========v1.55
		/*3D3C*/	REG_CMD(GetIsPlayerOverencumbered)
		/*3D3D*/	REG_CMD(RefillPlayerAmmo)
		/*3D3E*/	REG_CMD(AuxTimerStart)
		/*3D3F*/	REG_CMD(AuxTimerStop)
		/*3D40*/	REG_CMD(AuxTimerPaused)
		/*3D41*/	REG_CMD(AuxTimerTimeElapsed)
		/*3D42*/	REG_CMD(AuxTimerTimeToCountdown)
		/*3D43*/	REG_CMD(AuxTimerTimeLeft)
		/*3D44*/	REG_CMD(SetOnAuxTimerStartHandler)
		/*3D45*/	REG_CMD(SetOnAuxTimerStopHandler)
		/*3D46*/	REG_CMD(ClearShowoffSavedData)  //todo: fix not working for aux stringMaps.		

		//========v1.60
		/*3D47*/	REG_CMD(SetAmmoName)
		/*3D48*/	REG_CMD_STR(GetAmmoName)
		/*3D49*/	REG_CMD(KillAllHostiles)

		//========v1.65
		/*3D4A*/	REG_CMD(IsAiming)
		/*3D4B*/	REG_CMD(IsBlockingOrAiming)
		/*3D4C*/	REG_CMD(SetOnAuxTimerUpdateHandler)
		/*3D4D*/	REG_CMD(SetItemHotkeyIconPath)
		/*3D4E*/	REG_CMD_STR(GetItemHotkeyIconPath)
		/*3D4F*/	REG_CMD(GetExplosionHitDamage)
		/*3D50*/	REG_CMD(SetExplosionHitDamage)
		/*3D51*/	REG_CMD(IsJumping_BROKEN)
		/*3D52*/	REG_CMD(FreezeAmmoRegen)

		//========v1.74
		/*3D53*/	REG_CMD(GetCalculatedActorSpread);
		
		//========v1.80
		/*3D54*/	REG_CMD(GetIsActivationPromptShown);
		/*3D55*/	REG_CMD(GetHitLocationLingering);
		/*3D56*/	REG_CMD(HighlightAdditionalReferenceAlt);
		/*3D57*/	REG_CMD(GetVATSMaxEngageDistance);
		/*3D58*/	REG_CMD(SetForceDrawHitscanProjectiles);
		/*3D59*/	REG_CMD(SetProjectileTracerChanceOverride);
		/*3D5A*/	REG_CMD_FORM(SpawnTracingProjectile);
		/*3D5B*/	REG_CMD(GetVATSTargetable);
		/*3D5C*/	REG_CMD(GetProjectileRefIsStuck);
		/*3D5D*/	REG_CMD(ForceRecoilAnim);
		/*3D5E*/	REG_CMD(ForceHitStaggerReaction);
		/*3D5F*/	REG_CMD(IsPlayerLookingAround);
		/*3D60*/	REG_CMD(GetActorFadeState);
		/*3D61*/	REG_CMD(IsActorInvisibleToPlayer);
		/*3D62*/	REG_CMD(IsFormLoading);
		/*3D63*/	REG_CMD(ApplyEasingAlt);
		/*3D64*/	REG_CMD(PatchFreezeTime);
		/*3D65*/	REG_CMD_FORM(PlaceAtReticleAlt);
		
		//========v1.82
		/*3D66*/ REG_CMD_ARR(GetWorldOffsetPosArray);
		
		//========v1.??
		//todo: always check to update/increase your opcode range when adding new functions



#if EnableSafeExtractArgsTests
		REG_CMD(TestSafeExtract_OneArray)
		REG_CMD(TestSafeExtract_OneNumber_OneOptionalString)
		REG_CMD(TestSafeExtract_OneNumber_OneOptionalString_Alt)
		REG_CMD(TestSafeExtract_OneOptionalStringOrNumber)
#endif
		
#if _DEBUG  //for functions being tested (or just abandoned).

		REG_CMD(SetShouldShowSleepWaitOverrideMessage);

		REG_CMD_ARR(CaravanDeckGetCards)
		REG_CMD_FORM(SetSingleItemRefCurrentHealth)

		REG_CMD(SetFlyCamera)

		REG_CMD(ForceWeaponJamAnimAlt)
		REG_CMD(GetHeadingAngleTEST)

		REG_CMD(Debug_DispatchEvent)
		REG_CMD_FORM(GetSelectedItemRefSO)


		REG_CMD(Flt_Equals)	// not needed; xNVSE's eval already uses this for "==" operator.

		REG_CMD_FORM(SayTo_GetUnk)
		REG_CMD(ApplyAddictionEffect)
		REG_CMD(SetPlantedExplosive)
		
		REG_CMD(GetItemCanRepairTarget)
		REG_CMD(GetItemCanBeRepairedByTarget)
		REG_CMD(GetCalculatedItemWeight)

		REG_CMD(TestMatrix)
		REG_CMD(ar_Test);
			
		REG_CMD(GetHealthExtraData)

		REG_CMD_ARR(GetPipboyRadioSounds)
		REG_CMD(GetPipBoyRadioSoundTimeRemaining)
		
		REG_CMD(GetHealthEffectsSum)
		
		//todo: REG_CMD(GetLevelUpMenuMaxPoints)
		//
		//REG_CMD(GetLevelUpMenuCanExitEarly)
		
		// IsBipedForm
		REG_CMD_FORM(GetActorPreferredWeapon)
		REG_CMD(TryDropWeapon)
		REG_CMD(SetSecuritronExpressionTemp)

		//more JSON functions...
		
		REG_CMD(GetPCCanSleepInOwnedBeds)
		REG_CMD(SetPCCanSleepInOwnedBeds)
		
		//REG_CMD_ARR(Ar_Init);
		REG_CMD(HasAnyScriptPackage)

		REG_CMD(SetProjectileRefFlag)
		REG_CMD(GetProjectileRefFlag)

		REG_CMD(SetChallengeProgress)
		REG_CMD(ModChallengeProgress)
		REG_CMD(CompleteChallenge)
	
		REG_CMD(SetBaseActorValue)


		//REG_CMD(UnequipItemsFromBitMask)  //tricky to get xData, idk if anyone will ever use this anyways.
		//REG_CMD(SetOnHitAltEventHandler);

		//REG_CMD(GetItemRefHealth);

		//REG_CMD(SetPlayerPickpocketBaseChance);
		//REG_CMD(GetFastTravelFlags);

		REG_CMD(SetPCCanPickpocketInCombat);
		
		REG_CMD(SetNoEquip);

		//REG_CMD(GetPCCanSleepWait)  //0x969FA0

		REG_CMD_ARR(GetQueuedCornerMessages);

		REG_CMD(TestDemo);

		
		REG_CMD(CanBeMoved)

		
		/* todo =======Function ideas ======
		 *
		 * GetActorValueShowoff - uses numeric keys like JG, but also has args to get the base/current/etc. Basically all-in-one.
		 * IsActorInRadius - returns true if the actor is within the Radius extradata of the reference.
		 * GetCalculatedItemWeight, using parts from GetCalc.equ.weight
		 * GetEquippedTotalValue
		 * IsWeaponThrowable = 10 and 13 is throw
		 * IsPlaceable = 11 and 12 is Place
		 *
		 *
		 */


		
		/* ===Forever Dead Function Graveyard===
		 *
		 * 
		 * //REG_CMD(DumpGameSettings) //pointless, see GetGameSettings in JIP
		 * //REG_CMD_ARR(Ar_GetInvalidRefs); //not possible via plugin
		 * //REG_CMD(GetEquippedWeaponType);  //JIP already made it for TTW.
		 * //REG_CMD(IsCornerMessageDisplayed)  //(any corner message). Redundant in the face of GetNumQueuedCornerMessages.
		 * //REG_CMD(PlayerIsUsingTurbo) //basically pointless, as it behaves the same as GetAV Turbo == 1, except it doesn't update in pipboy unlike getav.
		 * //REG_CMD(PlayerHasCateyeEnabled)  //Pointless, since Player.GetAV NightEye does the same thing
		 * //REG_CMD(PlayerHasImprovedSpotting)  //pointless, just use GetPerkModifier for the "Has Improved Spotting" perk entry.
		//REG_CMD(SetCellFullNameAlt)
			//These two functions are useless, the setting functions already safety check and even report if the setting could not be found via func result.
		REG_CMD(IsGameSetting) //For use in scripts to safety check; any other gamesetting function can already be used in console to check if a gamesetting exists.
		REG_CMD(IsINISetting) //Uses the GetNumericINISetting "SettingName:CategoryName" format
		 */		


#endif

		

		return true;
		

	}

};

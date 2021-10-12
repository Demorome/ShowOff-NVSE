#include "ShowOffNVSE.h"

#include <memory>
#include <mutex>
#include <shared_mutex>

#include "settings.h"

#include "memory_pool.h"
#include "containers.h"
#include "internal/serialization.h"
#include "jip_nvse.h"
#include "StewieMagic.h" 

// Functions
#include "functions/ShowOff_fn_Misc.h"
#include "functions/ShowOff_fn_Settings.h"
#include "functions/ShowOff_fn_Gameplay.h"
#include "functions/ShowOff_fn_Array.h"
#include "functions/ShowOff_fn_AuxVars.h" 
#include "functions/ShowOff_fn_Actors.h"
#include "functions/ShowOff_fn_Debug.h"
#include "functions/ShowOff_fn_Files.h"
#include "functions/ShowOff_fn_Items.h"

// Events
#include "Events/JohnnyEventPredefinitions.h"
#include "Events/EventFilteringInterface.h"
#include "Events/CustomEventFilters.h"
#include "Events/ShowOffEvents.h"


// Plugin Stuff
IDebugLog g_Log("ShowOffNVSE.log");
HMODULE	g_ShowOffHandle;
UInt32 g_PluginVersion = 125;

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
NVSEArrayElement EventResultPtr;
bool (*FunctionCallScriptAlt)(Script* funcScript, TESObjectREFR* callingObj, UInt8 numArgs, ...);
TESObjectREFR* (__stdcall *InventoryRefCreate)(TESObjectREFR* container, TESForm* itemForm, SInt32 countDelta, ExtraDataList* xData);
_CaptureLambdaVars CaptureLambdaVars;
_UncaptureLambdaVars UncaptureLambdaVars;

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

// Hook Globals
std::atomic<bool> g_canPlayerPickpocketInCombat = false;


//todo: remove INI globals and make a class for them (unordered map, access value via string key)

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

//-PreventBrokenItemRepairing (PBIR) INI globals 
std::atomic<bool> g_PBIR_On;
char* g_PBIR_FailMessage = nullptr;


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
		break;
	case NVSEMessagingInterface::kMessage_PreLoadGame:
		//_MESSAGE("Received PRELOAD message with file path %s", msg->data);
		break;
	case NVSEMessagingInterface::kMessage_PostLoadGame:
		//_MESSAGE("Received POST POS LOAD GAME message with file path %s", msg->data);
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
		//_MESSAGE("Received exit game to main menu message");
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

		HandleHooks::HandleDelayedGameHooks();

		Console_Print("ShowOff NVSE version: %.2f", (g_PluginVersion / 100.0F));

		break;

	case NVSEMessagingInterface::kMessage_MainGameLoop:
		for (const auto& EventInfo : EventsArray)
		{
			EventInfo->AddQueuedEvents();
			EventInfo->DeleteEvents();
		}
		break;

	case NVSEMessagingInterface::kMessage_RuntimeScriptError:
		//_MESSAGE("Received runtime script error message %s", msg->data);
		break;
	default:
		break;
	}
}


//No idea why the extern "C" is there.
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
			_ERROR("%s", buffer);
			return false;
		}
		
		if (!nvse->isEditor)
		{
			if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
			{
				char buffer[100];
				sprintf_s(buffer, 100, "Incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion, RUNTIME_VERSION_1_4_0_525);
				MessageBoxA(nullptr, buffer, "ShowOff", MB_ICONEXCLAMATION);
				_ERROR("%s", buffer);
				return false;
			}

			if (nvse->isNogore)
			{
				char buffer[] = "NoGore is not supported";
				MessageBoxA(nullptr, buffer, "ShowOff", MB_ICONEXCLAMATION);
				_ERROR("%s", buffer);
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
				_ERROR("%s", buffer);
				return false;
			}
		}

		// version checks pass
		// any version compatibility checks should be done here
		return true;
	}

	bool NVSEPlugin_Load(const NVSEInterface* nvse)
	{
		if (nvse->isEditor)
		{
			_MESSAGE("ShowOffNVSE Loaded successfully (Editor).\nShowOffNVSE plugin version: %u\n", g_PluginVersion);
		}
		else
		{
			_MESSAGE("ShowOffNVSE Loaded successfully (In-Game).\nShowOffNVSE plugin version: %u\n", g_PluginVersion);
		}
		
		// register to receive messages from NVSE
		((NVSEMessagingInterface*)nvse->QueryInterface(kInterface_Messaging))->RegisterListener(nvse->GetPluginHandle(), "NVSE", MessageHandler);

		if (!nvse->isEditor) 
		{
			PluginHandle nvsePluginHandle = nvse->GetPluginHandle();  //from JiPLN
			
			NVSEDataInterface* nvseData = (NVSEDataInterface*)nvse->QueryInterface(kInterface_Data);
			InventoryRefGetForID = (InventoryRef * (*)(UInt32 refID))nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
			InventoryRefCreate = (TESObjectREFR * (__stdcall*)(TESObjectREFR* container, TESForm *itemForm, SInt32 countDelta, ExtraDataList *xData))nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceCreateEntry);
			CaptureLambdaVars = (_CaptureLambdaVars)nvseData->GetFunc(NVSEDataInterface::kNVSEData_LambdaSaveVariableList);
			UncaptureLambdaVars = (_UncaptureLambdaVars)nvseData->GetFunc(NVSEDataInterface::kNVSEData_LambdaUnsaveVariableList);

			// From JiPLN (jip_nvse.cpp) 
			NVSESerializationInterface* serialization = (NVSESerializationInterface*)nvse->QueryInterface(kInterface_Serialization);
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
			serialization->SetNewGameCallback(nvsePluginHandle, NewGameCallback);
			
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

#if 0
			auto johnnyGuitar = GetModuleHandle("johnnyguitar.dll");
			JGCreateEvent = (CreateScriptEvent)GetProcAddress(johnnyGuitar, "JGCreateEvent");
			JGFreeEvent = (FreeScriptEvent)GetProcAddress(johnnyGuitar, "JGFreeEvent");
			//todo: if using these imports, add JG requirement check
#endif
			
			//HandleINIOptions();  //todo: overhaul INI options + add more before introducing to public
			HandleHooks::HandleGameHooks();
			HandleHooks::HandleEventHooks();
			RegisterEvents();
		}

		// Register script commands
		// https://geckwiki.com/index.php?title=NVSE_Opcode_Base
		nvse->SetOpcodeBase(0x3C93);

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
		/*3CAC*/ REG_CMD(ListAddArray)
		/*3CAD*/ REG_CMD(ListAddList)
		/*3CAE*/ REG_CMD(IsAnimPlayingExCond)
		/*3CAF*/ REG_CMD(GetNumCompassHostiles)

		// === TO DOCUMENT ====
		/*3CB0*/ REG_CMD(AuxStringMapArrayGetSize)
		/*3CB1*/ REG_CMD(AuxStringMapArrayGetValueType)
		/*3CB3*/ REG_CMD(AuxStringMapArrayGetFloat)
		/*3CB4*/ REG_CMD_FORM(AuxStringMapArrayGetRef)
		/*3CB5*/ REG_CMD_STR(AuxStringMapArrayGetString)
		/*3CB6*/ REG_CMD_ARR(AuxStringMapArrayGetFirst)
		/*3CB7*/ REG_CMD_ARR(AuxStringMapArrayGetNext)
		/*3CB8*/ REG_CMD_ARR(AuxStringMapArrayGetKeys)
		/*3CB9*/ REG_CMD_ARR(AuxStringMapArrayGetAll)  //todo: test this
		/*3CBA*/ REG_CMD_ARR(AuxStringMapArrayGetAsArray)
		/*3CBC*/ REG_CMD(AuxStringMapArraySetFromArray)
		/*3CBD*/ REG_CMD(AuxStringMapArraySetFloat)
		/*3CBE*/ REG_CMD(AuxStringMapArraySetRef)
		/*3CBF*/ REG_CMD(AuxStringMapArraySetString)
		/*3CC0*/ REG_CMD(AuxStringMapArrayEraseKey)
		/*3CC1*/ REG_CMD(AuxStringMapArrayValidateValues)  //todo: test
		/*3CC2*/ REG_CMD(AuxStringMapArrayDestroy)

		/*3CC3*/ REG_CMD(GetRadiationExtraData)
		/*3CC4*/ REG_CMD(SetRadiationExtraData)
		/*3CC5*/ REG_CMD(PlayerHasNightVisionActive)
		/*3CC6*/ REG_CMD(PlayerIsDrinkingPlacedWater)
		/*3CC7*/ REG_CMD(SetIsPCAMurderer)
		/*3CC8*/ REG_CMD(IsNight)
		/*3CC9*/ REG_CMD(IsLimbCrippled)
		/*3CCA*/ REG_CMD(GetNumCrippledLimbs)
		/*3CCB*/ REG_CMD(GetCrippledLimbsAsBitMask)
		/*3CCC*/ REG_CMD(GetNumBrokenEquippedItems)
		/*3CCD*/ REG_CMD(GetEquippedItemsAsBitMask)
		/*3CCE*/ REG_CMD(GetEquippedWeaponType)
		/*3CCF*/ REG_CMD(GetBaseEquippedWeight)
		/*3CD0*/ REG_CMD(GetCalculatedEquippedWeight)  //NOTE: bWeightlessWorn(Power)Armor from Stewie's is not accounted for.
		/*3CD1*/ REG_CMD(GetCalculatedMaxCarryWeight)

		//========v1.10
		/*3CD2*/ REG_CMD(GetShowOffDebugMode)
		/*3CD3*/ REG_CMD(SetShowOffDebugMode)
		/*3CD4*/ REG_CMD(UnequipItems)
		/*3CD5*/ REG_CMD_ARR(GetEquippedItems)
		/*3CD6*/ REG_CMD(IsBaseForm)
		/*3CD7*/ REG_CMD(SetRandomizerSeed)
		/*3CD8*/ REG_CMD(SetSeedUsingForm)
		/*3CD9*/ REG_CMD(GetRandomizerSeed)
		/*3CDA*/ REG_CMD(RandSeeded)
		/*3CDB*/ REG_CMD(GetRandomPercentSeeded)
		/*3CDC*/ REG_CMD(IsReferenceCloned)
		/*3CDD*/ REG_CMD(AdvanceSeed)

		//========v1.15
		/*3CDE*/ REG_CMD(IsTemporaryReference)  // KEEP UNDOCUMENTED, does not behave as I expected it to.
		/*3CDF*/ REG_CMD(GetPCHasScriptedFastTravelOverride)
		/*3CE0*/ REG_CMD(GetPCCanFastTravel)  //CheckAllowFastTravel 0x93D660
		/*3CE1*/ REG_CMD(GetWeaponHasFlag)
		/*3CE2*/ REG_CMD(GetActorHasBaseFlag)
		/*3CE3*/ REG_CMD(RemoveAllItemsShowOff)

		//========v1.17
		/*3CE4*/ REG_CMD(ForceWeaponJamAnim)
		/*3CE5*/ REG_CMD(DisableAlt)
		/*3CE6*/ REG_CMD(EnableAlt)
		/*3CE7*/ REG_CMD(GetCalculatedSkillPoints)
		/*3CE8*/ REG_CMD(GetLevelUpMenuPoints)
		/*3CE9*/ REG_CMD(ToggleQuestMessages)  // SuppressQuestMessages from JIP is probably way better implemented.

		//========v1.20
		/*3CEA*/ REG_CMD(GetCalculatedPerkPoints)
		/*3CEB*/ REG_CMD(GetLevelUpMenuCurrentPage)
		/*3CEC*/ REG_CMD(SetLevelUpMenuCurrentPage)
		/*3CED*/ REG_CMD(ShowPerkMenu)

		//========v1.25
		/*3CEE*/ REG_CMD(ShowSkillMenu)
		/*3CEF*/ REG_CMD(GetLevelUpMenuUnspentPoints)  //todo: consider making the result serialized.
		/*3CF0*/ REG_CMD(SetLevelUpMenuCanExitEarly)
		/*3CF1*/ REG_CMD(SetLevelUpMenuPoints)
		/*3CF2*/ REG_CMD(CreateFolder)
		/*3CF3*/ REG_CMD_FORM(GetExplosionRefSource)
		/*3CF4*/ REG_CMD(GetExplosionRefRadius)
		/*3CF5*/ REG_CMD(SetExplosionRefRadius)
		/*3CF6*/ REG_CMD(SetExplosionRefSource)
		/*3CF7*/ REG_CMD(GetActorValueDamage)
		/*3CF8*/ REG_CMD_ARR(GetPipboyRadioVoiceEntryData)  // todo: FIX CRASHING (when music plays and arg1 = 1?)
		/*3CF9*/ REG_CMD_ARR(GetEquippedItemRefs)
		/*3CFA*/ REG_CMD(SetNoEquipShowOff)
		/*3CFB*/ REG_CMD_AMB(GetNoEquipShowOff)	// can return a form (function) or a bool.
		/*3CFC*/ REG_CMD(SetShowOffOnCornerMessageEventHandler)

		//========v1.30
		/*3CFD*/ REG_CMD_FORM(GetIngestibleConsumeSound)
		/*3CFE*/ REG_CMD(SetIngestibleConsumeSound)

		
		//***Current Max OpCode: 0x3D10 (https://geckwiki.com/index.php?title=NVSE_Opcode_Base)
		
		//========v1.??
		//todo: always check to update/increase your opcode range when adding new functions



		
#if _DEBUG  //for functions being tested (or just abandoned).

		
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

		REG_CMD_ARR(ReadArrayFromJSON)
		//more JSON functions...
		
		REG_CMD(GetPCCanSleepInOwnedBeds)
		REG_CMD(SetPCCanSleepInOwnedBeds)
		
		REG_CMD(ClearShowoffSavedData)  //todo: test serialization. Seems broken???
		
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

		//REG_CMD(GetItemRefValue);
		//REG_CMD(GetItemRefHealth);  //THESE PROBABLY NEED SAFETY CHECKS (check if loaded in mid-high)
		//REG_CMD(GetCalculatedItemWeight);

		//REG_CMD(SetPlayerPickpocketBaseChance);
		//REG_CMD(GetFastTravelFlags);

		REG_CMD(SetPCCanPickpocketInCombat);
		
		REG_CMD(SetNoEquip);

		//REG_CMD(GetPCCanSleepWait)  //0x969FA0

		REG_CMD(SetEnableParent);
		REG_CMD_ARR(GetQueuedCornerMessages);

		REG_CMD(GetNVSEVersionFullAlt)
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

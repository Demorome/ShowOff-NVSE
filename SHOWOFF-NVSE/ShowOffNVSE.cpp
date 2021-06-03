#include <memory>
#include <mutex>
#include <shared_mutex>
#include <atomic>

#include "nvse/PluginAPI.h"
#include "nvse/CommandTable.h"
#include "nvse/GameAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/GameObjects.h"
#include "nvse/GameUI.h"
#include "nvse/GameRTTI.h"
#include "nvse/ArrayVar.h"
#include "nvse/SafeWrite.h"

#include "common/ICriticalSection.h"
#include "ShowOffNVSE.h"
#include "settings.h"


#include "GameData.h"
#include "GameScript.h"
#include "internal/decoding.h"
#include "internal/utility.h"
#include "internal/memory_pool.h"
#include "internal/containers.h"
#include "internal/jip_nvse.h"
#include "internal/StewieMagic.h"
#include "internal/Johnnnny Guitarrrrr.h"

#include "params.h"
#include "internal/serialization.h"

// Functions
#include "functions/ShowOff_fn_Misc.h"
#include "functions/ShowOff_fn_Settings.h"
#include "functions/ShowOff_fn_Gameplay.h"
#include "functions/ShowOff_fn_Array.h"
#include "functions/ShowOff_fn_AuxVars.h" 
#include "functions/ShowOff_fn_Actors.h"
#include "functions/ShowOff_fn_Debug.h"

// Events
#include "Events/JohnnyEventPredefinitions.h"
#include "Events/EventFilteringInterface.h"
#include "Events/CustomEventFilters.h"
#include "Events/ShowOffEvents.h"


// Plugin Stuff
IDebugLog g_Log("ShowOffNVSE.log");
HMODULE	g_ShowOffHandle;
UInt32 g_ShowOffVersion = 100;

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
	ScopedLock lock(&g_Lock);
	// do un-thread safe stuff like modify globals, only a single thread can be here at a time

	//will automatically leave critical section when leaving scope.
}

std::atomic<char*> doesn't really work.
*/
ICriticalSection g_Lock;

//NVSE Globals
bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);
bool (*ExtractFormatStringArgs)(UInt32 fmtStringPos, char* buffer, COMMAND_ARGS_EX, UInt32 maxParams, ...);  // From JIP_NVSE.H
NVSEArrayVarInterface* g_arrInterface = nullptr;
NVSEArrayVar* (*CreateArray)(const NVSEArrayElement* data, UInt32 size, Script* callingScript);
NVSEArrayVar* (*CreateStringMap)(const char** keys, const NVSEArrayElement* values, UInt32 size, Script* callingScript);
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

//Singletons
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

//-Hook Globals
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


// This is a message handler for nvse events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case NVSEMessagingInterface::kMessage_LoadGame: // Credits to C6 for the help.
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
		g_thePlayer = PlayerCharacter::GetSingleton();
		g_playerAVOwner = &g_thePlayer->avOwner;
		g_processManager = (ProcessManager*)0x11E0E80;
		g_bsWin32Audio = BSWin32Audio::GetSingleton();
		g_dataHandler = DataHandler::Get();
		g_audioManager = (BSAudioManager*)0x11F6EF0;
		g_currentSky = (Sky**)0x11DEA20;
		g_HUDMainMenu = *(HUDMainMenu**)0x11D96C0;  // From JiP's patches game
		g_interfaceManager = *(InterfaceManager**)0x11D8A80; // From JiP's patches game
		g_tileMenuArray = *(TileMenu***)0x11F350C; // From JiP's patches game
		g_screenWidth = *(UInt32*)0x11C73E0; // From JiP's patches game
		g_screenHeight = *(UInt32*)0x11C7190; // From JiP's patches game

		break;

	//case NVSEMessagingInterface::kMessage_MainGameLoop:
		//_MESSAGE("MainLOOP");
		//break;

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
		info->version = g_ShowOffVersion;
		//s_debug.CreateLog("Demo_NVSE_Debug.log");
		//g_Log.open??

		// version checks
		if (nvse->nvseVersion < PACKED_NVSE_VERSION)
		{
			char buffer[100];
			sprintf_s(buffer, 100,"NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
			MessageBoxA(nullptr, buffer, "ShowOff", MB_ICONEXCLAMATION);
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
				char buffer[30] = "NoGore is not supported";
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
			_MESSAGE("ShowOffNVSE Loaded successfully (Editor).\nShowOffNVSE plugin version: %u\n", g_ShowOffVersion);
		}
		else
		{
			_MESSAGE("ShowOffNVSE Loaded successfully (In-Game).\nShowOffNVSE plugin version: %u\n", g_ShowOffVersion);
		}
		
		// register to receive messages from NVSE
		((NVSEMessagingInterface*)nvse->QueryInterface(kInterface_Messaging))->RegisterListener(nvse->GetPluginHandle(), "NVSE", MessageHandler);

		if (!nvse->isEditor) 
		{
			PluginHandle nvsePluginHandle = nvse->GetPluginHandle();
			
			NVSEDataInterface* nvseData = (NVSEDataInterface*)nvse->QueryInterface(kInterface_Data);
			InventoryRefGetForID = (InventoryRef * (*)(UInt32))nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);

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
			
			g_commandInterface = (NVSECommandTableInterface*)nvse->QueryInterface(kInterface_CommandTable);
			
			g_strInterface = (NVSEStringVarInterface*)nvse->QueryInterface(kInterface_StringVar);
			GetStringVar = g_strInterface->GetString;
			AssignString = g_strInterface->Assign;

			g_arrInterface = (NVSEArrayVarInterface*)nvse->QueryInterface(kInterface_ArrayVar);
			CreateArray = g_arrInterface->CreateArray;
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
#endif
			
			HandleINIOptions();
			HandleGameHooks();
			HandleEventHooks();
		}

		// Register script commands
		// https://geckwiki.com/index.php?title=NVSE_Opcode_Base
		nvse->SetOpcodeBase(0x3A9F);

		/* ONLY COMMANDS WITH LISTED OPCODES SHOULD BE USED IN SCRIPTS */
		
		REG_CMD(ModNumericGameSetting)
		REG_CMD(ModNumericINISetting)
		REG_CMD(SetPlayerCanPickpocketEquippedItems)
		REG_CMD(GetPlayerCanPickpocketEquippedItems)
		REG_CMD(GetPCHasSleepWaitOverride)
		REG_CMD(SetPCHasSleepWaitOverride)
		REG_CMD(ShowingOffDisable)  //Showing off by copying vanilla style way functions! 
		REG_CMD(ShowingOffEnable) //best to keep undocumented to not waste people's time.
		REG_CMD(GetNumActorsInRangeFromRef)
		REG_CMD(GetNumCombatActorsFromActor)
		
		REG_CMD(IsWeaponMelee)
		REG_CMD(IsEquippedWeaponMelee)
		REG_CMD(IsWeaponRanged)
		REG_CMD(IsEquippedWeaponRanged)
		
		REG_CMD(GetChallengeProgress)
		
		REG_CMD(TestCondition)
		
		REG_CMD(MessageExAltShowoff) //Keep undocumented; don't recommend for general use, extra feature is jank.
		//REG_CMD(IsCornerMessageDisplayed)  //(any corner message). Redundant in the face of GetNumQueuedCornerMessages.
		REG_CMD(GetNumQueuedCornerMessages)
		
		REG_CMD(GetCreatureTurningSpeed)
		REG_CMD(SetCreatureTurningSpeed)
		REG_CMD(GetCreatureFootWeight)
		REG_CMD(SetCreatureFootWeight)
		REG_CMD(SetCreatureReach)
		REG_CMD(SetCreatureBaseScale) //JG's GetBaseScale should return the creature's base scale, so no need for a GetCreatureBaseScale.

		REG_CMD(ListAddArray)
		REG_CMD(ListAddList)
		
		REG_CMD(IsAnimPlayingExCond)
		REG_CMD(GetNumCompassHostilesInRange)


#if _DEBUG  //for functions being tested (or just abandoned).

		//todo: test AuxStringMap serialization / clearing.
		REG_CMD(AuxStringMapArrayGetSize)
		REG_CMD(AuxStringMapArrayGetValueType)
		REG_CMD(AuxStringMapArrayGetFloat)
		REG_CMD_FORM(AuxStringMapArrayGetRef)
		REG_CMD_STR(AuxStringMapArrayGetString)
		REG_CMD_ARR(AuxStringMapArrayGetFirst)
		REG_CMD_ARR(AuxStringMapArrayGetNext)
		REG_CMD_ARR(AuxStringMapArrayGetKeys)
		REG_CMD_ARR(AuxStringMapArrayGetAll) 
		REG_CMD_ARR(AuxStringMapArrayGetAsArray)
		REG_CMD(AuxStringMapArraySetFromArray)
		REG_CMD(AuxStringMapArraySetFloat)
		REG_CMD(AuxStringMapArraySetRef)
		REG_CMD(AuxStringMapArraySetString)
		REG_CMD(AuxStringMapArrayEraseKey)
		REG_CMD(AuxStringMapArrayValidateValues)
		REG_CMD(AuxStringMapArrayDestroy)

		REG_CMD(GetRadiationExtraData)
		REG_CMD(SetRadiationExtraData)
		
		REG_CMD(PlayerHasNightVision)
		REG_CMD(PlayerIsUsingTurbo) //todo: needs testing
		REG_CMD(PlayerHasCateyeEnabled)
		REG_CMD(PlayerHasImprovedSpotting)
		REG_CMD(PlayerIsDrinkingPlacedWater)
		REG_CMD(SetPlayerIsAMurderer)
		REG_CMD(IsNight)
		REG_CMD(IsLimbCrippled)
		REG_CMD(GetNumCrippledLimbs)
		REG_CMD(GetCrippledLimbsAsBitMask)
		REG_CMD(GetNumBrokenEquippedItems)
		REG_CMD(GetEquippedItemsAsBitMask)
		//REG_CMD(UnequipItemsFromBitMask)  //tricky to get xData, idk if anyone will ever use this anyways.
		//todo: GetEquippedTotalValue
		REG_CMD(ClearShowoffSavedData)  //todo: test serialization

		//todo: make GetCalculatedEquippedWeight / GetBaseEquippedWeight allow for float min, instead of int min.
		REG_CMD(GetBaseEquippedWeight)
		REG_CMD(GetCalculatedEquippedWeight)  //NOTE: bWeightlessWorn(Power)Armor from Stewie's is not accounted for.
		REG_CMD(GetCalculatedMaxCarryWeight)

		REG_CMD(SetShowOffOnCornerMessageEventHandler)
		

		REG_CMD(DumpFormList)  //todo: verify if char* usage is safe. Breaks for large arrays!

		
		//REG_CMD_ARR(Ar_Init);
		REG_CMD(SetCellFullNameAlt)
		REG_CMD(HasAnyScriptPackage)

		//These two functions are useless, the setting functions already safety check and even report if the setting could not be found via func result.
		REG_CMD(IsGameSetting) //For use in scripts to safety check; any other gamesetting function can already be used in console to check if a gamesetting exists.
		REG_CMD(IsINISetting) //Uses the GetNumericINISetting "SettingName:CategoryName" format
		
		REG_CMD(SetProjectileRefFlag)
		REG_CMD(GetProjectileRefFlag)

		REG_CMD(SetChallengeProgress)
		REG_CMD(ModChallengeProgress)
		REG_CMD(CompleteChallenge)
	
		REG_CMD(SetBaseActorValue)

		//REG_CMD(DumpGameSettings) //pointless, see GetGameSettings in JIP

		//REG_CMD(SetOnHitAltEventHandler);

		REG_CMD(GetItemRefValue);
		REG_CMD(GetItemRefHealth);  //THESE PROBABLY NEED SAFETY CHECKS (check if loaded in mid-high)
		//REG_CMD(GetCalculatedItemWeight);

		//REG_CMD(SetPlayerPickpocketBaseChance);
		//REG_CMD(GetFastTravelFlags);

		REG_CMD(SetPCCanPickpocketInCombat);
		
		REG_CMD(SetNoEquip);

		//REG_CMD_ARR(Ar_GetInvalidRefs); //gave up
		
		//REG_CMD(GetEquippedWeaponType);  //JIP already made it for TTW.

		REG_CMD(SetEnableParent);
		REG_CMD_ARR(GetQueuedCornerMessages);
		REG_CMD(GetPCHasFastTravelOverride)


		REG_CMD(TestDemo);

		/*todo =======Function ideas ======
		 *
		 * GetActorValueShowoff - uses numeric keys like JG, but also has args to get the base/current/etc. Basically all-in-one.
		 * IsActorInRadius - returns true if the actor is within the Radius extradata of the reference.
		 * GetCalculatedItemWeight, using parts from GetCalc.equ.weight
		 * Cmd_ShowSleepWaitMenu::CheckPreconditions -> GetPCCanSleepWait (0x969FA0)
		 * 
		 */

#endif

		

		return true;
		

	}

};

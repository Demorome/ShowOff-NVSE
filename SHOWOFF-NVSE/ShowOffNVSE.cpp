#include "nvse/PluginAPI.h"
#include "nvse/CommandTable.h"
#include "nvse/GameAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/GameObjects.h"
#include "nvse/GameUI.h"
#include "nvse/GameRTTI.h"
#include "nvse/ArrayVar.h"
#include "nvse/SafeWrite.h"

#include "internal/decoding.h"
#include "internal/utility.h"
#include "internal/StewieMagic.h"
#include "internal/jip_nvse.h"
#include "internal/Johnnnny Guitarrrrr.h"

#include "params.h"
#include "ShowOffNVSE.h"
#include "GameData.h"
#include "settings.h"
#include "GameScript.h"

// Functions
#include "functions/Trooper_fn_misc.h"
#include "functions/Demo_fn_Misc.h"
#include "functions/Demo_fn_Settings.h"
#include "functions/Demo_fn_Gameplay.h"
#include "functions/Demo_fn_Array.h"
#include "functions/Demo_fn_Actors.h"
#include "functions/Demo_fn_Debug.h"

// Events
#include "Events/EventFilteringInterface.h"
#include "Events/Demo_ev_Misc.h"


// Plugin Stuff

IDebugLog	gLog("ShowOffNVSE.log");
HMODULE	ShowOffHandle;


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
			ShowOffHandle = hModule;
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
		//gLog.open??

		// version checks
		if (nvse->nvseVersion < PACKED_NVSE_VERSION)
		{
			_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
			return false;
		}
		
		if (!nvse->isEditor)
		{
			if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
			{
				_ERROR("incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion, RUNTIME_VERSION_1_4_0_525);
				return false;
			}

			if (nvse->isNogore)
			{
				_ERROR("NoGore is not supported");
				return false;
			}
		}
		else
		{
			if (nvse->editorVersion < CS_VERSION_1_4_0_518)
			{
				_ERROR("incorrect editor version (got %08X need at least %08X)", nvse->editorVersion, CS_VERSION_1_4_0_518);
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
			NVSEDataInterface* nvseData = (NVSEDataInterface*)nvse->QueryInterface(kInterface_Data);
			InventoryRefGetForID = (InventoryRef * (*)(UInt32))nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
			g_script = (NVSEScriptInterface*)nvse->QueryInterface(kInterface_Script);
			CmdIfc = (NVSECommandTableInterface*)nvse->QueryInterface(kInterface_CommandTable);
			g_strInterface = (NVSEStringVarInterface*)nvse->QueryInterface(kInterface_StringVar); // From JG
			g_arrInterface = (NVSEArrayVarInterface*)nvse->QueryInterface(kInterface_ArrayVar); // From JG
			GetElement = g_arrInterface->GetElement;
			ExtractArgsEx = g_script->ExtractArgsEx;
			ExtractFormatStringArgs = g_script->ExtractFormatStringArgs;
			auto johnnyGuitar = GetModuleHandle("johnnyguitar.dll");
			
			
			handleIniOptions();
			DoHooks();
		}

		// Register script commands
		// https://geckwiki.com/index.php?title=NVSE_Opcode_Base
		nvse->SetOpcodeBase(0x3A9F);

		/* ONLY COMMANDS WITH LISTED OPCODES SHOULD BE USED IN SCRIPTS */

		REG_CMD(DumpFormList)

		REG_CMD(ModNumericGameSetting)
		REG_CMD(ModNumericINISetting)

		REG_CMD(SetPlayerCanPickpocketEquippedItems)
		REG_CMD(GetPlayerCanPickpocketEquippedItems)
		REG_CMD(GetPCCanFastTravel)
		REG_CMD(GetPCCanSleepWait)
		REG_CMD(SetPCCanSleepWait)
	
		//Showing off by copying vanilla style way functions!
		REG_CMD(ShowingOffDisable)  //best to keep undocumented to not waste people's time.
		REG_CMD(ShowingOffEnable)

		REG_CMD(GetNumActorsInRangeFromRef)
		REG_CMD(GetNumCombatActorsFromActor)
		
		REG_CMD(IsWeaponMelee)
		REG_CMD(IsEquippedWeaponMelee)
		REG_CMD(IsWeaponRanged)
		REG_CMD(IsEquippedWeaponRanged)
		
		REG_CMD(GetChallengeProgress)
		
		REG_CMD(TestCondition)
		
		REG_CMD(MessageExAltShowoff) //Keep undocumented; don't recommend for general use
		REG_CMD(IsCornerMessageDisplayed)  //(any corner message)
		REG_CMD(GetNumQueuedCornerMessages)
		
		REG_CMD(GetCreatureTurningSpeed)
		REG_CMD(SetCreatureTurningSpeed)
		REG_CMD(GetCreatureFootWeight)
		REG_CMD(SetCreatureFootWeight)
		REG_CMD(GetCreatureAttackReach)
		REG_CMD(SetCreatureAttackReach)
		REG_CMD(SetCreatureBaseScale) //JG's GetBaseScale should return the creature's base scale, so no need for a GetCreatureBaseScale.

		REG_CMD(ListAddArray)
		REG_CMD(ListAddList)
		
		REG_CMD(IsAnimPlayingExCond)
		REG_CMD(GetNumCompassHostilesInRange)


#if _DEBUG  //for functions being tested (or just abandoned).


		
#if 0
		REG_CMD_ARR(Ar_Init);
#endif

		REG_CMD(SetCellFullNameAlt)
		REG_CMD_STR(GetCellFullName);

		REG_CMD(HasAnyScriptPackage);

		//These two functions are useless, the setting functions already safety check and even report if the setting could not be found via func result.
		REG_CMD(IsGameSetting); //For use in scripts to safety check; any other gamesetting function can already be used in console to check if a gamesetting exists.
		REG_CMD(IsINISetting); //Uses the GetNumericINISetting "SettingName:CategoryName" format
		
		REG_CMD(SetProjectileRefFlag);
		REG_CMD(GetProjectileRefFlag);

		REG_CMD(SetChallengeProgress);
		REG_CMD(ModChallengeProgress);
		REG_CMD(CompleteChallenge);
		
		REG_CMD(SetBaseActorValue);

		REG_CMD(DumpGameSettings); //no idea how to do this...

		//REG_CMD(SetOnHitAltEventHandler);

		REG_CMD(GetItemRefValue);
		REG_CMD(GetItemRefHealth);  //THESE PROBABLY NEED SAFETY CHECKS (check if loaded in mid-high)
		//REG_CMD(GetCalculatedItemWeight);

		//REG_CMD(SetPlayerPickpocketBaseChance);
		REG_CMD(GetFastTravelFlags);

		REG_CMD(SetPCCanPickpocketInCombat);
		
		REG_CMD(SetNoEquip);

		//REG_CMD_ARR(Ar_GetInvalidRefs); //gave up
		
		//REG_CMD(GetEquippedWeaponType);  //JIP already made it for TTW.

		REG_CMD(SetEnableParent);

		REG_CMD(ApplyPoison);

		REG_CMD_ARR(GetQueuedCornerMessages);

		REG_CMD(TestDemo);

#endif

		

		return true;
		

	}

};

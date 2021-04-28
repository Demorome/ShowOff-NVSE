#define IFYOULIKEBROKENSHIT 1


#include "nvse/PluginAPI.h"
#include "nvse/CommandTable.h"
#include "nvse/GameAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/GameObjects.h"
#include "nvse/GameUI.h"
#include "internal/decoding.h"
#include "params.h"
#include "internal\utility.h"
#include "nvse\ArrayVar.h"
#include "ShowOffNVSE.h"
#include "GameData.h"
#include "SafeWrite.h"
#include "ShowOffNVSE.h"
#include "internal/StewieMagic.h"
#include "internal/jip_nvse.h"

//#include "nvse\nvse\iomanip"

#include <string>

#include "settings.h"
#include "internal/decoding.h"




#ifndef RegisterScriptCommand
#define RegisterScriptCommand(name) 	nvse->RegisterCommand(&kCommandInfo_ ##name);
#endif

IDebugLog		gLog("DemoNVSE.log");


PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

NVSEMessagingInterface* g_messagingInterface;
NVSEArrayVarInterface* ArrIfc = NULL;
NVSEInterface* g_nvseInterface;
NVSECommandTableInterface* g_cmdTable;
const CommandInfo* g_TFC;

bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);


NVSEStringVarInterface* StrIfc = NULL;
HUDMainMenu* g_HUDMainMenu = NULL;
TileMenu** g_tileMenuArray = NULL;
UInt32 g_screenWidth = 0;
UInt32 g_screenHeight = 0;


UInt32 DemoNVSEVersion = 100;
#define NUM_ARGS *((UInt8*)scriptData + *opcodeOffsetPtr)


/*
UnorderedMap<const char*, UInt32> s_menuNameToID({ {"MessageMenu", kMenuType_Message}, {"InventoryMenu", kMenuType_Inventory}, {"StatsMenu", kMenuType_Stats},
	{"HUDMainMenu", kMenuType_HUDMain}, {"LoadingMenu", kMenuType_Loading}, {"ContainerMenu", kMenuType_Container}, {"DialogMenu", kMenuType_Dialog},
	{"SleepWaitMenu", kMenuType_SleepWait}, {"StartMenu", kMenuType_Start}, {"LockpickMenu", kMenuType_LockPick}, {"QuantityMenu", kMenuType_Quantity},
	{"MapMenu", kMenuType_Map}, {"BookMenu", kMenuType_Book}, {"LevelUpMenu", kMenuType_LevelUp}, {"RepairMenu", kMenuType_Repair},
	{"RaceSexMenu", kMenuType_RaceSex}, {"CharGenMenu", kMenuType_CharGen}, {"TextEditMenu", kMenuType_TextEdit}, {"BarterMenu", kMenuType_Barter},
	{"SurgeryMenu", kMenuType_Surgery}, {"HackingMenu", kMenuType_Hacking}, {"VATSMenu", kMenuType_VATS}, {"ComputersMenu", kMenuType_Computers},
	{"RepairServicesMenu", kMenuType_RepairServices}, {"TutorialMenu", kMenuType_Tutorial}, {"SpecialBookMenu", kMenuType_SpecialBook},
	{"ItemModMenu", kMenuType_ItemMod}, {"LoveTesterMenu", kMenuType_LoveTester}, {"CompanionWheelMenu", kMenuType_CompanionWheel},
	{"TraitSelectMenu", kMenuType_TraitSelect}, {"RecipeMenu", kMenuType_Recipe}, {"SlotMachineMenu", kMenuType_SlotMachine},
	{"BlackjackMenu", kMenuType_Blackjack}, {"RouletteMenu", kMenuType_Roulette}, {"CaravanMenu", kMenuType_Caravan}, {"TraitMenu", kMenuType_Trait} });
*/

typedef NVSEArrayVarInterface::Array NVSEArrayVar;
typedef NVSEArrayVarInterface::Element NVSEArrayElement;




#if 0
Tile* InterfaceManager::GetActiveTile() //probably from JiP
{
	return activeTile ? activeTile : activeTileAlt;
}
#endif

// SaveFileVars
//////////////////////////////
char SavegameFolder[0x4000];
//Temp
char* s_SaveTemp;

//LastLoaded
UInt32 iLoadSGLength;
char LoadedSGName[0x4000] = "NULL";
char LoadedSGPathFOS[0x4000] = "NULL";
char LoadedSGPathNVSE[0x4000] = "NULL";

//LastSaved
UInt32 iSavedSGLength;
char SavedSGName[0x4000] = "NULL";
char SavedSGPathFOS[0x4000] = "NULL";
char SavedSGPathNVSE[0x4000] = "NULL";
//////////////////////////////



#if 0
extern UnorderedSet<UInt32> s_gameLoadedInformedScriptsSUP;
#endif

#if RUNTIME
NVSEScriptInterface* g_script;
#endif
// This is a message handler for nvse events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case NVSEMessagingInterface::kMessage_LoadGame: // Credits to C6 for the help.
		break;
	case NVSEMessagingInterface::kMessage_SaveGame:

		UInt32 iLength2;
		s_SaveTemp = (char*)msg->data;
		strcpy(SavedSGName, "");
		iLength2 = strlen(s_SaveTemp) - 4;
		strncat(SavedSGName, s_SaveTemp, iLength2);



		strcpy(SavedSGPathFOS, SavegameFolder);
		strcat(SavedSGPathFOS, s_SaveTemp);

		strcpy(SavedSGPathNVSE, SavegameFolder);
		iSavedSGLength = strlen(s_SaveTemp);
		iLength2 = iSavedSGLength - 3;
		strncat(SavedSGPathNVSE, s_SaveTemp, iLength2);
		strcat(SavedSGPathNVSE, "nvse");

		//_MESSAGE("Current Saved FOS name is %s", SavedSGPathFOS);
		//_MESSAGE("Current Saved NVSE name is %s", SavedSGPathNVSE);

		break;
	case NVSEMessagingInterface::kMessage_PreLoadGame:
#if 0
		s_gameLoadedInformedScriptsSUP.Clear();
#endif

		_MESSAGE("Received PRELOAD message with file path %s", msg->data);

		UInt32 iLength;
		s_SaveTemp = (char*)msg->data;
		strcpy(LoadedSGName, "");
		iLength = strlen(s_SaveTemp) - 4;
		strncat(LoadedSGName, s_SaveTemp, iLength);



		//strncat(LoadedSGName, s_SaveTemp, iLength);

		strcpy(LoadedSGPathFOS, SavegameFolder);
		strcat(LoadedSGPathFOS, s_SaveTemp);

		strcpy(LoadedSGPathNVSE, SavegameFolder);
		iLoadSGLength = strlen(s_SaveTemp);
		iLength = iLoadSGLength - 3;
		strncat(LoadedSGPathNVSE, s_SaveTemp, iLength);
		strcat(LoadedSGPathNVSE, "nvse");

		//_MESSAGE("Current Loaded FOS name is %s", LoadedSGPathFOS);
		//_MESSAGE("Current Loaded NVSE name is %s", LoadedSGPathNVSE);



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

		CALL_MEMBER_FN(SaveGameManager::GetSingleton(), ConstructSavegamePath)(SavegameFolder);
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

/*
#include "Tomm_fn_Misc.h"
#include "Tomm_fn_UI.h"   //May be useful to look at, but won't be included.
*/

#include "functions/Demo_fn_Misc.h"
#include "Trooper_fn_misc.h"
#include "functions/Demo_fn_Settings.h"
#include "functions/Demo_fn_GameMechanics.h"


#if RUNTIME
//In here we define a script function
//Script functions must always follow the Cmd_FunctionName_Execute naming convention

#endif

//This defines a function without a condition, that does not take any arguments







//No idea why the extern "C" is there.
extern "C"
{

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
		//_MESSAGE("query");

		// fill out the info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "Demo NVSE Plugin";
		info->version = DemoNVSEVersion;


		//s_debug.CreateLog("Demo_NVSE_Debug.log");

		// version checks
		if (nvse->nvseVersion < NVSE_VERSION_INTEGER)
		{
			_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, NVSE_VERSION_INTEGER);
			return false;
		}

		if (!nvse->isEditor)
		{
			g_script = (NVSEScriptInterface*)nvse->QueryInterface(kInterface_Script);
			ExtractArgsEx = g_script->ExtractArgsEx;
			StrIfc = (NVSEStringVarInterface*)nvse->QueryInterface(kInterface_StringVar); // From JG

			ArrIfc = (NVSEArrayVarInterface*)nvse->QueryInterface(kInterface_ArrayVar); // From JG


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
		g_pluginHandle = nvse->GetPluginHandle();

		// save the NVSEinterface in cas we need it later
		g_nvseInterface = (NVSEInterface*)nvse;

		// register to receive messages from NVSE
		g_messagingInterface = (NVSEMessagingInterface*)nvse->QueryInterface(kInterface_Messaging);
		g_messagingInterface->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);

		_MESSAGE("DemoNVSE Version: %d", DemoNVSEVersion);

		handleIniOptions();

#if RUNTIME
		g_script = (NVSEScriptInterface*)nvse->QueryInterface(kInterface_Script);
#endif

		if (!nvse->isEditor) 
		{
			DoHooks();
			
			NVSEDataInterface* nvseData = (NVSEDataInterface*)nvse->QueryInterface(kInterface_Data);
			InventoryRefGetForID = (InventoryRef * (*)(UInt32))nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
		}

		// Register script commands
		// 
		//https://geckwiki.com/index.php?title=NVSE_Opcode_Base
#define REG_TYPED_CMD(name, type) nvse->RegisterTypedCommand(&kCommandInfo_##name,kRetnType_##type);
#define REG_CMD_STR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_String) // From JIP_NVSE.H
#define REG_CMD_ARR(name) nvse->RegisterTypedCommand(&kCommandInfo_##name, kRetnType_Array) // From JIP_NVSE.H

		nvse->SetOpcodeBase(0x3A9F);

#if 0 //examples
		RegisterScriptCommand(GetSavedSaveSize);
		REG_CMD_STR(GetSaveName);

		REG_CMD_ARR(GetNearCells, Array);
		REG_CMD_ARR(DumpTileInfoToArray, Array);
#endif

#if IFYOULIKEBROKENSHIT //aka functions being tested (or just abandoned).
		RegisterScriptCommand(CompleteChallenge);
		RegisterScriptCommand(SetBaseActorValue);

		/*
		RegisterScriptCommand(SetNumericGameSettingAlt);
		RegisterScriptCommand(SetNumericINISettingAlt);
		*/
		RegisterScriptCommand(DumpGameSettings);

		//RegisterScriptCommand(SetOnHitAltEventHandler);

		RegisterScriptCommand(GetItemRefValue);
		RegisterScriptCommand(GetItemRefHealth);  //THESE PROBABLY NEED SAFETY CHECKS (check if loaded in mid-high)
		//RegisterScriptCommand(GetCalculatedItemWeight);

		RegisterScriptCommand(MultiJump);  //super broke
		//RegisterScriptCommand(SetPlayerPickpocketBaseChance);
		RegisterScriptCommand(GetFastTravelFlags);
		RegisterScriptCommand(SetPCCanPickpocketInCombat);
		RegisterScriptCommand(SetNoEquip);
#endif

		/* ONLY COMMANDS WITH LISTED OPCODES SHOULD BE USED IN SCRIPTS */
		RegisterScriptCommand(GetChallengeProgress);
		RegisterScriptCommand(SetChallengeProgress);
		RegisterScriptCommand(ModChallengeProgress);

		RegisterScriptCommand(DumpFormList);

		RegisterScriptCommand(IsGameSetting); //For use in scripts to safety check; any other gamesetting function can already be used in console to check if a gamesetting exists.
		RegisterScriptCommand(IsINISetting); //Uses the GetNumericINISetting "SettingName:CategoryName" format
		RegisterScriptCommand(ModNumericGameSetting);
		RegisterScriptCommand(ModNumericINISetting);

		RegisterScriptCommand(SetPlayerCanPickpocketEquippedItems);
		RegisterScriptCommand(GetPlayerCanPickpocketEquippedItems);
		RegisterScriptCommand(GetPCCanFastTravel);
		RegisterScriptCommand(GetPCCanSleepWait);
		RegisterScriptCommand(SetPCCanSleepWait);
	
		//Showing off by copying vanilla style way functions!

		RegisterScriptCommand(ShowingOffDisable);
		RegisterScriptCommand(ShowingOffEnable);

		return true;

	}

};

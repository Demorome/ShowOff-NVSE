#pragma once
#include <SimpleINILibrary\SimpleIni.h>
#define INI_NAME "ShowOff_nvse.ini"
#define MO2_VFS_DLL_NAME "usvfs_x86.dll"

//-- Declare Global Vars before reading INI values.

//For Force Pickpocketting (enabled via function)
float g_fForcePickpocketBaseAPCost;
float g_fForcePickpocketMinAPCost;
float g_fForcePickpocketMaxAPCost;
float g_fForcePickpocketPlayerAgilityMult;
float g_fForcePickpocketPlayerSneakMult;
float g_fForcePickpocketTargetPerceptionMult;
float g_fForcePickpocketItemWeightMult;
float g_fForcePickpocketItemValueMult;
float g_fForcePickpocketPlayerStrengthMult;
float g_fForcePickpocketTargetStrengthMult;
char* g_fForcePickpocketFailureMessage = nullptr;

#if 0
//For PreventBackwardsRangedAttacks (PBRA)
bool g_PBRA_On;
float g_PBRA_MaxAbsHeadingAngle;
#endif

//For PreventBrokenItemRepairing (PBIR)
bool g_PBIR_On;
char* g_PBIR_FailMessage = nullptr;



//-- Read INI values.

extern HMODULE ShowOffHandle;

void HandleIniOptions()
{
	char iniPath[MAX_PATH];
	GetModuleFileNameA(ShowOffHandle, iniPath, MAX_PATH);
	strcpy((char*)(strrchr(iniPath, '\\') + 1), INI_NAME);

	CSimpleIniA ini;
	ini.SetUnicode();
	auto errVal = ini.LoadFile(iniPath);
	if (errVal == SI_Error::SI_FILE)
	{
		if (GetModuleHandle(MO2_VFS_DLL_NAME))
		{
			// user is using VFS and will probably not notice the ini if it were to generate in the MO2 overwrites folder, so don't create one
			MessageBoxA(NULL, "ShowOff INI not found, please download it or create an empty file ShowOff_nvse.ini in \'Data\\NVSE\\Plugins\\\' !", "ShowOff NVSE", MB_ICONINFORMATION);
			return;
		}
	}

	// INI
	ini.SetSortAlphabetically(ini.GetOrCreate("INI", "bSortAlphabetically", 0, "; sort the ini alphabetically"));
	ini.SetPrependNewKeys(ini.GetOrCreate("INI", "bPrependNewSettings", 1, "; add new settings to the top of the ini"));

	// Main

	// Force Pickpocket (idk if it should be renamed to Combat Pickpocket)
	g_fForcePickpocketBaseAPCost = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketBaseAPCost", 15, NULL);
	g_fForcePickpocketMinAPCost = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketMinAPCost", 10, NULL);
	g_fForcePickpocketMaxAPCost = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketMaxAPCost", 100, NULL);
	g_fForcePickpocketPlayerAgilityMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketPlayerAgilityMult", 1.3, NULL);
	g_fForcePickpocketPlayerSneakMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketPlayerSneakMult", 0.15, NULL);
	g_fForcePickpocketTargetPerceptionMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketTargetPerceptionMult", 1.4, NULL);
	g_fForcePickpocketItemWeightMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketItemWeightMult", 0.7, NULL);
	g_fForcePickpocketItemValueMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketItemValueMult", 0.05, NULL);
	g_fForcePickpocketPlayerStrengthMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketPlayerStrengthMult", 2, NULL);
	g_fForcePickpocketTargetStrengthMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketTargetStrengthMult", 2.5, NULL);
	g_fForcePickpocketFailureMessage = _strdup(ini.GetOrCreate("Force Pickpocket", "fForcePickpocketFailureMessage", "You don't have enough Action Points to steal this item.", "; Sets the text that will be displayed when the player does not have enough AP to pickpocket."));

#if 0
	//For PreventBackwardsRangedAttacks (PBRA)
	g_PBRA_On = ini.GetOrCreate("Prevent Backwards Ranged Attacks", "bOn", 0, ";EXPERIMENTAL. Prevents actors from firing if they aren't facing their target within a certain angle.");
	g_PBRA_MaxAbsHeadingAngle = ini.GetOrCreate("Prevent Backwards Ranged Attacks", "fMaxAbsHeadingAngle", 75, ";Controls the maximum heading angle that an actor must face their target in order to fire their weapon.");
#endif

	//For PreventBrokenItemRepairing (PBIR)
	g_PBIR_On = ini.GetOrCreate("Prevent Repairing Broken Items", "bOn", 0, NULL);
#if 0
	g_PBIR_FailMessage = _strdup(ini.GetOrCreate("Prevent Repairing Broken Items", "sFailMessage", "You cannot repair broken items.", NULL));;
#endif

	ini.SaveFile(iniPath, false);
}

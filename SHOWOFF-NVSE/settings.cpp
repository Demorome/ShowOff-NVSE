#include "ShowOffNVSE.h"
#include "settings.h"
#include "utility.h"

#include <SimpleIni.h>


int GetINIValFromPlugin(const char* iniName, const char* section, const char* key, int defaultVal)
{
	char iniPath[MAX_PATH];
	GetModuleFileNameA(g_ShowOffHandle, iniPath, MAX_PATH);
	strcpy((strrchr(iniPath, '\\') + 1), iniName);

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(iniPath);

	return ini.GetLongValue(section, key, defaultVal);
}

int GetINIValFromTweaks(const char* key, int defaultVal)
{
	if (!IsDllRunning(TWEAKS_DLL_NAME))
		return defaultVal;
	return GetINIValFromPlugin(TWEAKS_INI_NAME, "Tweaks", key, defaultVal);
}

int GetINIValFromTweaks(const char* key, const char* section, int defaultVal)
{
	if (!IsDllRunning(TWEAKS_DLL_NAME))
		return defaultVal;
	return GetINIValFromPlugin(TWEAKS_INI_NAME, section, key, defaultVal);
}



// INI usage copied from lStewieAl's Tweaks (settings.h)
void HandleINIOptions()
{
	char iniPath[MAX_PATH];
	GetModuleFileNameA(g_ShowOffHandle, iniPath, MAX_PATH);
	strcpy((strrchr(iniPath, '\\') + 1), INI_NAME);

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
	g_fForcePickpocketBaseAPCost = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketBaseAPCost", 15, nullptr);
	g_fForcePickpocketMinAPCost = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketMinAPCost", 10, nullptr);
	g_fForcePickpocketMaxAPCost = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketMaxAPCost", 100, nullptr);
	g_fForcePickpocketPlayerAgilityMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketPlayerAgilityMult", 1.3, nullptr);
	g_fForcePickpocketPlayerSneakMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketPlayerSneakMult", 0.15, nullptr);
	g_fForcePickpocketTargetPerceptionMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketTargetPerceptionMult", 1.4, nullptr);
	g_fForcePickpocketItemWeightMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketItemWeightMult", 0.7, nullptr);
	g_fForcePickpocketItemValueMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketItemValueMult", 0.05, nullptr);
	g_fForcePickpocketPlayerStrengthMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketPlayerStrengthMult", 2, nullptr);
	g_fForcePickpocketTargetStrengthMult = ini.GetOrCreate("Force Pickpocket", "fForcePickpocketTargetStrengthMult", 2.5, nullptr);
	g_fForcePickpocketFailureMessage = _strdup(ini.GetOrCreate("Force Pickpocket", "fForcePickpocketFailureMessage", "You don't have enough Action Points to steal this item.", "; Sets the text that will be displayed when the player does not have enough AP to pickpocket."));

	//For PreventBrokenItemRepairing (PBIR)
	g_PBIR_On = ini.GetOrCreate("Prevent Repairing Broken Items", "bOn", 0, nullptr);
#if 0
	g_PBIR_FailMessage = _strdup(ini.GetOrCreate("Prevent Repairing Broken Items", "sFailMessage", "You cannot repair broken items.", NULL));;
#endif

	ini.SaveFile(iniPath, false);
}

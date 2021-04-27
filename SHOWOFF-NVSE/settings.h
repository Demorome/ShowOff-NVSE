#pragma once
#include <SimpleINILibrary\SimpleIni.h>
#define INI_NAME "ShowOff_nvse.ini"
#define MO2_VFS_DLL_NAME "usvfs_x86.dll"

//-- Declare Global Vars before reading INI values.
float g_fForcePickpocketBaseAPCost;
float g_fForcePickpocketPlayerAgilityMult;
float g_fForcePickpocketPlayerLuckMult;
float g_fForcePickpocketPlayerSneakMult;
float g_fForcePickpocketItemValueMult;
float g_fForcePickpocketItemWeightMult;



//-- Read INI values.

extern HMODULE ShowOffHandle;

void handleIniOptions()
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
	
	ini.SetSortAlphabetically(ini.GetOrCreate("INI", "bSortAlphabetically", 0, "; sort the ini alphabetically"));
	ini.SetPrependNewKeys(ini.GetOrCreate("INI", "bPrependNewSettings", 1, "; add new settings to the top of the ini"));
	//g_fForcePickpocketBaseChance = ini.GetOrCreate("Main", "fForcePickpocketBaseChance", 1, "; fuck you");
	
	//ini.GetOrCreate("INI", "???", ? ? , "; ");
	ini.SaveFile(iniPath, false);
}

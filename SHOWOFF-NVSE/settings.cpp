#include "ShowOffNVSE.h"
#include "settings.h"
#include "utility.h"

#include <SimpleIni.h>



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
	const char* sectionName = "Main";
	g_bResetInteriorResetsActors = ini.GetOrCreate(sectionName, "bResetInteriorResetsActors", 0, "; If 1 (true), ResetInterior will behave like ResetInteriorAlt.");


	// Fatigue
	sectionName = "Fatigue";
	g_bUseGamesettingsForFistFatigueDamage = ini.GetOrCreate(sectionName,
		"bUseGamesettingsForFistFatigueDamage", 0,
		"; Instead of multiplying the hit damage by 0.5 to get the fist fatigue damage, the formula is now:\n"
				"; fistFatigueDmg = fHandFatigueDamageBase + (fHandFatigueDamageMult * regularDmg)");

	g_bCreaturesDealMeleeFatigueDmg = ini.GetOrCreate(sectionName,
		"bCreaturesDealMeleeFatigueDmg", 0,
		"; Lets creatures deal fatigue damage from melee attacks. The formula is:\n"
		"; meleeFatigueDmg = fHandFatigueDamageBase + (fHandFatigueDamageMult * regularDmg)");

	g_bUnarmedWeaponsDealFatigueDmg = ini.GetOrCreate(sectionName,
	"bUnarmedWeaponsDealFatigueDmg", 0,
		"; Lets unarmed weapons deal fatigue damage, except for those with the 'Spin' animation type. The formula is:\n"
		"; fatigueDmg = fFatigueAttackWeaponBase + (fFatigueAttackWeaponMult * regularDmg)");


	// Repairing
	sectionName = "Repair";
	g_bNoSelfRepairingBrokenItems = ini.GetOrCreate(sectionName, "bNoSelfRepairingBrokenItems", 0, 
		"; If 1 (true), broken items (0% condition) cannot be repaired in the pipboy repair menu.");
	g_bNoVendorRepairingBrokenItems = ini.GetOrCreate(sectionName, "bNoVendorRepairingBrokenItems", 0, 
		"; If 1 (true), broken items (0% condition) cannot be repaired in the vendor repair menu.");

	//Experimental (hidden)
	sectionName = "Experimental";
	g_bAlwaysUpdateWeatherForInteriors = ini.GetBoolValue(sectionName, "bAlwaysUpdateWeatherForInteriors");


#if 0
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
#endif


	ini.SaveFile(iniPath, false);
}

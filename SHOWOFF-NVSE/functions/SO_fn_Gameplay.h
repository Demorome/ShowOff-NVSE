#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_COMMAND_ALT_PLUGIN(SetPlayerCanPickpocketEquippedItems, SetPCCanStealEqItems, "Toggles the ability to pickpocket equipped items.", 
	false, kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetPlayerCanPickpocketEquippedItems, GetPCCanStealEqItems, "Checks if the player can pickpocket equipped items.", 
	false, NULL);
DEFINE_CMD_COND_PLUGIN(IsWeaponMelee, "Returns 1 if the weapon's base form is of one of the three weapon types belonging to melee-range weapons.", 
	false, kParams_OneOptionalObjectID);
DEFINE_CMD_COND_PLUGIN(IsEquippedWeaponMelee, "Returns 1 if the calling actor's equipped weapon's base form is of one of the three weapon types belonging to melee-range weapons.", 
	true, NULL);
DEFINE_CMD_COND_PLUGIN(IsWeaponRanged, "Returns 1 if the weapon's base form is one of the weapon types belonging to NON melee-range weapons.", 
	false, kParams_OneOptionalObjectID);
DEFINE_CMD_COND_PLUGIN(IsEquippedWeaponRanged, "Returns 1 if the calling actor's equipped weapon's base form is one of the weapon types belonging to NON melee-range weapons.", 
	true, NULL);
DEFINE_CMD_COND_PLUGIN(GetChallengeProgress, "Returns the progress made on a challenge.", false, kParams_OneChallenge)
DEFINE_CMD_COND_PLUGIN(GetPCHasScriptedFastTravelOverride, "Returns whether or not the player is restricted by EnableFastTravel", 
	false, NULL);
DEFINE_CMD_COND_PLUGIN(GetPCCanFastTravel, "", false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetWeaponHasFlag, WeaponHasFlag, "", false, kParams_OneInt_OneOptionalObjectID);
DEFINE_COMMAND_ALT_PLUGIN(SetWeaponFlag, SetWeaponHasFlag, "", false, kParams_TwoInts_OneOptionalObjectID);
DEFINE_CMD_ALT_COND_PLUGIN(GetActorHasBaseFlag, ActorHasBaseFlag, "", false, kParams_OneInt_OneOptionalActorBase);
DEFINE_COMMAND_ALT_PLUGIN(ForceWeaponJamAnim, ForceJamAnim, "", true, NULL);
#if _DEBUG
DEFINE_COMMAND_ALT_PLUGIN(ForceWeaponJamAnimAlt, ForceJamAnimAlt, "", true, NULL);
#endif
DEFINE_CMD_ALT_COND_PLUGIN(GetCalculatedSkillPoints, GetCalculatedSkillPointsEarnedPerLevel, "Gets the amount of skill points the player would get for their current level.", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetLevelUpMenuPoints, "", false, kParams_TwoOptionalInts);
DEFINE_CMD_ALT_COND_PLUGIN(GetCalculatedPerkPoints, GetCalculatedPerkPointsEarnedPerLevel, "Gets the amount of perk points the player would get for their current level.", false, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(GetLevelUpMenuCurrentPage, GetLevelUpMenuPage, "", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(SetLevelUpMenuCurrentPage, SetLevelUpMenuPage, "", false, kParams_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(ShowPerkMenu, IfIDecideToGoWithYourFunctionWhatAreThePerks, "Opens the Level-Up menu to the Perk-menu page, and prevents going back to Skills tab.", false, kParams_JIP_OneOptionalInt_OneOptionalString);
DEFINE_COMMAND_ALT_PLUGIN(ShowSkillMenu, ShowSkillsMenu, "Opens the Level-Up menu to the Skills page, and sets that this is the last page (no visiting perks).", false, kParams_TwoOptionalInts_OneOptionalString);
DEFINE_COMMAND_PLUGIN(GetLevelUpMenuUnspentPoints, "", false, kParams_OneInt);
DEFINE_COMMAND_PLUGIN(SetLevelUpMenuCanExitEarly, "", false, kParams_OneInt);  //todo: Get is a WIP, also "SetLevelUpMenuCanExitEarly 0" does not work currently.
DEFINE_COMMAND_PLUGIN(SetLevelUpMenuPoints, "", false, kParams_TwoInts);
DEFINE_COMMAND_PLUGIN(GetExplosionRefSource, "", true, NULL);
DEFINE_COMMAND_PLUGIN(SetExplosionRefSource, "", true, kParams_OneActorRef);
DEFINE_CMD_COND_PLUGIN(GetExplosionRefRadius, "Accounts for AdjustExplosionRadius perk entry point.", true, NULL);
DEFINE_COMMAND_PLUGIN(SetExplosionRefRadius, "", true, kParams_OneFloat);
DEFINE_COMMAND_ALT_PLUGIN(SetNoEquipShowOff, SetNoEquipSO, "Sets whether or not there's a prevention for an item baseform from being activated from an actor's inventory.", false, kParams_OneForm_OneInt_OneOptionalScript);
DEFINE_COMMAND_ALT_PLUGIN(GetNoEquipShowOff, GetNoEquipSO, "Returns whether or not there's a prevention for an item baseform from being activated from an actor's inventory.", false, kParams_OneForm_OneInt);
DEFINE_COMMAND_ALT_PLUGIN(SetOwnershipTemp, ImGoingToBorrowThis, "A mix of SetOwnership and SetCellOwnership, but non-savebaked.", false, kParams_TwoOptionalForms);
DEFINE_COMMAND_ALT_PLUGIN(GetCalculatedAPCost, GetCalculatedWeaponAttackAPCost, "", false, kParams_OneInt_OneOptionalForm);
DEFINE_CMD_COND_PLUGIN(GetPCHasSleepWaitOverride, "Returns whether or not the player has a Sleep/Wait prevention override", false, nullptr);
DEFINE_COMMAND_PLUGIN(SetPCHasSleepWaitOverride, "Sets whether or not the player has a Sleep/Wait prevention override", false, kParams_OneInt);
DEFINE_CMD_COND_PLUGIN(IsAiming, "", true, nullptr);
DEFINE_CMD_COND_PLUGIN(IsBlockingOrAiming, "", true, nullptr);
DEFINE_COMMAND_PLUGIN(FreezeAmmoRegen, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(IsJumping_BROKEN, "", true, nullptr);
DEFINE_CMD_COND_ONLY_PLUGIN(GetHitLocationLingering, "Unlike GetHitLocation_Eval, the hit location is allowed to linger after the initial hit.", true, nullptr);
DEFINE_COMMAND_PLUGIN(GetVATSMaxEngageDistance, "Accounts for Tweak's bMaxVATSDistanceUsesWeaponRange", false, nullptr);
DEFINE_CMD_COND_PLUGIN(GetVATSTargetable, "", true, nullptr);

#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_COMMAND_ALT_PLUGIN(ShowingOffDisable, DisableIFYouDidntNotice, "Does the same thing as vanilla Disable. For showing off!", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(ShowingOffEnable, EnableIFYouDidntNotice, "Does the same thing as vanilla Enable. For showing off!", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(DisableAlt, SnigOff, "Ignores the EnableParent limitation.", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(EnableAlt, SnigOn, "Ignores the EnableParent limitation.", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(ListAddList, AddFormListToFormList, "", false, kParams_TwoFormLists_OneOptionalIndex);
DEFINE_COMMAND_PLUGIN(MessageExAltShowoff, "", false, kParams_JIP_OneFloat_OneInt_OneFormatString);
DEFINE_CMD_COND_PLUGIN(IsCornerMessageDisplayed, "Returns 1/0 depending on if a corner message is displayed.", false, NULL);
DEFINE_CMD_COND_PLUGIN(GetNumQueuedCornerMessages, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(IsAnimPlayingExCond, "Same as IsAnimPlayingEx, but available as a condition. Had to cut the variationFlags filter.", true, kParams_JIP_OneInt_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetRadiationExtraData, "", true, NULL);
DEFINE_COMMAND_PLUGIN(SetRadiationExtraData, "", true, kParams_OneFloat);
DEFINE_CMD_COND_PLUGIN(PlayerHasNightVisionActive, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerIsUsingTurbo, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerHasCateyeEnabled, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerHasImprovedSpottingActive, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerIsDrinkingPlacedWater, "", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(SetIsPCAMurderer, YoureAMurderer, "", false , kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(IsNight, BloodyTears, "Returns true if it's night according to the current (or specified) climate.", false, kParams_OneOptionalForm);
DEFINE_CMD_COND_PLUGIN(IsLimbCrippled, "If no args are passed / arg is -1, returns true if actor has any crippled limbs. Otherwise, checks if the specified limb is crippled.", true, kParams_TwoOptionalInts);
DEFINE_CMD_COND_PLUGIN(GetNumCrippledLimbs, "", true, kParams_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(GetCrippledLimbsAsBitMask, "", true, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(ClearShowoffSavedData, "", false, kParams_TwoInts);
DEFINE_CMD_ALT_COND_PLUGIN(GetCalculatedMaxCarryWeight, GetMaxCarryWeightPerkModified, "Accounts for GetMaxCarryWeight perk entry.", true, NULL);
DEFINE_COMMAND_ALT_PLUGIN(SetRandomizerSeed, SetSeed, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(SetSeedUsingForm, SetFormSeed, "", false, kParams_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(GetRandomizerSeed, GetSeed, "", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(RandSeeded, GenerateSeededRandomNumber, "", false, kParams_TwoInts);
DEFINE_COMMAND_PLUGIN(GetRandomPercentSeeded, "", false, NULL);
DEFINE_COMMAND_PLUGIN(AdvanceSeed, "Discards would-be generated numbers to advance in the seed generation pattern.", false, kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(IsReferenceCloned, IsRefrCloned, "Checks if the reference's modIndex is 0xFF", true, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(IsTemporaryReference, IsTempRefr, "Checks if the reference does not persist in the savegame.", false, NULL); //todo: Set equivalents?
DEFINE_COMMAND_PLUGIN(ToggleQuestMessages, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetPipboyRadioVoiceEntryData, "", false, kParams_OneInt_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(FormListRemoveForm, RemoveFormFromFormList, "", false, kParams_OneFormList_OneForm);
DEFINE_COMMAND_PLUGIN(GetZoneRespawns, "Returns if an Encounter Zone has the NoRespawn flag set or not.", false, kParams_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(ClearCinematicTextQueue, ClearQuestMessageQueue, "", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(GetCellEncounterZone, GetCellZone, "", false, kParams_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(RemoveFormFromLeveledList, SorryYoureNotOnTheListAnymore, "", false, kParams_TwoForms);
DEFINE_COMMAND_ALT_PLUGIN(ResetInteriorAlt, IWantThisRoomCleanRightNow, "", false, kParams_OneInteriorCell);
DEFINE_COMMAND_ALT_PLUGIN(GetActorValueName, GetAVName, "", false, kParams_OneActorValue_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(ShowPauseMenu, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(SetEnableParent, SetParentRef, "", true, kParams_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(SetAmmoName, "", false, kParams_OneString_OneOptionalInt_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(GetAmmoName, "", false, kParams_OneOptionalInt_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(SetForceDrawHitscanProjectiles, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(SetProjectileTracerChanceOverride, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(SpawnTracingProjectile, "Spawns a projectile following the calling projectile reference.", true, kParams_OneForm_TwoOptionalInts);
DEFINE_CMD_COND_PLUGIN(IsPlayerLookingAround, "", false, nullptr);
DEFINE_COMMAND_PLUGIN(IsFormLoading, "", false, kParams_OneForm);
DEFINE_COMMAND_PLUGIN(PatchFreezeTime, "", false, nullptr);
DEFINE_COMMAND_PLUGIN(ToANSIChar, "", false, kParams_OneInt_OneOptionalInt);
#if _DEBUG
DEFINE_COMMAND_PLUGIN(CaravanDeckGetCards, "", false, kParams_OneForm);
DEFINE_COMMAND_PLUGIN(CaravanDeckRemoveCard, "", false, kParams_TwoForms);
DEFINE_COMMAND_PLUGIN(CaravanDeckAddCard, "", false, kParams_TwoForms_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(CaravanDeckGetCount, "", false, kParams_OneForm);
DEFINE_COMMAND_PLUGIN(CaravanDeckGetCardIndex, "", false, kParams_TwoForms);
DEFINE_COMMAND_PLUGIN(GetHeadingAngleTEST, "", false, kParams_OneForm);
DEFINE_COMMAND_PLUGIN(GetPipboyRadioSounds, "", false, NULL);
DEFINE_COMMAND_PLUGIN(GetPipBoyRadioSoundTimeRemaining, "", false, NULL);
DEFINE_COMMAND_PLUGIN(SetCellFullNameAlt, "Like SetCellFullName but accepts a string.", false, kParams_JIP_OneCell_OneString);
DEFINE_COMMAND_PLUGIN(GetQueuedCornerMessages, "Returns the queued corner messages as a multidimensional array.", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(SetBaseActorValue, SetBaseAV, "", false, kParams_JIP_OneActorValue_OneFloat_OneOptionalActorBase);
#endif

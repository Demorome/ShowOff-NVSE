#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>
#include <NVSEParamInfoList.h>

DEFINE_CMD_ALT_COND_PLUGIN(GetNumBrokenEquippedItems, GetNumBrokenEq, "", true, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetEquippedItemsAsBitMask, GetOccupiedEquipSlots, , true, NULL);
DEFINE_COMMAND_PLUGIN(UnequipItems, "", true, kParams_FourOptionalInts);
DEFINE_COMMAND_PLUGIN(GetEquippedItems, "", true, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetEquippedItemRefs, "", true, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(RemoveAllItemsShowOff, "", true, kParams_TwoOptionalInts_OneOptionalContainerRef_OneOptionalList);
DEFINE_CMD_COND_PLUGIN(GetEquippedWeaponType, "", true, NULL);
DEFINE_CMD_COND_PLUGIN(GetBaseEquippedWeight, "", true, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(GetCalculatedEquippedWeight, "Accounts for perk effects + weapon mods.", true, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetIngestibleConsumeSound, "", false, kParams_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(SetIngestibleConsumeSound, "", false, kParams_OneForm_OneOptionalForm);
DEFINE_COMMAND_PLUGIN(GetEquippedItemRefForItem, "Returns the equipped inv ref for the base item if that base item is equipped.", true, kParams_OneForm);
DEFINE_CMD_COND_PLUGIN(GetItemCanHaveHealth, "Returns 1 if the item form can be given health data, 0 otherwise.", false, kParams_OneOptionalForm);
DEFINE_CMD_COND_PLUGIN(GetCalculatedItemValue, "Returns the item's value, affected by condition and any attached weapon mods (and extra).", false, kParams_OneOptionalInt_OneOptionalObject);
DEFINE_COMMAND_PLUGIN(GetEquippedWeapon, "", true, nullptr);
DEFINE_COMMAND_PLUGIN(GetEquippedWeaponRef, "", true, nullptr);
DEFINE_COMMAND_PLUGIN_EXP(SetItemHotkeyIconPath, "", false, kNVSEParams_OneForm_OneString);
DEFINE_COMMAND_PLUGIN(GetItemHotkeyIconPath, "", false, kParams_OneForm_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(GetSpellUsageNumEx, "Returns the number of times an Ingestible has been used by an actor.", true, kParams_JIP_OneMagicItem);
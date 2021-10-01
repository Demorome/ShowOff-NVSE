#pragma once
#include "utility.h"


DEFINE_COMMAND_PLUGIN(GetIngestibleConsumeSound, "", false, kParams_OneOptionalForm);
bool Cmd_GetIngestibleConsumeSound_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESForm* form = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &form))
	{
		return true;
	}
	
	if (form = TryExtractBaseForm(form, thisObj))
	{
		if (auto const ingestible = DYNAMIC_CAST(form, TESForm, AlchemyItem))
		{
			REFR_RES = ingestible->consumeSound->refID;
		}
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(SetIngestibleConsumeSound, "", false, kParams_OneForm_OneOptionalForm);
bool Cmd_SetIngestibleConsumeSound_Execute(COMMAND_ARGS)
{
	//Returns if the change was successful or not.
	*result = false;
	
	TESSound* newSound;
	TESForm* form = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &newSound, &form) || NOT_TYPE(newSound, TESSound))
	{
		return true;
	}
	
	if (form = TryExtractBaseForm(form, thisObj))
	{
		if (auto const ingestible = DYNAMIC_CAST(form, TESForm, AlchemyItem))
		{
			ingestible->consumeSound = newSound;
			*result = true;
		}
	}
	return true;
}
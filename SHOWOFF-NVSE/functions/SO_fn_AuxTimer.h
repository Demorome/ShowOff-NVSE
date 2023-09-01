#pragma once
#include "AuxTimers.h"
#include "JohnnyEventPredefinitions.h"

extern EventInformation* OnAuxTimerStart;
extern EventInformation* OnAuxTimerStop;

using namespace AuxTimer;

ICriticalSection g_AuxTimerLock;
#define AUX_TIMER_CS ScopedLock lock(g_AuxTimerLock)

DEFINE_COMMAND_PLUGIN(AuxTimerStart, "Starts an auxvar timer on a form, or can restart it if exists already.", 
	false, kParams_OneString_OneOptionalDouble_OneOptionalInt_OneOptionalForm);
bool Cmd_AuxTimerStart_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	double timeToCountdown = -1.0; // if -1.0, will restart the timer while keeping the old flags if not specifying them, assuming it was set up before. 
	UInt32 flags = AuxTimerValue::kFlag_Defaults;
	TESForm* form = nullptr;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &timeToCountdown, &flags, &form))
	{
		if (!thisObj)
			thisObj = g_thePlayer;

		AuxTimerMapInfo const varInfo(form, thisObj, scriptObj, varName);
		if (varInfo.ownerID)
		{
			AUX_TIMER_CS;
			AuxTimerValue* value = GetTimerValue(varInfo, false);
			if (!value)
			{
				if (timeToCountdown == -1.0)
					return true; //todo: dispatch xNVSE error
				value = GetTimerValue(varInfo, true);
			}

			if (timeToCountdown == -1.0)
			{
				if (flags != AuxTimerValue::kFlag_Defaults)
					value->m_flags = flags;
				value->RestartTimer();
			}
			else
			{
				value->SetTimeToCountdown(timeToCountdown);
				value->m_flags = flags;
			}

			for (auto const& callback : OnAuxTimerStart->EventCallbacks) {
				auto* filter = reinterpret_cast<JohnnyEventFiltersOneFormOneString*>(callback.eventFilter);
				if (filter->IsInFilter(0, varInfo.ownerID) && filter->IsInFilter(1, varName)) {
					if (varInfo.modIndex != 0xFF || callback.ScriptForEvent->GetOverridingModIdx() == varInfo.modIndex) {
						FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnAuxTimerStart->numMaxArgs, varName, LookupFormByRefID(varInfo.ownerID));
					}
				}
			}
			
			if (varInfo.isPerm)
			{
				s_dataChangedFlags |= kChangedFlag_AuxTimerMaps;
				if (thisObj)
					thisObj->MarkAsModified(0);
			}
			*result = 1;
		}
	}
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN(AuxTimerStop, AuxTimerErase, "Stops/erases an auxvar timer on a form.",
	false, kParams_OneString_OneOptionalInt_OneOptionalForm);
bool Cmd_AuxTimerStop_Execute(COMMAND_ARGS)
{
	*result = 0;
	char varName[0x50];
	UInt32 fireEvent = false;
	TESForm* form = nullptr;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &fireEvent, &form))
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		AuxTimerMapInfo const varInfo(form, thisObj, scriptObj, varName);
		if (varInfo.ownerID)
		{
			AuxTimerValue* value = GetTimerValue(varInfo, false);
			if (!value)
				return true;

			if (fireEvent)
			{
				for (auto const& callback : OnAuxTimerStop->EventCallbacks) {
					auto* filter = reinterpret_cast<JohnnyEventFiltersOneFormOneString*>(callback.eventFilter);
					if (filter->IsInFilter(0, varInfo.ownerID) && filter->IsInFilter(1, varName)) {
						if (varInfo.modIndex != 0xFF || callback.ScriptForEvent->GetOverridingModIdx() == varInfo.modIndex) {
							FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnAuxTimerStop->numMaxArgs, varName, LookupFormByRefID(varInfo.ownerID));
						}
					}
				}
			}
			AUX_TIMER_CS;
			std::vector<AuxTimerPendingRemoval>& timersToRemove = varInfo.isPerm ? g_auxTimersToRemovePerm : g_auxTimersToRemoveTemp;
			timersToRemove.emplace_back(AuxTimerPendingRemoval{ varInfo.modIndex, varInfo.ownerID, varName });
			value->m_flags |= AuxTimerValue::kFlag_PendingRemoval;

			if (varInfo.isPerm)
			{
				s_dataChangedFlags |= kChangedFlag_AuxTimerMaps;
				if (thisObj)
					thisObj->MarkAsModified(0);
			}
			*result = 1;
		}
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(AuxTimerPaused, "Gets/Sets if an auxvar timer is paused.",
	false, kParams_OneString_OneOptionalInt_OneOptionalForm);
bool Cmd_AuxTimerPaused_Execute(COMMAND_ARGS)
{
	*result = -1;
	char varName[0x50];
	int setPaused = -1; // -1 = just check, 0 and above will modify.
	TESForm* form = nullptr;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &setPaused, &form))
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		AuxTimerMapInfo const varInfo(form, thisObj, scriptObj, varName);
		if (varInfo.ownerID)
		{
			AuxTimerValue* value = GetTimerValue(varInfo, false);
			if (!value)
				return true;

			*result = value->m_flags & AuxTimerValue::kFlag_IsPaused;

			if (setPaused >= 0 && value->m_timeRemaining > 0.0)
			{
				AUX_TIMER_CS;
				if (!setPaused)
					value->m_flags &= ~AuxTimerValue::kFlag_IsPaused;
				else
					value->m_flags |= AuxTimerValue::kFlag_IsPaused;

				if (varInfo.isPerm)
				{
					s_dataChangedFlags |= kChangedFlag_AuxTimerMaps;
					if (thisObj)
						thisObj->MarkAsModified(0);
				}
			}
		}
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(AuxTimerTimeElapsed, "Gets how much time passed since timer started (limited by the original countdown time).",
	false, kParams_OneString_OneOptionalInt_OneOptionalForm);
bool Cmd_AuxTimerTimeElapsed_Execute(COMMAND_ARGS)
{
	*result = -1;
	char varName[0x50];
	int unused = -1; // todo: -1 = just check, 0 and above will modify.
	TESForm* form = nullptr;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &unused, &form))
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		AuxTimerMapInfo const varInfo(form, thisObj, scriptObj, varName);
		if (varInfo.ownerID)
		{
			AuxTimerValue* value = GetTimerValue(varInfo, false);
			if (!value)
				return true;
			*result = value->GetTimeElapsed();
		}
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(AuxTimerTimeToCountdown, "Gets the original countdown time.",
	false, kParams_OneString_OneOptionalForm);
bool Cmd_AuxTimerTimeToCountdown_Execute(COMMAND_ARGS)
{
	*result = -1;
	char varName[0x50];
	TESForm* form = nullptr;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &form))
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		AuxTimerMapInfo const varInfo(form, thisObj, scriptObj, varName);
		if (varInfo.ownerID)
		{
			AuxTimerValue* value = GetTimerValue(varInfo, false);
			if (!value)
				return true;
			*result = value->m_timeToCountdown;
		}
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(AuxTimerTimeLeft, "Gets the time left for the countdown, -1 if it does not exist.",
	false, kParams_OneString_OneOptionalForm);
bool Cmd_AuxTimerTimeLeft_Execute(COMMAND_ARGS)
{
	*result = -1;
	char varName[0x50];
	TESForm* form = nullptr;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &varName, &form))
	{
		if (!thisObj)
			thisObj = g_thePlayer;
		AuxTimerMapInfo const varInfo(form, thisObj, scriptObj, varName);
		if (varInfo.ownerID)
		{
			AuxTimerValue* value = GetTimerValue(varInfo, false);
			if (!value)
				return true;
			*result = value->m_timeRemaining;
		}
	}
	return true;
}
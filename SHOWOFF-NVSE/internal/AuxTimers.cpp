#include "AuxTimers.h"
#include "JohnnyEventPredefinitions.h"
#include "utility.h"

extern EventInformation* OnAuxTimerStart;
extern EventInformation* OnAuxTimerStop;

namespace AuxTimer
{
	AuxTimerModsMap s_auxTimerMapArraysPerm, s_auxTimerMapArraysTemp;

	AuxTimerValue* __fastcall GetTimerValue(const AuxTimerMapInfo& varInfo, bool createIfNotFound, bool* isCreated)
	{
		if (createIfNotFound) {
			if (isCreated)
				*isCreated = true;
			return &varInfo.ModsMap()[varInfo.modIndex][varInfo.ownerID][varInfo.varName];
		}

		if (isCreated)
			*isCreated = false;
		AuxTimerOwnersMap* ownersMap = varInfo.ModsMap().GetPtr(varInfo.modIndex);
		if (!ownersMap)
			return nullptr;
		AuxTimerVarsMap* varsMap = ownersMap->GetPtr(varInfo.ownerID);
		if (!varsMap)
			return nullptr;
		return varsMap->GetPtr(varInfo.varName);
	}

	void DoCountdown(double vatsTimeMult, bool isMenuMode, AuxTimerModsMap &auxTimers)
	{
		if (auxTimers.Empty())
			return;

		const double secondsDeltaWithMult = static_cast<double>(g_timeGlobal->secondsPassed) * vatsTimeMult;
		const double secondsDeltas[2] = { secondsDeltaWithMult, static_cast<double>(g_timeGlobal->secondsPassed) };

		for (auto modMapIter = auxTimers.Begin(); !modMapIter.End(); ++modMapIter)
		{
			for (auto refOwnersMapIter = modMapIter.Get().Begin(); 
				!refOwnersMapIter.End(); ++refOwnersMapIter)
			{
				for (auto auxVarNameMapIter = refOwnersMapIter.Get().Begin(); 
					!auxVarNameMapIter.End(); ++auxVarNameMapIter)
				{
					const auto ownerFormID = refOwnersMapIter.Key();
					if (const auto* ownerForm = LookupFormByRefID(ownerFormID))
					{
						auto& timer = auxVarNameMapIter.Get();

						if (timer.m_flags & AuxTimerValue::kFlag_IsPaused)
							continue;

						if (timer.m_flags & AuxTimerValue::kFlag_DontRunWhenPaused)
						{
							if (IsGamePaused())
								continue;
						}
							
						if ((timer.m_flags & AuxTimerValue::kFlag_RunInMenuMode && isMenuMode) 
							|| (timer.m_flags & AuxTimerValue::kFlag_RunInGameMode && !isMenuMode))
						{
							if (timer.m_flags & AuxTimerValue::kFlag_CountInSeconds)
							{
								const bool notAffectedByTimeMult = 
									(timer.m_flags & AuxTimerValue::kFlag_NotAffectedByTimeMult_InMenuMode)
									&& isMenuMode;
								timer.m_timeRemaining -= secondsDeltas[notAffectedByTimeMult ? 1 : 0];
							}
							else {
								--timer.m_timeRemaining;
							}

							if (timer.m_timeRemaining <= 0.0)
							{
								const bool isPerm = (auxVarNameMapIter.Key()[0] != '*');
								const bool isPrivate = (auxVarNameMapIter.Key()[!isPerm] == '_');

								for (auto const& callback : OnAuxTimerStop->EventCallbacks) {
									auto* filter = reinterpret_cast<JohnnyEventFiltersOneFormOneString*>(callback.eventFilter);
									if (filter->IsInFilter(0, ownerFormID) && filter->IsInFilter(1, auxVarNameMapIter.Key())) {
										if (!isPrivate || callback.ScriptForEvent->GetOverridingModIdx() == modMapIter.Key()) {
											FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnAuxTimerStop->numMaxArgs, auxVarNameMapIter.Key(), ownerForm);
										}
									}
								}

								if (timer.m_flags & AuxTimerValue::kFlag_AutoRestarts) {
									timer.m_timeRemaining = timer.m_timeToCountdown;

									for (auto const& callback : OnAuxTimerStart->EventCallbacks) {
										auto* filter = reinterpret_cast<JohnnyEventFiltersOneFormOneString*>(callback.eventFilter);
										if (filter->IsInFilter(0, ownerFormID) && filter->IsInFilter(1, auxVarNameMapIter.Key())) {
											if (!isPrivate || callback.ScriptForEvent->GetOverridingModIdx() == modMapIter.Key()) {
												FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnAuxTimerStart->numMaxArgs, auxVarNameMapIter.Key(), ownerForm);
											}
										}
									}
								}
								else {
									auxVarNameMapIter.Remove(); // valid mid-loop according to JIP's ClearJIPSavedData
								}
							}
						}
					}
					else // invalid form
					{
						// Don't care about invalidating the iterator
						auxVarNameMapIter.Remove();
					}
				}
				if (refOwnersMapIter.Get().Empty())
					refOwnersMapIter.Remove();
			}
			if (modMapIter.Get().Empty())
				modMapIter.Remove();
		}
	}

	void HandleAutoRemoveTempTimers()
	{
		for (auto modMapIter = s_auxTimerMapArraysTemp.Begin(); !modMapIter.End(); ++modMapIter)
		{
			for (auto refOwnersMapIter = modMapIter.Get().Begin();
				!refOwnersMapIter.End(); ++refOwnersMapIter)
			{
				for (auto auxVarNameMapIter = refOwnersMapIter.Get().Begin();
					!auxVarNameMapIter.End(); ++auxVarNameMapIter)
				{
					if (auxVarNameMapIter.Get().m_flags & AuxTimerValue::kFlag_AutoRemoveOnLoadAndMainMenu)
						auxVarNameMapIter.Remove();
					// intentionally not running OnTimerStop event for these
				}
				if (refOwnersMapIter.Get().Empty())
					refOwnersMapIter.Remove();
			}
			if (modMapIter.Get().Empty())
				modMapIter.Remove();
		}
	}
}


#include "AuxTimers.h"
#include "JohnnyEventPredefinitions.h"
#include "utility.h"

extern EventInformation* OnAuxTimerStart;
extern EventInformation* OnAuxTimerStop;
extern EventInformation* OnAuxTimerUpdate;

namespace AuxTimer
{
	AuxTimerModsMap s_auxTimerMapArraysPerm, s_auxTimerMapArraysTemp;

	std::vector<AuxTimerPendingRemoval> g_auxTimersToRemovePerm, g_auxTimersToRemoveTemp;

	AuxTimerValue* __fastcall GetTimerValue(const AuxTimerMapInfo& varInfo, bool createIfNotFound)
	{
		if (createIfNotFound) {
			auto &newEntry = varInfo.ModsMap()[varInfo.modIndex][varInfo.ownerID][varInfo.varName];
			// Remove pending removal flag, just in case we're not creating a new value.
			newEntry.m_flags &= ~AuxTimerValue::kFlag_PendingRemoval;
			return &newEntry;
		}

		AuxTimerOwnersMap* ownersMap = varInfo.ModsMap().GetPtr(varInfo.modIndex);
		if (!ownersMap)
			return nullptr;
		AuxTimerVarsMap* varsMap = ownersMap->GetPtr(varInfo.ownerID);
		if (!varsMap)
			return nullptr;
		auto* result = varsMap->GetPtr(varInfo.varName);
		if (result && result->IsPendingRemoval())
			return nullptr; // act as if we can't find it
		return result;
	}

	namespace Impl
	{
		void DoCountdown(double vatsTimeMult, bool isMenuMode, bool isTemp)
		{
			AuxTimerModsMap& auxTimers = isTemp ? s_auxTimerMapArraysTemp : s_auxTimerMapArraysPerm;

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
								float timePassed;
								if (timer.m_flags & AuxTimerValue::kFlag_CountInSeconds)
								{
									const bool notAffectedByTimeMult = 
										(timer.m_flags & AuxTimerValue::kFlag_NotAffectedByTimeMult_InMenuMode)
										&& isMenuMode;
									timePassed = secondsDeltas[notAffectedByTimeMult ? 1 : 0];
									timer.m_timeRemaining -= timePassed;
								}
								else {
									timePassed = 1;
									--timer.m_timeRemaining;
								}

								const bool isPerm = (auxVarNameMapIter.Key()[0] != '*');
								const bool isPublic = (auxVarNameMapIter.Key()[!isPerm] == '_');

								if (timer.m_flags & AuxTimerValue::kFlag_RunOnTimerUpdateEvent) {
									for (auto const& callback : OnAuxTimerUpdate->EventCallbacks) {
										auto* filter = reinterpret_cast<JohnnyEventFiltersOneFormOneString*>(callback.eventFilter);
										if (filter->IsInFilter(0, ownerFormID) && filter->IsInFilter(1, auxVarNameMapIter.Key())) {
											if (isPublic || callback.ScriptForEvent->GetOverridingModIdx() == modMapIter.Key()) {
												FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnAuxTimerUpdate->numMaxArgs, auxVarNameMapIter.Key(), ownerForm, *(UInt32*)&timePassed);
											}
										}
									}
								}

								// Timer could have stopped from inside OnAuxTimerUpdate handlers
								if (timer.IsPendingRemoval())
									continue;

								if (timer.m_timeRemaining <= 0.0)
								{
									// Handle end-of-timer code.

									for (auto const& callback : OnAuxTimerStop->EventCallbacks) {
										auto* filter = reinterpret_cast<JohnnyEventFiltersOneFormOneString*>(callback.eventFilter);
										if (filter->IsInFilter(0, ownerFormID) && filter->IsInFilter(1, auxVarNameMapIter.Key())) {
											if (isPublic || callback.ScriptForEvent->GetOverridingModIdx() == modMapIter.Key()) {
												FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnAuxTimerStop->numMaxArgs, auxVarNameMapIter.Key(), ownerForm);
											}
										}
									}

									// User could have called AuxTimerStop or AuxTimerStart in the OnAuxTimerStop event.
									if (timer.IsPendingRemoval() || timer.m_timeRemaining > 0.0)
										continue;

									if (timer.m_flags & AuxTimerValue::kFlag_AutoRestarts) {
										timer.m_timeRemaining = timer.m_timeToCountdown;

										for (auto const& callback : OnAuxTimerStart->EventCallbacks) {
											auto* filter = reinterpret_cast<JohnnyEventFiltersOneFormOneString*>(callback.eventFilter);
											if (filter->IsInFilter(0, ownerFormID) && filter->IsInFilter(1, auxVarNameMapIter.Key())) {
												if (isPublic || callback.ScriptForEvent->GetOverridingModIdx() == modMapIter.Key()) {
													FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnAuxTimerStart->numMaxArgs, auxVarNameMapIter.Key(), ownerForm);
												}
											}
										}
									}
									else {
										// don't care about delaying removal here since it should be thread-safe anyways
										auxVarNameMapIter.Remove(); // valid mid-loop according to JIP's ClearJIPSavedData
									}
								}
							}
						}
						else // invalid form
						{
							// Don't care about delaying removal here, it's just invalid garbage now
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
	}

	void DoCountdown(double vatsTimeMult, bool isMenuMode)
	{
		Impl::DoCountdown(vatsTimeMult, isMenuMode, true);
		Impl::DoCountdown(vatsTimeMult, isMenuMode, false);
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

	namespace Impl
	{
		void RemovePendingTimers(bool clearTemp)
		{
			std::vector<AuxTimerPendingRemoval>& timersToRemove = clearTemp ? g_auxTimersToRemoveTemp : g_auxTimersToRemovePerm;
			if (timersToRemove.empty())
				return;

			// After deleting some timers, clear out the maps the timers were contained in if they're now empty.
			std::unordered_set<UInt32> modMapsToUpdate;

			AuxTimerModsMap& modsMapOfAllTimers = clearTemp ? s_auxTimerMapArraysTemp : s_auxTimerMapArraysPerm;
			for (auto& timerToRemove : timersToRemove)
			{
				auto* modEntry = modsMapOfAllTimers.GetPtr(timerToRemove.modIndex);
				auto* modAndRefEntry = modEntry->GetPtr(timerToRemove.ownerID);

				// If the AuxTimer is no longer pending removal, 
				// ... likely because AuxTimerStart was called on it in the same frame it was stopped,
				// ... avoid deleting the timer.
				if (!modAndRefEntry->GetPtr(const_cast<char*>(timerToRemove.varName.c_str()))->IsPendingRemoval())
					continue;

				modAndRefEntry->Erase(const_cast<char*>(timerToRemove.varName.c_str()));
				if (modAndRefEntry->Empty()) {
					modEntry->Erase(timerToRemove.ownerID);
					// modAndRefEntry is no longer valid!
					modMapsToUpdate.insert(timerToRemove.modIndex);
				}
			}
			timersToRemove.clear();

			for (auto modIndex : modMapsToUpdate)
			{
				if (modsMapOfAllTimers.GetPtr(modIndex)->Empty())
					modsMapOfAllTimers.Erase(modIndex);
			}
		}
	}

	void RemovePendingTimers()
	{
		Impl::RemovePendingTimers(true);
		Impl::RemovePendingTimers(false);
	}
}


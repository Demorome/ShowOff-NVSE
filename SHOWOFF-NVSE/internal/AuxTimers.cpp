#include "AuxTimers.h"

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
			
		for (auto modMapIter = auxTimers.Begin(); !modMapIter.End(); ++modMapIter)
		{
			for (auto refOwnersMapIter = modMapIter.Get().Begin(); 
				!refOwnersMapIter.End(); ++refOwnersMapIter)
			{
				for (auto auxVarNameMapIter = refOwnersMapIter.Get().Begin(); 
					!auxVarNameMapIter.End(); ++auxVarNameMapIter)
				{
					auto ownerFormID = refOwnersMapIter.Key();
					if ((ownerFormID = GetResolvedRefID(ownerFormID)) && (LookupFormByRefID(ownerFormID) || HasChangeData(ownerFormID)))
					{
						auto& timer = auxVarNameMapIter.Get();

						if (timer.m_flags & AuxTimerValue::kFlag_IsPaused)
							continue;
						
						if ((timer.m_flags & AuxTimerValue::kFlag_RunInMenuMode && isMenuMode) 
							|| (timer.m_flags & AuxTimerValue::kFlag_RunInGameMode && !isMenuMode))
						{
							if (timer.m_flags & AuxTimerValue::kFlag_CountInSeconds)
							{
								const double secondsDelta = static_cast<double>(g_timeGlobal->secondsPassed) * vatsTimeMult;
								timer.m_timeRemaining -= secondsDelta;
							}
							else {
								--timer.m_timeRemaining;
							}

							if (timer.m_timeRemaining <= 0.0)
							{
								// TODO: run OnTimerStop

								if (timer.m_flags & AuxTimerValue::kFlag_AutoRestarts) {
									// TODO: run OnTimerStart

									timer.m_timeRemaining = timer.m_timeToCountdown;
								}
								else {
									auxVarNameMapIter.Remove(); // valid mid-loop according to JIP's ClearJIPSavedData
								}
							}
						}
					}
					else // invalid form
					{
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
				}
				if (refOwnersMapIter.Get().Empty())
					refOwnersMapIter.Remove();
			}
			if (modMapIter.Get().Empty())
				modMapIter.Remove();
		}
	}
}


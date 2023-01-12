#pragma once
#include "jip_nvse.h"

// Code structure copied from JIP's jip_core.h

constexpr UInt8 AuxTimerVersion = 1;

struct AuxTimerValue
{
	double m_timeToCountdown; //original value of time to start counting down.
	double m_timeRemaining; //counts down to 0. If below or at 0, timer is stopped/not running.
	
	enum Flags
	{
		// On by default
		kFlag_RunInMenuMode = 1 << 0,
		kFlag_RunInGameMode = 1 << 1,
		kFlag_Defaults = kFlag_RunInMenuMode | kFlag_RunInGameMode,

		// Off by default
		kFlag_AutoRemoveOnLoadAndMainMenu = 1 << 2,
		kFlag_AutoRestarts = 1 << 3,
		kFlag_IsPaused = 1 << 4,
	};
	UInt32 m_flags;

	AuxTimerValue() : m_flags(kFlag_Defaults) { ReadValData(); }
	AuxTimerValue(double timeToCountdown, UInt32 flags = kFlag_Defaults) :
		m_timeToCountdown(timeToCountdown), m_timeRemaining(timeToCountdown), m_flags(flags) { }

	[[nodiscard]] double GetTimeLeft() const { return m_timeRemaining; }

	void SetTimeToCountdown(double time) {
		m_timeToCountdown = time;
		m_timeRemaining = time;
	}
	void RestartTimer() {
		m_timeRemaining = m_timeToCountdown;
		m_flags &= ~kFlag_IsPaused;
	}
	[[nodiscard]] double GetTimeToCountdown() const {
		return m_timeToCountdown;
	}
	[[nodiscard]] double GetTimeRemaining() const {
		return m_timeRemaining;
	}
	[[nodiscard]] double GetTimeElapsed() const {
		return m_timeToCountdown - m_timeRemaining;
	}

	void WriteValData() const {
		WriteRecord64(&m_timeToCountdown);
		WriteRecord64(&m_timeRemaining);
		WriteRecord32(m_flags);
	}

private:

	void ReadValData() {
		ReadRecord64(&m_timeToCountdown);
		ReadRecord64(&m_timeRemaining);
		m_flags = ReadRecord32();
	}
};
static_assert(sizeof(AuxTimerValue) == 24);

using NameString = char*;

using AuxTimerVarsMap = UnorderedMap<NameString, AuxTimerValue>;
using AuxTimerOwnersMap = UnorderedMap<RefID, AuxTimerVarsMap>;
using AuxTimerModsMap = UnorderedMap<ModID, AuxTimerOwnersMap>;
// Ensure thread safety when modifying these globals!
extern AuxTimerModsMap s_auxTimerMapArraysPerm, s_auxTimerMapArraysTemp;

struct AuxTimerMapInfo
{
	UInt32		ownerID;
	UInt32		modIndex;
	char*		varName;
	bool		isPerm;

	AuxTimerMapInfo(TESForm* form, TESObjectREFR* thisObj, Script* scriptObj, char* pVarName)
	{
		if (!pVarName[0])
		{
			ownerID = 0;
			return;
		}
		ownerID = GetSubjectID(form, thisObj);
		if (ownerID)
		{
			varName = pVarName;
			isPerm = (varName[0] != '*');
			modIndex = (varName[!isPerm] == '_') ? 0xFF : scriptObj->GetOverridingModIdx();
		}
	}

	AuxTimerMapInfo(TESForm* form, TESObjectREFR* thisObj, Script* scriptObj, UInt8 type) :
		ownerID(GetSubjectID(form, thisObj)), varName(nullptr)
	{
		if (ownerID)
		{
			isPerm = !(type & 1);
			modIndex = (type > 1) ? 0xFF : scriptObj->GetOverridingModIdx();
		}
	}

	[[nodiscard]] AuxTimerModsMap& ModsMap() const {
		return isPerm ? s_auxTimerMapArraysPerm : s_auxTimerMapArraysTemp;
	}
};

AuxTimerValue* __fastcall ATGetValue(const AuxTimerMapInfo& varInfo, bool createIfNotFound, bool *isCreated = nullptr)
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
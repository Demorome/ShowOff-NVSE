#pragma once

#include <Utilities.h>

// Added to remove a cyclic dependency between GameForms.h and GameExtraData.h

class TESForm;
class TESFaction;

// C+?
class BSExtraData
{
public:
	BSExtraData();
	virtual ~BSExtraData();

	virtual void	Fn_01(void);

	static BSExtraData* Create(uint8_t xType, uint32_t size, uint32_t vtbl);

//	void		** _vtbl;	// 000
	uint8_t		type;		// 004
	uint8_t		pad[3];		// 005
	BSExtraData	* next;		// 008
};

// 020
struct BaseExtraList
{
	virtual ~BaseExtraList();

	BSExtraData		*m_data;					// 004
	uint8_t			m_presenceBitfield[0x15];	// 008 - if a bit is set, then the extralist should contain that extradata
	uint8_t			pad1D[3];					// 01D

	bool HasType(uint8_t type) const;
	BSExtraData *GetByType(uint8_t type) const;
	void Remove(BSExtraData *toRemove, bool doFree = false);
	void RemoveByType(uint8_t type);
	BSExtraData *Add(BSExtraData *xData);
	void RemoveAll(bool doFree = true);
	bool MarkScriptEvent(uint32_t eventMask, TESForm *eventTarget);
	void Copy(BaseExtraList *sourceList); // safe even if copying a nullptr list
	void DebugDump() const;
	bool IsWorn();
	char GetExtraFactionRank(TESFaction *faction);
	int32_t GetCount() const;
};

class BGSEncounterZone;
class TESObjectREFR;

struct ExtraDataList : public BaseExtraList
{
	ExtraDataList *CreateCopy();
	static ExtraDataList* __stdcall Create(BSExtraData *xBSData = NULL);

	BGSEncounterZone* GetEncounterZone() const {
		return ThisCall<BGSEncounterZone*>(0x421C30, this);
	}

	void SetEnableStateParent(TESObjectREFR* apParent) {
		ThisCall(0x41DA40, this, apParent);
	}

	void AddEnableStateChild(TESObjectREFR* apChild) {
		ThisCall(0x41DCD0, this, apChild);
	}

	void RemoveEnableStateChild(TESObjectREFR* apChild) {
		ThisCall(0x41DDA0, this, apChild);
	}
};

static_assert(sizeof(ExtraDataList) == 0x020);


typedef BSExtraData* (__thiscall* _GetExtraData)(const BaseExtraList*, uint8_t);
extern const _GetExtraData GetExtraData;

typedef BSExtraData* (__thiscall* _AddExtraData)(BaseExtraList*, BSExtraData*);
extern const _AddExtraData AddExtraData;

typedef void(__thiscall* _RemoveExtraData)(BaseExtraList*, BSExtraData*, bool);
extern const _RemoveExtraData RemoveExtraData;

typedef void(__thiscall* _RemoveExtraType)(BaseExtraList*, uint8_t);
extern const _RemoveExtraType RemoveExtraType;

typedef void(__thiscall* _ClearExtraDataList)(BaseExtraList*, bool);
extern const _ClearExtraDataList ClearExtraDataList;

typedef void(__thiscall* _CopyExtraDataList)(BaseExtraList*, const BaseExtraList*);
extern const _CopyExtraDataList CopyExtraDataList;
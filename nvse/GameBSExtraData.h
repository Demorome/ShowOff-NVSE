#pragma once

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

	static BSExtraData* Create(UInt8 xType, UInt32 size, UInt32 vtbl);

//	void		** _vtbl;	// 000
	UInt8		type;		// 004
	UInt8		pad[3];		// 005
	BSExtraData	* next;		// 008
};

// 020
struct BaseExtraList
{
	virtual void	Destroy(bool doFree);

	BSExtraData		*m_data;					// 004
	UInt8			m_presenceBitfield[0x15];	// 008 - if a bit is set, then the extralist should contain that extradata
	UInt8			pad1D[3];					// 01D

	bool HasType(UInt32 type) const;
	BSExtraData *GetByType(UInt32 type) const;
	void MarkType(UInt32 type, bool bCleared);
	void Remove(BSExtraData *toRemove, bool doFree = false);
	void RemoveByType(UInt32 type);
	BSExtraData *Add(BSExtraData *xData);
	void RemoveAll(bool doFree = true);
	bool MarkScriptEvent(UInt32 eventMask, TESForm *eventTarget);
	void Copy(BaseExtraList *sourceList); // safe even if copying a nullptr list
	void DebugDump() const;
	bool IsWorn();
	char GetExtraFactionRank(TESFaction *faction);
	SInt32 GetCount() const;
};

struct ExtraDataList : public BaseExtraList
{
	ExtraDataList *CreateCopy();
	static ExtraDataList* __stdcall Create(BSExtraData *xBSData = NULL);
};

STATIC_ASSERT(sizeof(ExtraDataList) == 0x020);


typedef BSExtraData* (__thiscall* _GetExtraData)(const BaseExtraList*, UInt8);
extern const _GetExtraData GetExtraData;

typedef BSExtraData* (__thiscall* _AddExtraData)(BaseExtraList*, BSExtraData*);
extern const _AddExtraData AddExtraData;

typedef void(__thiscall* _RemoveExtraData)(BaseExtraList*, BSExtraData*, bool);
extern const _RemoveExtraData RemoveExtraData;

typedef void(__thiscall* _RemoveExtraType)(BaseExtraList*, UInt8);
extern const _RemoveExtraType RemoveExtraType;

typedef void(__thiscall* _ClearExtraDataList)(BaseExtraList*, bool);
extern const _ClearExtraDataList ClearExtraDataList;

typedef void(__thiscall* _CopyExtraDataList)(BaseExtraList*, const BaseExtraList*);
extern const _CopyExtraDataList CopyExtraDataList;
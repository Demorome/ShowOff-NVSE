#include "GameBSExtraData.h"
#include "GameAPI.h"
#include "GameExtraData.h"


const _GetExtraData GetExtraData = (_GetExtraData)0x410220;
const _AddExtraData AddExtraData = (_AddExtraData)0x40FF60;
const _RemoveExtraData RemoveExtraData = (_RemoveExtraData)0x410020;
const _RemoveExtraType RemoveExtraType = (_RemoveExtraType)0x410140;
const _ClearExtraDataList ClearExtraDataList = (_ClearExtraDataList)0x40FAE0;
const _CopyExtraDataList CopyExtraDataList = (_CopyExtraDataList)0x411EC0;

bool BaseExtraList::HasType(UInt8 type) const
{
	return ThisCall<bool>(0x40FE80, this, type);
}

BSExtraData* BaseExtraList::GetByType(UInt8 type) const
{
	return ThisCall<BSExtraData*>(0x410220, this, type);
}

void BaseExtraList::Remove(BSExtraData *toRemove, bool doFree)
{
	ThisCall(0x410020, this, toRemove, doFree);
}

BSExtraData *BaseExtraList::Add(BSExtraData *xData)
{
	return ThisCall<BSExtraData*>(0x40FF60, this, xData);
}

ExtraDataList *ExtraDataList::Create(BSExtraData *xBSData)
{
	ExtraDataList *xData = (ExtraDataList*)GameHeapAlloc(sizeof(ExtraDataList));
	ThisCall(0x410360, xData);
	if (xBSData) xData->Add(xBSData);
	return xData;
}

void BaseExtraList::RemoveByType(UInt8 type)
{
	ThisCall(0x410140, this, type);
}

void BaseExtraList::RemoveAll(bool doFree)
{
	ThisCall(0x411FD0, this, doFree);
}

void BaseExtraList::Copy(BaseExtraList *sourceList)
{
	ThisCall(0x411EC0, this, sourceList);
}

bool BaseExtraList::IsWorn()
{
	return HasType(kExtraData_Worn);
}

char BaseExtraList::GetExtraFactionRank(TESFaction *faction)
{
	ExtraFactionChanges *xFactionChanges = GetExtraType((*this), FactionChanges);
	if (xFactionChanges && xFactionChanges->data)
	{
		ListNode<FactionListData> *traverse = xFactionChanges->data->Head();
		FactionListData *pData;
		do
		{
			pData = traverse->data;
			if (pData && (pData->faction == faction))
				return pData->rank;
		}
		while (traverse = traverse->next);
	}
	return -1;
}

void BaseExtraList::DebugDump() const
{
	_MESSAGE("\nBaseExtraList Dump:");
	Console_Print("BaseExtraList Dump:");
	g_Log.Indent();
	if (m_data)
	{
		for (BSExtraData *traverse = m_data; traverse; traverse = traverse->next)
		{
			_MESSAGE("%08X\t%02X\t%s\t%08X", traverse, traverse->type, GetExtraDataName(traverse->type), ((UInt32*)traverse)[3]);
			Console_Print("%08X  %02X  %s  %08X", traverse, traverse->type, GetExtraDataName(traverse->type), ((UInt32*)traverse)[3]);
		}
		Console_Print(" ");
	}
	else
	{
		_MESSAGE("No data in list");
		Console_Print("No data in list");
	}
	g_Log.Outdent();
}

bool BaseExtraList::MarkScriptEvent(UInt32 eventMask, TESForm* eventTarget)
{
	return MarkBaseExtraListScriptEvent(eventTarget, this, eventMask);
}


SInt32 BaseExtraList::GetCount() const {
	return ThisCall<int16_t>(0x418770, this);
}
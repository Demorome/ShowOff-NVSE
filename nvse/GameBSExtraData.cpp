#include "GameBSExtraData.h"
#include "GameAPI.h"
#include "GameExtraData.h"


const _GetExtraData GetExtraData = (_GetExtraData)0x410220;
const _AddExtraData AddExtraData = (_AddExtraData)0x40FF60;
const _RemoveExtraData RemoveExtraData = (_RemoveExtraData)0x410020;
const _RemoveExtraType RemoveExtraType = (_RemoveExtraType)0x410140;
const _ClearExtraDataList ClearExtraDataList = (_ClearExtraDataList)0x40FAE0;
const _CopyExtraDataList CopyExtraDataList = (_CopyExtraDataList)0x411EC0;

#if RUNTIME
static const UInt32 s_ExtraDataListVtbl							= 0x010143E8;	//	0x0100e3a8;
#endif

bool BaseExtraList::HasType(UInt32 type) const
{
	return (m_presenceBitfield[type >> 3] & (1 << (type & 7))) != 0;
}

void BaseExtraList::MarkType(UInt32 type, bool bCleared)
{
	UInt8 bitMask = 1 << (type & 7);
	UInt8 &flag = m_presenceBitfield[type >> 3];
	if (bCleared) flag &= ~bitMask;
	else flag |= bitMask;
}

ExtraDataList* ExtraDataList::Create(BSExtraData* xBSData)
{
	ExtraDataList* xData = (ExtraDataList*)FormHeap_Allocate(sizeof(ExtraDataList));
	memset(xData, 0, sizeof(ExtraDataList));
	((UInt32*)xData)[0] = s_ExtraDataListVtbl;
	if (xBSData)
		xData->Add(xBSData);
	return xData;
}

bool BaseExtraList::IsWorn()
{
	return HasType(kExtraData_Worn);
}

void BaseExtraList::DebugDump() const
{
	_MESSAGE("BaseExtraList Dump:");
	gLog.Indent();

	if (m_data)
	{
		for(BSExtraData * traverse = m_data; traverse; traverse = traverse->next) {
			_MESSAGE("%s", GetObjectClassName(traverse));
			//_MESSAGE("Extra types %4x (%s) %s", traverse->type, GetExtraDataName(traverse->type), GetExtraDataValue(traverse));
		}
	}
	else
		_MESSAGE("No data in list");

	gLog.Outdent();
}

bool BaseExtraList::MarkScriptEvent(UInt32 eventMask, TESForm* eventTarget)
{
	return MarkBaseExtraListScriptEvent(eventTarget, this, eventMask);
}

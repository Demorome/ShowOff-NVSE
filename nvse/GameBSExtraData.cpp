#include "GameBSExtraData.h"
#include "GameExtraData.h"

const _GetExtraData GetExtraData = (_GetExtraData)0x410220;
const _AddExtraData AddExtraData = (_AddExtraData)0x40FF60;
const _RemoveExtraType RemoveExtraType = (_RemoveExtraType)0x410140;

bool BaseExtraList::HasType(uint8_t type) const
{
	return ThisCall<bool>(0x40FE80, this, type);
}

BSExtraData* BaseExtraList::GetByType(uint8_t type) const
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

ExtraDataList *ExtraDataList::Create()
{
	ExtraDataList *xData = (ExtraDataList*)GameHeapAlloc(sizeof(ExtraDataList));
	ThisCall(0x410360, xData);
	return xData;
}

void BaseExtraList::RemoveByType(uint8_t type)
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

int32_t BaseExtraList::GetCount() const {
	return ThisCall<int16_t>(0x418770, this);
}
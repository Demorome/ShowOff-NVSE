#include "GameData.h"


TESDataHandler* TESDataHandler::GetSingleton() {
	return *reinterpret_cast<TESDataHandler**>(0x11C3F2C);
}

const ModInfo* TESDataHandler::GetFile(uint32_t auiIndex) const {
	return ThisCall<ModInfo*>(0x465010, this, auiIndex);
}
const ModInfo * TESDataHandler::GetListFile(const char * modName) {
	return ThisCall<ModInfo*>(0x462F40, this, modName);
}

UInt8 TESDataHandler::GetModIndex(const char *modName)
{
	const ModInfo* pFile = GetListFile(modName);
	return pFile ? pFile->modIndex : 0xFF;
}

const char* TESDataHandler::GetNthModName(UInt32 modIndex)
{
	const ModInfo* pFile = GetFile(modIndex);
	return pFile ? pFile->name : "";
}
UInt8 TESDataHandler::GetActiveModCount() const
{
	return modList.modInfoList.Count();
}

ModInfo::ModInfo() {
	//
};

ModInfo::~ModInfo() {
	//
};

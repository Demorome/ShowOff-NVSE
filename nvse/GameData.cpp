#include "GameData.h"


TESDataHandler* TESDataHandler::GetSingleton() {
	return *reinterpret_cast<TESDataHandler**>(0x11C3F2C);
}

const TESFile* TESDataHandler::GetFile(uint32_t auiIndex) const {
	return ThisCall<TESFile*>(0x465010, this, auiIndex);
}

const TESFile * TESDataHandler::GetListFile(const char * modName) const {
	return ThisCall<TESFile*>(0x462F40, this, modName);
}

const TESFile* TESDataHandler::GetFileByFormID(uint32_t auiFormID) const {
	uint32_t uiFileIndex = (auiFormID >> 24) & 0xFF;
	if (SupportsSmallPugins() && uiFileIndex == 0xFE)
		uiFileIndex = auiFormID;

	return GetFile(uiFileIndex);
}

uint8_t TESDataHandler::GetModIndex(const char *modName) const {
	const TESFile* pFile = GetListFile(modName);
	return pFile ? pFile->modIndex : 0xFF;
}

uint8_t TESDataHandler::GetActiveModCount() const
{
	return modInfoList.Count();
}
#include "GameData.h"


TESDataHandler* TESDataHandler::GetSingleton() {
	return *reinterpret_cast<TESDataHandler**>(0x11C3F2C);
}

class LoadedModFinder
{
	const char * m_stringToFind;

public:
	LoadedModFinder(const char * str) : m_stringToFind(str) { }

	bool Accept(ModInfo* modInfo)
	{
		return _stricmp(modInfo->name, m_stringToFind) == 0;
	}
};

const ModInfo * TESDataHandler::LookupModByName(const char * modName)
{
	LoadedModFinder tmp(modName);
	return modList.modInfoList.Find(tmp);
}

const ModInfo ** TESDataHandler::GetActiveModList()
{
	static const ModInfo* activeModList[0x100] = { 0 };

	if (!(*activeModList))
	{
		UInt16 index = 0;
		for (index = 0  ; index < TESDataHandler::GetSingleton()->modList.modInfoList.Count() ; index++)
		{
			ModInfo* entry = TESDataHandler::GetSingleton()->modList.modInfoList.GetNthItem(index);
			if (entry->IsLoaded())
				activeModList[index] = entry;
		}
	}

	return activeModList;
}

UInt8 TESDataHandler::GetModIndex(const char *modName)
{
	ListNode<ModInfo> *iter = modList.modInfoList.Head();
	ModInfo *modInfo;
	do
	{
		modInfo = iter->data;
		if (modInfo && StrEqualCI(modInfo->name, modName))
			return modInfo->modIndex;
	}
	while (iter = iter->next);
	return 0xFF;
}

const char* TESDataHandler::GetNthModName(UInt32 modIndex)
{
	const ModInfo** activeModList = GetActiveModList();
	if (modIndex < GetActiveModCount() && activeModList[modIndex])
		return activeModList[modIndex]->name;
	else
		return "";
}

struct IsModLoaded
{
	bool Accept(ModInfo* pModInfo) const {
		return pModInfo->IsLoaded();
	}
};

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

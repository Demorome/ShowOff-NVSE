#include "GameTasks.h"

ModelLoader *ModelLoader::GetSingleton()
{
	return *(ModelLoader**)0x011C3B3C;
}

void ModelLoader::QueueReference(TESObjectREFR *refr, uint32_t arg2, uint32_t arg3)
{
	ThisCall(0x444850, this, refr, arg2, arg3);
}
#include "GameAPI.h"

static NVSEStringVarInterface* s_StringVarInterface = NULL;

// arg1 = 1, ignored if canCreateNew is false, passed to 'init' function if a new object is created
typedef void * (* _GetSingleton)(bool canCreateNew);

const _ExtractArgs ExtractArgs = (_ExtractArgs)0x005ACCB0;

const _FormHeap_Allocate FormHeap_Allocate = (_FormHeap_Allocate)0x00401000;
const _FormHeap_Free FormHeap_Free = (_FormHeap_Free)0x00401030;

const _GetSingleton ConsoleManager_GetSingleton = (_GetSingleton)0x0071B160;

const _QueueUIMessage QueueUIMessage = (_QueueUIMessage)0x007052F0;

const _ShowMessageBox ShowMessageBox = (_ShowMessageBox)0x00703E80;
const _ShowMessageBox_Callback ShowMessageBox_Callback = (_ShowMessageBox_Callback)0x005B4A70;
const _ShowMessageBox_pScriptRefID ShowMessageBox_pScriptRefID = (_ShowMessageBox_pScriptRefID)0x011CAC64;
const _ShowMessageBox_button ShowMessageBox_button = (_ShowMessageBox_button)0x0118C684;

const _GetActorValueEditorIDName GetActorValueEditorIDName = (_GetActorValueEditorIDName)0x66EAC0;	// See Cmd_GetActorValue_Eval
const _GetActorValueLocalizedName GetActorValueShortLocalizedName = (_GetActorValueLocalizedName)0x66EB00;

#if RUNTIME

struct TLSData
{
	// thread local storage

	uint32_t	pad000[(0x260 - 0x000) >> 2];	// 000
	NiNode			* lastNiNode;			// 260
	TESObjectREFR	* lastNiNodeREFR;		// 264
	uint8_t			consoleMode;			// 268
	uint8_t			pad269[3];				// 269
	// 25C is used as do not head track the player , 2B8 is used to init QueudFile::unk0018
};

struct _TEB {
	uint32_t		 padding[0x2C / 4];
	struct TLSData** ThreadLocalStoragePointer;
};

TLSData* GetTLSData() {
	return NtCurrentTeb()->ThreadLocalStoragePointer[*reinterpret_cast<uint32_t*>(0x126FD98)];
}

bool IsConsoleMode()
{
	TLSData * tlsData = GetTLSData();

	if(tlsData)
		return tlsData->consoleMode != 0;

	return false;
}

const char * GetFullName(TESForm * baseForm)
{
	if(baseForm)
	{
		TESFullName* fullName = baseForm->GetFullName();
		if(fullName && fullName->name.m_data)
		{
			if (fullName->name.m_dataLen)
				return fullName->name.m_data;
		}
	}

	return "<no name>";
}

ConsoleManager * ConsoleManager::GetSingleton(void)
{
	return (ConsoleManager *)ConsoleManager_GetSingleton(true);
}

void Console_Print(const char * fmt, ...)  //use Console_Print_Long if strlen(char*) >= 512
{
	ConsoleManager	* mgr = ConsoleManager::GetSingleton();
	if(mgr)
	{
		va_list	args;

		va_start(args, fmt);

		CALL_MEMBER_FN(mgr, Print)(fmt, args);

		va_end(args);
	}
}

ScriptVar *ScriptEventList::GetVariable(uint32_t id)
{
	if (m_vars)
	{
		ListNode<ScriptVar> *varIter = m_vars->Head();
		ScriptVar *scriptVar;
		do
		{
			scriptVar = varIter->data;
			if (scriptVar && (scriptVar->id == id))
				return scriptVar;
		}
		while ((varIter = varIter->next));
	}
	return NULL;
}
#endif

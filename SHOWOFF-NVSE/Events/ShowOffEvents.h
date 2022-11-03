#pragma once
#include "EventParams.h"
#include "GameEffects.h"

// Credits to Karut (from JohnnyGuitar) for making the Event Framework.
EventInformation* OnCornerMessage;
EventInformation* OnActorValueChange;

DEFINE_COMMAND_ALT_PLUGIN(SetShowOffOnCornerMessageEventHandler, SetOnCornerMessageEventHandler, "", false, kParams_Event);
bool Cmd_SetShowOffOnCornerMessageEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove;
	Script* script;
	UInt32 flags = 0;  //reserved for future use
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags) || NOT_TYPE(script, Script))) return true;
	if (OnCornerMessage)
	{
		if (setOrRemove)
			OnCornerMessage->RegisterEvent(script, NULL);
		else OnCornerMessage->RemoveEvent(script, NULL);
	}
	return true;
}

#if _DEBUG
DEFINE_COMMAND_ALT_PLUGIN(SetShowOffOnActorValueChangeEventHandler, SetOnAVChangeEventHandler, "", false, kParams_Event);	//TODO: change args
bool Cmd_SetShowOffOnActorValueChangeEventHandler_Execute(COMMAND_ARGS)
{
	UInt32 setOrRemove;
	Script* script;
	UInt32 flags = 0;  //reserved for future use
	//todo: extract AV code filter, positive or negative change bool filter
	if (!(ExtractArgsEx(EXTRACT_ARGS_EX, &setOrRemove, &script, &flags) || NOT_TYPE(script, Script))) return true;
	if (OnActorValueChange)
	{
		//TODO: Add filters
		if (setOrRemove)
			OnActorValueChange->RegisterEvent(script, NULL);
		else OnActorValueChange->RemoveEvent(script, NULL);
	}
	return true;
}
#endif

namespace CornerMessageHooks
{
	// iconPath and soundPath can be null.
	bool __fastcall CornerMessageEventHook(HUDMainMenu* menu, void* edx, char* msgText, eEmotion IconType, char* iconPath, char* soundPath, float displayTime, bool instantEndCurrentMessage)
	{
		bool bFireEvent = true;
		
		// Avoid throwing exceptions with bad char*
		std::string msgTextStr, iconPathStr, soundPathStr;
		msgTextStr = iconPathStr = soundPathStr = "<void>";
		if (msgText && msgText[0]) msgTextStr = msgText;
		else bFireEvent = false;
		if (iconPath && iconPath[0]) iconPathStr = iconPath;
		if (soundPath && soundPath[0]) soundPathStr = soundPath;

		if (bFireEvent) {
			for (auto const& callback : OnCornerMessage->EventCallbacks) {
				FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnCornerMessage->numMaxArgs, msgTextStr.c_str(), IconType, iconPathStr.c_str(), soundPathStr.c_str(), *(UInt32*)&displayTime);
			}
		}
#if false
		Console_Print("==CornerMessageEventHook==\n -msgText: %s\n -IconType: %d\n -iconPath: %s\n -soundPath: %s\n -displayTime: %f\n -instantEndCurrentMessage: %d", msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
#endif
		return ThisStdCall_B(0x775380, menu, msgText, IconType, iconPath, soundPath, displayTime, instantEndCurrentMessage);
	}
	
	void WriteDelayedHook()
	{
		// SPECIAL CASE: 0x8B959B is Nop'd by Tweaks if bNoCrippleCriticalMessages = 1 && g_bPatchCripple = 1.
		auto const tweakConflictAddr = 0x8B959B;
		auto const vanillaDerefAddr = 0xEBBDE0E8;

		// TODO: write a RelJump inside the function itself for compatibility.
		
		std::vector cornerMessageHookJmpSrc{ 0x705379, 0x7EE74D, 0x7EE87D, 0x7EEA6C, 0x833303, tweakConflictAddr };

		// Credits for hook collision detection method goes to lStewieAl.
		if (*(UInt32*)tweakConflictAddr != vanillaDerefAddr)
		{
			cornerMessageHookJmpSrc.erase(
				std::find(cornerMessageHookJmpSrc.begin(), cornerMessageHookJmpSrc.end(), tweakConflictAddr)
			);
			_MESSAGE("Conflict detected with (presumably) Tweaks, preventing hook from being installed at %x", tweakConflictAddr);
		}
		
		for (auto const jmpSrc : cornerMessageHookJmpSrc)
		{
			WriteRelCall(jmpSrc, (UInt32)CornerMessageEventHook);
		}
	}
}

#if 0
namespace ActorValueChangeHooks
{
	void __fastcall ActorValueChange_EventHandler(ActorValueOwner *avOwner, int avCode, float previousVal, float newVal, ActorValueOwner *attackerAvOwner)
	{
		// todo: get forms from avOwners, pass to UDF
		
		for (auto const& callback : OnActorValueChange->EventCallbacks) 
		{
			FunctionCallScriptAlt(callback.ScriptForEvent, nullptr, OnActorValueChange->numMaxArgs, *(UInt32*)&previousVal, *(UInt32*)&newVal);
		}
		
#if _DEBUG
		Console_Print("==ActorValueChangeHook==\n");
#endif
	}

	__declspec(naked) void HandleAVChangeHook()
	{
		static const UInt32 overwrittenFuncAddr = 0x406D70;
		
		enum Offsets
		{
			avCode = 0xC,
			affectedAvOwner = 0x8,
			attackerAvOwner = 0x18,
			newVal = 0x14,
			previousVal = 0x10,
		};

		_asm
		{
			push    ebp
			mov     ebp, esp

			
			// Call GetActorValueInfo (what was overwritten)
			// AV code was pushed on stack right before this.
			call overwrittenFuncAddr
			// result is now in eax, which will not be affected before being checked at 0x66EE60.

			
			// Set up to call ActorValueChange_EventHandler
			/*
			mov ecx, [ebp + affectedAvOwner]
			mov edx, [ebp + avCode]

			// Is pushing directly a bad thing?
			push [ebp + affectedAvOwner]
			push [ebp + newVal]
			push [ebp + previousVal]
			
			call ActorValueChange_EventHandler
			*/
			// Function is __fastcall, so no need to clean up the args.

			
			mov     esp, ebp
			pop     ebp
			// Pop return address from the stack (given by Call).
			ret
		}
	}
	
	void WriteHook()
	{
		// Replace "GetActorValueInfo(avCode)" call
		WriteRelCall(0x66EE58, (UInt32)HandleAVChangeHook);
	}
}
#endif

namespace OnPreActivate
{
	constexpr char eventName[] = "ShowOff:OnPreActivate";

	bool __fastcall HandleEvent(TESObjectREFR* activated, Actor* activator)
	{
		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAdrr) -> bool
		{
			if (UInt32 &shouldActivate = *static_cast<UInt32*>(shouldActivateAdrr))
			{
				if (result.IsValid())
					shouldActivate = result.Bool();
			}
			return true;
		};
		UInt32 shouldActivate = true;
		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldActivate, activated, activator, &shouldActivate);

#if _DEBUG
		Console_Print("OnActivate HOOK - Activated: [%08X] {%s} (%s), activator: [%08X]", activated ? activated->refID : 0, 
			activated ? activated->GetName() : "",
			activated ? activated->GetTheName() : "",
			activator ? activator->refID : 0);
#endif
		return shouldActivate != 0;
	}	//result in AL

	__declspec(naked) void Hook()
	{
		static UInt32 const retnAddr = 0x57334C, getBaseForm = 0x7AF430,
			retnFalse = 0x573396;
		enum
		{
			activator = 8
		};
		_asm
		{
			//Check if this instance of TESObjectREFR::Activate was called by Activate func.
			//(We don't want the event to run for that)
			mov eax, dword ptr [ebp + 4]	//rtn addr
			cmp eax, 0x5B5B1D	//one of the return addresses to Activate_Execute
			je doNormal
			cmp eax, 0x5B5B4D
			je doNormal

			pushad	//unknown what the __fastcall function will preserve, so store everything.
			mov edx, dword ptr [ebp + activator]
			call HandleEvent
			test al, al
			popad
			jnz doNormal
			jmp retnFalse

			doNormal:
			call getBaseForm	
			jmp retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x573347, (UInt32)Hook);
	}
}

static const UInt32 g_inventoryMenuSelectionAddr = 0x11D9EA8;

namespace PreActivateInventoryItem
{
	constexpr char eventName[] = "ShowOff:OnPreActivateInventoryItem";

	bool __fastcall CanUseItem(ContChangesEntry* itemEntry, void* edx, bool isHotkeyUse)
	{
		if (!itemEntry || !itemEntry->type)
			return false;

		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldActivateAdrr) -> bool
		{
			if (UInt32& shouldActivate = *static_cast<UInt32*>(shouldActivateAdrr))
			{
				if (result.IsValid())
					shouldActivate = result.Bool();
			}
			return true;
		};
		UInt32 shouldActivate = true;
		auto const itemForm = itemEntry->type;
		auto* invRef = CreateRefForStack(g_thePlayer, itemEntry);

		UInt32 selectedHotkey = 0;
		if (isHotkeyUse)
		{
			auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
			auto const hotkeyData = *reinterpret_cast<HotKeyWheel**>(ebp - 0x1C);
			selectedHotkey = hotkeyData->selectedHotkey + 1;
		}

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldActivate, 
			g_thePlayer, itemForm, invRef, &shouldActivate, selectedHotkey);

		if (g_ShowFuncDebug)
			Console_Print("CanActivateItemHook: CanActivate: %i, Item: [%08X], %s, type: %u", shouldActivate, itemForm->refID, itemForm->GetName(), itemForm->typeID);

		return shouldActivate;
	}

	__declspec(naked) void HookOnClickAmmo()
	{
		_asm
		{
			//ecx = g_inventoryMenuSelection
			mov		eax, [ecx + 8]
			test	eax, eax
			jz		Done

			push	0
			call	CanUseItem // register stomping should be fine.
			test	al, al
			jz		PreventActivation

			mov		ecx, g_inventoryMenuSelectionAddr
			mov		ecx, dword ptr ds : [ecx]
			mov		eax, [ecx + 8]
			ret

			PreventActivation :
			mov		eax, 0
			Done :
			ret
		}
	}

	__declspec(naked) void HookHandleOnClickEquipOrUnEquip()
	{
		static const UInt32 endHandleClick = 0x780B8E,
			HandleEquipOrUnEquip = 0x780D60;

		_asm
		{
			//ecx = InventoryMenu
			mov		ecx, g_inventoryMenuSelectionAddr
			mov		ecx, dword ptr ds : [ecx]
			push	0
			call	CanUseItem // register stomping should be fine.
			test	al, al
			jz		PreventActivation

			mov		ecx, [ebp-0x64] 
			call	HandleEquipOrUnEquip 
			ret

			PreventActivation :
			add		esp, 4	// remove return addr that was pushed.
			jmp		endHandleClick
		}
	}

	__declspec(naked) void HookHandleHotkeyEquipOrUnEquip()
	{
		static const UInt32 normalRetnAdrr = 0x701FB3,
			endFunctionAddr = 0x702130;
		_asm
		{
			//ecx = ContChangesEntry
			push	1
			call	CanUseItem //register stomping should be fine
			test	al, al
			jz		PreventActivation

			mov		ecx, [ebp - 0xC]  //in case ecx got stomped, restore it
			mov		eax, [ecx + 8]
			jmp		normalRetnAdrr

			PreventActivation :
			jmp		endFunctionAddr
		}
	}

	void WriteHooks()
	{
		// Replace "call InventoryMenu::HandleEquipOrUnEquip"
		WriteRelCall(0x7805CC, (UInt32)HookHandleOnClickEquipOrUnEquip);

		// Replace "call TESForm::GetFlags(g_inventoryMenuSelection)"
		WriteRelCall(0x780648, (UInt32)HookOnClickAmmo);

		// Replace "call TESForm::GetFlags(entry)"
		WriteRelJump(0x701FAE, (UInt32)HookHandleHotkeyEquipOrUnEquip);
	}
}

namespace PreDropInventoryItem
{
	constexpr char eventName[] = "ShowOff:OnPreDropInventoryItem";

	bool HandleEvent()
	{
		auto constexpr resultCallback = [](NVSEArrayVarInterface::Element& result, void* shouldDropAddr) -> bool
		{
			if (UInt32& shouldDrop = *static_cast<UInt32*>(shouldDropAddr))
				if (result.IsValid())
					shouldDrop = result.Bool();
			return true;
		};
		UInt32 shouldDrop = true;

		auto const itemEntry = *reinterpret_cast<ContChangesEntry**>(g_inventoryMenuSelectionAddr);
		auto const itemForm = itemEntry->type;
		auto* invRef = CreateRefForStack(g_thePlayer, itemEntry);

		g_eventInterface->DispatchEventAlt(eventName, resultCallback, &shouldDrop,
			g_thePlayer, itemForm, invRef, &shouldDrop);

		return shouldDrop;
	}

	__declspec(naked) void Hook()
	{
		static UInt32 const IsEnoughRoomNearPlayerToDropItem = 0x9614B0,
			NotEnoughRoomAddr = 0x780AD2, CancelledByEventAddr = 0x780B8E,
			DropItemAddr = 0x780AFC;
		_asm
		{
			call	IsEnoughRoomNearPlayerToDropItem
			movzx	ecx, al
			test	ecx, ecx
			jz		NotEnoughRoom

			call	HandleEvent
			movzx	ecx, al
			test	ecx, ecx
			jz		CancelledByEvent

			jmp		DropItemAddr

			NotEnoughRoom :
			jmp		NotEnoughRoomAddr

			CancelledByEvent :  // skip showing the "not enough room" UI message.
			jmp		CancelledByEventAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x780AC6, (UInt32)Hook);
	}
}

namespace OnQuestAdded
{
	constexpr char eventName[] = "ShowOff:OnQuestAdded";

	void __cdecl handleQuestUpdateMessage(TESQuest* quest)
	{
		g_eventInterface->DispatchEvent(eventName, nullptr, quest);
		CdeclCall(0x77A480, quest);
	}

	void WriteHook()
	{
		WriteRelCall(0x5EC66E, (UInt32)handleQuestUpdateMessage);
	}
}

namespace OnCalculateSellPrice
{
	constexpr char eventNameAdd[] = "ShowOff:OnCalculateSellPrice:+";
	constexpr char eventNameSub[] = "ShowOff:OnCalculateSellPrice:-";
	constexpr char eventNameMult[] = "ShowOff:OnCalculateSellPrice:*";

	void HandleEvent(float& newPrice, ContChangesEntry* itemEntry)
	{
		auto* baseItem = itemEntry->type;
		auto* invRef = CreateRefForStack(g_thePlayer, itemEntry);

		auto constexpr multCallback = [](NVSEArrayVarInterface::Element& result, void* newPriceAddr) -> bool
		{
			if (result.type != NVSEArrayVarInterface::kType_Numeric) [[unlikely]]
				return true;
			float& newPrice = *static_cast<float*>(newPriceAddr);
			newPrice *= result.Number();
			return true;
		};
		g_eventInterface->DispatchEventAlt(eventNameMult, multCallback, &newPrice, g_thePlayer, baseItem, invRef);

		auto constexpr addCallback = [](NVSEArrayVarInterface::Element& result, void* newPriceAddr) -> bool
		{
			if (result.type != NVSEArrayVarInterface::kType_Numeric) [[unlikely]]
				return true;
			float& newPrice = *static_cast<float*>(newPriceAddr);
			newPrice += result.Number();
			return true;
		};
		g_eventInterface->DispatchEventAlt(eventNameAdd, addCallback, &newPrice, g_thePlayer, baseItem, invRef);

		auto constexpr subCallback = [](NVSEArrayVarInterface::Element& result, void* newPriceAddr) -> bool
		{
			if (result.type != NVSEArrayVarInterface::kType_Numeric) [[unlikely]]
				return true;
			float& newPrice = *static_cast<float*>(newPriceAddr);
			newPrice -= result.Number();
			return true;
		};
		g_eventInterface->DispatchEventAlt(eventNameSub, subCallback, &newPrice, g_thePlayer, baseItem, invRef);

#if _DEBUG && 0
		Console_Print("== ShowOff:OnCalculateSellPrice - newPrice: %f, itemEditorID: %s ==", 
			newPrice, itemEntry->type->GetName());
#endif
	}

	// Recalculate sell price of an item.
	static double __cdecl HookFAbs(float price)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());  //credits to Kormakur for this trick.

		auto& newPrice = *reinterpret_cast<float*>(ebp - 0xC);
		auto* itemEntry = *reinterpret_cast<ContChangesEntry**>(ebp + 0xC);
		HandleEvent(newPrice, itemEntry);

		return fabs(static_cast<double>(newPrice));
	}

	void WriteHook()
	{
		WriteRelCall(0x72EFFE, (UInt32)HookFAbs);
	}
}

namespace OnProjectileDestroy
{
	constexpr char eventName[] = "ShowOff:OnProjectileDestroy";

	void __fastcall HandleEvent(Projectile* proj)
	{
		g_eventInterface->DispatchEvent(eventName, proj, proj->sourceRef, proj->sourceWeap);
	}

	void __declspec(naked) Projectile_Free_Hook()
	{
		static UInt32 const retnAddr = 0x9BC489;
		_asm
		{
			push	ecx
			call	HandleEvent  // ecx should still be the Projectile (this)
			pop		ecx
			mov		eax, dword ptr ds : [0x11C16BC]
			jmp		retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x9BC484, (UInt32)Projectile_Free_Hook);
	}
}
namespace OnProjectileCreate
{
	constexpr char eventName[] = "ShowOff:OnProjectileCreate";

	void __fastcall HandleEvent(Projectile* proj, Actor* projOwner, TESObjectWEAP* weapon)
	{
		g_eventInterface->DispatchEvent(eventName, proj, projOwner, weapon);
	}

	void __declspec(naked) CreateProjectile_Hook()
	{
		static UInt32 const retnAddr = 0x9BD523;
		_asm
		{
			mov		ecx, [ebp - 0x14] // liveProjectile (arg1)
			mov		edx, [ebp + 0xC] // actor (arg2)
			mov		esi, [ebp + 0x14] // weapon (arg3)
			push	esi
			call	HandleEvent

			// do the regular code
			mov		eax, [ebp - 0x14]
			mov		ecx, [ebp - 0xC]
			jmp		retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x9BD51D, (UInt32)CreateProjectile_Hook);
	}
}
namespace OnProjectileImpact
{
	constexpr char eventName[] = "ShowOff:OnProjectileImpact";

	void __fastcall HandleEvent(Projectile* proj, void* edx)
	{
		proj->hasImpacted = true; //was gonna be set to true later, anyways.

		if (static_cast<BGSProjectile*>(proj->baseForm)->type != 2)  //if not Lobber-type
		{
			// Copying JIP code for ProjectileImpactHook
			// Updating the proj's position to its impactPos.
			// Likely done to make "GetPos" calls accurate inside the event handler.
			if (auto const impactData = proj->impactDataList.Head()->data)
				*proj->PosVector() = impactData->pos;
		}
		g_eventInterface->DispatchEvent(eventName, proj, proj->sourceRef, proj->sourceWeap, proj->GetImpactRef());
	}

	static const UInt32 loopEndAddr = 0x9C20BF;
	void __declspec(naked) HookLoopCheck()
	{
		static const UInt32 continueLoopAddr = 0x9C1BD6;
		__asm
		{
			movzx	edx, al
			test	edx, edx
			jz		continueLoop
			// else, loop is ending, call event.
			mov     ecx, [ebp - 0x58]  // proj
			call	HandleEvent
			jmp		loopEndAddr

			continueLoop:
			jmp		continueLoopAddr
		}
	}

	void __declspec(naked) HookJz()
	{
		static const UInt32 keepLoopGoingAddr = 0x9C1BE0;
		__asm
		{
			jnz		keepLoopGoing

			mov     ecx, [ebp - 0x58]  // proj
			call	HandleEvent
			jmp		loopEndAddr

			keepLoopGoing:
			jmp		keepLoopGoingAddr
		}
	}

	void __declspec(naked) HookJnz()
	{
		static const UInt32 keepLoopGoingAddr = 0x9C1BF3;
		__asm
		{
			jz		keepLoopGoing

			mov     ecx, [ebp - 0x58]  // proj
			call	HandleEvent
			jmp		loopEndAddr

			keepLoopGoing :
			jmp		keepLoopGoingAddr
		}
	}

	void WriteHooks()
	{
		//Don't hook 0x9C20BF, because JIP does so already.
		WriteRelJump(0x9C20B1, (UInt32)HookLoopCheck);
		WriteRelJump(0x9C1BDA, (UInt32)HookJz);
		WriteRelJump(0x9C1BED, (UInt32)HookJnz);
	}
}

namespace OnLockpickMenuClose
{
	constexpr char eventName[] = "ShowOff:OnLockpickMenuClose";

	enum class CloseReason : UInt32
	{
		kLockOpened = 0,
		kLockForceBroken,
		kManualExit
	};

	void HandleEvent(const LockPickMenu* menu, CloseReason reason)
	{
		g_eventInterface->DispatchEvent(eventName, menu->targetRef, reason);
	}

	void __cdecl ForceFailHook()
	{
		auto const menu = LockPickMenu::GetSingleton();
		menu->stage = 7;
		HandleEvent(menu, CloseReason::kLockForceBroken);
	}

	void __cdecl ManualExitHook()
	{
		auto const menu = LockPickMenu::GetSingleton();
		menu->stage = 7;
		HandleEvent(menu, CloseReason::kManualExit);
	}

	void __cdecl OpenLockHook()
	{
		auto const menu = LockPickMenu::GetSingleton();
		menu->stage = 7;
		HandleEvent(menu, CloseReason::kLockOpened);
	}

	void WriteHooks()
	{
		WriteRelCall(0x790383, (UInt32)ManualExitHook);
		WriteRelCall(0x78F97B, (UInt32)OpenLockHook);
		WriteRelCall(0x7904AF, (UInt32)ForceFailHook);
	}
}

namespace OnQueueCornerMessage
{
	constexpr char eventName[] = "ShowOff:OnQueueCornerMessage";

	// Warning: msgText is not fully formatted, i.e. can have "&sUActnVats" and other formatting tricks.
	void DispatchEvent(HUDMainMenu::QueuedMessage* msg)
	{
		void* displayTime = *(void**)&msg->displayTime;
		g_eventInterface->DispatchEvent(eventName, nullptr, msg->msgText, 
			msg->iconPath, msg->soundPath, displayTime);
	}

	void __fastcall tListAppendHook(tList<HUDMainMenu::QueuedMessage>* msgList, void* edx, 
		HUDMainMenu::QueuedMessage** msg)
	{
		// Run our code
		DispatchEvent(*msg);

		// Regular code
		ThisStdCall(0x905820, msgList, msg);
	}

	void __fastcall tListInsertHook(tList<HUDMainMenu::QueuedMessage>* msgList, void* edx,
		HUDMainMenu::QueuedMessage** msg)
	{
		// Run our code
		DispatchEvent(*msg);

		// Regular code
		ThisStdCall(0x5AE3D0, msgList, msg);
	}

	void WriteHooks()
	{
		WriteRelCall(0x7754FA, (UInt32)tListAppendHook);
		WriteRelCall(0x775624, (UInt32)tListAppendHook);
		WriteRelCall(0x775610, (UInt32)tListInsertHook);
	}
}

namespace OnShowCornerMessage
{
	constexpr char eventName[] = "ShowOff:OnShowCornerMessage";

	std::string g_msgText, g_iconPath, g_soundPath;

	// Warning: msgText is not fully formatted, i.e. can have "&sUActnVats" and other formatting tricks.
	void DispatchEvent(HUDMainMenu::QueuedMessage* msg)
	{
		auto constexpr postCallback = [](void* nothing, NVSEEventManagerInterface::DispatchReturn retn)
		{
			g_msgText.clear();
			g_iconPath.clear();
			g_soundPath.clear();
		};

		// Keep strings alive in case of dispatch deferral.
		g_msgText = msg->msgText;
		g_iconPath = msg->iconPath;
		g_soundPath = msg->soundPath;

		void* displayTime = *(void**)&msg->displayTime;
		// Event can run outside main thread; defer dispatch in that case.
		g_eventInterface->DispatchEventThreadSafe(eventName, postCallback, nullptr, g_msgText.c_str(),
			g_iconPath.c_str(), g_soundPath.c_str(), displayTime);
	}

	tList<HUDMainMenu::QueuedMessage>* __fastcall
		thisHook(tList<HUDMainMenu::QueuedMessage>* msgList)
	{
		// Our code
		DispatchEvent(msgList->Head()->data);

		// regular code
		return msgList;
	}

	void WriteHooks()
	{
		WriteRelCall(0x7757F9, (UInt32)thisHook);
		WriteRelCall(0x77550A, (UInt32)thisHook);
	}
}

namespace OnFireWeapon
{
	constexpr char eventName[] = "ShowOff:OnFireWeapon";

	// Runs before ModifyChanceForAmmoItem perk entry point can add items.
	// Runs before the weapon is damaged from firing.
	// 
	void __fastcall DispatchEvent(Actor* actor, TESObjectWEAP* weap)
	{
		g_eventInterface->DispatchEvent(eventName, actor, weap);
	}

	void __declspec(naked) Hook()
	{
		UInt32 static const retnAddr = 0x523199;
		__asm
		{
			// regular code
			mov		eax, [edx + 0x100]
			call	eax
			push	eax  // store result

			// Our code
			mov		ecx, [ebp + 0x8] //actor
			mov		edx, [ebp - 0x238] //weapon
			call	DispatchEvent

			// back to regular
			pop		eax  // restore result
			jmp		retnAddr
		}
	}

	void WriteHook()
	{
		WriteRelJump(0x523191, (UInt32)Hook);
	}
}

namespace OnCalculateEffectEntryMagnitude
{
	constexpr char eventName[] = "ShowOff:OnCalculateEffectMagnitude";

	inline NumberModifications<float> g_MagnitudeModModifiers;
	inline EffectItem* g_liveEffectItem = nullptr;

	double HandleEvent(float modifier, UInt32 isHostile, UInt8* ebp)
	{
		auto* target = *reinterpret_cast<MagicTarget**>(ebp - 0x28);
		auto* caster = *reinterpret_cast<MagicCaster**>(ebp + 8);
		auto* magicItem = *reinterpret_cast<MagicItem**>(ebp + 0xC);
		auto* itemForm = DYNAMIC_CAST(magicItem, MagicItem, TESForm);

		const auto* activeEff = *reinterpret_cast<ActiveEffect**>(ebp + 0x10);
		auto* baseEffect = activeEff->effectItem->setting;

		void* multArg = *(void**)&modifier;
		void* magnitude = *(void**)&activeEff->magnitude;
		void* duration = *(void**)&activeEff->duration; // not modified by perk effects, like Modify Positive Chem Duration

		// Store effect Item for one frame, so it can be queried in a function inside the event.
		g_liveEffectItem = activeEff->effectItem;

		// WARNING: can run outside main thread (i.e when loading).
		// Thus, no calls to Console_Print should occur during then!
		// Can't delay this dispatch, otherwise effects wouldn't apply in time.
		g_eventInterface->DispatchEvent(eventName, target->GetActor(), caster->GetActor(), 
			itemForm, baseEffect, activeEff->spellType, activeEff->enchantObject, isHostile, multArg,
			magnitude, duration);

		g_liveEffectItem = nullptr;
			
		g_MagnitudeModModifiers.ModValue(modifier);
		g_MagnitudeModModifiers.Clear();

		return modifier;
	}

	double __cdecl HandleHostileEffect(float normalModifier)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto magnitudeMult = HandleEvent(normalModifier, true, ebp);

		// TODO: Cap magnitudeMult by max player resistance (if target is the player).

		return magnitudeMult;
	}
	double __cdecl HandleNormalEffect(float normalModifier)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());

		// No cap needs to be applied, since the player isn't resisting anything.
		return HandleEvent(normalModifier, false, ebp);
	}

	void __declspec(naked) CalcRegularEffectHook()
	{
		__asm
		{
			//result in st(0)
			push	ecx
			fstp	[esp]
			call	HandleNormalEffect  //result in st(0)
			//add	esp, 4

			// do regular code
			mov     esp, ebp
			pop     ebp
			retn    0xC
		}
	}

	void __declspec(naked) CalcHostileEffectResistHook()
	{
		__asm
		{
			// just called CalculateSpellMagnitudeMultFromResistance, result in st(0)
			push	ecx
			fstp	[esp]
			call	HandleHostileEffect  //result in st(0)
			//add	esp, 4  //not needed

			// do regular code
			mov     esp, ebp
			pop     ebp
			retn    0xC
		}
	}

	//TODO: hook near 0x7E0C1F (watch for Tweaks)
	void WriteHooks()
	{
		// Overwrite function epilogue
		WriteRelJump(0x8C4322, (UInt32)CalcRegularEffectHook);

		// Overwrite code following call to CalculateSpellMagnitudeMultFromResistance
		WriteRelJump(0x8C431C, (UInt32)CalcHostileEffectResistHook);
	}
}

DEFINE_COMMAND_PLUGIN(SetLiveEffectMagnitudeModifier, "", false, kParams_OneFloat_OneString);
bool Cmd_SetLiveEffectMagnitudeModifier_Execute(COMMAND_ARGS)
{
	*result = false;
	float fMod;
	char modType[2];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fMod, &modType) || !modType[0])
		return true;

	ScopedLock lock(g_Lock);
	*result = OnCalculateEffectEntryMagnitude::g_MagnitudeModModifiers.TryAddMod(fMod, modType[0]);

	return true;
}

DEFINE_COMMAND_PLUGIN(GetLiveEffectBaseTrait, "", false, kParams_OneInt);
// Copied code from JIP's GetNthEffectTraitNumeric
bool Cmd_GetLiveEffectBaseTrait_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 traitID;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &traitID))
		return true;
	EffectItem* effItem = OnCalculateEffectEntryMagnitude::g_liveEffectItem;
	if (!effItem) return true;
	switch (traitID)
	{
	case 0:
		*result = effItem->magnitude;
		break;
	case 1:
		*result = effItem->area;
		break;
	case 2:
		*result = effItem->duration;
		break;
	case 3:
		*result = effItem->range;
		break;
	case 4:
		*result = effItem->actorValueOrOther;
		break;
	case 5:
		*result = effItem->cost;  // unused in NV?
		break;
	}
	return true;
}

namespace OnPCMiscStatChange
{
	constexpr char eventName[] = "ShowOff:OnPCMiscStatChange";

	int __cdecl HandleEvent()
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		auto const statCode = *reinterpret_cast<MiscStatCode*>(ebp + 0x8);
		auto const modVal = *reinterpret_cast<int*>(ebp + 0xC);
		g_eventInterface->DispatchEventThreadSafe(eventName, nullptr, g_thePlayer, statCode, modVal);

		// Do regular code
		return 1003; // StatsMenu::GetMenuID
	}

	void WriteHook()
	{
		// replace "call StatsMenu::GetMenuID"
		WriteRelCall(0x4D5E6A, (UInt32)HandleEvent);
	}
}

namespace OnDisplayOrCompleteObjective
{
	constexpr char onDisplayName[] = "ShowOff:OnDisplayObjective";
	constexpr char onCompleteName[] = "ShowOff:OnCompleteObjective";

	static UInt32 hookedAddr = 0;

	TESQuest* __fastcall Hook(BGSQuestObjective* objective)
	{
		auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
		const auto newStatus = *reinterpret_cast<UInt32*>(ebp + 8);

		if (newStatus == 1 && !objective->status)
		{
			g_eventInterface->DispatchEventThreadSafe(onDisplayName, nullptr, nullptr, objective->quest, objective->objectiveId);
		}
		else if (newStatus == 3 && objective->status == 1 && !objective->quest->IsCompleted())
		{
			g_eventInterface->DispatchEventThreadSafe(onCompleteName, nullptr, nullptr, objective->quest, objective->objectiveId);
		}

		return ThisStdCall<TESQuest*>(hookedAddr, objective);
	}

	void WriteDelayedHook()
	{
		// Add compatibility with Tweaks by indirectly calling the function at the address.
		hookedAddr = GetRelJumpAddr(0x5EC5DC);
		WriteRelCall(0x5EC5DC, (UInt32)Hook);
		// We could have hooked the HUDMainMenu::SetQuestUpdateText calls, but that would cause incompatibility with Tweaks' "No Quest Messages".
	}
}

// Like NVSE OnAdd, but runs for all vanilla OnAdd instances.
// (Needs unique hooks, NVSE hooks didn't expect non-references to be passed to MergeScriptEvent).
// Relevant addresses: 0x57506B, 0x574AFA, 0x574C28, 0x574D00, 0x574F03
// Event runs slightly AFTER vanilla OnAdd.
namespace OnAddAlt
{
	constexpr char eventName[] = "ShowOff:OnAdd";

	// non-null if picking up a reference in the game world
	static TESObjectREFR* g_AddedItemRef = nullptr;

	struct InvRefCreatingInfo
	{
		void Fill(TESObjectREFR* owner, TESForm* item, ExtraDataList* xData, SInt32 count)
		{
			m_owner = owner; m_item = item; m_xData = xData; m_count = count;
		}
		TESObjectREFR* GetInvRef()
		{
			if (m_cachedInvRef)
				return m_cachedInvRef;
			if (!m_item || !m_owner)
				return nullptr;
			m_cachedInvRef = InventoryRefCreateEntry(m_owner, m_item, m_count, m_xData);
			return m_cachedInvRef;
		}
		void Reset()
		{
			m_owner = {}; m_item = {}; m_xData = {}; m_count = {}; m_cachedInvRef = {};
		}
	private:
		TESObjectREFR* m_owner{};
		TESForm* m_item{};
		ExtraDataList* m_xData{};
		SInt32 m_count{}; //xCount?

		TESObjectREFR* m_cachedInvRef{};
	};
	static InvRefCreatingInfo g_InvRefCreatingInfo;

	// using a function so creating an invRef is on-demand, instead of a constant drain.
	TESObjectREFR* GetItemRef()
	{
		if (g_AddedItemRef)
			return g_AddedItemRef;
		//else, must get invRef.
		return g_InvRefCreatingInfo.GetInvRef();
	}

	void HandleEvent(TESObjectREFR* newOwner, TESForm* item, ExtraDataList* xData, SInt32 count)
	{
		g_InvRefCreatingInfo.Fill(newOwner, item, xData, count);
		g_eventInterface->DispatchEvent(eventName, nullptr, item, newOwner);
		g_InvRefCreatingInfo.Reset();
	}

	void HandleEvent(TESObjectREFR* newOwner, TESObjectREFR* itemRef)
	{
		g_AddedItemRef = itemRef;
		g_eventInterface->DispatchEvent(eventName, nullptr, itemRef->baseForm, newOwner);
		g_AddedItemRef = nullptr;
	}

	namespace HandleAddItem
	{
		void __fastcall Hook(void* xChanges, void* edx, TESForm* item, ExtraDataList* xData, SInt32 count)
		{
			auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
			auto* newOwner = *reinterpret_cast<TESObjectREFR**>(ebp - 0xC);

			HandleEvent(newOwner, item, xData, count);

			// Call ExtraContainerChanges_4C29A0 (which we overwrote)
			ThisStdCall(0x4C29A0, xChanges, item, xData, count);
		}

		void WriteHook()
		{
			WriteRelCall(0x575091, (UInt32)Hook);
		}
	}

	namespace EquipForRef
	{
		void __fastcall Hook(ExtraContainerChanges::Data* xChanges, void* edx, TESForm* item, SInt32 count,
			TESObjectREFR* newOwner, ExtraDataList* xData, int a6, int a7)
		{
			HandleEvent(newOwner, item, xData, count);

			// Call ExtraContainerChanges__Data__EquipForRef (which we overwrote)
			ThisStdCall(0x4BFFE0, xChanges, item, count, newOwner, xData, a6, a7);
		}

		void WriteHook()
		{
			WriteRelCall(0x574B19, (UInt32)Hook);
		}
	}

	namespace PickUpItem
	{
		namespace InitialOnAdd
		{
			void __cdecl Hook(TESObjectREFR* newOwner, ExtraDataList* xData, int eventId)
			{
				CdeclCall(MergeScriptEventAddr, newOwner, xData, eventId);

				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				auto* itemRef = *reinterpret_cast<TESObjectREFR**>(ebp + 8);

				HandleEvent(newOwner, itemRef);
			}

			void WriteHook()
			{
				WriteRelCall(0x574C28, (UInt32)Hook);
			}
		}

		namespace RockItProjectile
		{
			void __cdecl Hook(TESObjectREFR* newOwner, ExtraDataList* xData, int eventId)
			{
				CdeclCall(MergeScriptEventAddr, newOwner, xData, eventId);

				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				const auto* rockItContChanges = *reinterpret_cast<ContChangesEntry**>(ebp - 0x60);

				//todo: is it safe to use countDelta here? Should Count extradata be checked? idk
				HandleEvent(newOwner, rockItContChanges->type, xData, rockItContChanges->countDelta);
			}

			void WriteHook()
			{
				WriteRelCall(0x574D00, (UInt32)Hook);
			}
		}
	
		namespace OtherProjectile
		{
			void __cdecl Hook(TESObjectREFR* newOwner, ExtraDataList* xData, int eventId)
			{
				CdeclCall(MergeScriptEventAddr, newOwner, xData, eventId);

				auto* ebp = GetParentBasePtr(_AddressOfReturnAddress());
				auto* form = *reinterpret_cast<TESForm**>(ebp - 0x24);

				// todo: verify if assuming count = 1 is safe!
				HandleEvent(newOwner, form, xData, 1);
			}

			void WriteHook()
			{
				WriteRelCall(0x574F03, (UInt32)Hook);
			}
		}

		void WriteHooks()
		{
			InitialOnAdd::WriteHook();
			RockItProjectile::WriteHook();
			OtherProjectile::WriteHook();
		}
	}

	void WriteHooks()
	{
		HandleAddItem::WriteHook();
		EquipForRef::WriteHook();
		PickUpItem::WriteHooks();
	}
}

DEFINE_COMMAND_ALT_PLUGIN(GetAddedItemRefShowOff, GetAddedItemRefSO, "", false, nullptr);
bool Cmd_GetAddedItemRefShowOff_Execute(COMMAND_ARGS)
{
	if (auto const itemRef = OnAddAlt::GetItemRef())
	{
		REFR_RES = itemRef->refID;
	}
	else *result = 0;
	return true;
}

namespace OnDropAlt
{
	constexpr char eventName[] = "ShowOff:OnDrop";

}



using EventFlags = NVSEEventManagerInterface::EventFlags;

template<UInt8 N>
bool RegisterEvent(const char* eventName, EventParamType(&paramTypes)[N], 
	EventFlags flags = EventFlags::kFlags_None)
{
	return g_eventInterface->RegisterEvent(eventName, std::size(paramTypes), paramTypes, flags);
};
bool RegisterEvent(const char* eventName, nullptr_t null,
	EventFlags flags = EventFlags::kFlags_None)
{
	return g_eventInterface->RegisterEvent(eventName, 0, nullptr, flags);
};

void RegisterEvents()
{
	OnCornerMessage = JGCreateEvent("OnCornerMessage", 5, 0, NULL);
#if _DEBUG
	OnActorValueChange = JGCreateEvent("OnActorValueChange", 4, 0, NULL);
#endif

	RegisterEvent(OnPreActivate::eventName, kEventParams_OneReference_OneIntPtr);
	RegisterEvent(PreActivateInventoryItem::eventName, kEventParams_OneBaseForm_OneReference_OneIntPtr_OneInt);
	RegisterEvent(PreDropInventoryItem::eventName, kEventParams_OneBaseForm_OneReference_OneIntPtr);
	RegisterEvent(OnQuestAdded::eventName, kEventParams_OneBaseForm);
	 
	RegisterEvent(OnCalculateSellPrice::eventNameAdd, kEventParams_OneBaseForm_OneReference);
	RegisterEvent(OnCalculateSellPrice::eventNameSub, kEventParams_OneBaseForm_OneReference);
	RegisterEvent(OnCalculateSellPrice::eventNameMult, kEventParams_OneBaseForm_OneReference);

	RegisterEvent(OnProjectileDestroy::eventName, kEventParams_OneReference_OneBaseForm, EventFlags::kFlag_FlushOnLoad);
	RegisterEvent(OnProjectileCreate::eventName, kEventParams_OneReference_OneBaseForm, EventFlags::kFlag_FlushOnLoad);
	RegisterEvent(OnProjectileImpact::eventName, kEventParams_OneReference_OneBaseForm_OneReference, EventFlags::kFlag_FlushOnLoad);

	RegisterEvent(OnLockpickMenuClose::eventName, kEventParams_OneInt);

	RegisterEvent(OnQueueCornerMessage::eventName, kEventParams_ThreeStrings_OneFloat);
	RegisterEvent(OnShowCornerMessage::eventName, kEventParams_ThreeStrings_OneFloat);
	RegisterEvent(OnFireWeapon::eventName, kEventParams_OneReference_OneBaseForm);

#if _DEBUG
	RegisterEvent(OnCalculateEffectEntryMagnitude::eventName,
		kEventParams_OneReference_TwoBaseForms_OneInt_OneBaseForm_OneInt_ThreeFloats);
#endif

	RegisterEvent(OnPCMiscStatChange::eventName, kEventParams_TwoInts);

	RegisterEvent(OnDisplayOrCompleteObjective::onDisplayName, kEventParams_OneBaseForm_OneInt);
	RegisterEvent(OnDisplayOrCompleteObjective::onCompleteName, kEventParams_OneBaseForm_OneInt);

	RegisterEvent(OnAddAlt::eventName, kEventParams_OneBaseForm_OneReference);
#if _DEBUG

#endif
	/*
	// For debugging the Event API
	constexpr char DebugEventName[] = "ShowOff:DebugEvent";
	RegisterEvent(DebugEventName,kEventParams_OneInt_OneFloat_OneArray_OneString_OneForm_OneReference_OneBaseform);
	*/
}

namespace EventHandling
{
	void HandleGameLoopEvents()
	{

	}
}

namespace HandleHooks
{
	void HandleEventHooks()
	{
		OnPreActivate::WriteHook();
		PreActivateInventoryItem::WriteHooks();
		OnQuestAdded::WriteHook();
		OnCalculateSellPrice::WriteHook();
		OnProjectileDestroy::WriteHook();
		OnProjectileCreate::WriteHook();
		OnProjectileImpact::WriteHooks();
		OnLockpickMenuClose::WriteHooks();
		OnQueueCornerMessage::WriteHooks();
		OnShowCornerMessage::WriteHooks();
		OnFireWeapon::WriteHook();
		OnCalculateEffectEntryMagnitude::WriteHooks();
		OnPCMiscStatChange::WriteHook();
		OnAddAlt::WriteHooks();

#if _DEBUG
		//ActorValueChangeHooks::WriteHook();
		PreDropInventoryItem::WriteHook();
#endif
	}

	void HandleDelayedEventHooks()
	{
		CornerMessageHooks::WriteDelayedHook();
		OnDisplayOrCompleteObjective::WriteDelayedHook();

#if _DEBUG
#endif
	}

}

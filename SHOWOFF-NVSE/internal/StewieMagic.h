#pragma once
#include "GameProcess.h"
#include "jip_nvse.h"
#include "decoding.h"

// Everything here was ripped from lStewieAl.


enum
{
	kAddr_GetItemHealthPerc = 0x4BCDB0,
	kAddr_GetItemValue = 0x4BD400,
	kAddr_ReadXML = 0xA01B00,
	kAddr_TileFromTemplate = 0xA1DDB0,
	kAddr_EvaluateConditions = 0x680C60,
	kAddr_EvaluateCondition = 0x681600,
};

bool __fastcall ContainerMenuCheckIsTargetChild(ContChangesEntry* entry, void* edx, int a1);

extern void (*ApplyPerkModifiers)(uint32_t entryPointID, TESObjectREFR* perkOwner, void* arg3, ...);
extern void(__cdecl* IncPCMiscStat)(MiscStatCode code);
extern void(__cdecl* DecPCMiscStat)(MiscStatCode code);

enum KarmaTier
{
	Good = 0x0,
	NeutralKarma = 0x1,
	Evil = 0x2,
	VeryGood = 0x3,
	VeryEvil = 0x4,
};

extern KarmaTier(__cdecl* GetKarmaTier)(float karma);

// can be passed to QueueUIMessage to determine Vaultboy icon displayed
enum eEmotion {
	neutral = 0,
	happy = 1,
	sad = 2,
	pain = 3
};

enum QueuedMenuRequestType
{
	kContainerMenu = 1,
	kHackingMenu = 2,
	kComputerMenu = 3,
	kDialogMenu = 4,
	kRaceSexMenu = 5,
	kStartMenu = 6,
	kMessageMenu = 7,
	kCompanionWheelMenu = 8,
	kPlayerNameMenu = 9
};

enum ContainerMode
{
	kNormal = 1,
	kPickpocket = 2,
	kTeammate = 3,
	kRockItLauncher = 4
};


// 1C
struct VoiceEntry
{
	struct Response
	{
		String str;
		char* fileName;
	};

	tList<void> list00;
	Response* response;
	TESTopicInfo* topicInfo;
	TESTopic* topic;
	TESQuest* quest;
	Actor* actor;
};

namespace Radio
{
	struct RadioEntry
	{

		struct Struct04
		{

			struct Struct00
			{
				tList<VoiceEntry> voiceEntries;
				Struct00* next;
			};

			Struct00* unk00;
			uint32_t unk04;
			uint32_t rng08;
			uint32_t soundTimeRemaining0C;
			uint8_t byte10;
			uint8_t byte11;
			uint8_t gap12[2];
			uint32_t flags;
			tList<Sound*> sounds;
		};


		TESObjectREFR* radioRef;
		Struct04 unk04;

		static RadioEntry* GetSingleton() { return *(RadioEntry**)0x11DD42C; }  // Gets the active pipboy radio.
	};
	typedef RadioEntry::Struct04::Struct00 VoiceEntryList;
}
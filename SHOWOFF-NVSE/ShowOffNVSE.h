#pragma once
#include "GameData.h"

//Singletons
PlayerCharacter* g_thePlayer = nullptr;
ProcessManager* g_processManager = nullptr;
InterfaceManager* g_interfaceManager = nullptr;
BSWin32Audio* g_bsWin32Audio = nullptr;
DataHandler* g_dataHandler = nullptr;
BSAudioManager* g_audioManager = nullptr;
Sky** g_currentSky = nullptr; 


//Globals
bool g_canPlayerPickpocketEqItems() { return *(UInt32*)0x75E87B != 0xFFD5F551; }
//Checks if the address has been changed, to take into account Stewie's Tweaks' implementation.
//If the address was changed by something else, uh... Well I don't take that into account.

UINT32 g_ExtraPickpocketChance = 0;
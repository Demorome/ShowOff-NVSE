#pragma once

UINT32 canPlayerPickpocketEqItems() { return *(UInt32*)0x75E87A != 0x4BDDD0; }  //Checks if the address has been changed, to take into account Stewie's Tweaks' implementation.
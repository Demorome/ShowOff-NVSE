#pragma once

UINT32 canPlayerPickpocketEqItems() { return *(UInt32*)0x75E87B != 0xFFD5F551; }
//Checks if the address has been changed, to take into account Stewie's Tweaks' implementation.
//If the address was changed by something else, uh... Well I don't take that into account.
#pragma once

//not really useful for now...
bool(__cdecl* Cmd_GetAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BD8A0;
bool(__cdecl* Cmd_SetAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BDCD0;
bool(__cdecl* Cmd_ForceAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BE190;
bool(__cdecl* Cmd_ModAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BDE40;
bool(__cdecl* Cmd_DamageAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BDF20;
bool(__cdecl* Cmd_RestoreAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BE080;
bool(__cdecl* Cmd_GetBaseAV)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5BE6E0;
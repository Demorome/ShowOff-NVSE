#pragma once

DEFINE_COMMAND_PLUGIN(ShowingOffDisable, , 1, 1, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(ShowingOffEnable, , 1, 1, kParams_OneOptionalInt);

bool(__cdecl* Cmd_Disable)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5C45E0;
bool(__cdecl* Cmd_Enable)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5C43D0;

bool Cmd_ShowingOffDisable_Execute(COMMAND_ARGS) {
	return Cmd_Disable(PASS_COMMAND_ARGS);
}

bool Cmd_ShowingOffEnable_Execute(COMMAND_ARGS) {
	return Cmd_Enable(PASS_COMMAND_ARGS);
}
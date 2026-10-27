#include "SO_fn_Settings.h"

#include "GameSettings.h"

// Code mostly ripped from FOSE's gamesetting functions (GetNumericGameSetting etc).
bool Cmd_IsGameSetting_Execute(COMMAND_ARGS) {
	char settingName[512] = {};
	Setting* setting;
	GameSettingCollection* gmsts = GameSettingCollection::GetSingleton();

	if (ExtractArgs(EXTRACT_ARGS, &settingName)) {
		if (gmsts && gmsts->GetGameSetting(settingName, &setting)) {
			*result = 1;
			if (IsConsoleMode())
				Console_Print("IsGameSetting >> VALID GAMESETTING");
		}
		else {
			*result = 0;
			if (IsConsoleMode())
				Console_Print("IsGameSetting >> INVALID GAMESETTING");
		}
	}

	return true;
}

bool Cmd_IsINISetting_Execute(COMMAND_ARGS) {
	char settingName[512] = {};
	Setting* setting;

	if (ExtractArgs(EXTRACT_ARGS, &settingName)) {
		if (GetINISetting(settingName, &setting)) {
			*result = 1;
			if (IsConsoleMode())
				Console_Print("IsINISetting >> VALID INI SETTING");
		}
		else {
			*result = 0;
			if (IsConsoleMode())
				Console_Print("IsINISetting >> INVALID INI SETTING");
		}
	}

	return true;
}

bool Cmd_ModNumericINISetting_Execute(COMMAND_ARGS) {
	char settingName[512] = {};
	Setting* setting;
	float modVal = 0;
	double curVal = 0;

	if (ExtractArgs(EXTRACT_ARGS, &settingName, &modVal)) {
		if ((GetINISetting(settingName, &setting)) && (setting->Get(curVal))) {
			if (setting->Set(curVal + modVal)) {
				*result = 1;
				if (IsConsoleMode()) {
					double newVal;
					setting->Get(newVal);
					Console_Print("ModNumericINISetting >> %s -> %f", settingName, newVal);
				}
			}
		}
		else {
			*result = 0;
			if (IsConsoleMode())
				Console_Print("ModNumericINISetting >> INI SETTING NOT FOUND");
		}
	}

	return true;
}

bool Cmd_ModNumericINISettingAlt_Execute(COMMAND_ARGS) {
	char settingName[512] = {};
	Setting* setting;
	float modVal = 0;
	double curVal = 0;

	if (ExtractArgs(EXTRACT_ARGS, &settingName, &modVal)) {
		if ((GetINISetting(settingName, &setting)) && (setting->Get(curVal))) {
			if (setting->Set(curVal + modVal)) {
				*result = 1;
				if (IsConsoleMode()) {
					double newVal;
					setting->Get(newVal);
					Console_Print("ModNumericINISetting >> %s -> %f", settingName, newVal);
				}
			}
		}
		else {
			*result = 0;
			if (IsConsoleMode())
				Console_Print("ModNumericINISetting >> INI SETTING NOT FOUND");
		}
	}

	return true;
}

bool Cmd_ModNumericGameSetting_Execute(COMMAND_ARGS) {
	char settingName[512] = {};
	float modVal = 0;
	*result = 0;

	if (ExtractArgs(EXTRACT_ARGS, &settingName, &modVal)) {
		Setting* setting;
		GameSettingCollection* gmsts = GameSettingCollection::GetSingleton();
		if (gmsts && gmsts->GetGameSetting(settingName, &setting)) {
			double curVal;
			if (setting->Get(curVal)) {
				double newVal = curVal + modVal;
				if (setting->Set(newVal)) {
					*result = 1;
					if (IsConsoleMode())
						Console_Print("ModNumericGameSetting >> %s -> %f", settingName, newVal);
				}
			}
		}
		else if (IsConsoleMode())
			Console_Print("ModNumericGameSetting >> SETTING NOT FOUND");
	}

	return true;
}
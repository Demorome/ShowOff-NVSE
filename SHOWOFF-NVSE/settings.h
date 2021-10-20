#pragma once
#define INI_NAME "ShowOff_nvse.ini"
#define MO2_VFS_DLL_NAME "usvfs_x86.dll"
#define TWEAKS_INI_NAME "nvse_stewie_tweaks.ini"
#define TWEAKS_DLL_NAME "nvse_stewie_tweaks.dll"

extern HMODULE g_ShowOffHandle;

int GetINIValFromPlugin(const char* iniName, const char* section, const char* key, int defaultVal = 0);
int GetINIValFromTweaks(const char* key, int defaultVal = 0);
int GetINIValFromTweaks(const char* key, const char* section, int defaultVal = 0);

void HandleINIOptions();

#pragma once
#include "jip_nvse.h"


// ripped from JIP LN

enum
{
	kChangedFlag_AuxStringMaps = 1 << 0,

	kChangedFlag_All = kChangedFlag_AuxStringMaps //| kChangedFlag_RefMaps | kChangedFlag_LinkedRefs,
};


void DoLoadGameCleanup() 
{
	ScopedLock lock(g_Lock);
	if (s_dataChangedFlags & kChangedFlag_AuxStringMaps) s_auxStringMapArraysPerm.Clear();
	s_dataChangedFlags = 0;
}

char s_lastLoadedPath[MAX_PATH];  //static buffer, use locks before modifying

void LoadGameCallback(void*)
{
	const char* currentPath = GetSavePath();
	g_Lock.Enter();
	if (strcmp(s_lastLoadedPath, currentPath) != 0)
	{
		StrCopy(s_lastLoadedPath, currentPath);
		s_dataChangedFlags = kChangedFlag_All;
	}
	g_Lock.Leave();
	UInt8 const changedFlags = s_dataChangedFlags;
	DoLoadGameCleanup();

	UInt32 type, length, buffer4, skipSize;
	UInt8 buffer1, modIdx, loopBuffer;
	UInt16 nRecs, nVals, nVars;
	char varName[0x50];
	char keyName[0x50];

	while (GetNextRecordInfo(&type, (UInt32*)&s_serializedVersion, &length))
	{
		switch (type)
		{
		case 'SMSO':
		{
			if (!(changedFlags & kChangedFlag_AuxStringMaps)) continue;
			AuxStringMapVarsMap* rVarsMap;
			AuxStringMapIDsMap* idsMap;
			skipSize = (s_serializedVersion < 10) ? 4 : 8;  //No idea if this is still needed (was in JIP)
			nRecs = ReadRecord16();  //the saved size of s_auxStringMapArraysPerm
			while (nRecs)
			{
				nRecs--;
				buffer1 = ReadRecord8();  //modID
				if (!ResolveRefID(buffer1 << 24, &buffer4)) continue;  //checks if mod is still loaded(?)
				rVarsMap = NULL;
				modIdx = buffer4 >> 24;
				nVars = ReadRecord16();  //amount of auxStringMaps owned by the mod.
				while (nVars)
				{
					idsMap = NULL;
					buffer1 = ReadRecord8();  //length of char* for the name of an auxStringMap
					ReadRecordData(varName, buffer1);  //retrieve the char*
					varName[buffer1] = 0; //idk
					nVals = ReadRecord16();  //amount of key/value pairs for the specific auxStringMap
					while (nVals)
					{  
						loopBuffer = ReadRecord8();  //length of char*
						ReadRecordData(keyName, loopBuffer);  //retrieve the char*
						keyName[loopBuffer] = 0; //idk
						buffer1 = ReadRecord8();  //associated data (str/ref/flt)
						if (keyName[0])
						{
							if (!idsMap)
							{
								ScopedLock lock(g_Lock);
								if (!rVarsMap) rVarsMap = s_auxStringMapArraysPerm.Emplace(modIdx, nVars);
								idsMap = rVarsMap->Emplace(varName, nVals);
							}
							idsMap->Emplace(keyName, buffer1);
						}
						//idk if I should keep the stuff below here. What does it do?
						//Doesn't seem to run. The original if was "if LookupFormByID(UInt32:Key)" (if ID is a valid form)
						//I'll keep it for now, but I doubt it'll get used.
						//Who knows, maybe I need to skip bytes myself, for whatever reason.
						else if (buffer1 == 1)
							SkipNBytes(skipSize);  //if not a valid form, skip bytes?
						else if (buffer1 == 2)
							SkipNBytes(4);
						else SkipNBytes(ReadRecord16());
						nVals--;
					}
					nVars--;
				}
			}
			break;
		}
		default:
			break;
		}
	}

}

void SaveGameCallback(void*)
{
	UInt8 buffer1, loopBuffer;
	UInt16 buffer2;

	ScopedLock lock(g_Lock);
	 
	StrCopy(s_lastLoadedPath, GetSavePath());
	s_dataChangedFlags = 0;

	if (buffer2 = s_auxStringMapArraysPerm.Size())
	{
		WriteRecord('SMSO', 10, &buffer2, 2);
		for (auto rmModIt = s_auxStringMapArraysPerm.Begin(); rmModIt; ++rmModIt)
		{
			WriteRecord8(rmModIt.Key());
			WriteRecord16(rmModIt().Size());
			for (auto rmVarIt = rmModIt().Begin(); rmVarIt; ++rmVarIt)
			{
				buffer1 = StrLen(rmVarIt.Key());
				WriteRecord8(buffer1);
				WriteRecordData(rmVarIt.Key(), buffer1);
				WriteRecord16(rmVarIt().Size());
				for (auto rmRefIt = rmVarIt().Begin(); rmRefIt; ++rmRefIt)
				{
					loopBuffer = StrLen(rmRefIt.Key());
					WriteRecord8(loopBuffer);
					WriteRecordData(rmRefIt.Key(), loopBuffer);
					rmRefIt().WriteValData();
				}
			}
		}
	}
}

void NewGameCallback(void*)
{
	s_dataChangedFlags = kChangedFlag_All;
	DoLoadGameCleanup();
	
	ScopedLock lock(g_Lock);
	s_lastLoadedPath[0] = 0;
}
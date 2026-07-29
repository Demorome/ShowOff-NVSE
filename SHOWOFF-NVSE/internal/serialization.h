#pragma once
#include "AuxTimers.h"
#include "jip_nvse.h"


// All ripped from JIP LN's serialization.h

void ProcessDataChangedFlags(DataChangedFlags changedFlags)
{
	if (changedFlags & kChangedFlag_AuxStringMaps) s_auxStringMapArraysPerm.Clear();
	if (changedFlags & kChangedFlag_AuxTimerMaps) AuxTimer::s_auxTimerMapArraysPerm.Clear();
}

char s_lastLoadedPath[MAX_PATH];

uint8_t* s_loadGameBuffer = nullptr;
uint32_t s_loadGameBufferSize = 0x10000;

__declspec(noinline) uint8_t* __fastcall GetLoadGameBuffer(uint32_t length)
{
	if (s_loadGameBufferSize < length)
	{
		s_loadGameBufferSize = length;
		if (s_loadGameBuffer)
			_aligned_free(s_loadGameBuffer);
		s_loadGameBuffer = (uint8_t*)_aligned_malloc(length, 0x10);
	}
	else if (!s_loadGameBuffer)
		s_loadGameBuffer = (uint8_t*)_aligned_malloc(s_loadGameBufferSize, 0x10);
	ReadRecordData(s_loadGameBuffer, length);
	return s_loadGameBuffer;
}

void LoadGameCallback(void*)
{
	// loaded path is checked in message handler.
	uint8_t const changedFlags = s_dataChangedFlags;
	ProcessDataChangedFlags(static_cast<DataChangedFlags>(changedFlags));
	// s_dataChangedFlags is reset @ PostLoadGame msg handler.

	uint32_t type, version, length, nRefs, buffer4;
	uint8_t buffer1, modIdx, loopBuffer;
	uint16_t nRecs, nVals, nVars;
	char varName[0x50];
	char keyName[0x50];

	while (GetNextRecordInfo(&type, &version, &length))
	{
		switch (type)
		{
		case 'SMSO':
		{
			if (!(changedFlags & kChangedFlag_AuxStringMaps)) continue;
			nRecs = ReadRecord16();  //the saved size of s_auxStringMapArraysPerm
			while (nRecs)
			{
				nRecs--;
				buffer1 = ReadRecord8();  //modID

				//checks if mod is still loaded(?)
				if (!ResolveRefID(buffer1 << 24, &buffer4))
				{
					continue;
				}   
				
				AuxStringMapVarsMap* rVarsMap = NULL;
				modIdx = buffer4 >> 24;
				nVars = ReadRecord16();  //amount of auxStringMaps owned by the mod.
				while (nVars)
				{
					AuxStringMapIDsMap* idsMap = NULL;
					buffer1 = ReadRecord8();  //length of char* for the name of an auxStringMap
					ReadRecordData(varName, buffer1);  //retrieve the char*
					varName[buffer1] = 0; // add null terminator at the end
					nVals = ReadRecord16();  //amount of key/value pairs for the specific auxStringMap
					while (nVals)
					{  
						loopBuffer = ReadRecord8();  //length of char*
						ReadRecordData(keyName, loopBuffer);  //retrieve the char*
						keyName[loopBuffer] = 0; // add null terminator at the end
						buffer1 = ReadRecord8();  //associated data (str/ref/flt)
						if (keyName[0])
						{
							if (!idsMap)
							{
								ScopedLock lock(g_Lock);
								if (!rVarsMap) 
								{
									rVarsMap = s_auxStringMapArraysPerm.Emplace(
										modIdx, 
										nVars
									);
								}
								idsMap = rVarsMap->Emplace(varName, nVals);
							}
							idsMap->Emplace(keyName, buffer1);
						}
						//idk if I should keep the stuff below here. What does it do?
						//Doesn't seem to run. The original if was "if LookupFormByID(uint32_t:Key)" (if ID is a valid form)
						//I'll keep it for now, but I doubt it'll get used.
						//Who knows, maybe I need to skip bytes myself, for whatever reason.
						else if (buffer1 == 1)
							SkipNBytes(8);  //if not a valid form, skip bytes?
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
		case 'TAOS':
		{
			if (!(changedFlags & kChangedFlag_AuxTimerMaps) 
				|| (version < AuxTimer::AuxTimerVersion))
			{
				break;
			}
			uint8_t* bufPos = GetLoadGameBuffer(length);
			nRecs = *(uint16_t*)bufPos;
			bufPos += sizeof(uint16_t);
			while (nRecs)
			{
				modIdx = *bufPos++;
				nRecs--;
				if (modIdx > 5 && GetResolvedModIndex(&modIdx))
				{
					AuxTimer::AuxTimerOwnersMap* ownersMap = nullptr;
					nRefs = *(uint16_t*)bufPos;
					bufPos += sizeof(uint16_t);
					while (nRefs)
					{
						uint32_t refID = *(uint32_t*)bufPos;
						bufPos += sizeof(uint32_t);
						nVars = *(uint16_t*)bufPos;
						bufPos += sizeof(uint16_t);
						if ((refID = GetResolvedRefID(refID)) 
							&& (LookupFormByRefID(refID) || HasChangeData(refID)))
						{
							if (!ownersMap)
							{
								ownersMap = AuxTimer::s_auxTimerMapArraysPerm.Emplace(
									modIdx, 
									AlignBucketCount(nRefs)
								);
							}
							AuxTimer::AuxTimerVarsMap* aVarsMap = ownersMap->Emplace(
								refID, 
								AlignBucketCount(nVars)
							);

							while (nVars)
							{
								buffer1 = *bufPos++; // strLen for auxvar name
								if (!buffer1)
									goto avSkipVars;
								uint8_t* namePos = bufPos;
								bufPos += buffer1; // skip to after auxvar name

								auto timeToCountdown = *(double*)bufPos;
								*bufPos = 0; // add null terminator for auxvar name
								bufPos += sizeof(double);

								auto timeLeft = *(double*)bufPos;
								bufPos += sizeof(double);

								auto flags = *(uint32_t*)bufPos;
								bufPos += sizeof(uint32_t);

								// emplace AuxTimerValue
								aVarsMap->Emplace((char*)namePos, timeToCountdown, timeLeft, flags); 
								nVars--;
							}
						}
						else
						{
							while (nVars)
							{
								buffer1 = *bufPos++; // strLen for auxvar name
								bufPos += buffer1; // skip to after auxvar name
							avSkipVars:
								bufPos += sizeof(double) + sizeof(double) + sizeof(uint32_t);
								nVars--;
							}
						}
						nRefs--;
					}
				}
				else
				{
					// Skip over invalid saved data (invalid modID).
					// Unsure if needed, but JIP has it, so...
					nRefs = *(uint16_t*)bufPos;
					bufPos += 2;
					while (nRefs)
					{
						bufPos += 4;
						nVars = *(uint16_t*)bufPos;
						bufPos += 2;
						while (nVars)
						{
							buffer1 = *bufPos++; // strLen for auxvar name
							bufPos += buffer1; // skip to after auxvar name
							bufPos += sizeof(double) + sizeof(double) + sizeof(uint32_t);
							nVars--;
						}
						nRefs--;
					}
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
	uint8_t buffer1, loopBuffer;
	uint16_t buffer2;

	// s_dataChangedFlags gets set to 0 in NVSE OnLoad message handler.

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

	if (buffer2 = AuxTimer::s_auxTimerMapArraysPerm.Size())
	{
		// "ShowOff (SO) AuxTimer (AT)"
		WriteRecord('TAOS', AuxTimer::AuxTimerVersion, &buffer2, 2);
		for (auto avModIt = AuxTimer::s_auxTimerMapArraysPerm.Begin(); avModIt; ++avModIt)
		{
			WriteRecord8(avModIt.Key());
			WriteRecord16(avModIt().Size());
			for (auto avOwnerIt = avModIt().Begin(); avOwnerIt; ++avOwnerIt)
			{
				WriteRecord32(avOwnerIt.Key());
				WriteRecord16(avOwnerIt().Size());
				for (auto avVarIt = avOwnerIt().Begin(); avVarIt; ++avVarIt)
				{
					buffer1 = StrLen(avVarIt.Key());
					WriteRecord8(buffer1);
					WriteRecordData(avVarIt.Key(), buffer1);
					avVarIt().WriteValData();
				}
			}
		}
	}

}
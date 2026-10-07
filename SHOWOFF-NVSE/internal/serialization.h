#pragma once
#include "AuxTimers.h"
#include "jip_nvse.h"


// All ripped from JIP LN's serialization.h

void ClearScriptAuxData()
{
	s_auxStringMapArraysPerm.Clear();
	AuxTimer::s_auxTimerMapArraysPerm.Clear();
}

uint8_t* s_loadGameBuffer = nullptr;
uint32_t s_loadGameBufferSize = 0x10000;

constexpr uint32_t AuxStringMapVersion = 11;

uint32_t __fastcall LoadFormID(bool abSpecialIDs, bool abSaveSupportsSpecialFormIDs) {
	const uint8_t ucModIndex = ReadRecord8();
	const bool bESL = abSpecialIDs && ucModIndex == 0xFE;
	const bool bMedium = abSpecialIDs && ucModIndex == 0xFD;

	uint32_t uiTempFormID = ucModIndex << 24;

	if (abSaveSupportsSpecialFormIDs) {
		const uint16_t usSecondIndex = ReadRecord16();
		if (bESL)
			uiTempFormID |= usSecondIndex << 12;
		else if (bMedium) {
			uiTempFormID |= usSecondIndex << 16;
		}
	}
	else if (bESL || bMedium) {
		// File won't be found, so skip early
		uiTempFormID = 0xFF << 24;
	}
	return uiTempFormID;
}

void __fastcall SaveFormID(const TESFile* apFile, bool abSpecialIDs) {
	WriteRecord8(apFile->modIndex);
	WriteRecord16(abSpecialIDs ? apFile->smallIndex : 0);
}

void LoadGameCallback(void*)
{
	ClearScriptAuxData();
	// s_dataChangedFlags is reset @ PostLoadGame msg handler.

	const bool bSpecialIDs = TESDataHandler::HasNewFileTypeSupport();

	uint32_t type, uiVersion, length, nRefs;
	uint8_t buffer1, loopBuffer;
	uint16_t nRecs, nVals, nVars;
	char varName[0x50];
	char keyName[0x50];

	while (GetNextRecordInfo(&type, &uiVersion, &length))
	{
		switch (type)
		{
		case 'SMSO':
		{
			if (uiVersion > AuxStringMapVersion)
				break;

			const bool bSaveSupportsSpecialFormIDs = uiVersion > 10;

			nRecs = ReadRecord16();  //the saved size of s_auxStringMapArraysPerm
			while (nRecs)
			{
				nRecs--;
				uint32_t uiTempFormID = LoadFormID(bSpecialIDs, bSaveSupportsSpecialFormIDs);

				const TESFile* pFile = nullptr;
				if (ResolveRefID(uiTempFormID, &uiTempFormID))
					pFile = TESDataHandler::GetSingleton()->GetFileByFormID(uiTempFormID);
				
				if (!pFile)
					continue;
				
				AuxStringMapVarsMap* rVarsMap = NULL;
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
										pFile,
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
			if (uiVersion > AuxTimer::AuxTimerVersion)
				break;

			const bool bSaveSupportsSpecialFormIDs = uiVersion > 1;

			nRecs = ReadRecord16();
			while (nRecs)
			{
				uint32_t uiTempFormID = LoadFormID(bSpecialIDs, bSaveSupportsSpecialFormIDs);

				nRecs--;

				const TESFile* pFile = nullptr;
				if (ResolveRefID(uiTempFormID, &uiTempFormID))
					pFile = TESDataHandler::GetSingleton()->GetFileByFormID(uiTempFormID);

				if (pFile)
				{
					AuxTimer::AuxTimerOwnersMap* ownersMap = nullptr;
					nRefs = ReadRecord16();
					while (nRefs)
					{
						uint32_t refID = ReadRecord32();

						nVars = ReadRecord16();
						if (ResolveRefID(refID, &refID) && TESDataHandler::GetSingleton()->IsFormIDInUse(refID))
						{
							if (!ownersMap)
								ownersMap = AuxTimer::s_auxTimerMapArraysPerm.Emplace(pFile, AlignBucketCount(nRefs));

							AuxTimer::AuxTimerVarsMap* aVarsMap = ownersMap->Emplace(refID, AlignBucketCount(nVars));
							while (nVars)
							{
								const uint8_t ucNameLength = ReadRecord8();
								char cName[MAX_PATH];
								ReadRecordData(cName, ucNameLength);
								cName[ucNameLength] = 0;

								double dTimeToCountdown;
								ReadRecord64(&dTimeToCountdown);

								double dTimeLeft;
								ReadRecord64(&dTimeLeft);

								uint32_t uiFlags = ReadRecord32();

								// emplace AuxTimerValue
								aVarsMap->Emplace(cName, dTimeToCountdown, dTimeLeft, uiFlags);
								nVars--;
							}
						}
						else
						{
							while (nVars)
							{
								const uint8_t ucNameLength = ReadRecord8();
								SkipNBytes(ucNameLength + sizeof(double) + sizeof(double) + sizeof(uint32_t));
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
					nRefs = ReadRecord16();
					while (nRefs)
					{
						SkipNBytes(sizeof(uint32_t));
						nVars = ReadRecord16();
						while (nVars)
						{
							const uint8_t ucNameLength = ReadRecord8();
							SkipNBytes(ucNameLength + sizeof(double) + sizeof(double) + sizeof(uint32_t));
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
	const bool bSpecialIDs = TESDataHandler::HasNewFileTypeSupport();

	{
		const uint16_t usMapSize = s_auxStringMapArraysPerm.Size();
		if (usMapSize) {
			WriteRecord('SMSO', AuxStringMapVersion, &usMapSize, sizeof(usMapSize));
			for (auto rmModIt = s_auxStringMapArraysPerm.Begin(); rmModIt; ++rmModIt) {
				const TESFile* pMod = rmModIt.Key();
				if (!pMod)
					continue;

				SaveFormID(pMod, bSpecialIDs);
				WriteRecord16(rmModIt().Size());
				for (auto rmVarIt = rmModIt().Begin(); rmVarIt; ++rmVarIt) {
					const uint8_t ucKeyLength = strlen(rmVarIt.Key());
					WriteRecord8(ucKeyLength);
					WriteRecordData(rmVarIt.Key(), ucKeyLength);
					WriteRecord16(rmVarIt().Size());
					for (auto rmRefIt = rmVarIt().Begin(); rmRefIt; ++rmRefIt) {
						const uint8_t ucStringLength = strlen(rmRefIt.Key());
						WriteRecord8(ucStringLength);
						WriteRecordData(rmRefIt.Key(), ucStringLength);
						rmRefIt().WriteValData();
					}
				}
			}
		}
	}

	{
		const uint16_t usMapSize = AuxTimer::s_auxTimerMapArraysPerm.Size();
		if (usMapSize) {
			// "ShowOff (SO) AuxTimer (AT)"
			WriteRecord('TAOS', AuxTimer::AuxTimerVersion, &usMapSize, sizeof(usMapSize));
			for (auto avModIt = AuxTimer::s_auxTimerMapArraysPerm.Begin(); avModIt; ++avModIt) {
				const TESFile* pMod = avModIt.Key();
				if (!pMod)
					continue;

				SaveFormID(pMod, bSpecialIDs);
				WriteRecord16(avModIt().Size());
				for (auto avOwnerIt = avModIt().Begin(); avOwnerIt; ++avOwnerIt) {
					WriteRecord32(avOwnerIt.Key());
					WriteRecord16(avOwnerIt().Size());
					for (auto avVarIt = avOwnerIt().Begin(); avVarIt; ++avVarIt) {
						const uint8_t ucKeyLength = strlen(avVarIt.Key());
						WriteRecord8(ucKeyLength);
						WriteRecordData(avVarIt.Key(), ucKeyLength);
						avVarIt().WriteValData();
					}
				}
			}
		}
	}
}
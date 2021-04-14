#pragma once
//Not used, but may be useful to look at -Demorome.

#if 0

DEFINE_COMMAND_PLUGIN(GetCalculatedPos, "Gets current PosXYZ", 1, 4, kParams_Tomm_TwoFloats_TwoScriptVars)
DEFINE_COMMAND_PLUGIN(GetSUPVersion, "Get plugin version", 0, 0, NULL)
DEFINE_COMMAND_PLUGIN(GetFileSize, "Get file size", 0, 1, kParams_Tomm_OneString)
DEFINE_COMMAND_PLUGIN(GetLoadedSaveSize, "", 0, 1, kParams_Tomm_OneInt)
DEFINE_COMMAND_PLUGIN(GetSavedSaveSize, "", 0, 1, kParams_Tomm_OneInt)
DEFINE_COMMAND_PLUGIN(GetSaveName, "", 0, 1, kParams_Tomm_OneInt)
DEFINE_COMMAND_PLUGIN(RoundAlt, "", 0, 3, kParams_Tomm_OneFloat_TwoOptionalInt)
DEFINE_COMMAND_PLUGIN(Round, "", 0, 1, kParams_Tomm_OneFloat)
DEFINE_COMMAND_PLUGIN(MarkScriptOnLoad, "", 0, 0, NULL)
DEFINE_COMMAND_PLUGIN(IsScriptMarkedOnLoad, "", 0, 1, kParams_Tomm_OneForm)
DEFINE_COMMAND_PLUGIN(GetNearCells, "", 0, 1, kParams_Tomm_OneInt)





bool Cmd_GetCalculatedPos_Execute(COMMAND_ARGS) //cos sin values taken from JiP

{
	ScriptVar* outX, * outY;
	float fAngle, fDistance;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &fAngle, &fDistance, &outX, &outY))
	{
		outX->data.num = ((thisObj->posX) + fDistance * (sin(fAngle * 0.017453292519943295)));
		outY->data.num = ((thisObj->posY) + fDistance * (cos(fAngle * 0.017453292519943295)));
	}

	return true;

}


bool Cmd_GetSUPVersion_Execute(COMMAND_ARGS)
{
	*result = -1;
	Console_Print("%s %d", "SUPNVSEVersion>>", SUPNVSEVersion);
	return true;

}


#include <Tomm_JiP_FileStream.h>



bool Cmd_GetFileSize_Execute(COMMAND_ARGS)
{
	alignas(16) char s_strArgTemp[0x4000];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &s_strArgTemp))
	{
		if (strstr(s_strArgTemp, ":\\"))
		{ 
			*result = -1;
			return true;
		}
		if (strstr(s_strArgTemp, "..\\")) // thanks C6 
		{
			*result = -1;
			return true; 
		}
		if (strstr(s_strArgTemp, "://"))
		{
			*result = -1;
			return true;
		}
		if (strstr(s_strArgTemp, "..//")) 
		{
			*result = -1;
			return true;
		}
		FileStream srcFile;
		if (!srcFile.Open(s_strArgTemp))
		{
			*result = -1;
			return true;
		}
		UInt32 length = srcFile.GetLength();
		*result = length;
		return true;
	}
}


//0 for FOS
//1 for NVSE
bool Cmd_GetLoadedSaveSize_Execute(COMMAND_ARGS)
{
	FileStream srcFile;
	UInt32 iRequest = 0;
	char* SavegameName;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iRequest))
	{

		if (iRequest == 0)
		{SavegameName = LoadedSGPathFOS;}
		else
		{SavegameName = LoadedSGPathNVSE;}

		if (!srcFile.Open(SavegameName))
		{
			*result = -1;
		}
		else
		{
			UInt32 length = srcFile.GetLength();
			*result = length;
		}
		return true;
	}
}


//0 for FOS
//1 for NVSE
bool Cmd_GetSavedSaveSize_Execute(COMMAND_ARGS)
{
	FileStream srcFile;
	UInt32 iRequest = 0;
	char* SavegameName;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iRequest))
	{
		if (iRequest == 0)
		{
			SavegameName = SavedSGPathFOS;
		}
		else
		{
			SavegameName = SavedSGPathNVSE;
		}

		if (!srcFile.Open(SavegameName))
		{
			*result = -1;
		}
		else
		{
			UInt32 length = srcFile.GetLength();
			*result = length;
		}
		return true;
	}
}




//0 for Last saved
//1 for Last Loaded
bool Cmd_GetSaveName_Execute(COMMAND_ARGS)
{
	UInt32 iRequest = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iRequest))
	{
		if (iRequest == 1)
		{if (iLoadSGLength > 0){StrIfc->Assign(PASS_COMMAND_ARGS, LoadedSGName);}}
		else
		{if (iSavedSGLength > 0){StrIfc->Assign(PASS_COMMAND_ARGS, SavedSGName);}}
	}
	return true;
}


bool Cmd_RoundAlt_Execute(COMMAND_ARGS) //made by me for a change
{
	float fValue, fAdd = 0;
	UInt32 iRound = 0, iPrecision = 0, iMult;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &fValue,&iPrecision,&iRound))
	{
		if (iPrecision == 0) {iMult = 1;}else { iMult = pow(10, iPrecision); }

		if (iRound == 1){fAdd = 0.5;};

		float value = (int)((fValue * iMult) + fAdd);
		*result = (float)value / iMult;
	}
	return true;
}



bool Cmd_Round_Execute(COMMAND_ARGS) //math taken from MATH
{
	float fValue;


	if (ExtractArgsEx(EXTRACT_ARGS_EX, &fValue))
	{
		*result = round(fValue);
	}
	return true;
}

UnorderedSet<UInt32> s_gameLoadedInformedScriptsSUP;

bool Cmd_MarkScriptOnLoad_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (scriptObj)
	{
		if (s_gameLoadedInformedScriptsSUP.Insert(scriptObj->refID))
		{
			*result = 1;
		}
	}
	return true;
}

bool Cmd_IsScriptMarkedOnLoad_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESForm* scriptArg = NULL;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &scriptArg))
	{
		if (scriptArg)
		{
			if (s_gameLoadedInformedScriptsSUP.HasKey(scriptArg->refID))
			{
				*result = 1;
			}
		}
	}

	return true;
}





struct CellScanInfo
{
	TESObjectCELL::RefList::Iterator	prev;	//last ref returned to script
		TESObjectCELL* curCell;					//cell currently being scanned
		TESObjectCELL* cell;						//player's current cell
		TESWorldSpace* world;
	SInt8	curX;										//offsets of curCell from player's cell
	SInt8	curY;
	UInt8	cellDepth;									//depth of adjacent cells to scan


	CellScanInfo() {}
	CellScanInfo(UInt8 _cellDepth, TESObjectCELL* _cell)
		: curCell(NULL), cell(_cell), world(NULL), curX(0), curY(0),  cellDepth(_cellDepth)
	{
		world = cell->worldSpace;

		if (world && cellDepth)		//exterior, cell depth > 0
		{
			curX = cell->coords->x - cellDepth;
			curY = cell->coords->y - cellDepth;
			UInt32 key = (curX << 16) + ((curY << 16) >> 16);
			curCell = world->cellMap->Lookup(key);
		}
		else
		{
			cellDepth = 0;
			curCell = cell;
			curX = cell->coords->x;
			curY = cell->coords->y;
		}
	}

	bool NextCell()		//advance to next cell in area
	{
		if (!world || !cellDepth)
		{
			curCell = NULL;
			return false;
		}

		do
		{
			if (curX - cell->coords->x == cellDepth)
			{
				if (curY - cell->coords->y == cellDepth)
				{
					curCell = NULL;
					return false;
				}
				else
				{
					curY++;
					curX -= cellDepth * 2;
					UInt32 key = (curX << 16) + ((curY << 16) >> 16);
					curCell = world->cellMap->Lookup(key);
				}
			}
			else
			{
				curX++;
				UInt32 key = (curX << 16) + ((curY << 16) >> 16);
				curCell = world->cellMap->Lookup(key);
			}
		} while (!curCell);

		return true;
	}

	void FirstCell()	//init curCell to point to first valid cell
	{
		if (!curCell)
			NextCell();
	}

};




bool Cmd_GetNearCells_Execute(COMMAND_ARGS) // from NVSE
{
	NVSEArrayVar* CellsArr = ArrIfc->CreateArray(NULL, 0, scriptObj);
	SInt32 cellDepth = 1; 

	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &cellDepth)) return true;

	PlayerCharacter* pc = PlayerCharacter::GetSingleton();
	if (!pc || !(pc->parentCell))
		return true;						//avoid crash when these functions called in main menu before parentCell instantiated

	TESObjectCELL* cell = NULL;
	cell = pc->parentCell;

	if (!cell)
		return true;

	if (cellDepth == -127)
		cellDepth = 0;
	//else if (cellDepth == -1)
		//if (GetNumericIniSetting("uGridsToLoad:General", &uGrid))
		//	cellDepth = uGrid;
		//else
		//	cellDepth = 0;

	CellScanInfo info(cellDepth,cell);
	info.FirstCell();

	while (info.curCell)
	{
		ArrIfc->AppendElement(CellsArr, NVSEArrayElement(info.curCell));
		//_MESSAGE("Current Cell is [%x]", info.curCell->refID);
		info.NextCell();
	}
	ArrIfc->AssignCommandResult(CellsArr, result);
	return true;
}

//NVSEArrayVar* CellsArr = ArrIfc->CreateStringMap(NULL, NULL, 0, scriptObj);
//ArrIfc->SetElement(CellsArr, "dff", 2);
//ArrIfc->SetElement(CellsArr, "dff34234", 3);

#endif
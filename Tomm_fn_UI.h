#pragma once

#if 0

DEFINE_COMMAND_PLUGIN(GetHUDVisibilityFlags, "Gets HUD visibility flags", 0, 0, NULL)
DEFINE_COMMAND_PLUGIN(SetHUDVisibilityFlags, "Sets HUD visibility flags", 0, 1, kParams_Tomm_OneInt)
DEFINE_COMMAND_PLUGIN(DumpTileInfo, "Dumps XML element info", 0, 2, kParams_Tomm_OneIntOptional_OneStringOptional)
DEFINE_COMMAND_PLUGIN(DumpTileInfoAll, "Dumps ALL kRootMenu which means everything plus cursor", 0, 1, kParams_Tomm_OneIntOptional)
DEFINE_COMMAND_PLUGIN(GetScreenTrait, "Get Screen traits", 0, 1, kParams_Tomm_OneInt)
DEFINE_COMMAND_PLUGIN(GetCursorTrait, "Gets cursor traits", 0, 1, kParams_Tomm_OneInt)
DEFINE_COMMAND_PLUGIN(SetCursorTrait, "Sets cursor traits", 0, 2, kParams_Tomm_OneInt_OneFloat)
DEFINE_COMMAND_PLUGIN(SetCursorTraitGradual, "Sets cursor trait gradually", 0, 5, kParams_Tomm_OneInt_ThreeOptFloats_OneOptInt)
DEFINE_COMMAND_PLUGIN(DumpTileInfoToArray, "Dumps XML element info to array", 0, 1, kParams_Tomm_OneStringOptional)



void(__cdecl* HUDMainMenu_UpdateVisibilityState)(signed int) = (void(__cdecl*)(signed int))(0x771700); //Credits to lStewieAL - From Tweaks

bool Cmd_SetHUDVisibilityFlags_Execute(COMMAND_ARGS)
{
	UInt32 iVisFLags = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iVisFLags))
	{
		if (g_HUDMainMenu)
		{
			g_HUDMainMenu->visibilityOverrides = iVisFLags;
			HUDMainMenu_UpdateVisibilityState(HUDMainMenu::kRECALCULATE);
		}
	}
	return true;
}


bool Cmd_GetHUDVisibilityFlags_Execute(COMMAND_ARGS)
{
	*result = g_HUDMainMenu->visibilityOverrides; 
	return true;
}



TileMenu* __fastcall GetMenuTile(const char* componentPath) ///Taken from JiP or NVSE?? dont' remember.
{
	UInt32 menuID = s_menuNameToID.Get(componentPath);
	return menuID ? g_tileMenuArray[menuID - kMenuType_Min] : NULL;
}


Tile* __fastcall GetTargetComponent(const char* componentPath, Tile::Value** value = NULL)
{
	char* slashPos = SlashPos(componentPath);
	if (!slashPos)
		return GetMenuTile(componentPath);
	*slashPos = 0;
	Tile* component = GetMenuTile(componentPath);
	if (!component)
		return NULL;
	const char* trait = NULL;
	component = component->GetComponent(slashPos + 1, &trait);
	if (!component)
		return NULL;
	if (trait)
	{
		if (!value || !(*value = component->GetValueName(trait)))
			return NULL;
	}
	else if (value)
		return NULL;
	return component;
}



NVSEArrayVar* TileArrayStore;


void Tile::DumpArray(void)
{
	ArrIfc->SetElement(TileArrayStore, "Tilename", name.m_data);
	for (UInt32 i = 0; i < values.size; i++)
	{
		Value* val = values[i];
		const char* traitName = TraitIDToName(val->id);
		char		traitNameIDBuf[16];

		if (!traitName)
		{
			sprintf_s(traitNameIDBuf, "%08X", val->id);
			traitName = traitNameIDBuf;
		}

		if (val->str)
			ArrIfc->SetElement(TileArrayStore, traitName, val->str);
		//else if (val->action)
			//ArrIfc->SetElement(TileArrayStore, traitName, "NoValue");
		else
			ArrIfc->SetElement(TileArrayStore, traitName, val->num);
	}
	//gLog.Outdent();
}



bool Cmd_DumpTileInfoToArray_Execute(COMMAND_ARGS)
{
	alignas(16) char s_strArgTemp2[0x4000];
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &s_strArgTemp2))
	{
		Tile* component = GetTargetComponent(s_strArgTemp2);
		if (component){
			//_MESSAGE("Extracting specified HUD component.");
			TileArrayStore = ArrIfc->CreateStringMap(NULL, NULL, 0, scriptObj);
			component->DumpArray();
			ArrIfc->AssignCommandResult(TileArrayStore, result);
		}else 
		{
			Tile* component = g_interfaceManager->GetActiveTile();
			if (component)
			{
				//_MESSAGE("Extracting active HUD component.");
				TileArrayStore = ArrIfc->CreateStringMap(NULL, NULL, 0, scriptObj);
				component->DumpArray();
				ArrIfc->AssignCommandResult(TileArrayStore, result);
			}
		}
	}
	return true;
}













bool Cmd_DumpTileInfo_Execute(COMMAND_ARGS) // used as debugging function so don't care about performance.
{

	UInt32 iToFile = 0;
	alignas(16) char s_strArgTemp2[0x4000];

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iToFile, &s_strArgTemp2 ))
	{

		Tile* component = GetTargetComponent(s_strArgTemp2);

		if (component)
		{
			if (iToFile == 1)
			{
				_MESSAGE("Extracting specified HUD component.");
				component->DumpAlt();
			}
			else {
				Console_Print("Extracting specified HUD component.");
				component->Dump();
			}

		}
		else {

			Tile* component = g_interfaceManager->GetActiveTile();
			if (component)
			{
				if (iToFile == 1)
				{
					_MESSAGE("Extracting active HUD component.");
					component->DumpAlt();
				}
				else {
					Console_Print("Extracting active HUD component.");
					component->Dump();
				}

			}

		}

	}


	return true;
}



bool Cmd_DumpTileInfoAll_Execute(COMMAND_ARGS)


{

	UInt32 iToFile = 0;
	Tile* component = g_interfaceManager->menuRoot;
	Tile* component2 = g_interfaceManager->cursor;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iToFile))
	{

		if (iToFile == 1)
		{

			if (component)
			{
				_MESSAGE("Dumping ALL menus.");
				component->DumpAlt();
			}

			if (component2)
			{
				_MESSAGE("Dumping Cursor.");
				component2->DumpAlt();
			}


		}else
		{

			if (component)
			{
				Console_Print("Dumping ALL menus.");
				component->Dump();
			}

			if (component2)
			{
				Console_Print("Dumping Cursor.");
				component2->Dump();
			}


		}

	}






	return true;


}


// 0 for UI Width
// 1 for UI Height
// 2 for Real Weight
// 3 for Real Height
// 4 for Resolution Converter


bool Cmd_GetScreenTrait_Execute(COMMAND_ARGS)
{
	UInt32 iTrait = -1;
	Tile::Value* val = 0;
	Tile* component = g_interfaceManager->menuRoot;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iTrait))
	{
		if (component)
		{
			switch (iTrait)
			{
			case 0:
				val = component->GetValue(kTileValue_width);
				*result = val->num;
				break;
			case 1:
				val = component->GetValue(kTileValue_height);
				*result = val->num;
				break;
			case 2:
				*result = g_screenWidth;
				break;
			case 3:
				*result = g_screenHeight;
				break;
			case 4:
				val = component->GetValue(kTileValue_resolutionconverter);
				*result = val->num;
				break;
			}
		}
	}
	return true;
}

//traits
//0 for Alpha
//1 for Width
//2 for Height

bool Cmd_GetCursorTrait_Execute(COMMAND_ARGS) ////////////Concept taken from JiP
{
	*result = 0;
	UInt32 iTrait = -1;
	Tile::Value* val = 0;
	Tile* component = g_interfaceManager->cursor;
	const char* resStr = NULL;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iTrait))
	{
		if (component)
		{
			switch (iTrait)
			{
			case 0:
				val = component->GetValue(kTileValue_alpha);
				*result = val->num;
				break;
			case 1:
				val = component->GetValue(kTileValue_width);
				*result = val->num;
				break;
			case 2:
				val = component->GetValue(kTileValue_height);
				*result = val->num;
				break;
			case 3:
				val = component->GetValue(kTileValue_red);
				*result = val->num;
				break;
			case 4:
				val = component->GetValue(kTileValue_green);
				*result = val->num;
				break;
			case 5:
				val = component->GetValue(kTileValue_blue);
				*result = val->num;
				break;
			}
		}
	}
	return true;
}





//traits
//0 for Alpha
//1 for Width
//2 for Height
//3 for Red
//4 for Green
//5 for Blue


bool Cmd_SetCursorTrait_Execute(COMMAND_ARGS) ////////////Concept taken from JiP
{
	UInt32 iTrait;
	float fValue;
	Tile::Value* val = 0;
	Tile* component = g_interfaceManager->cursor;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &iTrait, &fValue))
	{
		if (component)
		{
			switch (iTrait)
			{
			case 0:
				component->SetFloat(kTileValue_alpha, fValue);
				break;
			case 1:
				component->SetFloat(kTileValue_width, fValue);
				break;
			case 2:
				component->SetFloat(kTileValue_height, fValue);
				break;
			case 3:
				component->SetFloat(kTileValue_red, fValue);
				break;
			case 4:
				component->SetFloat(kTileValue_green, fValue);
				break;
			case 5:
				component->SetFloat(kTileValue_blue, fValue);
				break;

			}
		}
	}
	return true;
}




bool Cmd_SetCursorTraitGradual_Execute(COMMAND_ARGS) ////////////SetUIFloatGradual By JIP
{
	UInt32 iTrait,iMode, iNumArgs =NUM_ARGS;
	float fStartVal,fEndVal,fSec;
	Tile::Value* val = 0;
	Tile* component = g_interfaceManager->cursor;
	if (ExtractArgsEx(EXTRACT_ARGS_EX,&iTrait, &fStartVal, &fEndVal, &fSec, &iMode))
	{
		if (component)
		{
			if (iNumArgs >1)
			{
				switch (iTrait)
				{
				case 0:
					component->GradualSetFloat(kTileValue_alpha, fStartVal, fEndVal, fSec, iMode);
					break;
				case 1:
					component->GradualSetFloat(kTileValue_width, fStartVal, fEndVal, fSec, iMode);
					break;
				case 2:
					component->GradualSetFloat(kTileValue_height, fStartVal, fEndVal, fSec, iMode);
					break;
				case 3:
					component->GradualSetFloat(kTileValue_red, fStartVal, fEndVal, fSec, iMode);
					break;
				case 4:
					component->GradualSetFloat(kTileValue_green, fStartVal, fEndVal, fSec, iMode);
					break;
				case 5:
					component->GradualSetFloat(kTileValue_blue, fStartVal, fEndVal, fSec, iMode);
					break;
				}
			}
			else {
				switch (iTrait)
				{
				case 0:
					ThisCall(0xA07DC0, component, kTileValue_alpha);
					break;
				case 1:
					ThisCall(0xA07DC0, component, kTileValue_width);
					break;
				case 2:
					ThisCall(0xA07DC0, component, kTileValue_height);
					break;
				case 3:
					ThisCall(0xA07DC0, component, kTileValue_red);
					break;
				case 4:
					ThisCall(0xA07DC0, component, kTileValue_green);
					break;
				case 5:
					ThisCall(0xA07DC0, component, kTileValue_blue);
					break;
				}
			}
		}
	}
	return true;
}







//float value = std::numeric_limits<float>::quiet_NaN();

///////////////////////////////////////////////////////////// For later::

//bool Cmd_SetCursorFilename_Execute(COMMAND_ARGS) ////////////Concept taken from JiP
//{
//	*result = 0;
//
//	Tile::Value* val = 0;
//	Tile* component = g_interfaceManager->cursor;
//	const char* resStr = NULL;
//	alignas(16) char s_strArgBuffer[0x4000]; // From JiPNVSE.h
//	
//
//	if (component)
//	{
//		if (ExtractArgsEx(EXTRACT_ARGS_EX, &s_strArgBuffer))
//		{
//			int length = strlen(s_strArgBuffer);
//			//Console_Print("%s %d", "LENGHT>>", length);
//
//
//			if (length > 0) {
//
//				//component->SetFloat(kTileValue_visible, 0);
//				//component->SetString(kTileValue_filename, "dfs");
//				//component->SetString(kTileValue_texatlas, "Interface\InterfaceShared1gh.tai");
//				//component->SetFloat(kTileValue_visible, 0);
//				//component->SetFloat(kTileValue_visible, 1);
//
//				
//				//component->SetFloat(kTileValue_red, 0);
//				//component->SetFloat(kTileValue_green, 0);
//				//component->SetFloat(kTileValue_blue, 255);
//
//			}
//			else {
//				//val = component->GetValue(kTileValue_filename);
//				val = component->GetValue(kTileValue_texatlas);
//				resStr = val->str;
//				StrIfc->Assign(PASS_COMMAND_ARGS, resStr);
//				Console_Print("Text >> %s", resStr);
//			}
//
//		}
//	}
//
//	return true;
//}





//Console_Print("CASE 3");



//const char kComponentTempXML[] = "Tomm_temp.xml";
//bool Cmd_SetCursorSize_Execute(COMMAND_ARGS) ////////////Concept taken from JiP
//{
//
//	float fValue = -99999999999; /////// Because no NumArgs!!! Someone PLZ HELP SOS SOS SOS SOS SOS
//	if (ExtractArgsEx(EXTRACT_ARGS_EX, &fValue))
//	{
//		Tile* component = g_interfaceManager->cursor;
//
//		if (component)
//		{
//			if (fValue != -99999999999) {
//				//Console_Print("SETTING Size");
//				component->SetFloat(kTileValue_width, fValue);
//				component->SetFloat(kTileValue_height, fValue); 
//			}else{
//				//Console_Print("GETTING Size!");
//				Tile::Value* val = component->GetValue(kTileValue_width);
//				*result = val->num;
//			}
//		}
//	}
//
//	return true;
//}




//component->Dump();
//Console_Print("Dumping Cursor BEFORE.");

//if (tempFile.Create(kComponentTempXML))
//{
	//tempFile.WriteStr("<systemcolor>0</systemcolor>");
	//tempFile.Close();
	//*result = component->ReadXML(kComponentTempXML) ? 1 : 0;
//}

//component->Dump();
//Console_Print("Dumping Cursor AFTER.");


//case 3:
	//val = component->GetValue(kTileValue_systemcolor);
	//val = component->GetCachedComponentValue(kTileValue_systemcolor);

	//*result = val->num;
	//break;
	//component->SetFloat(kTileValue_systemcolor, fValue);
	//break;

				//Console_Print("CASE 3");
				//val = component->GetValueName("filename");
				//resStr = val->str;
				//StrIfc->Assign(PASS_COMMAND_ARGS, resStr);
				//Console_Print("Text >> %s", resStr);
				//break;


//ScriptVar outX, outY;
//if (component)
//{
//	if (ExtractArgsEx(EXTRACT_ARGS_EX, &outX, &outY))
//	{
//		Tile::Value* val = component->GetValue(kTileValue_width);
//		outX->data.num = val->num;
//		val = component->GetValue(kTileValue_height);
//		outY->data.num = val->num;
//	}
//
//}

#endif
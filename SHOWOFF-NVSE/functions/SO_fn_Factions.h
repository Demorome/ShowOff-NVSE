#pragma once



static ParamInfo kParams_SetFactionCombatReaction[] =
{
	{	"faction1",	kParamType_Faction,	0	},
	{	"faction2",	kParamType_Faction,	0	},

	{	"setAllyOrEnemy (int)",kParamType_Integer,		0	},

	{	"F1toF2Flag (int)",kParamType_Integer,		1	},
	{	"F2toF1Flag (int)",kParamType_Integer,		1	},
};

DEFINE_COMMAND_PLUGIN(SetFactionCombatReactionTemp, "Sets the combat reaction between two factions. Unlike SetAlly/SetEnemy, isn't savebaked.", false, kParams_SetFactionCombatReaction);
bool Cmd_SetFactionCombatReactionTemp_Execute(COMMAND_ARGS)
{
	*result = 0; //bSuccess
	TESFaction* faction1, * faction2;
	UInt32 setAllyOrEnemy;	// emulate SetAlly or SetEnemy

	// If setAlly, and if true, then faction1 will react as a Friend to F2, otherwise Ally.
	// If setEnemy, and if true, then F2 will react as Neutral to F1, otherwise Enemy.
	UInt32 F1toF2Flag = 0;

	// (inverse faction1 with F2)
	UInt32 F2toF1Flag = 0;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &faction1, &faction2, &setAllyOrEnemy, &F1toF2Flag, &F2toF1Flag)
		&& faction1 && faction2)
	{
		faction1->SetFactionCombatReactionTemp(faction2, setAllyOrEnemy, F1toF2Flag, F2toF1Flag);
		*result = 1;
	}
	return true;
}

static ParamInfo kParams_SetAllyOrEnemyTemp[] =
{
	{	"faction1",	kParamType_Faction,	0	},
	{	"faction2",	kParamType_Faction,	0	},
	{	"F1toF2Flag (int)",kParamType_Integer,		1	},
	{	"F2toF1Flag (int)",kParamType_Integer,		1	},
};

DEFINE_COMMAND_ALT_PLUGIN(SetAllyTemp, SetAllyShowOff, "Sets the combat reaction between two factions to something friendly. Unlike SetAlly, isn't savebaked.", false, kParams_SetAllyOrEnemyTemp);
bool Cmd_SetAllyTemp_Execute(COMMAND_ARGS)
{
	*result = 0; //bSuccess
	TESFaction* faction1, *faction2;
	
	// If true, then faction1 will react as a Friend to faction2, otherwise Ally (default).
	UInt32 F1toF2Flag = 0;

	// If true, then faction2 will react as a Friend to faction1, otherwise Ally (default).
	UInt32 F2toF1Flag = 0;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &faction1, &faction2, &F1toF2Flag, &F2toF1Flag)
		&& faction1 && faction2)
	{
		faction1->SetFactionCombatReactionTemp(faction2, true, F1toF2Flag, F2toF1Flag);
		*result = 1;
	}
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN(SetEnemyTemp, SetEnemyShowOff, "Sets the combat reaction between two factions to enemy/neutral status. Unlike SetEnemy, isn't savebaked.", false, kParams_SetAllyOrEnemyTemp);
bool Cmd_SetEnemyTemp_Execute(COMMAND_ARGS)
{
	*result = 0; //bSuccess
	TESFaction* faction1, * faction2;

	// If true, then faction1 will react as a Neutral to faction2, otherwise Enemy (default).
	UInt32 F1toF2Flag = 0;

	// If true, then faction2 will react as Neutral to faction1, otherwise Enemy (default).
	UInt32 F2toF1Flag = 0;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &faction1, &faction2, &F1toF2Flag, &F2toF1Flag)
		&& faction1 && faction2)
	{
		faction1->SetFactionCombatReactionTemp(faction2, false, F1toF2Flag, F2toF1Flag);
		*result = 1;
	}
	return true;
}

/* Kinda redundant with SetReputation, as it turns out

static ParamInfo kParams_OneFaction_OneOptionalInt[] =
{
	{	"faction",	kParamType_Faction,	0	},
	{	"bFame",	kParamType_Integer,	1	},
};

DEFINE_COMMAND_ALT_PLUGIN(ClearReputation, ResetReputation, , false, kParams_OneFaction_OneOptionalInt);
bool Cmd_ClearReputation_Execute(COMMAND_ARGS)
{
	TESFaction* faction;
	UInt32 bFame = -1; // -1 = clear both reps. 0 = clear infamy. 1 = clear fame
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &faction, &bFame) && faction)
	{
		
	}
	return true;
}
*/
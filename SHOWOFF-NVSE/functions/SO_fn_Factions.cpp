#include "SO_fn_Factions.h"

#include <GameForms.h>

extern bool (*ExtractArgsEx)(COMMAND_ARGS_EX, ...);

bool Cmd_SetFactionCombatReactionTemp_Execute(COMMAND_ARGS) {
	*result = 0; //bSuccess
	TESFaction* faction1 = nullptr, * faction2 = nullptr;
	uint32_t setAllyOrEnemy;	// emulate SetAlly or SetEnemy

	// If setAlly, and if true, then faction1 will react as a Friend to F2, otherwise Ally.
	// If setEnemy, and if true, then F2 will react as Neutral to F1, otherwise Enemy.
	uint32_t F1toF2Flag = 0;

	// (inverse faction1 with F2)
	uint32_t F2toF1Flag = 0;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &faction1, &faction2, &setAllyOrEnemy, &F1toF2Flag, &F2toF1Flag)
		&& faction1 && faction2) {
		faction1->SetFactionCombatReactionTemp(faction2, setAllyOrEnemy, F1toF2Flag, F2toF1Flag);
		*result = 1;
	}
	return true;
}

bool Cmd_SetAllyTemp_Execute(COMMAND_ARGS) {
	*result = 0; //bSuccess
	TESFaction* faction1 = nullptr, * faction2 = nullptr;

	// If true, then faction1 will react as a Friend to faction2, otherwise Ally (default).
	uint32_t F1toF2Flag = 0;

	// If true, then faction2 will react as a Friend to faction1, otherwise Ally (default).
	uint32_t F2toF1Flag = 0;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &faction1, &faction2, &F1toF2Flag, &F2toF1Flag)
		&& faction1 && faction2) {
		faction1->SetFactionCombatReactionTemp(faction2, true, F1toF2Flag, F2toF1Flag);
		*result = 1;
	}
	return true;
}

bool Cmd_SetEnemyTemp_Execute(COMMAND_ARGS) {
	*result = 0; //bSuccess
	TESFaction* faction1 = nullptr, * faction2 = nullptr;

	// If true, then faction1 will react as a Neutral to faction2, otherwise Enemy (default).
	uint32_t F1toF2Flag = 0;

	// If true, then faction2 will react as Neutral to faction1, otherwise Enemy (default).
	uint32_t F2toF1Flag = 0;

	if (ExtractArgsEx(EXTRACT_ARGS_EX, &faction1, &faction2, &F1toF2Flag, &F2toF1Flag)
		&& faction1 && faction2) {
		faction1->SetFactionCombatReactionTemp(faction2, false, F1toF2Flag, F2toF1Flag);
		*result = 1;
	}
	return true;
}
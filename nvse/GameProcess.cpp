#include "GameProcess.h"

void AnimData::CreateForcedIdle(TESIdleForm* idleForm, Actor* actor, UInt32 sequenceID, int a5)
{
	ThisStdCall(0x497F20, this, idleForm, actor, sequenceID, a5);
}

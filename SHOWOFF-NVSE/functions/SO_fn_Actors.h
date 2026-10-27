#pragma once

#include <CommandTable.h>
#include <ParamInfos.h>

DEFINE_CMD_COND_PLUGIN(GetNumActorsInRangeFromRef, "Returns the amount of actors that are a certain distance nearby to the calling reference.", true, kParams_OneFloat_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(GetNumCombatActorsFromActor, "Returns the amount of actors that are allies/targets to the calling actor, with optional filters.", true, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(GetCreatureTurningSpeed, "", false, kParams_OneOptionalActorBase); 
DEFINE_COMMAND_PLUGIN(SetCreatureTurningSpeed, "", false, kParams_OneFloat_OneOptionalActorBase);
DEFINE_CMD_COND_PLUGIN(GetCreatureFootWeight, "", false, kParams_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureFootWeight, "", false, kParams_OneFloat_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureReach, "", false, kParams_OneInt_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureBaseScale, "", false, kParams_OneFloat_OneOptionalActorBase);
DEFINE_CMD_COND_PLUGIN(GetNumCompassHostiles, "Returns the amount of hostile actors on compass, w/ optional filters.", false, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetActorValueDamage, GetAVDamage, "Returns the damage modifier applied to the actor's AV", true, kParams_OneActorValue);
DEFINE_COMMAND_PLUGIN(SayTo_GetSpeakingActor, "", true, NULL);
DEFINE_COMMAND_PLUGIN(SayTo_GetTopic, "", true, NULL);
DEFINE_COMMAND_PLUGIN(SayTo_GetTopicInfo, "", true, NULL);
DEFINE_COMMAND_PLUGIN(SayTo_GetQuest, "", true, NULL);
DEFINE_COMMAND_PLUGIN(IsActorAlt, "Same as IsActor but accepts an optional baseform.", false, kParams_OneOptionalActorBase);
DEFINE_COMMAND_ALT_PLUGIN(SetSecuritronExpressionTemp, SetSecExpTemp, "", false, kParams_OneReference_TwoStrings);
DEFINE_CMD_ALT_COND_PLUGIN(GetIsPlayerOverencumbered, IsPCOverencumbered, "An alternative to SUP's IsPlayerOverencumbered", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetCalculatedActorSpread, "", true, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(HighlightAdditionalReferenceAlt, "", true, kParams_FourOptionalFloats);
DEFINE_COMMAND_PLUGIN(ForceRecoilAnim, "", true, nullptr);
DEFINE_COMMAND_ALT_PLUGIN(ForceHitStaggerReaction, ForceHitReaction, "Only works inside OnHit event handlers.", true, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetActorFadeState, "", true, nullptr);
DEFINE_CMD_COND_PLUGIN(IsActorInvisibleToPlayer, "", true, nullptr);
DEFINE_COMMAND_PLUGIN(BaseCopyFaceGenFrom, "", false, kParams_TwoActorBases);
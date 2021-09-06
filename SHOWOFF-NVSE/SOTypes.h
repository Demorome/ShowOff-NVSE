#pragma once

#include <set>
#include "LambdaVarContext.h"

typedef UInt32 RefID;
typedef RefID ActorRefID;
typedef RefID ItemRefID;

typedef std::pair<ActorRefID, ItemRefID> ActorAndItemPair;
typedef std::set<ActorAndItemPair> ActorAndItemPairs;


// Retrived via scriptObj->GetOverridingModIdx() in functions.
typedef UInt8 ModID;

typedef std::map<ModID, Function> mod_and_function_map;
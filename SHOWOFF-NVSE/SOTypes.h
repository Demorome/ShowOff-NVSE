#pragma once

#include <set>
#include <variant>

#include "LambdaVarContext.h"

typedef UInt32 RefID;
typedef RefID ActorRefID;
typedef RefID ItemRefID;
enum class StrongFormID : UInt32 {};

typedef std::pair<ActorRefID, ItemRefID> ActorAndItemPair;
typedef std::set<ActorAndItemPair> ActorAndItemPairs;


// Retrived via scriptObj->GetOverridingModIdx() in functions.
typedef UInt8 ModID;

typedef std::map<ModID, Function> mod_and_function_map;


using ScriptValue = std::variant<double, StrongFormID, std::string, NVSEArrayVar*>;
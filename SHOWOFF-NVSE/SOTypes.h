#pragma once

#include <set>

typedef UInt32 RefID;
typedef RefID ActorRefID;
typedef RefID ItemRefID;

typedef std::pair<ActorRefID, ItemRefID> ActorAndItemPair;
typedef std::set<ActorAndItemPair> ActorAndItemPairs;
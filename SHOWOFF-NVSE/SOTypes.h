#pragma once

#include <set>
#include <variant>
#include <deque>
#include <algorithm>

#include "LambdaVarContext.h"
#include "PluginAPI.h"

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

template <typename T>
class NumberModifications
{
	std::deque<T> mults;
	std::deque<T> divs;
	std::deque<T> adds;
	std::deque<T> subs;

public:

	enum ModType
	{
		kMult,
		kDiv,
		kAdd,
		kSub,

		kInvalid = -1
	};

	bool TryAddMod(T mod, ModType modType);
	bool TryAddMod(T mod, char modType);

	void ModValue(T &valToMod);

	void Clear()
	{
		mults.clear();
		divs.clear();
		adds.clear();
		subs.clear();
	}

	NumberModifications() = default;
	~NumberModifications() = default;
};




// Template definitions

template <typename T>
bool NumberModifications<T>::TryAddMod(T mod, ModType modType)
{
	switch (modType)
	{
	case kMult:
		mults.push_back(mod);
		break;
	case kDiv:
		if (mod == 0) //avoid division by 0
			return false;
		divs.push_back(mod);
		break;
	case kAdd:
		adds.push_back(mod);
		break;
	case kSub:
		subs.push_back(mod);
		break;
	case kInvalid:
	default:
		return false;
	}
	return true;
}

template <typename T>
bool NumberModifications<T>::TryAddMod(T mod, char modType)
{
	switch (modType)
	{
	case '*':
		mults.push_back(mod);
		break;
	case '/':
		if (mod == 0) //avoid division by 0
			return false;
		divs.push_back(mod);
		break;
	case '+':
		adds.push_back(mod);
		break;
	case '-':
		subs.push_back(mod);
		break;
	default:
		return false;
	}
	return true;
}

template <typename T>
void NumberModifications<T>::ModValue(T &valToMod)
{
	std::ranges::for_each(mults, [&](T& mult) { valToMod *= mult; });
	std::ranges::for_each(divs, [&](T& div) { valToMod /= div; });
	std::ranges::for_each(adds, [&](T& add) { valToMod += add; });
	std::ranges::for_each(subs, [&](T& sub) { valToMod -= sub; });
}

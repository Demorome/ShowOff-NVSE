#pragma once

#include <stdexcept>
#include <unordered_map>
#include <string_view>

#if 0
using std::unordered_map;
using internal_name_str_t = std::string_view;
using std::string;

// https://stackoverflow.com/a/53945555
template<class> inline constexpr bool always_false_v = false;

// The values are searched by internal name, which might include part of the INI section it was in.
class StoredINI
{
	unordered_map<bool, internal_name_str_t>	bools;
	unordered_map<int32_t, internal_name_str_t> ints;
	unordered_map<double, internal_name_str_t>	floats;
	unordered_map<string, internal_name_str_t>	strings;

public:
	template <typename T>
	bool TryGetValue(internal_name_str_t& key, T* outValue = nullptr)
	{
		if (key.empty())
			return false;
		
		constexpr string_view badPrefixErrStr = "Key "
		if constexpr (std::is_same_v<T, bool>)
		{
			if (key.front() != 'b')
				throw std::logic_error();
		}
		else if constexpr (std::is_same_v<T, int32_t>)
		{
			if (key.front() != 'i')
				throw std::logic_error();
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			
		}
		else if constexpr (std::is_same_v<T, string>)
		{
			
		}
		else
		{
			static_assert(always_false_v, "StoredINI >> Invalid type for outValue!");
		}
	}
};

extern StoredINI g_StoredINI;
#endif
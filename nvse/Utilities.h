#pragma once
#include <string>

template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret ThisCall(uint32_t _addr, const void* _this, Args ...args) noexcept(false) {
	if constexpr (std::is_class_v<T_Ret>) {
		T_Ret ret;
		((T_Ret * (__thiscall*)(const void*, T_Ret&, Args...))_addr)(_this, ret, std::forward<Args>(args)...);
		return ret;
	}
	else {
		return ((T_Ret(__thiscall*)(const void*, Args...))_addr)(_this, std::forward<Args>(args)...);
	}
}

template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret StdCall(uint32_t _addr, Args ...args) noexcept(false) {
	return ((T_Ret(__stdcall*)(Args...))_addr)(std::forward<Args>(args)...);
}

template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret CdeclCall(uint32_t _addr, Args ...args) noexcept(false) {
	return ((T_Ret(__cdecl*)(Args...))_addr)(std::forward<Args>(args)...);
}

template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret FastCall(uint32_t _addr, Args ...args) noexcept(false) {
	return ((T_Ret(__fastcall*)(Args...))_addr)(std::forward<Args>(args)...);
}

class Script;

const std::string & GetFalloutDirectory(void);
//std::string GetNVSEConfigOption(const char * section, const char * key);
//bool GetNVSEConfigOption_UInt32(const char * section, const char * key, uint32_t * dataOut);

// this has been tested to work for non-varargs functions
// varargs functions end up with 'this' passed as the last parameter (ie. probably broken)
// do NOT use with classes that have multiple inheritance

// if many member functions are to be declared, use MEMBER_FN_PREFIX to create a type with a known name
// so it doesn't need to be restated throughout the member list

// all of the weirdness with the _GetType function is because you can't declare a static const pointer
// inside the class definition. inlining automatically makes the function call go away since it's a const

#define MEMBER_FN_PREFIX(className)	\
	typedef className _MEMBER_FN_BASE_TYPE

#define DEFINE_MEMBER_FN_LONG(className, functionName, retnType, address, ...)		\
	typedef retnType (className::* _##functionName##_type)(__VA_ARGS__);			\
																					\
	inline _##functionName##_type * _##functionName##_GetPtr(void)					\
	{																				\
		static const uint32_t _address = address;										\
		return (_##functionName##_type *)&_address;									\
	}

#define DEFINE_MEMBER_FN(functionName, retnType, address, ...)	\
	DEFINE_MEMBER_FN_LONG(_MEMBER_FN_BASE_TYPE, functionName, retnType, address, __VA_ARGS__)

#define CALL_MEMBER_FN(obj, fn)	\
	((*(obj)).*(*((obj)->_##fn##_GetPtr())))


// ConsolePrint() limited to 512 chars; use this to print longer strings to console
//void Console_Print_Long(const std::string& str);

// Macro for debug output to console at runtime
#if RUNTIME
#ifdef _DEBUG
#define DEBUG_PRINT(x, ...) { Console_Print((x), __VA_ARGS__); }
#define DEBUG_MESSAGE(x, ...) { _MESSAGE((x), __VA_ARGS__); }
#else
#define DEBUG_PRINT(x, ...) { }
#define DEBUG_MESSAGE(x, ...) { }
#endif	//_DEBUG
#else
#define DEBUG_PRINT(x, ...) { }
#define DEBUG_MESSAGE(x, ...) { }
// This is so we don't have to handle size change with EditorData :)
#undef static_assert
#define static_assert(a)
#endif	// RUNTIME

class TESForm;

#define SIZEOF_ARRAY(arrayName, elementType) (sizeof(arrayName) / sizeof(elementType))

// this copies the string onto the FormHeap - used to work around alloc/dealloc mismatch when passing
// data between nvse and plugins
char* CopyCString(const char* src);

std::string FormatString(const char* fmt, ...);


// Definitions below are from JIP
#define GAME_HEAP_ALLOC __asm mov ecx, 0x11F6238 CALL_EAX(0xAA3E40)
#define GAME_HEAP_FREE  __asm mov ecx, 0x11F6238 CALL_EAX(0xAA4060)

#define GameHeapAlloc(size) ThisCall<void*>(0xAA3E40, (void*)0x11F6238, size)
#define GameHeapFree(ptr) ThisCall<void*>(0xAA4060, (void*)0x11F6238, ptr)

#define GetRandomInt(n) ThisCall<int32_t, int32_t>(0xAA5230, (void*)0x11C4180, n)
#define GetRandomIntInRange(iMin, iMax) (GetRandomInt(iMax - iMin) + iMin) 
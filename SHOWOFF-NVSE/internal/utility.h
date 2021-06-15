#pragma once
#include <stdexcept>
#include <string>

#include "GameAPI.h"
#include "GameData.h"

// Copied many things over from JIP LN.

const double
kDblZero = 0,
kDblPI = 3.141592653589793,
kDblPIx2 = 6.283185307179586,
kDblPIx3d2 = 4.71238898038469,
kDblPId2 = 1.5707963267948966,
kDblPId4 = 0.7853981633974483,
kDblPId6 = 0.5235987755982989,
kDblPId12 = 0.26179938779914946,
kDbl2dPI = 0.6366197723675814,
kDbl4dPI = 1.2732395447351628,
kDblTanPId6 = 0.5773502691896257,
kDblTanPId12 = 0.2679491924311227,
kDblPId180 = 0.017453292519943295;

const float
kFltZero = 0.0F,
kFltHalf = 0.5F,
kFltOne = 1.0F,
kFltTwo = 2.0F,
kFltFour = 4.0F,
kFltSix = 6.0F,
kFlt10 = 10.0F,
kFlt100 = 100.0F,
kFlt2048 = 2048.0F,
kFlt4096 = 4096.0F,
kFlt10000 = 10000.0F,
kFlt12288 = 12288.0F,
kFlt40000 = 40000.0F,
kFltMax = FLT_MAX;

// JIP assembly definitions.
#define CALL_EAX(addr) __asm mov eax, addr __asm call eax
#define JMP_EAX(addr)  __asm mov eax, addr __asm jmp eax
#define JMP_EDX(addr)  __asm mov edx, addr __asm jmp edx

// These are used for 10h aligning segments in ASM code (massive performance gain, particularly with loops).
#define EMIT(bt) __asm _emit bt
#define NOP_0x1 EMIT(0x90)
//	"\x90"
#define NOP_0x2 EMIT(0x66) NOP_0x1
//	"\x66\x90"
#define NOP_0x3 EMIT(0x0F) EMIT(0x1F) EMIT(0x00)
//	"\x0F\x1F\x00"
#define NOP_0x4 EMIT(0x0F) EMIT(0x1F) EMIT(0x40) EMIT(0x00)
//	"\x0F\x1F\x40\x00"
#define NOP_0x5 EMIT(0x0F) EMIT(0x1F) EMIT(0x44) EMIT(0x00) EMIT(0x00)
//	"\x0F\x1F\x44\x00\x00"
#define NOP_0x6 EMIT(0x66) NOP_0x5
//	"\x66\x0F\x1F\x44\x00\x00"
#define NOP_0x7 EMIT(0x0F) EMIT(0x1F) EMIT(0x80) EMIT(0x00) EMIT(0x00) EMIT(0x00) EMIT(0x00)
//	"\x0F\x1F\x80\x00\x00\x00\x00"
#define NOP_0x8 EMIT(0x0F) EMIT(0x1F) EMIT(0x84) EMIT(0x00) EMIT(0x00) EMIT(0x00) EMIT(0x00) EMIT(0x00)
//	"\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0x9 EMIT(0x66) NOP_0x8
//	"\x66\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0xA EMIT(0x66) NOP_0x9
//	"\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0xB EMIT(0x66) NOP_0xA
//	"\x66\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0xC NOP_0x8 NOP_0x4
#define NOP_0xD NOP_0x8 NOP_0x5
#define NOP_0xE NOP_0x7 NOP_0x7
#define NOP_0xF NOP_0x8 NOP_0x7

#define GAME_HEAP_ALLOC __asm mov ecx, 0x11F6238 CALL_EAX(0xAA3E40)
#define GAME_HEAP_FREE  __asm mov ecx, 0x11F6238 CALL_EAX(0xAA4060)

#define GameHeapAlloc(size) ThisStdCall<void*>(0xAA3E40, (void*)0x11F6238, size)
#define GameHeapFree(ptr) ThisStdCall<void*>(0xAA4060, (void*)0x11F6238, ptr)

#define GetRandomInt(n) ThisStdCall<SInt32, SInt32>(0xAA5230, (void*)0x11C4180, n)
#define GetRandomIntInRange(iMin, iMax) (GetRandomInt(iMax - iMin) + iMin) 


// LightCS definitions taken from JG
class LightCS
{
	UInt32	owningThread;
	UInt32	enterCount;

public:
	LightCS() : owningThread(0), enterCount(0) {}

	void Enter();
	void EnterSleep();
	void Leave();
};


bool fCompare(float lval, float rval);  //copied from JG

typedef void* (__cdecl* memcpy_t)(void*, const void*, size_t);
extern memcpy_t MemCopy, MemMove;

union Coordinate
{
	UInt32		xy;
	struct
	{
		SInt16	y;
		SInt16	x;
	};

	Coordinate() {}
	Coordinate(SInt16 _x, SInt16 _y) : x(_x), y(_y) {}

	inline Coordinate& operator =(const Coordinate &rhs)
	{
		xy = rhs.xy;
		return *this;
	}
	inline Coordinate& operator =(const UInt32 &rhs)
	{
		xy = rhs;
		return *this;
	}

	inline bool operator ==(const Coordinate &rhs) {return xy == rhs.xy;}
	inline bool operator !=(const Coordinate &rhs) {return xy != rhs.xy;}

	inline Coordinate operator +(const char *rhs)
	{
		return Coordinate(x + rhs[0], y + rhs[1]);
	}
};

template <typename T1, typename T2> inline T1 GetMin(T1 value1, T2 value2)
{
	return (value1 < value2) ? value1 : value2;
}

template <typename T1, typename T2> inline T1 GetMax(T1 value1, T2 value2)
{
	return (value1 > value2) ? value1 : value2;
}

template <typename T> inline T sqr(T value)
{
	return value * value;
}

bool fCompare(float lval, float rval);

int __stdcall lfloor(float value);
int __stdcall lceil(float value);

float __stdcall fSqrt(float value);
double __stdcall dSqrt(double value);

double dCos(double angle);
double dSin(double angle);
double dTan(double angle);

double dAtan(double value);
double dAsin(double value);
double dAcos(double value);
double dAtan2(double y, double x);

UInt32 __fastcall GetNextPrime(UInt32 num);

UInt32 __fastcall RGBHexToDec(UInt32 rgb);

UInt32 __fastcall RGBDecToHex(UInt32 rgb);

UInt32 __fastcall StrLen(const char *str);

char* __fastcall StrEnd(const char *str);

bool __fastcall MemCmp(const void *ptr1, const void *ptr2, UInt32 bsize);

void __fastcall MemZero(void *dest, UInt32 bsize);

char* __fastcall StrCopy(char *dest, const char *src);

char* __fastcall StrNCopy(char *dest, const char *src, UInt32 length);

char* __fastcall StrCat(char *dest, const char *src);

UInt32 __fastcall StrHash(const char *inKey);

bool __fastcall CmprLetters(const char *lstr, const char *rstr);

bool __fastcall StrEqualCS(const char *lstr, const char *rstr);

bool __fastcall StrEqualCI(const char *lstr, const char *rstr);

char __fastcall StrCompare(const char *lstr, const char *rstr);

char __fastcall StrBeginsCS(const char *lstr, const char *rstr);

char __fastcall StrBeginsCI(const char *lstr, const char *rstr);

void __fastcall FixPath(char *str);

void __fastcall StrToLower(char *str);

void __fastcall ReplaceChr(char *str, char from, char to);

char* __fastcall FindChr(const char *str, char chr);

char* __fastcall FindChrR(const char *str, UInt32 length, char chr);

char* __fastcall SubStr(const char *srcStr, const char *subStr);

char* __fastcall SlashPos(const char *str);

char* __fastcall SlashPosR(const char *str);

char* __fastcall GetNextToken(char *str, char delim);

char* __fastcall GetNextToken(char *str, const char *delims);

char* __fastcall GetNextTokenJIP(char* str, char delim);

char* __fastcall CopyString(const char *key);

char* __fastcall IntToStr(int num, char *str);

char* __fastcall FltToStr(float num, char *str);

int __fastcall StrToInt(const char *str);

double __fastcall StrToDbl(const char *str);

char* __fastcall UIntToHex(UInt32 num, char *str);

UInt32 __fastcall HexToUInt(const char *str);

//Begin JIP string / char stuff

extern const UInt8 kLwrCaseConverter[], kUprCaseConverter[];
extern char* GetStrArgBuffer();

//End JIP string / char stuff

bool __fastcall FileExists(const char *path);

class FileStream //Also check FileStreamJIP!
{
protected:
	HANDLE		theFile;
	UInt32		streamLength;
	UInt32		streamOffset;

public:
	FileStream() : theFile(INVALID_HANDLE_VALUE), streamLength(0), streamOffset(0) {}
	~FileStream() {if (theFile != INVALID_HANDLE_VALUE) Close();}

	bool Good() const {return theFile != INVALID_HANDLE_VALUE;}
	HANDLE GetHandle() const {return theFile;}
	UInt32 GetLength() const {return streamLength;}
	UInt32 GetOffset() const {return streamOffset;}
	bool HitEOF() const {return streamOffset >= streamLength;}

	bool Open(const char *filePath);
	bool OpenAt(const char *filePath, UInt32 inOffset);
	bool OpenWrite(const char *filePath);
	bool Create(const char *filePath);
	bool OpenWriteEx(char *filePath, bool append);
	void SetOffset(UInt32 inOffset);

	void Close()
	{
		CloseHandle(theFile);
		theFile = INVALID_HANDLE_VALUE;
	}

	void ReadBuf(void *outData, UInt32 inLength);
	void WriteBuf(const void *inData, UInt32 inLength);

	static void MakeAllDirs(char *fullPath);
};

//Begin JIP stuff

class FileStreamJIP
{
	FILE* theFile;

public:
	FileStreamJIP() : theFile(NULL) {}
	~FileStreamJIP() { if (theFile) fclose(theFile); }

	bool Open(const char* filePath);
	bool OpenAt(const char* filePath, UInt32 inOffset);
	bool OpenWrite(char* filePath, bool append);
	bool Create(const char* filePath);
	void SetOffset(UInt32 inOffset);

	void Close()
	{
		fclose(theFile);
		theFile = NULL;
	}

	UInt32 GetLength();
	char ReadChar();
	void ReadBuf(void* outData, UInt32 inLength);
	void WriteChar(char chr);
	void WriteStr(const char* inStr);
	void WriteBuf(const void* inData, UInt32 inLength);

	static void MakeAllDirs(char* fullPath);
};

static const char kIndentLevelStr[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

//End of JIP stuff

class LineIterator
{
protected:
	char	*dataPtr;

public:
	LineIterator(const char *filePath, char *buffer);

	bool End() const {return *dataPtr == 3;}
	void Next();
	char *Get() {return dataPtr;}
};

class DirectoryIterator
{
	HANDLE				handle;
	WIN32_FIND_DATA		fndData;

public:
	DirectoryIterator(const char *path) : handle(FindFirstFile(path, &fndData)) {}
	~DirectoryIterator() {Close();}

	bool End() const {return handle == INVALID_HANDLE_VALUE;}
	void Next() {if (!FindNextFile(handle, &fndData)) Close();}
	bool IsFile() const {return !(fndData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);}
	bool IsFolder() const
	{
		if (IsFile()) return false;
		if (fndData.cFileName[0] != '.') return true;
		if (fndData.cFileName[1] != '.') return fndData.cFileName[1] != 0;
		return fndData.cFileName[2] != 0;
	}
	const char *Get() const {return fndData.cFileName;}
	void Close()
	{
		if (handle != INVALID_HANDLE_VALUE)
		{
			FindClose(handle);
			handle = INVALID_HANDLE_VALUE;
		}
	}
};

bool FileToBuffer(const char *filePath, char *buffer);

void __fastcall GetTimeStamp(char *buffer);

// ConsolePrint() limited to 512 chars; use this to print longer strings to console
void Console_Print_Long(const std::string& str);

UInt32 __fastcall ByteSwap(UInt32 dword);

void DumpMemImg(void *data, UInt32 size, UInt8 extra = 0);

// From JIP
#define AUX_BUFFER_INIT_SIZE 0x8000

class AuxBuffer
{
	UInt8	*ptr;
	UInt32	size;

public:
	AuxBuffer() : ptr(NULL), size(AUX_BUFFER_INIT_SIZE) {}
};

extern AuxBuffer s_auxBuffers[3];

UInt8* __fastcall GetAuxBuffer(AuxBuffer& buffer, UInt32 reqSize);


// From kNVSE
inline void Log(const std::string& msg)
{
	_MESSAGE(msg.c_str());
}

// From kNVSE
inline int HexStringToInt(const std::string& str)
{
	char* p;
	const auto id = strtoul(str.c_str(), &p, 16);
	if (*p == 0)
		return id;
	return -1;
}

// From kNVSE
inline void DebugPrint(const std::string& str)
{
#if _DEBUG
	Console_Print(str.c_str());
#endif
	Log(str);
}

// From kNVSE
inline std::string GetCurPath()
{
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	return path;
}

inline float TryConvertStrToFloat(std::string const &str)
{
	float val = NAN;
	try
	{
		val = stof(str);
	}
	catch (const std::invalid_argument& ia)
	{
		_MESSAGE("Conversion of Str to Double failed (IA): %s", ia.what());
	}
	catch (const std::out_of_range& oor)
	{
		_MESSAGE("Conversion of Str to Double failed (OOR): %s", oor.what());
	}
	return val;
}

// Copied from kNVSE (https://github.com/korri123/kNVSE/blob/master/nvse_plugin_example/commands_animation.cpp).
TESForm* TryConvertStrToForm(std::string const &modName, std::string const &formIdStr)
{
	const auto* mod = DataHandler::Get()->LookupModByName(modName.c_str());
	if (!mod)
	{
		Log("TryConvertStrToForm - Mod name " + modName + " was not found");
	}
	auto formId = HexStringToInt(formIdStr);
	if (formId == -1)
	{
		Log("TryConvertStrToForm - FormID formatted, got " + formIdStr);
	}
	formId = (mod->modIndex << 24) + (formId & 0x00FFFFFF);
	auto* form = LookupFormByID(formId);
	if (!form)
	{
		Log(FormatString("TryConvertStrToForm - Form %X was not found", formId));
	}
	return form;
}


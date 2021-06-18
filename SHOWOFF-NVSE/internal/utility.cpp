#include "utility.h"

#include <ctime>
#include <stdexcept>

#include "GameData.h"
#include "GameProcess.h"


// From JIP
memcpy_t MemCopy = memcpy, MemMove = memmove;

// From JIP
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

// From JIP
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

// From JG
void LightCS::Enter()
{
	UInt32 threadID = GetCurrentThreadId();
	if (owningThread == threadID)
	{
		enterCount++;
		return;
	}
	while (InterlockedCompareExchange(&owningThread, threadID, 0));
	enterCount = 1;
}

#define FAST_SLEEP_COUNT 10000UL

void LightCS::EnterSleep()
{
	UInt32 threadID = GetCurrentThreadId();
	if (owningThread == threadID)
	{
		enterCount++;
		return;
	}
	UInt32 fastIdx = FAST_SLEEP_COUNT;
	while (InterlockedCompareExchange(&owningThread, threadID, 0))
	{
		if (fastIdx)
		{
			fastIdx--;
			Sleep(0);
		}
		else Sleep(1);
	}
	enterCount = 1;
}

void LightCS::Leave()
{
	if (!--enterCount)
		owningThread = 0;
}

// From JG
bool fCompare(float lval, float rval)
{
	return fabs(lval - rval) < FLT_EPSILON;
}

__declspec(naked) int __stdcall lfloor(float value)
{
	__asm
	{
		fld		dword ptr [esp+4]
		fstcw	[esp+4]
		mov		dx, [esp+4]
		or		word ptr [esp+4], 0x400
		fldcw	[esp+4]
		fistp	dword ptr [esp+4]
		mov		eax, [esp+4]
		mov		[esp+4], dx
		fldcw	[esp+4]
		retn	4
	}
}

__declspec(naked) int __stdcall lceil(float value)
{
	__asm
	{
		fld		dword ptr [esp+4]
		fstcw	[esp+4]
		mov		dx, [esp+4]
		or		word ptr [esp+4], 0x800
		fldcw	[esp+4]
		fistp	dword ptr [esp+4]
		mov		eax, [esp+4]
		mov		[esp+4], dx
		fldcw	[esp+4]
		retn	4
	}
}

__declspec(naked) float __stdcall fSqrt(float value)
{
	__asm
	{
		fld		dword ptr [esp+4]
		fsqrt
		retn	4
	}
}

__declspec(naked) double __stdcall dSqrt(double value)
{
	__asm
	{
		fld		qword ptr [esp+4]
		fsqrt
		retn	8
	}
}

double cos_p(double angle)
{
	angle *= angle;
	return 0.999999953464 + angle * (angle * (0.0416635846769 + angle * (0.00002315393167 * angle - 0.0013853704264)) - 0.499999053455);
}

double dCos(double angle)
{
	if (angle < 0) angle = -angle;
	while (angle > kDblPIx2)
		angle -= kDblPIx2;

	int quad = int(angle * kDbl2dPI);
	switch (quad)
	{
		case 0:
			return cos_p(angle);
		case 1:
			return -cos_p(kDblPI - angle);
		case 2:
			return -cos_p(angle - kDblPI);
		default:
			return cos_p(kDblPIx2 - angle);
	}
}

double dSin(double angle)
{
	return dCos(kDblPId2 - angle);
}


double tan_p(double angle)
{
	angle *= kDbl4dPI;
	double ang2 = angle * angle;
	return angle * (211.849369664121 - 12.5288887278448 * ang2) / (269.7350131214121 + ang2 * (ang2 - 71.4145309347748));
}

double dTan(double angle)
{
	while (angle > kDblPIx2)
		angle -= kDblPIx2;

	int octant = int(angle * kDbl4dPI);
	switch (octant)
	{
		case 0:
			return tan_p(angle);
		case 1:
			return 1.0 / tan_p(kDblPId2 - angle);
		case 2:
			return -1.0 / tan_p(angle - kDblPId2);
		case 3:
			return -tan_p(kDblPI - angle);
		case 4:
			return tan_p(angle - kDblPI);
		case 5:
			return 1.0 / tan_p(kDblPIx3d2 - angle);
		case 6:
			return -1.0 / tan_p(angle - kDblPIx3d2);
		default:
			return -tan_p(kDblPIx2 - angle);
	}
}

double dAtan(double value)
{
	bool sign = (value < 0);
	if (sign) value = -value;

	bool complement = (value > 1.0);
	if (complement) value = 1.0 / value;

	bool region = (value > kDblTanPId12);
	if (region)
		value = (value - kDblTanPId6) / (1.0 + kDblTanPId6 * value);

	double res = value;
	value *= value;
	res *= (1.6867629106 + value * 0.4378497304) / (1.6867633134 + value);

	if (region) res += kDblPId6;
	if (complement) res = kDblPId2 - res;

	return sign ? -res : res;
}

double dAsin(double value)
{
	__asm
	{
		fld		value
		fld		st
		fmul	st, st
		fld1
		fsubrp	st(1), st
		fsqrt
		fdivp	st(1), st
		fstp	value
	}
	return dAtan(value);
}

double dAcos(double value)
{
	return kDblPId2 - dAsin(value);
}

double dAtan2(double y, double x)
{
	if (x != 0)
	{
		double z = y / x;
		if (x > 0)
			return dAtan(z);
		else if (y < 0)
			return dAtan(z) - kDblPI;
		else
			return dAtan(z) + kDblPI;
	}
	else if (y > 0)
		return kDblPId2;
	else if (y < 0)
		return -kDblPId2;
	return 0;
}

UInt32 __fastcall GetNextPrime(UInt32 num)
{
	if (num <= 2) return 2;
	else if (num == 3) return 3;
	UInt32 a = num / 6, b = num - (6 * a), c = (b < 2) ? 1 : 5, d;
	num = (6 * a) + c;
	a = (3 + c) / 2;
	do {
		b = 4;
		c = 5;
		do {
			d = num / c;
			if (c > d) return num;
			if (num == (c * d)) break;
			c += b ^= 6;
		} while (true);
		num += a ^= 6;
	} while (true);
	return num;
}

__declspec(naked) UInt32 __fastcall RGBHexToDec(UInt32 rgb)
{
	__asm
	{
		movzx	eax, cl
		imul	eax, 0xF4240
		movzx	edx, ch
		imul	edx, 0x3E8
		add		eax, edx
		shr		ecx, 0x10
		add		eax, ecx
		retn
	}
}

__declspec(naked) UInt32 __fastcall RGBDecToHex(UInt32 rgb)
{
	__asm
	{
		push	ebx
		mov		eax, ecx
		mov		ecx, 0xF4240
		cdq
		idiv	ecx
		mov		ebx, eax
		mov		eax, edx
		mov		ecx, 0x3E8
		cdq
		idiv	ecx
		shl		eax, 8
		add		eax, ebx
		shl		edx, 0x10
		add		eax, edx
		pop		ebx
		retn
	}
}

__declspec(naked) UInt32 __fastcall StrLen(const char *str)
{
	__asm
	{
		mov		eax, ecx
		test	ecx, ecx
		jz		done
	iterHead:
		cmp		[eax], 0
		jz		done
		inc		eax
		jmp		iterHead
	done:
		sub		eax, ecx
		retn
	}
}

__declspec(naked) char* __fastcall StrEnd(const char *str)
{
	__asm
	{
		mov		eax, ecx
		test	ecx, ecx
		jz		done
	iterHead:
		cmp		[eax], 0
		jz		done
		inc		eax
		jmp		iterHead
	done:
		retn
	}
}

__declspec(naked) bool __fastcall MemCmp(const void *ptr1, const void *ptr2, UInt32 bsize)
{
	__asm
	{
		push	ebx
		mov		ebx, [esp+8]
	iterHead:
		mov		eax, [ecx]
		cmp		[edx], eax
		jnz		retnFalse
		add		ecx, 4
		add		edx, 4
		sub		ebx, 4
		jnz		iterHead
		mov		al, 1
		jmp		done
	retnFalse:
		xor		al, al
	done:
		pop		ebx
		retn	4
	}
}

__declspec(naked) void __fastcall MemZero(void *dest, UInt32 bsize)
{
	__asm
	{
		test	ecx, ecx
		jz		done
		pxor	xmm0, xmm0
	copy16:
		cmp		edx, 0x10
		jb		copy8
		movdqu	xmmword ptr [ecx], xmm0
		add		ecx, 0x10
		sub		edx, 0x10
		jnz		copy16
		jmp		done
	copy8:
		test	dl, dl
		jz		done
		cmp		dl, 8
		jb		copy4
		movq	qword ptr [ecx], xmm0
		add		ecx, 8
		sub		dl, 8
		jz		done
	copy4:
		mov		dword ptr [ecx], 0
	done:
		retn
	}
}

void* (__cdecl *_memcpy)(void *destination, const void *source, size_t num) = memcpy;

__declspec(naked) char* __fastcall StrCopy(char *dest, const char *src)
{
	__asm
	{
		push	ebx
		mov		eax, ecx
		test	ecx, ecx
		jz		done
		test	edx, edx
		jz		nullTerm
		xor		ebx, ebx
	getSize:
		cmp		[edx+ebx], 0
		jz		doCopy
		inc		ebx
		jmp		getSize
	doCopy:
		push	ebx
		push	edx
		push	eax
		call	_memcpy
		add		esp, 0xC
		add		eax, ebx
	nullTerm:
		mov		[eax], 0
	done:
		pop		ebx
		retn
	}
}

__declspec(naked) char* __fastcall StrNCopy(char *dest, const char *src, UInt32 length)
{
	__asm
	{
		push	ebx
		mov		eax, ecx
		test	ecx, ecx
		jz		done
		test	edx, edx
		jz		nullTerm
		mov		ecx, [esp+8]
		xor		ebx, ebx
	getSize:
		cmp		[edx+ebx], 0
		jz		doCopy
		inc		ebx
		cmp		ebx, ecx
		jb		getSize
	doCopy:
		push	ebx
		push	edx
		push	eax
		call	_memcpy
		add		esp, 0xC
		add		eax, ebx
	nullTerm:
		mov		[eax], 0
	done:
		pop		ebx
		retn	4
	}
}

__declspec(naked) char* __fastcall StrCat(char *dest, const char *src)
{
	__asm
	{
		call	StrEnd
		mov		ecx, eax
		call	StrCopy
		retn
	}
}

__declspec(naked) UInt32 __fastcall StrHash(const char *inKey)
{
	__asm
	{
		xor		eax, eax
		test	ecx, ecx
		jz		done
		xor		edx, edx
	iterHead:
		mov		dl, [ecx]
		cmp		dl, 'Z'
		jg		notCap
		test	dl, dl
		jz		done
		cmp		dl, 'A'
		jl		notCap
		or		dl, 0x20
	notCap:
		imul	eax, 0x65
		add		eax, edx
		inc		ecx
		jmp		iterHead
	done:
		retn
	}
}

__declspec(naked) bool __fastcall CmprLetters(const char *lstr, const char *rstr)
{
	__asm
	{
		mov		al, [ecx]
		cmp		[edx], al
		jz		retnTrue
		cmp		al, 'A'
		jl		retnFalse
		cmp		al, 'z'
		jg		retnFalse
		cmp		al, 'Z'
		jle		isCap
		cmp		al, 'a'
		jl		retnFalse
	isCap:
		xor		al, 0x20
		cmp		[edx], al
		jz		retnTrue
	retnFalse:
		xor		al, al
		retn
	retnTrue:
		mov		al, 1
		retn
	}
}

__declspec(naked) bool __fastcall StrEqualCS(const char *lstr, const char *rstr)
{
	__asm
	{
		push	esi
		push	edi
		test	ecx, ecx
		jz		retnFalse
		test	edx, edx
		jz		retnFalse
		mov		esi, ecx
		call	StrLen
		mov		edi, eax
		mov		ecx, edx
		call	StrLen
		cmp		eax, edi
		jnz		retnFalse
		mov		edx, esi
	iterHead:
		mov		al, [ecx]
		test	al, al
		jz		retnTrue
		cmp		[edx], al
		jnz		retnFalse
		inc		ecx
		inc		edx
		jmp		iterHead
	retnTrue:
		mov		al, 1
		pop		edi
		pop		esi
		retn
	retnFalse:
		xor		al, al
		pop		edi
		pop		esi
		retn
	}
}

__declspec(naked) bool __fastcall StrEqualCI(const char *lstr, const char *rstr)
{
	__asm
	{
		push	esi
		push	edi
		test	ecx, ecx
		jz		retnFalse
		test	edx, edx
		jz		retnFalse
		mov		esi, ecx
		call	StrLen
		mov		edi, eax
		mov		ecx, edx
		call	StrLen
		cmp		eax, edi
		jnz		retnFalse
		mov		edx, esi
	iterHead:
		cmp		[ecx], 0
		jz		retnTrue
		call	CmprLetters
		test	al, al
		jz		retnFalse
		inc		ecx
		inc		edx
		jmp		iterHead
	retnTrue:
		mov		al, 1
		pop		edi
		pop		esi
		retn
	retnFalse:
		xor		al, al
		pop		edi
		pop		esi
		retn
	}
}

__declspec(naked) char __fastcall StrCompare(const char *lstr, const char *rstr)
{
	__asm
	{
		push	ebx
		test	ecx, ecx
		jnz		proceed
		test	edx, edx
		jz		retnEQ
		jmp		retnLT
	proceed:
		test	edx, edx
		jz		retnGT
	iterHead:
		mov		al, [ecx]
		mov		bl, [edx]
		test	al, al
		jz		iterEnd
		cmp		al, bl
		jz		iterNext
		cmp		al, 'Z'
		jg		lNotCap
		cmp		al, 'A'
		jl		lNotCap
		or		al, 0x20
	lNotCap:
		cmp		bl, 'Z'
		jg		rNotCap
		cmp		bl, 'A'
		jl		rNotCap
		or		bl, 0x20
	rNotCap:
		cmp		al, bl
		jl		retnLT
		jg		retnGT
	iterNext:
		inc		ecx
		inc		edx
		jmp		iterHead
	iterEnd:
		test	bl, bl
		jz		retnEQ
	retnLT:
		mov		al, -1
		pop		ebx
		retn
	retnGT:
		mov		al, 1
		pop		ebx
		retn
	retnEQ:
		xor		al, al
		pop		ebx
		retn
	}
}

__declspec(naked) char __fastcall StrBeginsCS(const char *lstr, const char *rstr)
{
	__asm
	{
		push	esi
		push	edi
		test	ecx, ecx
		jz		retn0
		test	edx, edx
		jz		retn0
		mov		esi, ecx
		call	StrLen
		mov		edi, eax
		mov		ecx, edx
		call	StrLen
		cmp		eax, edi
		jg		retn0
		mov		edx, esi
	iterHead:
		mov		al, [ecx]
		test	al, al
		jz		iterEnd
		cmp		[edx], al
		jnz		retn0
		inc		ecx
		inc		edx
		jmp		iterHead
	iterEnd:
		cmp		[edx], 0
		setz	al
		inc		al
		pop		edi
		pop		esi
		retn
	retn0:
		xor		al, al
		pop		edi
		pop		esi
		retn
	}
}

__declspec(naked) char __fastcall StrBeginsCI(const char *lstr, const char *rstr)
{
	__asm
	{
		push	esi
		push	edi
		test	ecx, ecx
		jz		retn0
		test	edx, edx
		jz		retn0
		mov		esi, ecx
		call	StrLen
		mov		edi, eax
		mov		ecx, edx
		call	StrLen
		cmp		eax, edi
		jg		retn0
		mov		edx, esi
	iterHead:
		cmp		[ecx], 0
		jz		iterEnd
		call	CmprLetters
		test	al, al
		jz		retn0
		inc		ecx
		inc		edx
		jmp		iterHead
	iterEnd:
		cmp		[edx], 0
		setz	al
		inc		al
		pop		edi
		pop		esi
		retn
	retn0:
		xor		al, al
		pop		edi
		pop		esi
		retn
	}
}

__declspec(naked) void __fastcall FixPath(char *str)
{
	__asm
	{
		test	ecx, ecx
		jz		done
	iterHead:
		mov		al, [ecx]
		test	al, al
		jz		done
		cmp		al, 'Z'
		jg		checkSlash
		cmp		al, 'A'
		jl		iterNext
		or		byte ptr [ecx], 0x20
		jmp		iterNext
	checkSlash:
		cmp		al, '\\'
		jnz		iterNext
		mov		byte ptr [ecx], '/'
	iterNext:
		inc		ecx
		jmp		iterHead
	done:
		retn
	}
}

__declspec(naked) void __fastcall StrToLower(char *str)
{
	__asm
	{
		test	ecx, ecx
		jz		done
	iterHead:
		mov		al, [ecx]
		cmp		al, 'Z'
		jg		iterNext
		test	al, al
		jz		done
		cmp		al, 'A'
		jl		iterNext
		or		byte ptr [ecx], 0x20
	iterNext:
		inc		ecx
		jmp		iterHead
	done:
		retn
	}
}

__declspec(naked) void __fastcall ReplaceChr(char *str, char from, char to)
{
	__asm
	{
		test	ecx, ecx
		jz		done
		mov		al, [esp+4]
	iterHead:
		cmp		[ecx], 0
		jz		done
		cmp		[ecx], dl
		jnz		iterNext
		mov		[ecx], al
	iterNext:
		inc		ecx
		jmp		iterHead
	done:
		retn	4
	}
}

__declspec(naked) char* __fastcall FindChr(const char *str, char chr)
{
	__asm
	{
		mov		eax, ecx
		test	ecx, ecx
		jz		done
	iterHead:
		cmp		[eax], 0
		jz		retnNULL
		cmp		[eax], dl
		jz		done
		inc		eax
		jmp		iterHead
	retnNULL:
		xor		eax, eax
	done:
		retn
	}
}

__declspec(naked) char* __fastcall FindChrR(const char *str, UInt32 length, char chr)
{
	__asm
	{
		test	ecx, ecx
		jz		retnNULL
		lea		eax, [ecx+edx]
		mov		dl, [esp+4]
	iterHead:
		cmp		eax, ecx
		jz		retnNULL
		dec		eax
		cmp		[eax], dl
		jz		done
		jmp		iterHead
	retnNULL:
		xor		eax, eax
	done:
		retn	4
	}
}

__declspec(naked) char* __fastcall SubStr(const char *srcStr, const char *subStr)
{
	__asm
	{
		push	ebx
		push	esi
		push	edi
		mov		esi, ecx
		mov		ecx, edx
		call	StrLen
		test	eax, eax
		jz		retnNULL
		mov		edi, edx
		mov		ebx, eax
		mov		ecx, esi
		call	StrLen
		sub		eax, ebx
		mov		ebx, eax
	mainHead:
		cmp		ebx, 0
		jl		retnNULL
	subHead:
		cmp		[edx], 0
		jnz		proceed
		mov		eax, esi
		jmp		done
	proceed:
		call	CmprLetters
		test	al, al
		jz		mainNext
		inc		ecx
		inc		edx
		jmp		subHead
	mainNext:
		dec		ebx
		inc		esi
		mov		ecx, esi
		mov		edx, edi
		jmp		mainHead
	retnNULL:
		xor		eax, eax
	done:
		pop		edi
		pop		esi
		pop		ebx
		retn
	}
}

__declspec(naked) char* __fastcall SlashPos(const char *str)
{
	__asm
	{
		mov		eax, ecx
		test	ecx, ecx
		jz		done
	iterHead:
		mov		cl, [eax]
		test	cl, cl
		jz		retnNULL
		cmp		cl, '/'
		jz		done
		cmp		cl, '\\'
		jz		done
		inc		eax
		jmp		iterHead
	retnNULL:
		xor		eax, eax
	done:
		retn
	}
}

__declspec(naked) char* __fastcall SlashPosR(const char *str)
{
	__asm
	{
		call	StrEnd
		test	eax, eax
		jz		done
	iterHead:
		cmp		eax, ecx
		jz		retnNULL
		dec		eax
		mov		dl, [eax]
		cmp		dl, '/'
		jz		done
		cmp		dl, '\\'
		jz		done
		jmp		iterHead
	retnNULL:
		xor		eax, eax
	done:
		retn
	}
}

__declspec(naked) char* __fastcall GetNextToken(char *str, char delim)
{
	__asm
	{
		push	ebx
		mov		eax, ecx
		xor		bl, bl
	iterHead:
		mov		cl, [eax]
		test	cl, cl
		jz		done
		cmp		cl, dl
		jz		chrEQ
		test	bl, bl
		jnz		done
		jmp		iterNext
	chrEQ:
		test	bl, bl
		jnz		iterNext
		mov		bl, 1
		mov		[eax], 0
	iterNext:
		inc		eax
		jmp		iterHead
	done:
		pop		ebx
		retn
	}
}

__declspec(naked) char* __fastcall GetNextTokenJIP(char* str, char delim)
{
	__asm
	{
		push	ebx
		mov		eax, ecx
		xor bl, bl
		jmp		iterHead
		done :
		pop		ebx
			retn
			and esp, 0xEFFFFFFF
			nop
			iterHead :
		mov		cl, [eax]
			test	cl, cl
			jz		done
			cmp		cl, dl
			jz		chrEQ
			test	bl, bl
			jnz		done
			jmp		iterNext
			chrEQ :
		test	bl, bl
			jnz		iterNext
			mov		bl, 1
			mov[eax], 0
			iterNext :
			inc		eax
			jmp		iterHead
	}
}

__declspec(naked) char* __fastcall GetNextToken(char *str, const char *delims)
{
	__asm
	{
		push	ebx
		push	esi
		mov		eax, ecx
		mov		esi, edx
		xor		bl, bl
	mainHead:
		mov		cl, [eax]
		test	cl, cl
		jz		done
	subHead:
		cmp		[edx], 0
		jz		wasFound
		cmp		cl, [edx]
		jz		chrEQ
		inc		edx
		jmp		subHead
	chrEQ:
		test	bl, bl
		jnz		mainNext
		mov		bl, 1
		mov		[eax], 0
	mainNext:
		inc		eax
		mov		edx, esi
		jmp		mainHead
	wasFound:
		test	bl, bl
		jz		mainNext
	done:
		pop		esi
		pop		ebx
		retn
	}
}

char* __fastcall CopyString(const char *key)
{
	char *newKey = (char*)malloc(StrLen(key) + 1);
	StrCopy(newKey, key);
	return newKey;
}

char* __fastcall IntToStr(int num, char *str)
{
	if (num < 0)
	{
		num = -num;
		*str++ = '-';
	}
	int temp = num, digits = 1;
	while (temp /= 10) digits++;
	temp = digits;
	str += digits;
	*str = 0;
	do
	{
		*--str = '0' + (num % 10);
		num /= 10;
	}
	while (--digits);
	return str + temp;
}

char* __fastcall FltToStr(float num, char *str)
{
	if (num != 0)
	{
		if (num < 0)
		{
			*str++ = '-';
			num = -num;
		}
		int intNum = (int)num;
		if (intNum)
		{
			str = IntToStr(intNum, str);
			if (intNum == num)
				return str;
		}
		else *str++ = '0';
		*str++ = '.';
		intNum = (int)((num - intNum) * 100000);
		int divis = intNum % 10;
		if (divis == 9) intNum++;
		else if (divis == 1) intNum--;
		divis = 10000;
		do
		{
			*str++ = '0' + (intNum / divis);
			intNum %= divis;
			divis /= 10;
		}
		while (intNum);
	}
	else *str++ = '0';
	*str = 0;
	return str;
}

int __fastcall StrToInt(const char *str)
{
	if (!str) return 0;
	int result = 0;
	bool neg = *str == '-';
	if (neg) str++;
	char chr;
	while (chr = *str++)
	{
		if ((chr < '0') || (chr > '9')) break;
		result *= 10;
		result += chr - '0';
	}
	return neg ? -result : result;
}

double __fastcall StrToDbl(const char *str)
{
	if (!str) return 0;
	double result = 0;
	UInt32 intPart = 0, divisor = 1;
	bool neg = *str == '-', point = false;
	if (neg) str++;
	char chr;
	while (chr = *str++)
	{
		if ((chr >= '0') && (chr <= '9'))
		{
			if (point) divisor *= 10;
			intPart *= 10;
			intPart += chr - '0';
		}
		else if (chr == '.')
		{
			if (point) break;
			point = true;
			result = intPart;
			intPart = 0;
		}
		else break;
	}
	result += ((double)intPart / divisor);
	return neg ? -result : result;
}

char* __fastcall UIntToHex(UInt32 num, char *str)
{
	char res[8];
	UInt8 size = 0, temp;
	do
	{
		temp = num % 0x10;
		res[size++] = temp + ((temp < 10) ? '0' : '7');
	}
	while (num >>= 4);
	do
	{
		*str++ = res[--size];
	}
	while (size);
	*str = 0;
	return str;
}

UInt32 __fastcall HexToUInt(const char *str)
{
	if (!str || !*str) return 0;
	char size = 0, chr;
	UInt32 result = 0, mult = 1;
	do
	{
		str++;
		size++;
	}
	while (*str && (size < 8));
	do
	{
		chr = 0x20 | *--str;
		if (chr < '0') break;
		else if (chr <= '9') chr -= '0';
		else if (chr < 'a') break;
		else if (chr <= 'f') chr -= 'W';
		else break;
		result += chr * mult;
		mult <<= 4;
	}
	while (--size);
	return result;
}


//===Begin JIP string / char stuff

alignas(16) const UInt8 kLwrCaseConverter[] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};
alignas(16) const UInt8 kUprCaseConverter[] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

__declspec(noinline) char* GetStrArgBuffer()
{
	thread_local char* s_strBuffer = NULL;
	if (!s_strBuffer)
		s_strBuffer = (char*)_aligned_malloc(0x10000, 0x10);
	return s_strBuffer;
}

//===End JIP string/char stuff


//===Begin JIP File stuff

bool __fastcall FileExists(const char *path)
{
	DWORD attr = GetFileAttributes(path);
	return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool FileStreamJIP::Open(const char* filePath)
{
	if (theFile) fclose(theFile);
	theFile = _fsopen(filePath, "rb", 0x20);
	return theFile ? true : false;
}

bool FileStreamJIP::OpenAt(const char* filePath, UInt32 inOffset)
{
	if (theFile) fclose(theFile);
	theFile = _fsopen(filePath, "rb", 0x20);
	if (!theFile) return false;
	fseek(theFile, 0, SEEK_END);
	if (ftell(theFile) < inOffset)
	{
		Close();
		return false;
	}
	fseek(theFile, inOffset, SEEK_SET);
	return true;
}

bool FileStreamJIP::OpenWrite(char* filePath, bool append)
{
	if (theFile) fclose(theFile);
	if (FileExists(filePath))
	{
		if (append)
		{
			theFile = _fsopen(filePath, "ab", 0x20);
			if (!theFile) return false;
			fputc('\n', theFile);
			fflush(theFile);
			return true;
		}
	}
	else MakeAllDirs(filePath);
	theFile = _fsopen(filePath, "wb", 0x20);
	return theFile ? true : false;
}

bool FileStreamJIP::Create(const char* filePath)
{
	if (theFile) fclose(theFile);
	theFile = _fsopen(filePath, "wb", 0x20);
	return theFile ? true : false;
}

UInt32 FileStreamJIP::GetLength()
{
	fseek(theFile, 0, SEEK_END);
	UInt32 result = ftell(theFile);
	rewind(theFile);
	return result;
}

void FileStreamJIP::SetOffset(UInt32 inOffset)
{
	fseek(theFile, 0, SEEK_END);
	if (ftell(theFile) > inOffset)
		fseek(theFile, inOffset, SEEK_SET);
}

char FileStreamJIP::ReadChar()
{
	return (char)fgetc(theFile);
}

void FileStreamJIP::ReadBuf(void* outData, UInt32 inLength)
{
	fread(outData, inLength, 1, theFile);
}

void FileStreamJIP::WriteChar(char chr)
{
	fputc(chr, theFile);
	fflush(theFile);
}

void FileStreamJIP::WriteStr(const char* inStr)
{
	fputs(inStr, theFile);
	fflush(theFile);
}

void FileStreamJIP::WriteBuf(const void* inData, UInt32 inLength)
{
	fwrite(inData, inLength, 1, theFile);
	fflush(theFile);
}

void FileStreamJIP::MakeAllDirs(char* fullPath)
{
	char* traverse = fullPath, curr;
	while (curr = *traverse)
	{
		if ((curr == '\\') || (curr == '/'))
		{
			*traverse = 0;
			CreateDirectory(fullPath, NULL);
			*traverse = '\\';
		}
		traverse++;
	}
}

//===End JIP File stuff

void __fastcall GetTimeStamp(char *buffer)
{
	time_t rawTime = time(NULL);
	tm timeInfo;
	localtime_s(&timeInfo, &rawTime);
	sprintf_s(buffer, 0x10, "%02d:%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}

__declspec(naked) UInt32 __fastcall ByteSwap(UInt32 dword)
{
	__asm
	{
		mov		eax, ecx
		bswap	eax
		retn
	}
}

void DumpMemImg(void *data, UInt32 size, UInt8 extra)
{
	UInt32 *ptr = (UInt32*)data;
	//Console_Print("Output");
	_MESSAGE("\nDumping  %08X\n", ptr);
	for (UInt32 iter = 0; iter < size; iter += 4, ptr++)
	{
		if (!extra) _MESSAGE("%03X\t\t%08X\t", iter, *ptr);
		else if (extra == 1) _MESSAGE("%03X\t\t%08X\t[%08X]\t", iter, *ptr, ByteSwap(*ptr));
		else if (extra == 2) _MESSAGE("%03X\t\t%08X\t%f", iter, *ptr, *(float*)ptr);
		/*else
		{
			UInt32 addr = *ptr;
			if (!(addr & 3) && (addr > 0x08000000) && (addr < 0x34000000))
				_MESSAGE("%03X\t\t%08X\t%08X\t", iter, *ptr, *(UInt32*)addr);
			else _MESSAGE("%03X\t\t%08X\t", iter, *ptr);
		}*/
	}
}

//From NVSE
void Console_Print_Long(const std::string& str)
{
	UInt32 numLines = str.length() / 500;
	for (UInt32 i = 0; i < numLines; i++)
		Console_Print("%s ...", str.substr(i * 500, 500).c_str());

	Console_Print("%s", str.substr(numLines * 500, str.length() - numLines * 500).c_str());
}


AuxBuffer s_auxBuffers[3];

__declspec(naked) UInt8* __fastcall GetAuxBuffer(AuxBuffer& buffer, UInt32 reqSize)
{
	__asm
	{
		mov		eax, [ecx]
		cmp[ecx + 4], edx
		jb		doRealloc
		test	eax, eax
		jz		doInit
		retn
		doInit :
		push	ecx
			push	0x10
			push	dword ptr[ecx + 4]
			jmp		doAlloc
			doRealloc :
		mov[ecx + 4], edx
			push	ecx
			push	0x10
			push	edx
			test	eax, eax
			jz		doAlloc
			push	eax
			call	_aligned_free
			pop		ecx
			doAlloc :
		call	_aligned_malloc
			add		esp, 8
			pop		ecx
			mov[ecx], eax
			retn
	}
}

// From kNVSE
inline std::string GetCurPath()
{
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	return path;
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
inline void Log(const std::string& msg) 
{
	_MESSAGE(msg.c_str());
}

// Copied from kNVSE (https://github.com/korri123/kNVSE/blob/master/nvse_plugin_example/commands_animation.cpp).
TESForm* StringToForm_Subroutine(const std::string& modName, const std::string& formIdStr)
{
	const auto* mod = DataHandler::Get()->LookupModByName(modName.c_str());
#if _DEBUG
	if (!mod)
	{
		Log("TryConvertStrToForm - Mod name " + modName + " was not found");
	}
#endif
	auto formId = HexStringToInt(formIdStr);
#if _DEBUG
	if (formId == -1)
	{
		Log("TryConvertStrToForm - FormID formatted, got " + formIdStr);
	}
#endif
	formId = (mod->modIndex << 24) + (formId & 0x00FFFFFF);
	auto* form = LookupFormByID(formId);
#if _DEBUG
	if (!form)
	{
		Log(FormatString("TryConvertStrToForm - Form %X was not found", formId));
	}
#endif
	return form;
}

inline double TryConvertStrToDouble(std::string const& str)
{
	double val = NAN;
	try
	{
		val = stod(str);
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

// Copies after JIP's StringToRef().
TESForm* __fastcall StringToForm(const std::string& str)  //calls upon _Subroutine
{
	// Expected input example: "@FalloutNV.esm:135F19"
	const std::size_t posOfSeparator = str.find(':');
	const std::string modStr = str.substr(0, posOfSeparator);  // From "@FalloutNV.esm:135F19", grab "FalloutNV.esm"
	const std::string formIdStr = str.substr(posOfSeparator + 1);  // From "@FalloutNV.esm:135F19", grab "135F19"
	return StringToForm_Subroutine(modStr, formIdStr);
}


#if 0 
// Copies from JIP's CreateForType()
ArrayElementR __fastcall ConvertStrToElem(std::string dataStr)
{
	ArrayElementR result;
	switch (dataStr[0])
	{
	case '@':  // Form
		result = StringToForm(dataStr);  // can pass null forms
		break;

	default:  // String - number case is ignored, since it is handled beforehand.
		result = dataStr.c_str();
	}
	return result;
}
#endif

bool AlchemyItem::IsPoison()
{
	EffectItem* effItem;
	EffectSetting* effSetting = NULL;
	ListNode<EffectItem>* iter = magicItem.list.list.Head();
	do
	{
		if (!(effItem = iter->data)) continue;
		effSetting = effItem->setting;
		if (effSetting && !(effSetting->effectFlags & 4)) return false;
	} while (iter = iter->next);
	return effSetting != NULL;
}

TESActorBase* Actor::GetActorBase()
{
	ExtraLeveledCreature* xLvlCre = GetExtraType(extraDataList, LeveledCreature);
	return (xLvlCre && xLvlCre->form) ? (TESActorBase*)xLvlCre->form : (TESActorBase*)baseForm;
}



__declspec(naked) TESObjectCELL* TESObjectREFR::GetParentCell()
{
	__asm
	{
		mov		eax, [ecx + 0x40]
		test	eax, eax
		jnz		done
		push	kExtraData_PersistentCell
		add		ecx, 0x44
		call	BaseExtraList::GetByType
		test	eax, eax
		jz		done
		mov		eax, [eax + 0xC]
		done:
		retn
	}
}
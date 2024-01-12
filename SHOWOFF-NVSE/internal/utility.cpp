#include "utility.h"

#include <algorithm>
#include <ctime>
#include <format>
#include <stdexcept>

#include "GameData.h"
#include "GameProcess.h"
#include "GameRTTI.h"
#include "jip_nvse.h"


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
kDblPId180 = 0.017453292519943295,
kDblRad2Deg = 57.295779513;

// From JIP
const float
kFlt1d100K = 1.0e-05F,
kFlt1d1K = 0.001F,
kFlt1d200 = 0.005F,
kFlt1d100 = 0.01F,
kFltPId180 = 0.01745329238F,
kFlt1d10 = 0.1F,
kFltHalf = 0.5F,
kFltOne = 1.0F,
kFltPId2 = 1.570796371F,
kFltPI = 3.141592741F,
kFltPIx2 = 6.283185482F,
kFlt10 = 10.0F,
kFlt180dPI = 57.29578018F,
kFlt100 = 100.0F,
kFlt200 = 200.0F,
kFlt1000 = 1000.0F,
kFltMax = FLT_MAX;

// From JIP
alignas(16) const UInt32
kSSERemoveSignMaskPS[] = { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF },
kSSEChangeSignMaskPS[] = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 },
kSSEChangeSignMaskPS0[] = { 0x80000000, 0, 0, 0 },
kSSEDiscard4thPS[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };

// From JIP
alignas(16) const UInt64
kSSERemoveSignMaskPD[] = { 0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF },
kSSEChangeSignMaskPD[] = { 0x8000000000000000, 0x8000000000000000 };

// From JIP
alignas(16) const char
kLwrCaseConverter[] =
{
	'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
	'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
	'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F',
	'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F',
	'\x40', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F',
	'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
	'\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
	'\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
	'\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
	'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
	'\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
	'\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
	'\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
	'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};

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

__declspec(naked) UInt32 __vectorcall cvtd2ui(double value)
{
	__asm
	{
		lea		eax, [esp - 8]
		movq	qword ptr[eax], xmm0
		fld		qword ptr[eax]
		fisttp	qword ptr[eax]
		mov		eax, [eax]
		retn
	}
}

__declspec(naked) double __vectorcall cvtui2d(UInt32 value)
{
	__asm
	{
		push	0
		push	ecx
		fild	qword ptr[esp]
		fstp	qword ptr[esp]
		movq	xmm0, qword ptr[esp]
		add		esp, 8
		retn
	}
}

__declspec(naked) void __fastcall cvtui2d(UInt32 value, double* result)
{
	__asm
	{
		mov[edx], ecx
		mov		dword ptr[edx + 4], 0
		fild	qword ptr[edx]
		fstp	qword ptr[edx]
		retn
	}
}

__declspec(naked) int __vectorcall ifloor(float value)
{
	__asm
	{
		movd	eax, xmm0
		test	eax, eax
		jns		isPos
		push	0x3FA0
		ldmxcsr[esp]
		cvtss2si	eax, xmm0
		mov		byte ptr[esp + 1], 0x1F
		ldmxcsr[esp]
		pop		ecx
		retn
		isPos :
		cvttss2si	eax, xmm0
			retn
	}
}

__declspec(naked) int __vectorcall iceil(float value)
{
	__asm
	{
		movd	eax, xmm0
		test	eax, eax
		js		isNeg
		push	0x5FA0
		ldmxcsr[esp]
		cvtss2si	eax, xmm0
		mov		byte ptr[esp + 1], 0x1F
		ldmxcsr[esp]
		pop		ecx
		retn
		isNeg :
		cvttss2si	eax, xmm0
			retn
	}
}

__declspec(naked) float __vectorcall fMod(float numer, float denom)
{
	__asm
	{
		movss	xmm2, xmm0
		divss	xmm2, xmm1
		cvttps2dq	xmm2, xmm2
		cvtdq2ps	xmm2, xmm2
		mulss	xmm2, xmm1
		subss	xmm0, xmm2
		retn
	}
}

__declspec(naked) float __vectorcall Cos(float angle)
{
	static const float
		kFlt2dPI = 0.6366197467F, kCosCalc1 = 0.00002315393249F, kCosCalc2 = 0.001385370386F,
		kCosCalc3 = 0.04166358337F, kCosCalc4 = 0.4999990463F, kCosCalc5 = 0.9999999404F;
	__asm
	{
		andps	xmm0, kSSERemoveSignMaskPS
		movss	xmm2, kFltPIx2
		comiss	xmm0, xmm2
		jb		perdOK
		movss	xmm1, xmm0
		divss	xmm1, xmm2
		cvttps2dq	xmm1, xmm1
		cvtdq2ps	xmm1, xmm1
		mulss	xmm1, xmm2
		subss	xmm0, xmm1
		perdOK :
		movss	xmm1, kFlt2dPI
			mulss	xmm1, xmm0
			cvttps2dq	xmm1, xmm1
			movd	eax, xmm1
			pxor	xmm3, xmm3
			test	al, al
			jz		doCalc
			jp		fourthQ
			movss	xmm2, kFltPI
			movss	xmm3, kSSEChangeSignMaskPS0
			fourthQ :
		subss	xmm0, xmm2
			doCalc :
		mulss	xmm0, xmm0
			movss	xmm1, kCosCalc1
			mulss	xmm1, xmm0
			subss	xmm1, kCosCalc2
			mulss	xmm1, xmm0
			addss	xmm1, kCosCalc3
			mulss	xmm1, xmm0
			subss	xmm1, kCosCalc4
			mulss	xmm0, xmm1
			addss	xmm0, kCosCalc5
			xorps	xmm0, xmm3
			retn
	}
}

__declspec(naked) float __vectorcall Sin(float angle)
{
	__asm
	{
		movss	xmm1, xmm0
		movss	xmm0, kFltPId2
		subss	xmm0, xmm1
		jmp		Cos
	}
}

__declspec(naked) __m128 __vectorcall GetSinCos(float angle)
{
	__asm
	{
		movss	xmm4, xmm0
		call	Cos
		unpcklps	xmm0, xmm0
		mulss	xmm0, xmm0
		movss	xmm2, kFltOne
		subss	xmm2, xmm0
		pxor	xmm3, xmm3
		movss	xmm0, xmm3
		comiss	xmm2, kFlt1d100K
		jb		done
		sqrtss	xmm0, xmm2
		lea		edx, [eax - 3]
		neg		edx
		comiss	xmm4, xmm3
		cmovb	eax, edx
		cmp		al, 2
		jb		done
		xorps	xmm0, kSSEChangeSignMaskPS0
		done :
		retn
	}
}

__declspec(naked) float __vectorcall ATan(float x)
{
	static const float
		kFltTanPId12 = 0.2679491937F, kFltTanPId6 = 0.5773502588F, kFltPId6 = 0.5235987902F,
		kFltTerm1 = 1.686762929F, kFltTerm2 = 0.4378497303F;
	__asm
	{
		xorps	xmm5, xmm5
		movss	xmm6, xmm0
		andps	xmm6, kSSEChangeSignMaskPS0
		xorps	xmm0, xmm6
		movss	xmm3, kFltOne
		comiss	xmm0, xmm3
		seta	al
		jbe		skipRecpr
		divss	xmm3, xmm0
		movss	xmm0, xmm3
		skipRecpr :
		comiss	xmm0, kFltTanPId12
			jbe		noRegion
			movss	xmm4, kFltTanPId6
			movss	xmm3, xmm0
			mulss	xmm3, xmm4
			addss	xmm3, kFltOne
			subss	xmm0, xmm4
			divss	xmm0, xmm3
			movss	xmm5, kFltPId6
			noRegion :
		movss	xmm3, xmm0
			mulss	xmm3, xmm0
			movss	xmm4, kFltTerm2
			mulss	xmm4, xmm3
			addss	xmm3, kFltTerm1
			addss	xmm4, kFltTerm1
			mulss	xmm0, xmm4
			divss	xmm0, xmm3
			addss	xmm0, xmm5
			test	al, al
			jz		done
			subss	xmm0, kFltPId2
			xorps	xmm0, kSSEChangeSignMaskPS0
			done :
		xorps	xmm0, xmm6
			retn
	}
}

__declspec(naked) float __vectorcall ASin(float x)
{
	__asm
	{
		movss	xmm3, xmm0
		mulss	xmm3, xmm0
		movss	xmm4, kFltOne
		subss	xmm4, xmm3
		comiss	xmm4, kFlt1d100K
		jb		retnPId2
		sqrtss	xmm3, xmm4
		divss	xmm0, xmm3
		jmp		ATan
		retnPId2 :
		movss	xmm3, xmm0
			andps	xmm3, kSSEChangeSignMaskPS0
			movss	xmm0, kFltPId2
			xorps	xmm0, xmm3
			retn
	}
}

__declspec(naked) float __vectorcall ACos(float x)
{
	_asm
	{
		movss	xmm3, xmm0
		mulss	xmm3, xmm0
		movss	xmm4, kFltOne
		subss	xmm4, xmm3
		comiss	xmm4, kFlt1d100K
		jb		retnPI0
		sqrtss	xmm3, xmm4
		divss	xmm0, xmm3
		call	ATan
		movss	xmm3, xmm0
		movss	xmm0, kFltPId2
		subss	xmm0, xmm3
		retn
		retnPI0 :
		movss	xmm3, xmm0
			pxor	xmm0, xmm0
			comiss	xmm3, xmm0
			jnb		done
			movss	xmm0, kFltPI
			done :
		retn
	}
}

__declspec(naked) float __vectorcall ATan2(float y, float x)
{
	__asm
	{
		movss	xmm2, xmm0
		pxor	xmm5, xmm5
		comiss	xmm1, xmm5
		jz		xZero
		divss	xmm0, xmm1
		call	ATan
		pxor	xmm5, xmm5
		comiss	xmm1, xmm5
		ja		done
		andps	xmm2, kSSEChangeSignMaskPS0
		movss	xmm4, kFltPI
		xorps	xmm4, xmm2
		addss	xmm0, xmm4
		retn
		xZero :
		comiss	xmm2, xmm5
			jz		retn0
			movss	xmm0, kFltPId2
			ja		done
			xorps	xmm0, kSSEChangeSignMaskPS0
			retn
			retn0 :
		movss	xmm0, xmm5
			done :
		retn
	}
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

__declspec(naked) UInt32 __fastcall StrHashCI(const char* inKey)
{
	__asm
	{
		push	esi
		mov		eax, 0x1505
		test	ecx, ecx
		jz		done
		mov		esi, ecx
		xor ecx, ecx
		ALIGN 16
		iterHead:
		mov		cl, [esi]
			test	cl, cl
			jz		done
			mov		edx, eax
			shl		edx, 5
			add		eax, edx
			movzx	edx, kCaseConverter[ecx]
			add		eax, edx
			inc		esi
			jmp		iterHead
			done :
		pop		esi
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

__declspec(naked) char __fastcall StrCompareCS(const char* lstr, const char* rstr)
{
	__asm
	{
		test	ecx, ecx
		jz		retnLT
		push	ebx
		push	esi
		push	edi
		lea		esi, [ecx - 0x10]
		lea		edi, [edx - 0x10]
		and ecx, 0xF
		sub		esi, ecx
		sub		edi, ecx
		or ebx, 0xFFFFFFFF
		shl		ebx, cl
		ALIGN 16
		iterHead:
		add		esi, 0x10
			add		edi, 0x10
			movaps	xmm0, [esi]
			movups	xmm1, [edi]
			xorps	xmm2, xmm2
			pcmpeqb	xmm2, xmm0
			pmovmskb	ecx, xmm2
			xorps	xmm2, xmm2
			pcmpeqb	xmm2, xmm1
			pmovmskb	edx, xmm2
			movaps	xmm2, xmm0
			pcmpeqb	xmm2, xmm1
			pmovmskb	eax, xmm2
			not ax
			and eax, ebx
			or edx, ecx
			and edx, ebx
			jnz		foundEnd
			mov		edx, ebx
			or ebx, 0xFFFFFFFF
			test	eax, eax
			jz		iterHead
			bsf		ecx, eax
			pcmpgtb	xmm0, xmm1
			pmovmskb	eax, xmm0
			and eax, edx
			bsf		eax, eax
			cmp		al, cl
			setz	al
			cmovnz	eax, ebx
			add		ecx, edi
			pop		edi
			pop		esi
			pop		ebx
			retn
			retnLT :
		mov		al, 0xFF
			retn
			ALIGN 16
			foundEnd :
			test	eax, eax
			jz		done
			bsf		ecx, eax
			bsf		edx, edx
			xor eax, eax
			cmp		cl, dl
			ja		done
			pcmpgtb	xmm0, xmm1
			pmovmskb	edx, xmm0
			and edx, ebx
			bsf		edx, edx
			cmp		cl, dl
			setz	al
			lea		eax, [eax + eax - 1]
			add		ecx, edi
			done :
		pop		edi
			pop		esi
			pop		ebx
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

__declspec(noinline) char* __fastcall GetNextToken(char *str, const char *delims)
{
	if (!str) return NULL;
	bool table[0x100];
	MemZero(table, 0x100);
	UInt8 curr;
	while (curr = *delims)
	{
		table[curr] = true;
		delims++;
	}
	bool found = false;
	while (curr = *str)
	{
		if (table[curr])
		{
			*str = 0;
			found = true;
		}
		else if (found)
			break;
		str++;
	}
	return str;
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

__declspec(align(16)) const UInt8 kCaseConverter[] =
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
std::string GetCurPath()
{
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	return path;
}

// From kNVSE
int HexStringToInt(const std::string& str)
{
	char* p;
	const auto id = strtoul(str.c_str(), &p, 16);
	if (*p == 0)
		return id;
	return -1;
}

// From kNVSE
void Log(const std::string& msg) 
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

double TryConvertStrToDouble(std::string const& str)
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


TESBipedModelForm* CastFormToBipedModel(TESForm* form)
{
	if (!form) return nullptr;
	switch (form->typeID)
	{
		case kFormType_TESObjectARMO:
		{
			auto armo = (TESObjectARMO*)form;
			return &armo->bipedModel;
		}
		case kFormType_TESObjectARMA:
		{
			auto arma = (TESObjectARMA*)form;
			return &arma->bipedModel;
		}
		case kFormType_TESObjectCLOT:
		{
			auto clot = (TESObjectCLOT*)form;
			return &clot->bipedModel;
		}
	}
	return nullptr;
}

TESRace* CastFormToRace(TESForm* form)
{
	if (!form) return nullptr;
	switch (form->typeID)
	{
	case kFormType_TESRace:
		return (TESRace*)form;

	case kFormType_TESNPC:
		auto npc = (TESNPC*)form;
		return npc->race.race;
	}
	return nullptr;
}

// Copied after NVSE's IsPlayable
bool IsFormPlayable(TESForm* form)
{
	TESBipedModelForm* biped = CastFormToBipedModel(form);
	if (biped)
		return biped->IsPlayable();

	if (IS_TYPE(form, TESObjectWEAP))
		return ((TESObjectWEAP*)form)->IsPlayable();

	if (IS_TYPE(form, TESAmmo))
		return ((TESAmmo*)form)->IsPlayable();

	if (auto race = CastFormToRace(form))
		return race->IsPlayable();

	return true;
}

bool IsItemPlayable(TESForm* form)
{
	if (auto biped = CastFormToBipedModel(form))
		return biped->IsPlayable();

	if (IS_TYPE(form, TESObjectWEAP))
		return ((TESObjectWEAP*)form)->IsPlayable();

	if (IS_TYPE(form, TESAmmo))
		return ((TESAmmo*)form)->IsPlayable();

	return true;
}

short GetEquipType(TESForm* form)  // Ammo is not equip-able in the same sense as Weapons and Armor.
{
	if (IS_TYPE(form, TESObjectWEAP)) {
		return kEquipType_Weapon;
	}
	if (CastFormToBipedModel(form)) {
		return kEquipType_Armor;
	}
	return false;
}

bool IsEquipableItemPlayable(TESForm* form)  // Ammo is not equip-able in the same sense.
{
	TESBipedModelForm* biped = CastFormToBipedModel(form);
	if (biped)
		return biped->IsPlayable();
	if (IS_TYPE(form, TESObjectWEAP))
		return ((TESObjectWEAP*)form)->IsPlayable();
	return true;
}

// Inspired by FOSE's MatchBySlot
UInt32 GetFormEquipSlotMask(TESForm* form)
{
	UInt32 equipSlotMask = 0;  // return value
	if (IS_TYPE(form, TESObjectWEAP)) {
		equipSlotMask = TESBipedModelForm::eSlot_Weapon;
	}
	else
	{
		TESBipedModelForm* pBip = CastFormToBipedModel(form);
		if (pBip)
			equipSlotMask = pBip->partMask;
	}
	return equipSlotMask;
}

// If any slot bits from slotMask match with the slot bits from the form, returns true.
bool MatchAnySlotForForm(TESForm* form, UInt32 slotMask)
{
	auto const formSlotMask = GetFormEquipSlotMask(form);
	return (formSlotMask & slotMask) != 0;
}

bool MatchAnyEquipSlots(UInt32 slotMask1, UInt32 slotMask2)
{
	return (slotMask1 & slotMask2) != 0;
}

float GetFltGameSetting(UInt32 addr)
{
	return *(float*)(addr + 4);  // "the value of the gamesetting is always at +4" -c6
}

SInt32 GetIntGameSetting(UInt32 addr)
{
	return *(SInt32*)(addr + 4);
}

char* GetStrGameSetting(UInt32 addr)
{
	return *(char**)(addr + 4);
}


bool IsDllRunning(const char* dll)
{
	return GetModuleHandleA(dll);
}

TESForm* TryExtractBaseForm(TESForm* form, TESObjectREFR* thisObj)
{
	if (form) {
		form = form->TryGetREFRParent();
	}
	
	if (!form && thisObj) {
		form = thisObj->baseForm;
	}
	return form;
}

TESObjectREFR* TryGetREFR(TESForm* optionalForm, TESObjectREFR* thisObj)
{
	TESObjectREFR* result = nullptr;
	if (optionalForm && IS_REFERENCE(optionalForm))
	{
		result = (TESObjectREFR*)optionalForm;
	}
	else if (thisObj)
	{
		result = thisObj;
	}
	return result;
}

// Prefer base form if provided.
TESForm* TryGetBaseFormOrREFR(TESForm* form, TESObjectREFR* thisObj)
{
	if (form) {
		form = form->TryGetREFRParent();
	}
	if (!form)
		return thisObj;
	return form;
}

//assume val is non-null
void AssignScriptValueResult(const NVSEArrayElement* val, PluginExpressionEvaluator& eval, COMMAND_ARGS)
{
	switch (val->GetType())
	{
	case NVSEArrayVarInterface::kType_Numeric:
		*result = val->num;
		eval.SetExpectedReturnType(kRetnType_Default);
		break;
	case NVSEArrayVarInterface::kType_Array:
		g_arrInterface->AssignCommandResult(val->arr, result);
		eval.SetExpectedReturnType(kRetnType_Array);
		break;
	case NVSEArrayVarInterface::kType_String:
		g_strInterface->Assign(PASS_COMMAND_ARGS, val->str);
		eval.SetExpectedReturnType(kRetnType_String);
		break;
	case NVSEArrayVarInterface::kType_Form:
		REFR_RES = val->form->refID;
		eval.SetExpectedReturnType(kRetnType_Form);
		break;
	case NVSEArrayVarInterface::kType_Invalid:
		*result = 0;	//todo: consider a better error constant, like -999
		eval.SetExpectedReturnType(kRetnType_Default);
		break;
	default:
		throw std::logic_error("SHOWOFF - AssignScriptValueResult >> insufficient switch cases.");
	}
}

__declspec(noinline) UInt8* AuxBuffer::Get(UInt32 bufIdx, UInt32 reqSize)
{
	thread_local AuxBuffer s_auxBuffers[3];
	AuxBuffer* auxBuf = &s_auxBuffers[bufIdx];
	if (auxBuf->size < reqSize)
	{
		auxBuf->size = reqSize;
		if (auxBuf->ptr)
			_aligned_free(auxBuf->ptr);
		auxBuf->ptr = (UInt8*)_aligned_malloc(reqSize, 0x10);
	}
	else if (!auxBuf->ptr)
		auxBuf->ptr = (UInt8*)_aligned_malloc(auxBuf->size, 0x10);
	return auxBuf->ptr;
}

std::pair<std::string, std::string_view> GetFullPath(std::string &&relativePath)
{
	std::ranges::replace(relativePath, '/', '\\');
	auto curPath = GetCurPath();
	auto const curPathSize = curPath.size();
	auto const rel_path_size = relativePath.size();
	std::string JSON_FullPath = std::move(curPath) + "\\" + std::move(relativePath);
	std::string_view json_rel_path_view = { &JSON_FullPath[curPathSize + 1], rel_path_size };
	return std::make_pair(std::move(JSON_FullPath), json_rel_path_view);
}

//Mutates the entry string by inserting a null character where the first delimiter is found.
std::pair<const char*, const char*> SplitStringBySingleDelimiter(std::string& toSplit, const char* delims)
{
	const char* lh, * rh;

	if (auto const i = toSplit.find_first_of(delims);
		i != std::string::npos)
	{
		toSplit[i] = 0;
		lh = toSplit.c_str();
		if (i != toSplit.size() - 1)	//if not splitting at the end
		{
			rh = &toSplit[i+1];
		}
		else rh = nullptr;
	}
	else
	{
		lh = rh = nullptr;
	}

	return { lh, rh };
}

UnorderedMap<const char*, UInt32> s_strRefs;

UInt32 __fastcall StringToRef(char* refStr)
{
	UInt32* findStr;
	if (!s_strRefs.Insert(refStr, &findStr)) 
		return *findStr;
	*findStr = 0;
	if (char* colon = FindChr(refStr, ':');
		colon)
	{
		UInt8 modIdx;
		if (colon != refStr)
		{
			*colon = 0;
			modIdx = g_dataHandler->GetModIndex(refStr);
			*colon = ':';
			if (modIdx == 0xFF) 
				return 0;
		}
		else
		{
			modIdx = 0xFF;
		}
		*findStr = (modIdx << 24) | HexToUInt(colon + 1);
		return *findStr;
	}
	return ResolveRefID(HexToUInt(refStr), findStr) ? *findStr : 0;
}

UnorderedMap<UInt32, const char*> s_refStrings;

//Code copied from JIP LN, adapted to use std::string
std::string RefToString(TESForm* form)
{
	std::string_view constexpr invalidRef = ":00000000";
	if (!form)
		return std::string(invalidRef);

	if (auto const cachedStr = s_refStrings.GetPtr(form->refID))
		return *cachedStr;

	const char* modName = g_dataHandler->GetNthModName(form->modIndex);
	std::string result;
	if (!modName || !modName[0])
	{
		result = invalidRef;
		return result;
	}

	result = modName;
	result.reserve(result.size() + 9);
	result += ':';
	result += std::format("{:08X}", form->refID & 0xFFFFFF);

	//Cache the string
	s_refStrings.Emplace(form->refID, result.c_str());

	return result;
}

bool IsGamePaused()
{
	bool isMainOrPauseMenuOpen = *(Menu**)0x11DAAC0; // g_startMenu, credits to lStewieAl
	auto* console = ConsoleManager::GetSingleton();

	return isMainOrPauseMenuOpen || console->IsConsoleOpen();
}
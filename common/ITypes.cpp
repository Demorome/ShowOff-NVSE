#include "ITypes.h"

Bitstring::Bitstring()
:data(NULL)
{
	
}

Bitstring::Bitstring(UInt32 inLength)
:data(NULL)
{
	Alloc(inLength);
}

Bitstring::~Bitstring()
{
	Dispose();
}

void Bitstring::Alloc(UInt32 inLength)
{
	Dispose();

	inLength = (inLength + 7) & ~7;
	length = inLength >> 3;

	data = new UInt8[length];
}

void Bitstring::Dispose(void)
{
	delete [] data;
}

void Bitstring::Clear(void)
{
	memset(data, 0, length);
}

void Bitstring::Clear(UInt32 idx)
{
	data[idx >> 3] &= ~(1 << (idx & 7));
}

void Bitstring::Set(UInt32 idx)
{
	data[idx >> 3] |= (1 << (idx & 7));
}

bool Bitstring::IsSet(UInt32 idx)
{
	return (data[idx >> 3] & (1 << (idx & 7))) ? true : false;
}

bool Bitstring::IsClear(UInt32 idx)
{
	return (data[idx >> 3] & (1 << (idx & 7))) ? false : true;
}

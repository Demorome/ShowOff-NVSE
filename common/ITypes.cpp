#include "ITypes.h"

Bitstring::Bitstring()
:data(NULL)
{
	
}

Bitstring::Bitstring(uint32_t inLength)
:data(NULL)
{
	Alloc(inLength);
}

Bitstring::~Bitstring()
{
	Dispose();
}

void Bitstring::Alloc(uint32_t inLength)
{
	Dispose();

	inLength = (inLength + 7) & ~7;
	length = inLength >> 3;

	data = new uint8_t[length];
}

void Bitstring::Dispose(void)
{
	delete [] data;
}

void Bitstring::Clear(void)
{
	memset(data, 0, length);
}

void Bitstring::Clear(uint32_t idx)
{
	data[idx >> 3] &= ~(1 << (idx & 7));
}

void Bitstring::Set(uint32_t idx)
{
	data[idx >> 3] |= (1 << (idx & 7));
}

bool Bitstring::IsSet(uint32_t idx)
{
	return (data[idx >> 3] & (1 << (idx & 7))) ? true : false;
}

bool Bitstring::IsClear(uint32_t idx)
{
	return (data[idx >> 3] & (1 << (idx & 7))) ? false : true;
}

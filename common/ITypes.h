#pragma once

#include <stdint.h>

#pragma warning(disable: 4221)
#include <cmath>

inline uint32_t Extend16(uint32_t in)
{
	return (in & 0x8000) ? (0xFFFF0000 | in) : in;
}

inline uint32_t Extend8(uint32_t in)
{
	return (in & 0x80) ? (0xFFFFFF00 | in) : in;
}

inline uint16_t Swap16(uint16_t in)
{
	return	((in >> 8) & 0x00FF) |
			((in << 8) & 0xFF00);
}

inline uint32_t Swap32(uint32_t in)
{
	return	((in >> 24) & 0x000000FF) |
			((in >>  8) & 0x0000FF00) |
			((in <<  8) & 0x00FF0000) |
			((in << 24) & 0xFF000000);
}

inline uint64_t Swap64(uint64_t in)
{
	uint64_t	temp;

	temp = Swap32(in);
	temp <<= 32;
	temp |= Swap32(in >> 32);

	return temp;
}

inline void SwapFloat(float * in)
{
	uint32_t	* temp = (uint32_t *)in;

	*temp = Swap32(*temp);
}

inline void SwapDouble(double * in)
{
	uint64_t	* temp = (uint64_t *)in;

	*temp = Swap64(*temp);
}

inline bool IsBigEndian(void)
{
	union
	{
		uint16_t	u16;
		uint8_t	u8[2];
	} temp;

	temp.u16 = 0x1234;
	
	return temp.u8[0] == 0x12;
}

inline bool IsLittleEndian(void)
{
	return !IsBigEndian();
}

#define CHAR_CODE(a, b, c, d)	(((a & 0xFF) << 0) | ((b & 0xFF) << 8) | ((c & 0xFF) << 16) | ((d & 0xFF) << 24))
#define MACRO_SWAP16(a)			((((a) & 0x00FF) << 8) | (((a) & 0xFF00) >> 8))
#define MACRO_SWAP32(a)			((((a) & 0x000000FF) << 24) | (((a) & 0x0000FF00) << 8) | (((a) & 0x00FF0000) >> 8) | (((a) & 0xFF000000) >> 24))

#define VERSION_CODE(primary, secondary, sub)	(((primary & 0xFFF) << 20) | ((secondary & 0xFFF) << 8) | ((sub & 0xFF) << 0))
#define VERSION_CODE_PRIMARY(in)				((in >> 20) & 0xFFF)
#define VERSION_CODE_SECONDARY(in)				((in >> 8) & 0xFFF)
#define VERSION_CODE_SUB(in)					((in >> 0) & 0xFF)

#define MAKE_COLOR(a, r, g, b)	(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF) << 0))
#define COLOR_ALPHA(in)			((in >> 24) & 0xFF)
#define COLOR_RED(in)			((in >> 16) & 0xFF)
#define COLOR_GREEN(in)			((in >> 8) & 0xFF)
#define COLOR_BLUE(in)			((in >> 0) & 0xFF)

/**
 *	A 64-bit variable combiner
 *	
 *	Useful for endian-independent value extraction.
 */
union VarCombiner
{
	uint64_t	u64;
	int64_t	s64;
	double	f64;
	struct { uint32_t b; uint32_t a; } u32;
	struct { int32_t b; int32_t a; } s32;
	struct { float  b; float  a; } f32;
	struct { uint16_t d; uint16_t c; uint16_t b; uint16_t a; } u16;
	struct { int16_t d; int16_t c; int16_t b; int16_t a; } s16;
	struct { uint8_t  h; uint8_t  g; uint8_t  f; uint8_t  e;
			 uint8_t  d; uint8_t  c; uint8_t  b; uint8_t  a; } u8;
	struct { int8_t  h; int8_t  g; int8_t  f; int8_t  e;
			 int8_t  d; int8_t  c; int8_t  b; int8_t  a; } s8;
};

/**
 *	A bitfield.
 */
template <typename T>
class Bitfield
{
	public:
				Bitfield()					{ }
				~Bitfield()					{ }
		
		void	Clear(void)					{ field = 0; }						//!< Clears all bits
		void	RawSet(uint32_t data)			{ field = data; }					//!< Modifies all bits
		
		void	Set(uint32_t data)			{ field |= data; }					//!< Sets individual bits
		void	Clear(uint32_t data)			{ field &= ~data; }					//!< Clears individual bits
		void	UnSet(uint32_t data)			{ Clear(data); }					//!< Clears individual bits
		void	Mask(uint32_t data)			{ field &= data; }					//!< Masks individual bits
		void	Toggle(uint32_t data)			{ field ^= data; }					//!< Toggles individual bits
		void	Write(uint32_t data, bool state)
											{ if(state) Set(data); else Clear(data); }
		void	WriteBit(uint32_t bit, bool state)	{ Write(1 << bit, state); }		//!< May not work for non-uint8_t bitfields.
		
		T		Get(void) const				{ return field; }					//!< Gets all bits
		T		Get(uint32_t data) const		{ return field & data; }			//!< Gets individual bits
		T		Extract(uint32_t bit) const	{ return (field >> bit) & 1; }		//!< Extracts a bit
		T		ExtractField(uint32_t shift, uint32_t length)					//!< Extracts a series of bits
											{ return (field >> shift) & (0xFFFFFFFF >> (32 - length)); }
		
		bool	IsSet(uint32_t data) const	{ return ((field & data) == data) ? true : false; }	//!< Are all these bits set?
		bool	IsUnSet(uint32_t data) const	{ return (field & data) ? false : true; }			//!< Are all these bits clear?
		bool	IsClear(uint32_t data) const	{ return IsUnSet(data); }							//!< Are all these bits clear?
	
	private:
		T		field;	//!< bitfield data
};

typedef Bitfield <uint8_t>	Bitfield8;		//!< An 8-bit bitfield
typedef Bitfield <uint16_t>	Bitfield16;		//!< A 16-bit bitfield
typedef Bitfield <uint32_t>	Bitfield32;		//!< A 32-bit bitfield

static_assert(sizeof(Bitfield8) == 1);
static_assert(sizeof(Bitfield16) == 2);
static_assert(sizeof(Bitfield32) == 4);

/**
 *	A bitstring
 *	
 *	Essentially a long bitvector.
 */
class Bitstring
{
	public:
				Bitstring();
				Bitstring(uint32_t inLength);
				~Bitstring();

		void	Alloc(uint32_t inLength);
		void	Dispose(void);

		void	Clear(void);
		void	Clear(uint32_t idx);
		void	Set(uint32_t idx);

		bool	IsSet(uint32_t idx);
		bool	IsClear(uint32_t idx);

	private:
		uint8_t	* data;
		uint32_t	length;	//!< length in bytes
};

/**
 *	Time storage
 */
class Time
{
	public:
				Time()				{ Clear(); }
				~Time()				{ }
		
		//! Deinitialize the class
		void	Clear(void)			{ seconds = minutes = hours = 0; hasData = false; }
		//! Sets the class to the current time
		//! @todo implement this
		void	SetToNow(void)		{ Set(1, 2, 3); }
		
		//! Sets the class to the specified time
		void	Set(uint8_t inS, uint8_t inM, uint8_t inH)
									{ seconds = inS; minutes = inM; hours = inH; hasData = true; }
		
		//! Gets whether the class has been initialized or not
		bool	IsSet(void)			{ return hasData; }
		
		uint8_t	GetSeconds(void)	{ return seconds; }	//!< return the seconds portion of the time
		uint8_t	GetMinutes(void)	{ return minutes; }	//!< return the minutes portion of the time
		uint8_t	GetHours(void)		{ return hours; }	//!< return the hours portion of the time
	
	private:
		uint8_t	seconds, minutes, hours;
		bool	hasData;
};

const float kFloatEpsilon = 0.0001f;

inline bool FloatEqual(float a, float b) { float magnitude = a - b; if(magnitude < 0) magnitude = -magnitude; return magnitude < kFloatEpsilon; }

class Vector2
{
	public:
		Vector2() { }
		Vector2(const Vector2 & in)					{ x = in.x; y = in.y; }
		Vector2(float inX, float inY)				{ x = inX; y = inY; }
		~Vector2() { }

		void	Set(float inX, float inY)			{ x = inX; y = inY; }
		void	SetX(float inX)						{ x = inX; }
		void	SetY(float inY)						{ y = inY; }
		void	Get(float * outX, float * outY)		{ *outX = x; *outY = y; }
		float	GetX(void)							{ return x; }
		float	GetY(void)							{ return y; }

		void	Normalize(void)						{ float mag = Magnitude(); x /= mag; y /= mag; }
		float	Magnitude(void)						{ return sqrt(x*x + y*y); }

		void	Reverse(void)						{ float temp = -x; x = -y; y = temp; }

		void	Scale(float scale)					{ x *= scale; y *= scale; }

		void	SwapBytes(void)	{ SwapFloat(&x); SwapFloat(&y); }

		Vector2 &	operator+=(const Vector2 & rhs)	{ x += rhs.x; y += rhs.y; return *this; }
		Vector2 &	operator-=(const Vector2 & rhs)	{ x -= rhs.x; y -= rhs.y; return *this; }
		Vector2 &	operator*=(float rhs)			{ x *= rhs; y *= rhs; return *this; }
		Vector2 &	operator/=(float rhs)			{ x /= rhs; y /= rhs; return *this; }

		float	x;
		float	y;
};

inline Vector2 operator+(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
};

inline Vector2 operator-(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
};

inline Vector2 operator*(const Vector2 & lhs, float rhs)
{
	return Vector2(lhs.x * rhs, lhs.y * rhs);
};

inline Vector2 operator/(const Vector2 & lhs, float rhs)
{
	return Vector2(lhs.x / rhs, lhs.y / rhs);
};

inline bool MaskCompare(void * lhs, void * rhs, void * mask, uint32_t size)
{
	uint8_t	* lhs8 = (uint8_t *)lhs;
	uint8_t	* rhs8 = (uint8_t *)rhs;
	uint8_t	* mask8 = (uint8_t *)mask;

	for(uint32_t i = 0; i < size; i++)
		if((lhs8[i] & mask8[i]) != (rhs8[i] & mask8[i]))
			return false;

	return true;
}

class Vector3
{
public:
	Vector3()									{ }
	Vector3(const Vector3 & in)					{ x = in.x; y = in.y; z = in.z; }
	Vector3(float inX, float inY, float inZ)	{ x = inX; y = inY; z = inZ; }
	~Vector3()									{ }

	void	Set(float inX, float inY, float inZ)			{ x = inX; y = inY; z = inZ; }
	void	Get(float * outX, float * outY, float * outZ)	{ *outX = x; *outY = y; *outZ = z; }

	void	Normalize(void)	{ float mag = Magnitude(); x /= mag; y /= mag; z /= mag; }
	float	Magnitude(void)	{ return sqrt(x*x + y*y + z*z); }

	void	Scale(float scale)	{ x *= scale; y *= scale; z *= scale; }

	void	SwapBytes(void)	{ SwapFloat(&x); SwapFloat(&y); SwapFloat(&z); }

	Vector3 &	operator+=(const Vector3 & rhs)	{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3 &	operator-=(const Vector3 & rhs)	{ x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3 &	operator*=(const Vector3 & rhs)	{ x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	Vector3 &	operator/=(const Vector3 & rhs)	{ x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

	union
	{
		struct
		{
			float	x, y, z;
		};
		float	d[3];
	};
};

inline Vector3 operator+(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Vector3 operator-(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Vector3 operator*(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

inline Vector3 operator/(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

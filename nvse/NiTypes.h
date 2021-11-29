#pragma once

//#include "Utilities.h"
#include "utility.h"


#if RUNTIME

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
const UInt32 kNiTMapLookupAddr = 0x853130;
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
const UInt32 kNiTMapLookupAddr = 0x844740;
#else
#error
#endif

#else
const UInt32 _NiTMap_Lookup = 0;
#endif

struct NiVector4;
struct NiQuaternion;
struct NiMatrix33;

// 08
struct NiRTTI
{
	const char		*name;
	NiRTTI			*parent;
};

// 08, from JIP LN NVSE
struct NiPoint2
{
	float	x, y;

	NiPoint2() {}
	NiPoint2(float _x, float _y) : x(_x), y(_y) {}
	NiPoint2(const NiPoint2& rhs) { *this = rhs; }

	inline void operator=(const NiPoint2& rhs) { _mm_storeu_si64(this, _mm_loadu_si64(&rhs)); }

	inline operator __m128() const { return _mm_loadu_ps(&x); }
};

// 0C, from JIP LN NVSE
struct NiVector3
{
	float	x, y, z;

	NiVector3() {}
	NiVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	NiVector3(const NiVector3& rhs) { *this = rhs; }
	NiVector3(const NiVector4& rhs) { *this = rhs; }

	inline float& operator[](char axis)
	{
		return ((float*)&x)[axis];
	}

	inline void operator=(const NiVector3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}
	void operator=(const NiVector4& rhs);

	inline void operator+=(const NiVector3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
	}
	void operator+=(const NiVector4& rhs);
	inline void operator+=(float value)
	{
		x += value;
		y += value;
		z += value;
	}

	inline void operator-=(const NiVector3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	}
	inline void operator-=(float value)
	{
		x -= value;
		y -= value;
		z -= value;
	}
	inline void operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
	}
	inline void operator*=(const NiMatrix33& mat) { MultiplyMatrix(mat); }
	inline void operator*=(const NiQuaternion& qt) { MultiplyQuaternion(qt); }

	inline operator __m128() const { return _mm_loadu_ps(&x); }

	NiVector3* __fastcall MultiplyMatrix(const NiMatrix33& mat);
	NiVector3* __fastcall MultiplyMatrixRow(const NiMatrix33& rotMatrix, UInt32 whichRow);
	NiVector3* __fastcall MultiplyQuaternion(const NiQuaternion& qt);
	NiVector3* Normalize();
	NiVector3* __fastcall CrossProduct(const NiVector3& vB);
	bool RayCastCoords(NiVector3* posVector, NiMatrix33* rotMatrix, float maxRange, UInt32 axis = 0, UInt16 filter = 6);
};

// from JIP LN NVSE
float __vectorcall Point3Distance(NiVector3* pt1, NiVector3* pt2);

// from JIP LN NVSE
struct NiVector4
{
	float	x, y, z, w;

	NiVector4() {}
	NiVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	NiVector4(const NiVector4& rhs) { *this = rhs; }
	NiVector4(const NiVector3& rhs) { *this = rhs; }
	explicit NiVector4(const __m128 rhs) { *this = rhs; }

	inline void operator=(const NiVector4& rhs) { _mm_storeu_ps(&x, rhs); }
	inline void operator=(const NiVector3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}
	inline void operator=(const __m128 rhs) { _mm_storeu_ps(&x, rhs); }

	inline float& operator[](char axis)
	{
		return ((float*)&x)[axis];
	}

	inline void operator+=(const NiVector3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
	}

	inline void operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
	}

	inline operator __m128() const { return _mm_loadu_ps(&x); }
};

// from JIP LN NVSE
struct alignas(16) AlignedVector4
{
	float	x, y, z, w;

	AlignedVector4() {}
	AlignedVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	AlignedVector4(const AlignedVector4& from) { *this = from; }
	AlignedVector4(const NiVector4& from) { *this = from; }
	explicit AlignedVector4(const __m128 rhs) { *this = rhs; }

	inline void operator=(const AlignedVector4& from) { _mm_store_ps(&x, from); }
	inline void operator=(const NiVector3& from) { _mm_store_ps(&x, from); }
	inline void operator=(const NiVector4& from) { _mm_store_ps(&x, from); }
	inline void operator=(const __m128 rhs) { _mm_store_ps(&x, rhs); }
	inline void operator=(float* valPtr) { _mm_store_ps(&x, _mm_loadu_ps(valPtr)); }

	inline float& operator[](char axis)
	{
		return ((float*)&x)[axis];
	}

	inline void operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
	}

	inline operator __m128() const { return _mm_load_ps(&x); }
};

// from JIP LN NVSE
float __vectorcall Vector3Length(AlignedVector4* inVec);

// 10, copied from JIP LN NVSE
struct AxisAngle
{
	NiVector3	axis;
	float		theta;

	AxisAngle() {}
	AxisAngle(float x, float y, float z, float t) : axis(x, y, z), theta(t) {}
	AxisAngle(const AxisAngle& from) { *this = from; }
	AxisAngle(const NiVector3& ypr) { *this = ypr; }
	AxisAngle(const NiMatrix33& rotMat) { *this = rotMat; }
	AxisAngle(const NiQuaternion& qt) { *this = qt; }
	explicit AxisAngle(const __m128 rhs) { *this = rhs; }

	inline void operator=(const AxisAngle& from) { _mm_storeu_ps(&axis.x, from); }
	inline void operator=(const NiVector3& ypr) { FromEulerYPR(ypr); }
	inline void operator=(const NiMatrix33& rotMat) { FromRotationMatrix(rotMat); }
	inline void operator=(const NiQuaternion& qt) { FromQuaternion(qt); }
	inline void operator=(const __m128 rhs) { _mm_storeu_ps(&axis.x, rhs); }

	inline float operator[](char _axis) const { return ((float*)&axis)[_axis]; }

	inline operator __m128() const { return _mm_loadu_ps(&axis.x); }

	AxisAngle* __fastcall FromEulerYPR(const NiVector3& ypr);
	AxisAngle* __fastcall FromRotationMatrix(const NiMatrix33& rotMat);
	AxisAngle* __fastcall FromQuaternion(const NiQuaternion& qt);

	NiVector3* ToEulerYPR(NiVector3& ypr) const;
};

// 24, copied from JIP LN NVSE
struct NiMatrix33
{
	float	cr[3][3];

	NiMatrix33() {}
	NiMatrix33(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22)
	{
		cr[0][0] = m00;
		cr[0][1] = m10;
		cr[0][2] = m20;
		cr[1][0] = m01;
		cr[1][1] = m11;
		cr[1][2] = m21;
		cr[2][0] = m02;
		cr[2][1] = m12;
		cr[2][2] = m22;
	}
	NiMatrix33(const NiMatrix33& from) { *this = from; }
	NiMatrix33(const NiVector3& rot) { *this = rot; }
	NiMatrix33(const NiQuaternion& qt) { *this = qt; }
	NiMatrix33(const AxisAngle& axisAngle) { *this = axisAngle; }

	inline void operator=(const NiMatrix33& rhs)
	{
		_mm_storeu_ps(&cr[0][0], _mm_loadu_ps(&rhs.cr[0][0]));
		_mm_storeu_ps(&cr[1][1], _mm_loadu_ps(&rhs.cr[1][1]));
		cr[2][2] = rhs.cr[2][2];
	}
	inline void operator=(const NiVector3& rot) { RotationMatrix(rot); }
	inline void operator=(const NiQuaternion& qt) { FromQuaternion(qt); }
	inline void operator=(const AxisAngle& axisAngle) { FromAxisAngle(axisAngle); }

	inline void operator*=(const NiMatrix33& rhs) { MultiplyMatrices(rhs); }

	inline float operator[](UInt32 idx) const { return cr[0][idx]; }

	NiVector3* __fastcall ExtractAngles(NiVector3& outAngles) const;
	NiVector3* __fastcall ExtractAnglesInv(NiVector3& outAngles) const;
	NiMatrix33* __fastcall RotationMatrix(const NiVector3& rot);
	NiMatrix33* __fastcall RotationMatrixInv(const NiVector3& rot);
	NiMatrix33* __fastcall FromQuaternion(const NiQuaternion& qt);
	NiMatrix33* __fastcall FromAxisAngle(const AxisAngle& axisAngle);
	NiMatrix33* __fastcall MultiplyMatrices(const NiMatrix33& matB);
	NiMatrix33* __fastcall Rotate(const NiVector3& rot);
	NiMatrix33* Transpose();
	void Dump() const;
};

// 10 - always aligned?
// Member functions copied from JIP LN NVSE
struct NiQuaternion
{
	float	w, x, y, z;

	NiQuaternion() {}
	NiQuaternion(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}
	NiQuaternion(const NiQuaternion& from) { *this = from; }
	NiQuaternion(const NiMatrix33& rotMat) { *this = rotMat; }
	NiQuaternion(const NiVector3& ypr) { *this = ypr; }
	NiQuaternion(const AxisAngle& axisAngle) { *this = axisAngle; }
	explicit NiQuaternion(const __m128 rhs) { *this = rhs; }

	inline void operator=(const NiQuaternion& rhs) { _mm_storeu_ps(&w, rhs); }
	inline void operator=(const NiMatrix33& rotMat) { FromRotationMatrix(rotMat); }
	inline void operator=(const NiVector3& ypr) { FromEulerYPR(ypr); }
	inline void operator=(const AxisAngle& axisAngle) { FromAxisAngle(axisAngle); }
	inline void operator=(const __m128 rhs) { _mm_storeu_ps(&w, rhs); }

	inline void operator+=(const NiQuaternion& rhs)
	{
		*this = _mm_add_ps(*this, rhs);
	}

	inline void operator-=(const NiQuaternion& rhs)
	{
		*this = _mm_sub_ps(*this, rhs);
	}

	void __fastcall operator*=(const NiQuaternion& rhs);

	inline void operator*=(float s)
	{
		*this = _mm_mul_ps(*this, _mm_set_ps1(s));
	}

	inline NiQuaternion operator+(const NiQuaternion& rhs) const
	{
		NiQuaternion res = *this;
		res += rhs;
		return res;
	}

	inline NiQuaternion operator-(const NiQuaternion& rhs) const
	{
		NiQuaternion res = *this;
		res -= rhs;
		return res;
	}

	inline NiQuaternion operator*(float s) const
	{
		NiQuaternion res = *this;
		res *= s;
		return res;
	}

	inline NiQuaternion operator*(const NiQuaternion& rhs) const
	{
		NiQuaternion res = *this;
		res *= rhs;
		return res;
	}

	bool __fastcall operator==(const NiQuaternion& rhs) const;
	inline bool operator!=(const NiQuaternion& rhs) const { return !(*this == rhs); }

	inline operator __m128() const { return _mm_loadu_ps(&w); }

	NiQuaternion* __fastcall FromRotationMatrix(const NiMatrix33& rotMat);
	NiQuaternion* __fastcall FromEulerYPR(const NiVector3& ypr);
	NiQuaternion* __fastcall FromAxisAngle(const AxisAngle& axisAngle);

	inline NiQuaternion* Invert()
	{
		*this = _mm_xor_ps(*this, _mm_load_ps((const float*)0x10C8780));
		return this;
	}

	inline void Negate()
	{
		*this = _mm_xor_ps(*this, _mm_load_ps((const float*)kSSEChangeSignMaskPS));
	}

	inline float __vectorcall DotProduct(const NiQuaternion& rhs) const
	{
		__m128 m = _mm_mul_ps(*this, rhs);
		return _mm_hadd_ps(_mm_hadd_ps(m, m), m).m128_f32[0];
	}

	NiQuaternion* Normalize();
	NiVector3* __fastcall ToEulerYPR(NiVector3& ypr) const;

	NiQuaternion* Rotate(const NiQuaternion& rot)
	{
		NiQuaternion q = rot;
		*this *= *q.Invert();
		return this;
	}
	 
	NiQuaternion* Rotate(const NiVector3& rot)
	{
		NiQuaternion q = rot;
		*this *= *q.Invert();
		return this;
	}

	NiQuaternion* Rotate(const AxisAngle& rot)
	{
		NiQuaternion q = rot;
		*this *= *q.Invert();
		return this;
	}

	void __vectorcall lerp(const NiQuaternion& qb, float t);
	void __vectorcall slerp(const NiQuaternion& qb, float t);
};

// 34
struct NiTransform
{
	NiMatrix33	rotate;		// 00
	NiVector3	translate;	// 24
	float		scale;		// 30

	// From JIP LN NVSE
	NiVector4* __fastcall GetTranslatedPos(NiVector4* posMods);
};

// 10
struct NiSphere
{
	float	x, y, z, radius;
};

// 1C
struct NiFrustum
{
	float	l;			// 00
	float	r;			// 04
	float	t;			// 08
	float	b;			// 0C
	float	n;			// 10
	float	f;			// 14
	UInt8	o;			// 18
	UInt8	pad19[3];	// 19
};

// 10
struct NiViewport
{
	float	l;
	float	r;
	float	t;
	float	b;
};

// C
struct NiColor
{
	float	r;
	float	g;
	float	b;
};

// 10
struct NiColorAlpha
{
	float	r;
	float	g;
	float	b;
	float	a;
};

// 10
struct NiPlane
{
	NiVector3	nrm;
	float		offset;
};

// 10
// NiTArrays are slightly weird: they can be sparse
// this implies that they can only be used with types that can be NULL?
// not sure on the above, but some code only works if this is true
// this can obviously lead to fragmentation, but the accessors don't seem to care
// weird stuff
template <typename T_Data>
struct NiTArray
{
	virtual void* Destroy(UInt32 doFree);

	T_Data*		data;			// 04
	UInt16		capacity;		// 08 - init'd to size of preallocation
	UInt16		firstFreeEntry;	// 0A - index of the first free entry in the block of free entries at the end of the array (or numObjs if full)
	UInt16		numObjs;		// 0C - init'd to 0
	UInt16		growSize;		// 0E - init'd to size of preallocation

	T_Data operator[](UInt32 idx)
	{
		if (idx < firstFreeEntry)
			return data[idx];
		return NULL;
	}

	T_Data Get(UInt32 idx) { return data[idx]; }

	UInt16 Length() { return firstFreeEntry; }
	void AddAtIndex(UInt32 index, T_Data* item);	// no bounds checking
	void SetCapacity(UInt16 newCapacity);	// grow and copy data if needed

	class Iterator
	{
		friend NiTArray;

		T_Data* pData;
		UInt16		count;

	public:
		bool End() const { return !count; }
		void operator++()
		{
			pData++;
			count--;
		}

		T_Data& operator*() const { return *pData; }
		T_Data& operator->() const { return *pData; }
		T_Data& Get() const { return *pData; }

		Iterator(NiTArray& source) : pData(source.data), count(source.firstFreeEntry) {}
	};

	Iterator Begin() { return Iterator(*this); }
};
#if RUNTIME

template <typename T> void NiTArray<T>::AddAtIndex(UInt32 index, T* item)
{
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	ThisStdCall(0x00869640, this, index, item);
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	ThisStdCall(0x00869110, this, index, item);
#else
#error unsupported Oblivion version
#endif
}

template <typename T> void NiTArray<T>::SetCapacity(UInt16 newCapacity)
{
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	ThisStdCall(0x008696E0, this, newCapacity);
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	ThisStdCall(0x00869190, this, newCapacity);
#else
#error unsupported Runtime version
#endif
}

#endif

// 18
// an NiTArray that can go above 0xFFFF, probably with all the same weirdness
// this implies that they make fragmentable arrays with 0x10000 elements, wtf
template <typename T>
class NiTLargeArray
{
public:
	NiTLargeArray();
	~NiTLargeArray();

	void	** _vtbl;		// 00
	T		* data;			// 04
	UInt32	capacity;		// 08 - init'd to size of preallocation
	UInt32	firstFreeEntry;	// 0C - index of the first free entry in the block of free entries at the end of the array (or numObjs if full)
	UInt32	numObjs;		// 10 - init'd to 0
	UInt32	growSize;		// 14 - init'd to size of preallocation

	T operator[](UInt32 idx) {
		if (idx < firstFreeEntry)
			return data[idx];
		return NULL;
	}

	T Get(UInt32 idx) { return (*this)[idx]; }

	UInt32 Length(void) { return firstFreeEntry; }
};

// 8
template <typename T>
struct NiTSet
{
	T		* data;		// 00
	UInt16	capacity;	// 04
	UInt16	length;		// 06
};

// 10
// this is a NiTPointerMap <UInt32, T_Data>
// todo: generalize key
template <typename T_Data>
class NiTPointerMap  //copied JIP definition for iterator access.
{
public:
	NiTPointerMap();
	virtual ~NiTPointerMap();

	virtual UInt32	CalculateBucket(UInt32 key);
	virtual bool	CompareKey(UInt32 lhs, UInt32 rhs);
	virtual void	Fn_03(UInt32 arg0, UInt32 arg1, UInt32 arg2);	// assign to entry
	virtual void	Fn_04(UInt32 arg);
	virtual void	Fn_05(void);	// locked operations
	virtual void	Fn_06(void);	// locked operations

	struct Entry
	{
		Entry		*next;
		UInt32		key;
		T_Data		*data;
	};

	UInt32		m_numBuckets;	// 04
	Entry		**m_buckets;	// 08
	UInt32		m_numItems;		// 0C

	T_Data *Lookup(UInt32 key) const
	{
		for (Entry *traverse = m_buckets[key % m_numBuckets]; traverse; traverse = traverse->next)
			if (traverse->key == key) return traverse->data;
		return NULL;
	}

	bool Insert(Entry *newEntry)
	{
		UInt32 bucket = newEntry->key % m_numBuckets;
		Entry *entry = m_buckets[bucket], *prev;
		if (entry)
		{
			do
			{
				if (entry->key == newEntry->key) return false;
				prev = entry;
			}
			while (entry = entry->next);
			prev->next = newEntry;
		}
		else m_buckets[bucket] = newEntry;
		m_numItems++;
		return true;
	}

	class Iterator
	{
		friend NiTPointerMap;

		NiTPointerMap* table;
		Entry** bucket;
		Entry* entry;

		void FindNonEmpty()
		{
			for (Entry** end = &table->m_buckets[table->m_numBuckets]; bucket != end; bucket++)
				if (entry = *bucket) return;
		}

	public:
		Iterator(NiTPointerMap& _table) : table(&_table), bucket(table->m_buckets), entry(NULL) { FindNonEmpty(); }

		bool End() const { return !entry; }
		void operator++()
		{
			entry = entry->next;
			if (!entry)
			{
				bucket++;
				FindNonEmpty();
			}
		}
		T_Data* Get() const { return entry->data; }
		UInt32 Key() const { return entry->key; }
	};

	Iterator Begin() { return Iterator(*this); }
};

// 10
// todo: NiTPointerMap should derive from this
// cleaning that up now could cause problems, so it will wait
template <typename T_Key, typename T_Data>
class NiTMapBase
{
public:
	struct Entry
	{
		Entry* next;
		T_Key		key;
		T_Data		data;
	};

	virtual void	Destroy(bool doFree);
	virtual UInt32	CalculateBucket(T_Key key);
	virtual bool	Equal(T_Key key1, T_Key key2);
	virtual void	FillEntry(Entry* entry, T_Key key, T_Data data);
	virtual	void	FreeKey(Entry* entry);
	virtual	Entry* AllocNewEntry();
	virtual	void	FreeEntry(Entry* entry);

	UInt32		numBuckets;	// 04
	Entry** buckets;	// 08
	UInt32		numItems;	// 0C

	T_Data Lookup(T_Key key)
	{
		for (Entry* entry = buckets[CalculateBucket(key)]; entry; entry = entry->next)
			if (Equal(key, entry->key)) return entry->data;
		return NULL;
	}

	void FreeBuckets();

	class Iterator
	{
		friend NiTMapBase;

		NiTMapBase* table;
		Entry** bucket;
		Entry* entry;

		void FindNonEmpty()
		{
			for (Entry** end = &table->buckets[table->numBuckets]; bucket != end; bucket++)
				if (entry = *bucket) return;
		}

	public:
		Iterator(NiTMapBase& _table) : table(&_table), bucket(table->buckets), entry(NULL) { FindNonEmpty(); }

		explicit operator bool() const { return entry != NULL; }
		void operator++()
		{
			entry = entry->next;
			if (!entry)
			{
				bucket++;
				FindNonEmpty();
			}
		}
		T_Data Get() const { return entry->data; }
		T_Key Key() const { return entry->key; }
	};

	Iterator Begin() { return Iterator(*this); }
};

// 14
template <typename T_Data>
class NiTStringPointerMap : public NiTPointerMap<T_Data>
{
public:
	NiTStringPointerMap();
	~NiTStringPointerMap();

	UInt32	unk010;
};

// not sure how much of this is in NiTListBase and how much is in NiTPointerListBase
// 10
template <typename T>
class NiTListBase
{
public:
	NiTListBase();
	~NiTListBase();

	struct Node
	{
		Node	* next;
		Node	* prev;
		T		* data;
	};

	virtual void	Destructor(void);
	virtual Node *	AllocateNode(void);
	virtual void	FreeNode(Node * node);

//	void	** _vtbl;	// 000
	Node	* start;	// 004
	Node	* end;		// 008
	UInt32	numItems;	// 00C
};

// 10
template <typename T>
class NiTPointerListBase : public NiTListBase <T>
{
public:
	NiTPointerListBase();
	~NiTPointerListBase();
};

// 10
template <typename T>
class NiTPointerList : public NiTPointerListBase <T>
{
public:
	NiTPointerList();
	~NiTPointerList();
};

// 4
template <typename T>
class NiPointer
{
public:
	NiPointer(T *init) : data(init)		{	}

	T	* data;

	const T&	operator *() const { return *data; }
	T&			operator *() { return *data; }

	operator const T*() const { return data; }
	operator T*() { return data; }
};

// 14
template <typename T>
class BSTPersistentList
{
public:
	BSTPersistentList();
	~BSTPersistentList();

	virtual void	Destroy(bool destroy);

//	void	** _vtbl;	// 00
	UInt32	unk04;		// 04
	UInt32	unk08;		// 08
	UInt32	unk0C;		// 0C
	UInt32	unk10;		// 10
};

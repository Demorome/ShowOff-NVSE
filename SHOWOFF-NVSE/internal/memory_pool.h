﻿#pragma once

// Mostly ripped from JIP LN.

#define MAX_CACHED_BLOCK_SIZE 0x400
#define MEMORY_POOL_SIZE 0x1000

//These defines were taken from xNVSE
#define MAP_DEFAULT_ALLOC			8
#define MAP_DEFAULT_BUCKET_COUNT	8
#define MAP_MAX_BUCKET_COUNT		0x40000
#define VECTOR_DEFAULT_ALLOC		8

void* __fastcall Pool_Alloc(UInt32 size);
void __fastcall Pool_Free(void* pBlock, UInt32 size);
void* __fastcall Pool_Realloc(void* pBlock, UInt32 curSize, UInt32 reqSize);
void* __fastcall Pool_Alloc_Buckets(UInt32 numBuckets);	//from xNVSE
UInt32 __fastcall AlignBucketCount(UInt32 count);

#define POOL_ALLOC(count, type) (type*)Pool_Alloc(count * sizeof(type))
#define POOL_FREE(block, count, type) Pool_Free(block, count * sizeof(type))
#define POOL_REALLOC(block, curCount, newCount, type) block = (type*)Pool_Realloc(block, curCount * sizeof(type), newCount * sizeof(type))
#define ALLOC_NODE(type) (type*)Pool_Alloc(sizeof(type))
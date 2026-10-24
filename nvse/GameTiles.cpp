#pragma once

#include "nvse/GameTiles.h"
#include "nvse/GameForms.h"

uint32_t Tile::TraitNameToID(const char *traitName)
{
	return CdeclCall<uint32_t>(0xA01860, traitName);
}

Tile::Value *Tile::GetValue(uint32_t typeID)
{
	Value *value;
	for (BSSimpleArray<Value*>::Iterator iter(values); !iter.End(); ++iter)
	{
		value = *iter;
		if (!value || (value->id > typeID)) break;
		if (value->id == typeID) return value;
	}
	return NULL;
}

float Tile::GetValueFloat(uint32_t id)
{
	return ThisCall<float>(0xA011B0, this, id);
}

Menu *Tile::GetParentMenu()
{
	Tile *tile = this;
	do
	{
		if IS_TYPE(tile, TileMenu)
			return ((TileMenu*)tile)->menu;
	}
	while ((tile = tile->parent));
	return NULL;
}
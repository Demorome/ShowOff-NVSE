#include "nvse/GameUI.h"

uint8_t	* g_MenuVisibilityArray = (uint8_t *)0x011F308F;
NiTArray <TileMenu *> * g_TileMenuArray = (NiTArray <TileMenu *> *)0x011F3508;

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	static const uint32_t s_RaceSexMenu__UpdatePlayerHead	= 0x007B25A0;	// End of RaceSexMenu::Func003.case0, call containing QueuedHead::Init (3rd before jmp)
	static uint8_t*	g_bUpdatePlayerModel		= (uint8_t*)0x011C5CB4;	// this is set to true when player confirms change of race in RaceSexMenu -
																		// IF requires change of skeleton - and back to false when model updated
	const _TempMenuByType TempMenuByType = (_TempMenuByType)0x00707990;	// Called from called from call RaceSexMenu::Init
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	static const uint32_t s_RaceSexMenu__UpdatePlayerHead	= 0x007B2660;	// End of RaceSexMenu::Func003.case0, call containing QueuedHead::Init (3rd before jmp)
	static uint8_t*	g_bUpdatePlayerModel		= (uint8_t*)0x011C5CB4;	// this is set to true when player confirms change of race in RaceSexMenu -
																		// IF requires change of skeleton - and back to false when model updated
	const _TempMenuByType TempMenuByType = (_TempMenuByType)0x007078C0;
#elif EDITOR
#else
#error
#endif

InterfaceManager * InterfaceManager::GetSingleton(void)
{
	return *(InterfaceManager **)0x011D8A80;
}

bool InterfaceManager::IsMenuVisible(uint32_t menuType)
{
	if((menuType >= kMenuType_Min) && (menuType <= kMenuType_Max))
		return g_MenuVisibilityArray[menuType] != 0;

	return false;
}

Menu * InterfaceManager::GetMenuByType(uint32_t menuType)
{
	if((menuType >= kMenuType_Min) && (menuType <= kMenuType_Max))
	{
		TileMenu * tileMenu = g_TileMenuArray->Get(menuType - kMenuType_Min);
		if (tileMenu)
			return tileMenu->menu;
	}

	return NULL;
}

Menu * InterfaceManager::TempMenuByType(uint32_t menuType)
{
	if((menuType >= kMenuType_Min) && (menuType <= kMenuType_Max))
	{
		return ::TempMenuByType(menuType);
	}
	return NULL;
}

// Split component path into "top-level menu name" and "everything else".
// Path is of format "MenuType/tile/tile/..." following hierarchy defined in menu's xml.
// Returns pointer to top-level menu or NULL.
// pSlashPos is set to the slash character after the top-level menu name.
/*
TileMenu* InterfaceManager::GetMenuByPath(const char * componentPath, const char ** pSlashPos)
{
	// get menu name - stored by game as "&MENUNAME;" so need to fix it up
	const char* slashPos = strpbrk(componentPath, "\\/");
	if(slashPos)
	{
		std::string menuName("&");
		menuName.append(componentPath, (slashPos - componentPath));
		menuName.append(";");

		uint32_t menuType = Tile::TraitNameToID(menuName.c_str());
		if((menuType >= kMenuType_Min) && (menuType <= kMenuType_Max))
		{
			TileMenu * tileMenu = g_TileMenuArray->Get(menuType - kMenuType_Min);
			if(tileMenu)
			{
				*pSlashPos = slashPos;
				return tileMenu;
			}
		}
	}

	return NULL;
}

Tile::Value* InterfaceManager::GetMenuComponentValue(const char * componentPath)
{
	// path is of format "MenuType/tile/tile/.../traitName" following hierarchy defined in menu's xml
	const char* slashPos = NULL;
	TileMenu * tileMenu = GetMenuByPath(componentPath, &slashPos);
	if(tileMenu && slashPos)
	{
		return tileMenu->GetComponentValue(slashPos + 1);
	}
	return NULL;
}

Tile* InterfaceManager::GetMenuComponentTile(const char * componentPath)
{
	// path is of format "MenuType/tile/tile/.../tile" following hierarchy defined in menu's xml
	const char* slashPos = NULL;
	TileMenu * tileMenu = GetMenuByPath(componentPath, &slashPos);
	if(tileMenu && slashPos)
	{
		return tileMenu->GetComponentTile(slashPos + 1);
	}
	return NULL;
}
*/
void Debug_DumpMenus(void)
{
	for(uint32_t i = 0; i < g_TileMenuArray->Length(); i++)
	{
		TileMenu	* tileMenu = g_TileMenuArray->Get(i);

		if(tileMenu)
		{
			_MESSAGE("menu %d:");
			gLog.Indent();

			tileMenu->Dump();

			gLog.Outdent();
		}
	}
}

void RaceSexMenu::UpdatePlayerHead(void)
{
	ThisStdCall(s_RaceSexMenu__UpdatePlayerHead, this);
}

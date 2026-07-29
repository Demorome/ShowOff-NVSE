#pragma once

#include "GameTiles.h"
#include "GameTypes.h"

class TESNPC;
class Menu;
class SceneGraph;
class FOPipboyManager;
class NiObject;
class TESObjectREFR;
class NiNode;
class BSShaderAccumulator;
class ShadowSceneNode;
class NiSourceTexture;
class NiRefObject;

typedef Menu* (*_TempMenuByType)(uint32_t menuType);
extern const _TempMenuByType TempMenuByType;
#define MENU_VISIBILITY ((bool*)0x11F308F)	//by JIP

// 584
// Member variables taken from Tweaks
class InterfaceManager
{
public:
	InterfaceManager();
	~InterfaceManager();

	static InterfaceManager *	GetSingleton(void);
	static bool					IsMenuVisible(uint32_t menuType);
	static Menu *				GetMenuByType(uint32_t menuType);
	static Menu *				TempMenuByType(uint32_t menuType);
	//static TileMenu *			GetMenuByPath(const char * componentPath, const char ** slashPos);
	//static Tile::Value *		GetMenuComponentValue(const char * componentPath);
	//static Tile *				GetMenuComponentTile(const char * componentPath);

	uint32_t GetTopVisibleMenuID();
	Tile *GetActiveTile();

	// Taken from Tweaks
	struct RefAndNiNode
	{
		TESObjectREFR* ref;
		NiNode* node;
	};

	// Tweaks
	struct VATSHighlightData
	{
		uint32_t mode;						// 000
		RefAndNiNode target;				// 004
		uint32_t numHighlightedRefs;			// 00C
		uint32_t flashingRefIndex;			// 010
		RefAndNiNode highlightedRefs[32];	// 014
		uint32_t unk114;						// 114
		uint8_t isOcclusionEnabled;			// 118
		uint8_t unused119[16];				// 119
		uint8_t byte12B;						// 12B
		void* pPartialLimbQuery[16]; // IDirect3DQuery9
		void* pEntireLimbQuery[16]; // IDirect3DQuery9
		uint8_t hasExecutedQuery[16];
		uint32_t totalArea[16];
		uint32_t visibleArea[16];
		uint32_t selectedLimbID;
		uint32_t numHighlightedBodyParts;
		uint32_t highLightedBodyParts[16];
		uint8_t byte284;
		uint8_t pad285[3];
		float time288;
		float distortVertical;
		uint32_t distortDuration;
		float vatsDistortTime;
		uint8_t byte298;
		uint8_t pad299[3];
		float time29C;
		float burstDuration;
		float burstIntensity2;
		float burstIntensity;
		float pulseIntensity;
		float pulseRadius;
		uint8_t byte2B4;
		uint8_t byte2B5;
		uint8_t pad2B6[2];
		float unk2B8;
		float unk2BC;
		float fVATSTargetPulseRate;
		NiRefObject* unk2C4;
		uint32_t visibleAreaScale_alwaysOne;
		NiRefObject* unk2CC;
	};

	// From Tweaks
	struct Tutorials
	{
		int32_t tutorialFlags[41];
		uint32_t currentShownHelpID;
		uint32_t timeA8;
	};

	// Tweaks
	struct Struct0178
	{
		uint32_t unk00;
		uint32_t NiTPrimitiveArray[9];
		uint8_t byte28;
		uint8_t byte29;
		uint8_t byte2A;
		uint8_t byte2B;
		uint32_t startTime;
		float durationX;
		float durationY;
		float intensityX;
		float intensityY;
		float frequencyX;
		float frequencyY;
		float unk48;
		float unk4C;
		uint32_t imageSpaceEffectParam;
		uint8_t isFlycamEnabled;
		uint8_t byte55;
		uint8_t byte56;
		uint8_t byte57;
		float fBlurRadiusHUD;
		float fScanlineFrequencyHUD;
		float fBlurIntensityHUD;
	};

	uint32_t					flags;				// 000
	SceneGraph				*sceneGraph004;		// 004
	SceneGraph				*sceneGraph008;		// 008
	uint32_t					currentMode;		// 00C	1 = GameMode; 2 = MenuMode
	uint32_t					unk010;				// 010
	uint32_t					unk014;				// 014
	uint32_t					pickLength;			// 018
	uint32_t					unk01C;				// 01C
	uint8_t					byte020;			// 020
	uint8_t					byte021;			// 021
	uint8_t					byte022;			// 022
	uint8_t					byte023;			// 023
	uint32_t					unk024;				// 024
	TileImage				*cursor;			// 028
	float					flt02C;				// 02C
	float					flt030;				// 030
	float					flt034;				// 034
	float					cursorX;			// 038
	float					flt03C;				// 03C
	float					cursorY;			// 040
	float					mouseWheel;			// 044	-120.0 = down; 120.0 = up
	float					flt048;				// 048
	Tile					*draggedTile;		// 04C
	int						unk050;				// 050
	float					flt054;				// 054
	float					flt058;				// 058
	int						unk05C;				// 05C
	int						unk060;				// 060
	int						unk064;				// 064
	uint32_t					unk068[2];			// 068
	tList<TESObjectREFR>	selectableRefs;		// 070
	uint32_t					unk078;				// 078
	bool					debugText;			// 07C
	uint8_t					byte07D;			// 07D
	uint8_t					byte07E;			// 07E
	uint8_t					byte07F;			// 07F
	NiNode					*niNode080;			// 080
	NiNode					*niNode084;			// 084
	uint32_t					unk088;				// 088
	BSShaderAccumulator		*shaderAccum08C;	// 08C
	BSShaderAccumulator		*shaderAccum090;	// 090
	ShadowSceneNode			*shadowScene094;	// 094
	ShadowSceneNode			*shadowScene098;	// 098
	Tile					*menuRoot;			// 09C
	Tile					*globalsTile;		// 0A0	globals.xml
	NiNode					*unk0A4;			// 0A4 saw Tile? seen NiNode
	uint32_t					unk0A8;				// 0A8
	NiObject				*unk0AC;			// 0AC seen NiAlphaProperty
	uint32_t					unk0B0[3];			// 0B0
	Tile					*activeTileAlt;		// 0BC
	uint32_t					unk0C0;				// 0C0
	uint32_t					unk0C4;				// 0C4
	uint8_t					byte0C8;			// 0C8
	uint8_t					byte0C9;			// 0C9
	uint8_t					byte0CA;			// 0CA
	uint8_t					byte0CB;			// 0CB
	Tile					*activeTile;		// 0CC
	Menu					*activeMenu;		// 0D0
	Tile					*tile0D4;			// 0D4
	Menu					*menu0D8;			// 0D8
	uint32_t					unk0DC[2];			// 0DC
	uint8_t					msgBoxButton;		// 0E4 -1 if no button pressed
	uint8_t					byte0E5;			// 0E5
	uint8_t					byte0E6;			// 0E6
	uint8_t					byte0E7;			// 0E7
	uint32_t					unk0E8;				// 0E8
	uint8_t					byte0EC;			// 0EC
	uint8_t					byte0ED;			// 0ED
	uint8_t					byte0EE;			// 0EE
	uint8_t					byte0EF;			// 0EF
	TESObjectREFR			*debugSelection;	// 0F0	compared to activated object during Activate
	uint32_t					unk0F4;				// 0F4
	uint32_t					unk0F8;				// 0F8
	TESObjectREFR			*crosshairRef;		// 0FC
	uint32_t					unk100[4];			// 100
	uint8_t					byte110;			// 110
	uint8_t					pad111[3];			// 111
	uint32_t					menuStack[10];		// 114
	void					*ptr13C;			// 13C	Points to a struct, possibly. First member is *bhkSimpleShapePhantom
	uint32_t					unk140[5];			// 140
	uint32_t					unk154;				// 154
	uint32_t					unk158;				// 158
	uint32_t					unk15C[5];			// 15C
	uint8_t					byte170;			// 170
	uint8_t					byte171;			// 171
	uint8_t					byte172;			// 172
	uint8_t					byte173;			// 173
	FOPipboyManager			*pipboyManager;		// 174
	Struct0178				unk178;				// 178
	VATSHighlightData		vatsHighlightData;	// 1DC
	float					scale4AC;			// 4AC
	float					unk4B0;				// 4B0
	uint8_t					isRenderedMenuOrPipboyManager;		// 4B4
	uint8_t					byte4B5;			// 4B5
	uint8_t					byte4B6;			// 4B6
	uint8_t					byte4B7;			// 4B7
	uint32_t					queuedPipboyTabToSwitchTo;	// 4B8
	uint32_t					pipBoyMode;			// 4BC
	void (*onPipboyOpenCallback)(void);			// 4C0
	uint32_t					unk4C4[2];			// 4C4
	uint8_t					byte4CC;			// 4CC
	uint8_t					byte4CD;			// 4CD
	uint8_t					pad4CE;				// 4CE
	uint8_t					pad4CF;				// 4CF
	uint32_t					unk4D0;				// 4D0
	Tutorials				help;				// 4D4
};
static_assert(sizeof(InterfaceManager) == 0x580);

void Debug_DumpMenus(void);

enum
{
	kMenuType_Min =				0x3E9,
	kMenuType_Message =			kMenuType_Min,
	kMenuType_Inventory,
	kMenuType_Stats,
	kMenuType_HUDMain,
	kMenuType_Loading =			0x3EF,
	kMenuType_Container,
	kMenuType_Dialog,
	kMenuType_SleepWait =		0x3F4,
	kMenuType_Start,
	kMenuType_LockPick,
	kMenuType_Quantity =		0x3F8,
	kMenuType_Map =				0x3FF,
	kMenuType_Book =			0x402,
	kMenuType_LevelUp,
	kMenuType_Repair =			0x40B,
	kMenuType_RaceSex,
	kMenuType_Credits =			0x417,
	kMenuType_CharGen,
	kMenuType_TextEdit =		0x41B,
	kMenuType_Barter =			0x41D,
	kMenuType_Surgery,
	kMenuType_Hacking,
	kMenuType_VATS,
	kMenuType_Computers,
	kMenuType_RepairServices,
	kMenuType_Tutorial,
	kMenuType_SpecialBook,
	kMenuType_ItemMod,
	kMenuType_LoveTester =		0x432,
	kMenuType_CompanionWheel,
	kMenuType_TraitSelect,
	kMenuType_Recipe,
	kMenuType_SlotMachine =		0x438,
	kMenuType_Blackjack,
	kMenuType_Roulette,
	kMenuType_Caravan,
	kMenuType_Trait =			0x43C,
	kMenuType_Max =				kMenuType_Trait,
};

struct EventCallbackScripts;

class Menu
{
public:
	Menu();
	~Menu();

	virtual Menu	*Destructor(bool doFree);
	virtual void	SetField(uint32_t idx, Tile *value);
	virtual void	Unk_02(uint32_t arg0, uint32_t arg1);
	virtual void	HandleClick(int32_t tileID, Tile *clickedTile);
	virtual void	HandleMouseover(uint32_t arg0, Tile *activeTile);	//	Called on mouseover, activeTile is moused-over Tile
	virtual void	Unk_05(uint32_t arg0, uint32_t arg1);
	virtual void	Unk_06(uint32_t arg0, uint32_t arg1, uint32_t arg2);
	virtual void	Unk_07(uint32_t arg0, uint32_t arg1, uint32_t arg2);
	virtual void	Unk_08(uint32_t arg0, uint32_t arg1);
	virtual void	Unk_09(uint32_t arg0, uint32_t arg1);
	virtual void	Unk_0A(uint32_t arg0, uint32_t arg1);
	virtual void	Unk_0B(void);	// Called every frame while the menu is active
	virtual bool	HandleKeyboardInput(uint32_t inputChar);	// Return false for handling keyboard shortcuts
	virtual uint32_t	GetID(void);
	virtual bool	Unk_0E(uint32_t arg0, uint32_t arg1);
	virtual void	Unk_0F(void);
	virtual void	Unk_10(void);
	virtual void	Unk_11(void);

	TileMenu		*tile;		// 04
	uint32_t			unk08;		// 08
	uint32_t			unk0C;		// 0C
	uint32_t			unk10;		// 10
	uint32_t			unk14;		// 14
	uint32_t			unk18;		// 18
	uint32_t			unk1C;		// 1C
	uint32_t			id;			// 20
	uint32_t			unk24;		// 24

	Menu *HandleMenuInput(int tileID, Tile *clickedTile);
	Tile *AddTileFromTemplate(Tile *destTile, const char *templateName, uint32_t arg3);
};

// Taken from Tweaks
struct HotKeyWheel
{
	TileRect* parent;
	TileRect* hotkeys[8];
	uint8_t byte24;
	uint8_t gap25[3];
	uint32_t selectedHotkey;
	uint32_t selectedHotkeyTrait;
	uint32_t selectedTextTrait;

	void SetVisible(bool isVisible) { ThisCall(0x701760, this, isVisible); }
};

// 170
class RaceSexMenu : public Menu		// 1036
{
public:
	RaceSexMenu();
	~RaceSexMenu();

	uint32_t				unk028[44];		// 028
	TESNPC				*npc;			// 0D8
	uint32_t				unk0DC[37];		// 0DC

	void UpdatePlayerHead(void);
};
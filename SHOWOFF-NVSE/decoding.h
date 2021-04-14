#pragma once

struct HotKeyWheel
{
	TileRect* parent;		// 00
	TileRect* hotkeys[8];	// 04
	UInt8		byte24;			// 24
	UInt8		pad25[3];		// 25
	UInt32		unk28;			// 28
	UInt32		selectedHotkeyTrait;// 2C
	UInt32		selectedTextTrait;	// 30
};
STATIC_ASSERT(sizeof(HotKeyWheel) == 0x34);



// 278
class HUDMainMenu : public Menu			// 1004
{

public:

	struct QueuedMessage
	{
		char	msgText[0x204];			// 000
		char	iconPate[MAX_PATH];		// 204
		char	soundPath[MAX_PATH];	// 308
		float	displayTime;			// 40C

	};

	struct SubtitleData;

	struct Struct224
	{
		UInt8		byte00;		// 00
		UInt8		pad01[3];	// 01
		float		flt04;		// 04
		float		flt08;		// 08
		float		flt0C;		// 0C
		float		flt10;		// 10
		UInt32		unk14;		// 14
		UInt32		tickCount;	// 18
		UInt8		byte1C;		// 1C
		UInt8		byte1D;		// 1D
		UInt8		pad1E[2];	// 1E
	};

	struct QueuedQuestText
	{
		char		msg[0x104];
		bool		byte104;
		bool		byte105;
	};

	enum VisibilityFlags
	{
		kActionPoints = 0x1,//0
		kHitPoints = 0x2,//1
		kRadiationMeter = 0x4,//2
		kEnemyHealth = 0x8,//3
		kQuestReminder = 0x10,//4
		kRegionLocation = 0x20,//5
		kReticleCenter = 0x40,//6
		kSneakMeter = 0x80, //7
		kMessages = 0x100, //8
		kInfo = 0x200, //9
		kSubtibles = 0x400,//10
		kHotkeys = 0x800,//11
		kXpMeter = 0x1000,//12
		kBreathMeter = 0x2000,//13
		kExplosivePositioning = 0x4000,//14
		kCrippledLimbIndicator = 0x8000,//15
		kHardcoreMode = 0x10000,//16
	};

	enum HUDStates
	{
		kRECALCULATE = 0x1,
		kNormal,
		kPipBoy,
		kPause,
		kLoading,
		kDialog,
		kVATSMenu,
		kVATSPlayback,
		kContainer,
		kBeginSit,
		kSleepWait,
		kPlayerDisabledControls,
		kUnusedD,
		kLevelUpMenu,
		kHacking,
		kComputers,
		kMessage,
		kSpecialBook,
		kLoveTester,
		kVanityCam,
		kUnused15,
		kAiming,
		kAimingScope,
		kIntroMovie,
		kGambling,
	};

	UInt32							unk028;			// 028
	union
	{
		Tile* tiles[0x56];
		struct
		{
			TileImage* tile02C;		// 02C	HitPoints\meter
			TileText* tile030;		// 030	HitPoints\justify_right_text
			TileRect* tile034;		// 034	HitPoints\compass_window\compass_icon_group
			TileRect* tile038;		// 038	HitPoints\compass_window\compass_icon_group
			TileRect* tile03C;		// 03C	HitPoints\compass_window\compass_icon_group
			TileImage* tile040;		// 040	HitPoints\compass_window
			TileImage* tile044;		// 044	ActionPoints\meter
			TileText* tile048;		// 048	ActionPoints\justify_right_text
			TileText* tile04C;		// 04C	ActionPoints\justify_right_text
			TileImage* tile050;		// 050	ActionPoints\meter
			TileImage* tile054;		// 054	ActionPoints\MeterBackground
			TileText* tile058;		// 058	ActionPoints\justify_right_text
			TileRect* tile05C;		// 05C	QuestReminder\QuestStages
			TileRect* tile060;		// 060	QuestReminder\QuestAdded
			TileText* tile064;		// 064	Region_Location\justify_left_text
			TileText* tile068;		// 068	Region_Location\justify_left_text
			TileImage* tile06C;		// 06C	RadiationMeter\radiation_bracket
			TileImage* tile070;		// 070	RadiationMeter\radiation_pointer
			TileText* tile074;		// 074	RadiationMeter\radiation_text_value
			TileText* tile078;		// 078	RadiationMeter\radiation_text
			TileImage* tile07C;		// 07C	EnemyHealth\enemy_health_bracket
			TileImage* tile080;		// 080	EnemyHealth\meter
			TileText* tile084;		// 084	EnemyHealth\justify_center_text
			TileText* sneakLabel;	// 088	SneakMeter\sneak_nif
			TileImage* tile08C;		// 08C	Messages\message_icon
			TileText* tile090;		// 090	Messages\justify_left_text
			TileImage* tile094;		// 094	Messages\message_bracket
			TileText* tile098;		// 098	Subtitles\justify_center_text
			TileRect* tileTakePrompt;		// 09C	Info\justify_center_hotrect
			TileText* tile0A0;		// 0A0	Info\justify_center_hotrect\PCShortcutLabel
			TileImage* tile0A4;		// 0A4	Info\justify_center_hotrect\xbox_button
			TileText* tile0A8;		// 0A8	Info\justify_center_text
			TileText* tileLockLevelStr;	// 0AC	Info\justify_center_text
			TileText* tile0B0;		// 0B0	Info\justify_center_text
			TileText* tilePickupWeightStr;	// 0B4	Info\justify_right_text
			TileText* tilePickupWG;			// 0B8	Info\justify_left_text
			TileText* tilePickupValueStr;	// 0BC	Info\justify_right_text
			TileText* tilePickupVAL;		// 0C0	Info\justify_left_text
			TileImage* tile0C4;		// 0C4	Info\info_seperator
			TileRect* tile0C8;		// 0C8	Hokeys\hotkey_selector
			TileText* tile0CC;		// 0CC	Hokeys\justify_center_text
			TileImage* tile0D0;		// 0D0	HitPoints\left_bracket
			TileImage* tile0D4;		// 0D4	ActionPoints\right_bracket
			TileImage* tile0D8;		// 0D8	XPMeter\XPBracket
			TileText* tile0DC;		// 0DC	XPMeter\XPAmount
			TileText* tile0E0;		// 0E0	XPMeter\XPLabel
			TileImage* tile0E4;		// 0E4	XPMeter\XPPointer
			TileText* tile0E8;		// 0E8	XPMeter\XPLastLevel
			TileText* tile0EC;		// 0EC	XPMeter\XPNextLevel
			TileText* tile0F0;		// 0F0	XPMeter\XPLevelUp
			TileImage* tile0F4;		// 0F4	ReticleCenter\reticle_center
			TileImage* tile0F8;		// 0F8	crippled_limb_indicator\Face
			TileImage* tile0FC;		// 0FC	crippled_limb_indicator\Head
			TileImage* tile100;		// 100	crippled_limb_indicator\Torso
			TileImage* tile104;		// 104	crippled_limb_indicator\Left_Arm
			TileImage* tile108;		// 108	crippled_limb_indicator\Right_Arm
			TileImage* tile10C;		// 10C	crippled_limb_indicator\Left_Leg
			TileImage* tile110;		// 110	crippled_limb_indicator\Right_Leg
			TileRect* tile114;		// 114	ActionPoints
			TileRect* tile118;		// 118	HitPoints
			TileRect* tile11C;		// 11C	RadiationMeter
			TileRect* tile120;		// 120	EnemyHealth
			TileRect* tile124;		// 124	QuestReminder
			TileRect* tile128;		// 128	Region_Location
			TileRect* tile12C;		// 12C	ReticleCenter
			TileRect* tile130;		// 130	SneakMeter
			TileRect* tile134;		// 134	Messages
			TileRect* tile138;		// 138	Info
			TileRect* tile13C;		// 13C	Subtitles
			TileRect* tile140;		// 140	Hokeys
			TileRect* tile144;		// 144	XPMeter
			int* tile148;			// 148	BreathMeter
			TileRect* tile14C;		// 14C	Explosive_positioning_rect
			TileRect* crippledLimbIndicator;		// 150	crippled_limb_indicator
			TileImage* tile154;		// 154	DDTIcon
			TileImage* tile158;		// 158	DDTIconEnemy
			TileText* tile15C;		// 15C	AmmoTypeLabel
			TileRect* tile160;		// 160	HardcoreMode
			TileText* tile164;		// 164	HardcoreMode\Dehydration
			TileText* tile168;		// 168	HardcoreMode\Sleep
			TileText* tile16C;		// 16C	HardcoreMode\Hunger
			TileImage* tile170;		// 170	DDTIcon
			TileImage* tile174;		// 174	DDTIconEnemyAP
			TileText* tile178;		// 178	HardcoreMode\Rads
			TileText* tile17C;		// 17C	HardcoreMode\LMBs
			TileImage* tile180;		// 180	CNDArrows

		};
	};
	UInt32							unk184;				// 184
	float							flt188;				// 188
	tList<QueuedMessage>			queuedMessages;		// 18C
	UInt32							currMsgKey;			// 194
	BSSimpleArray<SubtitleData>		subtitlesArr;		// 198
	UInt32							unk1A8[4];			// 1A8
	TESObjectREFR* crosshairRef;		// 1B8
	UInt32							unk1BC;				// 1BC
	UInt32							visibilityOverrides;	// 1C0
	UInt32							stage;				// 1C4
	HotKeyWheel						hotKeyWheel;		// 1C8
	UInt8							isUsingScope;		// 1FC
	UInt8							byte1FD[3];			// 1FD
	NiControllerSequence* niContSeq;			// 200
	UInt8							isLevelUpQueued;	// 204
	UInt8							autoDisplayObjectives;	// 205
	UInt8							pad206[2];			// 206
	UInt32							unk208;				// 208
	UInt32							questTargets;		// 20C
	UInt32							unk210;				// 210
	UInt32							compassWidth;		// 214
	UInt32							maxCompassAngle;	// 218
	Actor* healthTarget;		// 21C
	UInt32							unk220;				// 220
	Struct224						unk224;				// 224
	UInt32							unk244;				// 244
	UInt32							unk248[4];			// 248
	tList<QueuedQuestText>			queuedQuestTextList;// 258
	UInt8							byte260;			// 260
	UInt8							byte261;			// 261
	UInt8							pad262[2];			// 262
	tList<UInt32>					xpMessages;			// 264
	tList<UInt32>					list26C;			// 26C
	float							hudShake;			// 274

	//static HUDMainMenu* GetSingleton() { return *(HUDMainMenu**)(0x11D96C0); }
};
STATIC_ASSERT(sizeof(HUDMainMenu) == 0x278);
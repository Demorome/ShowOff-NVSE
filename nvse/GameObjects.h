#pragma once

#include "GameForms.h"
#include "GameBSExtraData.h"
#include "GameExtraData.h"
#include "havok.h"

struct ScriptEventList;
class ActiveEffect;
struct AnimData;

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
static const uint32_t s_TESObject_REFR_init = 0x55A2F0;			// TESObject_REFR initialization routine (first reference to s_TESObject_REFR_vtbl)
static const uint32_t	s_Actor_EquipItem = 0x88C650;				// maybe, also, would be: 007198E0 for FOSE	4th call from the end of TESObjectREFR::RemoveItem (func5F)
static const uint32_t	s_Actor_UnequipItem = 0x88C790;				// maybe, also, would be: 007133E0 for FOSE next sub after EquipItem
static const uint32_t s_TESObjectREFR__GetContainer = 0x55D310;	// First call in REFR::RemoveItem
static const uint32_t s_TESObjectREFR_Set3D = 0x5702E0;			// void : (const char*)
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
static const uint32_t s_TESObject_REFR_init = 0x55A6B0;
static const uint32_t	s_Actor_EquipItem = 0x88C2B0;
static const uint32_t	s_Actor_UnequipItem = 0x88C3F0;
static const uint32_t s_TESObjectREFR__GetContainer = 0x55D6B0;
static const uint32_t s_TESObjectREFR_Set3D = 0x4E0F80;
#elif EDITOR
#else
#error
#endif
const uint32_t kUpdateAppearanceAddr = 0x8D3FA0;
struct NiPoint3;
// 68
class TESObjectREFR : public TESForm
{
public:
	MEMBER_FN_PREFIX(TESObjectREFR);

	TESObjectREFR();
	~TESObjectREFR();

	virtual void		Unk_4E(void);	// GetStartingPosition(Position, Rotation, WorldOrCell)
	virtual void		Unk_4F(void);
	virtual void		Unk_50(void);
	virtual void		Unk_51(void);
	virtual bool		CastShadows();
	virtual void		Unk_53(void);
	virtual void		Unk_54(void);
	virtual void		Unk_55(void);
	virtual void		Unk_56(void);
	virtual bool		IsObstacle();
	virtual void		Unk_58(void);
	virtual void		Unk_59(void);
	virtual void		Unk_5A(void);
	virtual void		Unk_5B(void);
	virtual void		Unk_5C(void);
	virtual void		Unk_5D(void);
	virtual void		Unk_5E(void);
	virtual TESObjectREFR	*RemoveItem(TESForm *toRemove, BaseExtraList *extraList, uint32_t quantity, bool keepOwner, bool drop, TESObjectREFR *destRef,
		uint32_t unk6, uint32_t unk7, bool unk8, bool unk9);
	virtual void		Unk_60(void);
	virtual bool		EquipObject(TESForm *item, uint32_t count, ExtraDataList *xData, bool lockEquip);
	virtual void		Unk_62(void);
	virtual void		Unk_63(void);
	virtual void		AddItem(TESForm *item, ExtraDataList *xDataList, uint32_t quantity);
	virtual void		Unk_65(void);
	virtual void		Unk_66(void);
	virtual void		Unk_67(void);					// Actor: GetMagicEffectList
	virtual bool		GetIsChildSize(bool checkHeight);		// 068 Actor: GetIsChildSize
	virtual uint32_t		GetActorUnk0148();			// result can be interchanged with baseForm, so TESForm* ?
	virtual void		SetActorUnk0148(uint32_t arg0);
	virtual void		Unk_6B(void);
	virtual void		Unk_6C(void);	// REFR: GetBSFaceGenNiNodeSkinned
	virtual void		Unk_6D(void);	// REFR: calls 006C
	virtual void		Unk_6E(void);	// MobileActor: calls 006D then NiNode::Func0040
	virtual void		Unk_6F(void);
	virtual bool		Unload3D();
	virtual void		AnimateNiNode();
	virtual void		GenerateNiNode(bool arg1);
	virtual void		Set3D(NiNode* niNode, bool unloadArt);
	virtual NiNode *	GetNiNode() const;
	virtual void		Unk_75(void);
	virtual void		Unk_76(void);
	virtual void		Unk_77(void);
	virtual void		Unk_78(void);
	virtual AnimData	*GetAnimData();			// 0079
	virtual ValidBip01Names * GetValidBip01Names(void);	// 007A	Character only
	virtual ValidBip01Names * CallGetValidBip01Names(void);
	virtual void		SetValidBip01Names(ValidBip01Names *validBip01Names);
	virtual NiPoint3*	GetPos();				// GetPos or GetDistance
	virtual void		Unk_7E(uint32_t arg0);
	virtual void		Unk_7F(void);
	virtual void		Unk_80(uint32_t arg0);
	virtual void		Unk_81(uint32_t arg0);
	virtual void		Unk_82(void);
	virtual uint32_t		Unk_83(void);
	virtual void		Unk_84(uint32_t arg0);
	virtual uint32_t		Unk_85(void);
	virtual bool		IsCharacter();			// return false for Actor and Creature, true for character and PlayerCharacter
	virtual bool		IsCreature();
	virtual bool		IsExplosion();
	virtual bool		IsProjectile();
	virtual void		Unk_8A(void);			// SetParentCell (Interior only ?)
	virtual bool		HasHealth(bool arg0);	// HasHealth (baseForm health > 0 or Flags bit23 set)
	virtual bool		GetHasKnockedState(void);
	virtual bool		Unk_8D(void);
	virtual void		Unk_8E(void);
	virtual void		Unk_8F(void);
	virtual void		Unk_90(void);

	enum {
		kFlags_Unk00000002			= 0x00000002,
		kFlags_Deleted				= 0x00000020,		// refr removed from .esp or savegame
		kFlags_Taken				= kFlags_Deleted | kFlags_Unk00000002,
		kFlags_Persistent			= 0x00000400,		//shared bit with kFormFlags_QuestItem
		kFlags_Temporary			= 0x00004000,
		kFlags_IgnoreFriendlyHits	= 0x00100000,
		kFlags_Destroyed			= 0x00800000,

		kChanged_Inventory			= 0x08000000,
	};

	struct RenderState
	{
		TESObjectREFR	*waterRef;		// 00
		uint32_t			unk04;			// 04	0-0x13 when fully-underwater; exterior only
		float			waterLevel;		// 08
		float			unk0C;			// 0C
		uint32_t			unk10;			// 10
		NiNode			*niNode14;		// 14
		NiNode			*niNode18;		// 18
	};

	struct EditorData {
		uint32_t	unk00;	// 00
	};
	// 0C

#ifdef EDITOR
	EditorData	editorData;			// +04
#endif

	TESChildCell	childCell;				// 018

	TESSound		*loopSound;				// 01C

	TESForm			*baseForm;				// 020

	float			rotX, rotY, rotZ;		// 024 - either public or accessed via simple inline accessor common to all child classes
	float			posX, posY, posZ;		// 030 - seems to be private
	float			scale;					// 03C

	TESObjectCELL	*parentCell;			// 040
	ExtraDataList	extraDataList;			// 044
	RenderState		*renderState;			// 064

	ScriptEventList *GetEventList() const;

	bool IsTaken() const {return (flags & kFlags_Taken) == kFlags_Taken;} // Need to implement
	bool IsPersistent() const {return (flags & kFlags_Persistent) != 0;}
	bool IsTemporary() {return (flags & kFlags_Temporary) ? true : false;}  // If the reference is NOT save-baked.
	bool IsDeleted() {return (flags & kFlags_Deleted) ? true : false;}
	bool IsDestroyed() {return (flags & kFlags_Destroyed) ? true : false;}
	const char *hk_GetName();
	NiVector3 *PosVector() {return (NiVector3*)&posX;}
	CoordXY *PosXY() {return (CoordXY*)&posX;}

	void Update3D();
	NiAVObject* __fastcall GetNiBlock2(const char* blockName) const;
	TESContainer *GetContainer();
	bool IsMapMarker();

	TESForm *GetBaseForm();
	bool GetDisabled();
	ExtraContainerChanges *GetOrCreateContainerChanges();
	InventoryChanges *GetInventoryChanges();
	int32_t GetItemCount(TESForm *form);
	void AddItemAlt(TESForm *item, uint32_t count, float condition, bool doEquip);
	bool GetInventoryItems(uint8_t typeID);
	TESObjectCELL *GetParentCell();
	TESWorldSpace *GetParentWorld();
	float GetDistance(TESObjectREFR *target);
	void SetPos(NiVector3 &posVector);
	void SetAngle(NiVector3 &rotVector);
	bool MoveToCell(TESForm *worldOrCell, NiVector3 &posVector);
	TESObjectREFR *GetMerchantContainer();
	ContChangesEntry* GetContainerChangesEntry(TESForm* itemForm) const;
	float GetWaterImmersionPerc();
	bool IsMobile();
	bool IsGrabbable();
	bool SetLinkedRef(TESObjectREFR *linkObj, uint8_t modIdx);
	bool ValidForHooks();
	NiAVObject *GetNiBlock(const char *blockName);
	NiNode *GetNode(const char *nodeName);
	hkpRigidBody *GetRigidBody(const char *nodeName) const;
	bool RunScriptSource(const char *sourceStr);

	// Does the same thing as GetNiNode
	NiNode* Get3D() const;

	TESObjectREFR* PlaceAtMe(TESForm* toPlace, int count = 1, int useNodePos = 0, int direction = 0);

	static TESObjectREFR* __stdcall Create(bool bTemp = false);

	MEMBER_FN_PREFIX(TESObjectREFR);
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	DEFINE_MEMBER_FN(Activate, bool, 0x00573170, TESObjectREFR*, uint32_t, uint32_t, uint32_t);	// Usage Activate(actionRef, 0, 0, 1); found inside Cmd_Activate_Execute as the last call (190 bytes)
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	DEFINE_MEMBER_FN(Activate, bool, 0x00573430, TESObjectREFR*, uint32_t, uint32_t, uint32_t);	// Usage Activate(actionRef, 0, 0, 1); found inside Cmd_Activate_Execute
#elif EDITOR
#else
#error
#endif
};

static_assert(sizeof(TESObjectREFR) == 0x068);

class BaseProcess;
struct NiPoint3;


// 88
class MobileObject : public TESObjectREFR
{
public:
	MobileObject();
	~MobileObject();

	virtual void		Unk_91(void);
	virtual void		Unk_92(void);
	virtual void		Unk_93(void);
	virtual void		Unk_94(void);
	virtual void		Unk_95(void);
	virtual void		Unk_96(void);
	virtual void		Unk_97(void);
	virtual void		Unk_98(void);
	virtual void		Unk_99(void);
	virtual void		Unk_9A(void);
	virtual void		Unk_9B(void);
	virtual void		Unk_9C(void);
	virtual void		Unk_9D(void);
	virtual void		Unk_9E(void);
	virtual void		Unk_9F(void);
	virtual void		Unk_A0(void);
	virtual void		Unk_A1(void);
	virtual void		Unk_A2(void);
	virtual void		Unk_A3(void);
	virtual void		Unk_A4(void);
	virtual void		Unk_A5(void);
	virtual void		Unk_A6(void);
	virtual void		Unk_A7(void);
	virtual void		Unk_A8(void);
	virtual void		Unk_A9(void);
	virtual void		Unk_AA(void);
	virtual void		Unk_AB(void);
	virtual void		Unk_AC(void);
	virtual void		Unk_AD(void);
	virtual void		Unk_AE(void);
	virtual float		AdjustRot(uint32_t arg1);
	virtual void		Unk_B0(void);
	virtual void		Unk_B1(void);
	virtual void		Unk_B2(void);
	virtual void		Unk_B3(void);
	virtual void		Unk_B4(void);
	virtual void		Unk_B5(void);
	virtual void		Unk_B6(void);
	virtual void		Unk_B7(void);
	virtual void		Unk_B8(void);
	virtual void		Unk_B9(void);
	virtual void		Unk_BA(void);
	virtual void		Unk_BB(void);
	virtual void		Unk_BC(void);
	virtual void		Unk_BD(void);
	virtual void		Unk_BE(void);
	virtual void		Unk_BF(void);
	virtual void		Unk_C0(void);

	BaseProcess		*baseProcess;	// 68
	uint32_t			unk6C;			// 6C - loaded
	TESObjectREFR	*unk70;			// 70 - loaded
	uint32_t			unk74;			// 74 - loaded
	uint32_t			unk78;			// 78 - loaded
	uint8_t			unk7C;			// 7C - loaded
	uint8_t			unk7D;			// 7D - loaded
	uint8_t			unk7E;			// 7E - loaded
	uint8_t			unk7F;			// 7F - loaded
	uint8_t			unk80;			// 80 - loaded
	uint8_t			unk81;			// 81 - loaded
	uint8_t			unk82;			// 82
	uint8_t			unk83;			// 83 - loaded
	uint8_t			unk84;			// 84 - loaded
	uint8_t			unk85;			// 85 - loaded
	uint8_t			unk86;			// 86 - loaded
	uint8_t			unk87;			// 87	Init'd to the inverse of NoLowLevelProcessing
};

static_assert(sizeof(MobileObject) == 0x88);

class MagicTarget;
typedef tList<ActiveEffect> ActiveEffectList;

// 0C
class MagicCaster
{
public:
	MagicCaster();
	~MagicCaster();

	virtual void	Unk_00(void);
	virtual void	Unk_01(void);
	virtual void	Unk_02(void);
	virtual void	CastSpell(MagicItem *spell, bool arg2, MagicTarget *target, float arg4, bool arg5);
	virtual void	Unk_04(void);
	virtual void	AddEffect(MagicItem *magItem, TESForm *itemForm, bool arg3);
	virtual void	Unk_06(void);
	virtual void	Unk_07(void);
	virtual void	Unk_08(void);
	virtual void	Unk_09(void);
	virtual void	Unk_0A(void);
	virtual Actor	*GetActor(void);
	virtual void	Unk_0C(void);
	virtual void	Unk_0D(void);
	virtual void	Unk_0E(void);
	virtual void	Unk_0F(void);
	virtual void	Unk_10(MagicItem *spell);
	virtual void	Unk_11(void);
	virtual void	Unk_12(MagicTarget *magicTarget);
	virtual void	Unk_13(void);

	uint32_t	unk04[2];	// 04
};
static_assert(sizeof(MagicCaster) == 0xC);

// 10
class MagicTarget
{
public:
	MagicTarget();
	~MagicTarget();

	virtual bool	ApplyEffect(MagicCaster *magicCaster, MagicItem *magicItem, ActiveEffect *activeEffect, bool arg4);
	virtual Actor	*GetActor(void);
	virtual ActiveEffectList	*GetEffectList(void);
	virtual void	Unk_03(void);
	virtual bool	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual void	Unk_07(void);
	virtual void	Unk_08(void);
	virtual float	Unk_09(MagicCaster *magicCaster, MagicItem *magicItem, ActiveEffect *activeEffect);
	virtual void	Unk_0A(void);
	virtual void	Unk_0B(void);

	uint32_t	unk04[3];	// 04

	void RemoveEffect(EffectItem *effItem);
};
static_assert(sizeof(MagicTarget) == 0x10);

class bhkRagdollController;
class bhkRagdollPenetrationUtil;
class ImageSpaceModifierInstanceDOF;
class ImageSpaceModifierInstanceDRB;

class PathingRequest;
class PathingSolution;
class DetailedActorPathHandler;
class ActorPathingMessageQueue;

// 28
class PathingLocation
{
public:
	virtual void	Unk_00(void);
	virtual void	Unk_01(void);
	virtual void	Unk_02(void);

	uint32_t			unk04[9];	// 04
};

// 88
class ActorMover
{
public:
	ActorMover();
	~ActorMover();

	virtual void		Unk_00(void);
	virtual void		Unk_01(void);
	virtual void		Unk_02(void);
	virtual void		Unk_03(void);
	virtual void		Unk_04(void);
	virtual void		Unk_05(void);
	virtual void		Unk_06(void);
	virtual void		Unk_07(void);
	virtual uint32_t		GetMovementFlags();
		//	Running		0x200
		//	Sneaking	0x400

		// bit 11 = swimming
		// bit 9 = sneaking
		// bit 8 = run
		// bit 7 = walk
		// bit 0 = keep moving (Q)
	virtual void		Unk_09(void);
	virtual void		Unk_0A(void);
	virtual void		Unk_0B(void);
	virtual void		Unk_0C(void);
	virtual void		Unk_0D(void);
	virtual void		Unk_0E(void);

	uint32_t						unk04[6];			// 04
	PathingRequest				*pathingRequest;	// 1C
	PathingSolution				*pathingSolution;	// 20
	DetailedActorPathHandler	*pathHandler;		// 24
	Actor						*actor;				// 28
	uint32_t						unk2C;				// 2C
	ActorPathingMessageQueue	*pathingMsgQueue;	// 30
	uint32_t						movementFlags1;		// 34
	uint32_t						unk38;				// 38
	uint32_t						movementFlags2;		// 3C
	uint32_t						unk40;				// 40
	PathingLocation				pathingLocation;	// 44
	uint32_t						unk6C;				// 6C
	uint8_t						unk70[4];			// 70
	uint32_t						unk74;				// 74
	uint32_t						unk78;				// 78
	uint32_t						unk7C;				// 7C
	uint32_t						unk80;				// 80
	uint32_t						unk84;				// 84
};

// A0
class PlayerMover : public ActorMover
{
public:
	PlayerMover();
	~PlayerMover();

	uint32_t			unk88;				// 88
	uint32_t			unk8C;				// 8C
	uint32_t			unk90;				// 90
	uint32_t			pcMovementFlags;	// 94
	uint32_t			unk98;				// 98
	uint32_t			unk9C;				// 9C
};

class CombatController;
struct PackageInfo;
struct CombatActors;
struct ItemEntryData;
class BSAnimGroupSequence;
class BackUpPackage;

typedef ActiveEffect *(*ActiveEffectCreate)(MagicCaster *magCaster, MagicItem *magItem, EffectItem *effItem);

class Actor : public MobileObject
{
public:
	Actor();
	~Actor();

	virtual void		Unk_C1(void);
	virtual void		Unk_C2(void);
	virtual void		Unk_C3(void);
	virtual void		Unk_C4(void);
	virtual void		Unk_C5(void);
	virtual void		Unk_C6(void);
	virtual void		SetIgnoreCrime(bool ignoreCrime);
	virtual bool		GetIgnoreCrime(void);
	virtual void		Unk_C9(void);
	virtual void		Unk_CA(void);
	virtual void		Unk_CB(void);
	virtual void		Unk_CC(void);
	virtual void		Unk_CD(void);
	virtual void		Unk_CE(void);
	virtual void		DamageActionPoints(float amount); // checks GetIsGodMode before decreasing
	virtual void		Unk_D0(void);
	virtual void		Unk_D1(void);
	virtual void		Unk_D2(void);
	virtual void		Unk_D3(void);
	virtual void		Unk_D4(void);
	virtual void		Unk_D5(void);
	virtual void		IsOverencumbered(void);
	virtual void		Unk_D7(void);
	virtual bool		IsPlayerRef(void);
	virtual void		Unk_D9(void);
	virtual void		Unk_DA(void);
	virtual void		Unk_DB(void);
	virtual void		Unk_DC(void);
	virtual void		Unk_DD(void);
	virtual void		Unk_DE(void);
	virtual void		Unk_DF(void);
	virtual void		Unk_E0(void);
	virtual void		Unk_E1(void);
	virtual void		Unk_E2(void);
	virtual void		Unk_E3(void);
	virtual uint32_t		GetActorType(void);	// Creature = 0, Character = 1, PlayerCharacter = 2
	virtual void		SetActorValue(uint32_t avCode, float value);
	virtual void		SetActorValueInt(uint32_t avCode, uint32_t value);
	virtual void		Unk_E7(void);
	virtual void		Unk_E8(void);
	virtual void		Unk_E9(void);
	virtual void		ModActorValue(uint32_t avCode, int modifier, uint32_t arg3);
	virtual void		DamageActorValue(uint32_t avCode, float damage, Actor *attacker);
	virtual void		Unk_EC(void);
	virtual void		Unk_ED(void);
	virtual void		Unk_EE(void);
	virtual ExtraContainerChanges::EntryData *GetPreferedWeapon(uint32_t unk);
	virtual void		Unk_F0(void);
	virtual void		Unk_F1(void);
	virtual void		Unk_F2(void);
	virtual void		Unk_F3(void);
	virtual void		Unk_F4(void);
	virtual void		Unk_F5(void);
	virtual void		Unk_F6(void);
	virtual void		Unk_F7(void);
	virtual void		Unk_F8(void);
	virtual void		Unk_F9(void);
	virtual void		Reload(TESObjectWEAP* weapon, uint32_t animType, uint8_t hasExtendedClip); // credits to JIP
	virtual void		Reload2(TESObjectWEAP* weapon, uint32_t animType, uint8_t hasExtendedClip, uint8_t isInstantSwapHotkey); // credits to JIP
	virtual void		Unk_FC(void);
	virtual void		Unk_FD(void);
	virtual void		Unk_FE(void);
	virtual void		Unk_FF(void);
	virtual void		Unk_100(void);
	virtual void		Unk_101(void);
	virtual void		Unk_102(void);
	virtual void		Unk_103(void);
	virtual void		Unk_104(void);
	virtual void		Unk_105(void);
	virtual void		Unk_106(void);
	virtual void		Unk_107(void);
	virtual void		Unk_108(void);
	virtual void		Unk_109(void);
	virtual CombatController	*GetCombatController(void);
	virtual Actor		*GetCombatTarget(void);
	virtual void		Unk_10C(void);
	virtual void		Unk_10D(void);
	virtual void		Unk_10E(void);
	virtual float		GetTotalArmorDR(void);
	virtual float		GetTotalArmorDT(void);
	virtual uint32_t		Unk_111(void);
	virtual void		Unk_112(void);
	virtual void		Unk_113(void);
	virtual void		Unk_114(void);
	virtual void		Unk_115(void);
	virtual float		CalcSpeedMult(void);
	virtual void		Unk_117(void);
	virtual void		Unk_118(void);
	virtual void		Unk_119(void);
	virtual void		Unk_11A(void);
	virtual void		Unk_11B(void);
	virtual void		Unk_11C(void);
	virtual void		Unk_11D(void);
	virtual void		Unk_11E(void);
	virtual void		Unk_11F(void);
	virtual void		Unk_120(void);
	virtual void		Unk_121(void);
	virtual void		RewardXP(uint32_t amount);
	virtual void		Unk_123(void);
	virtual void		Unk_124(void);
	virtual void		Unk_125(void);
	virtual void		SetPerkRank(BGSPerk *perk, uint8_t rank, bool alt);
	virtual void		RemovePerk(BGSPerk *perk, bool alt);
	virtual uint8_t		GetPerkRank(BGSPerk *perk, bool alt);
	virtual void		Unk_129(void);
	virtual void		Unk_12A(void);
	virtual void		Unk_12B(void);
	virtual void		Unk_12C(uint16_t animKey, bool unk1);
	virtual void		Unk_12D(void);
	virtual void		DoHealthDamage(Actor *attacker, float damage);
	virtual void		Unk_12F(void);
	virtual void		Unk_130(void);
	virtual float		Unk_131(void);
	virtual void		Unk_132(void);
	virtual void		Unk_133(void);
	virtual void		Unk_134(void);
	virtual void		Unk_135(void);
	virtual void		Unk_136(void);

	MagicCaster			magicCaster;			// 088
	MagicTarget			magicTarget;			// 094
	ActorValueOwner		avOwner;				// 0A4
	CachedValuesOwner	cvOwner;				// 0A8

	bhkRagdollController				*ragDollController;			// 0AC
	bhkRagdollPenetrationUtil			*ragDollPentrationUtil;		// 0B0
	uint32_t								unk0B4;						// 0B4-
	float								flt0B8;						// 0B8
	uint8_t								byte0BC;					// 0BC-
	uint8_t								byte0BD;					// 0BD
	uint8_t								byte0BE;					// 0BE
	uint8_t								byte0BF;					// 0BF
	Actor								*killer;					// 0C0
	uint8_t								byte0C4;					// 0C4-
	uint8_t								byte0C5;					// 0C5
	uint8_t								byte0C6;					// 0C6
	uint8_t								byte0C7;					// 0C7
	float								flt0C8;						// 0C8
	float								flt0CC;						// 0CC
	tList<void>							list0D0;					// 0D0
	uint8_t								byte0D8;					// 0D8
	uint8_t								byte0D9;					// 0D9
	uint8_t								byte0DA;					// 0DA
	uint8_t								byte0DB;					// 0DB
	uint32_t								unk0DC;						// 0DC
	tList<void>							list0E0;					// 0E0
	uint8_t								byte0E8;					// 0E8	const 1
	uint8_t								byte0E9;					// 0E9
	uint8_t								byte0EA;					// 0EA
	uint8_t								byte0EB;					// 0EB
	uint32_t								unk0EC;						// 0EC
	uint8_t								byte0F0;					// 0F0-
	uint8_t								byte0F1;					// 0F1-
	uint8_t								byte0F2;					// 0F2
	uint8_t								byte0F3;					// 0F3
	tList<void>							list0F4;					// 0F4
	tList<void>							list0FC;					// 0FC
	bool								isInCombat;					// 104

	enum
	{
		kHookActorFlag1_CombatDisabled = 1 << 0,
		kHookActorFlag1_ForceCombatTarget = 1 << 1,
		kHookActorFlag1_CombatAIModified = kHookActorFlag1_CombatDisabled | kHookActorFlag1_ForceCombatTarget,
		kHookActorFlag1_InfiniteAmmo = 1 << 2,
		kHookActorFlag1_LockedEquipment = 1 << 3,
		kHookActorFlag1_DetectionFix = 1 << 4,
		kHookActorFlag1_PCTeleportWait = 1 << 5,
		kHookActorFlag1_PCTeleportFollow = 1 << 6,
		kHookActorFlag1_PCTeleportAI = kHookActorFlag1_PCTeleportWait | kHookActorFlag1_PCTeleportFollow,
		kHookActorFlag1_DisableWheel = 1 << 7,

		kHookActorFlag2_ForceDetectionVal = 1 << 0,
		kHookActorFlag2_NonTargetable = 1 << 1,
		kHookActorFlag2_TeammateKillable = 1 << 2,
		kHookActorFlag2_NoGunWobble = 1 << 3,
		kHookActorFlag2_CastImmediate = 1 << 4,

		kHookActorFlag3_OnAnimAction = 1 << 0,
		kHookActorFlag3_OnPlayGroup = 1 << 1,
		kHookActorFlag3_OnHealthDamage = 1 << 2,
		kHookActorFlag3_OnCrippledLimb = 1 << 3,
		kHookActorFlag3_OnFireWeapon = 1 << 4,
		kHookActorFlag3_OnHit = 1 << 5,

		kHookActorFlags_DetectionModified = kHookActorFlag1_DetectionFix | (kHookActorFlag2_ForceDetectionVal << 8),
	};

	uint8_t								jipActorFlags1;				// 105
	uint8_t								jipActorFlags2;				// 106
	uint8_t								jipActorFlags3;				// 107

	uint32_t								lifeState;					// 108	saved as byte HasHealth = 1 or 2, optionally 6, 5 = IsRestrained
	uint32_t								criticalStage;				// 10C
	uint32_t								animGroupID110;				// 110- decoded by Tweaks (?).
	float								flt114;						// 114
	uint8_t								byte118;					// 118-
	uint8_t								byte119;					// 119+
	uint16_t								jip11A;						// 11A+
	uint32_t								unk11C;						// 11C-
	uint32_t								unk120;						// 120-
	bool								forceRun;					// 124
	bool								forceSneak;					// 125
	uint8_t								byte126;					// 126-
	uint8_t								byte127;					// 127-
	Actor								*combatTarget;				// 128
	BSSimpleArray<Actor*>				*combatTargets;				// 12C
	BSSimpleArray<Actor*>				*combatAllies;				// 130
	uint8_t								byte134;					// 134-
	uint8_t								byte135;					// 135+
	uint16_t								jip136;						// 136+
	uint32_t								unk138;						// 138-
	uint32_t								unk13C;						// 13C-
	uint32_t								actorFlags;					// 140	0x80000000 - IsEssential
	bool								ignoreCrime;				// 144
	uint8_t								byte145;					// 145	Has to do with package evaluation
	uint8_t								byte146;					// 146	Has to do with package evaluation
	uint8_t								byte147;					// 147
	uint32_t								unk148;						// 148-
	uint8_t								inWater;					// 14C
	uint8_t								isSwimming;					// 14D
	uint16_t								jip14E;						// 14E+
	uint32_t								unk150;						// 150-
	float								flt154;						// 154
	float								flt158;						// 158
	uint8_t								byte15C;					// 15C-
	uint8_t								byte15D;					// 15D-
	uint16_t								jip15E;						// 15E+
	NiVector3							startingPos;				// 160
	float								flt16C;						// 16C
	TESForm								*startingWorldOrCell;		// 170
	uint8_t								byte174;					// 174-
	uint8_t								byte175;					// 175-
	uint16_t								jip176;						// 176+
	float								flt178;						// 178
	float								flt17C;						// 17C
	float								flt180;						// 180
	float								flt184;						// 184
	float								flt188;						// 188
	uint8_t								byte18C;					// 18C-
	bool								isTeammate;					// 18D
	uint8_t								byte18E;					// 18E-
	uint8_t								byte18F;					// 18F
	ActorMover							*actorMover;				// 190
	uint32_t								unk194;						// 194-
	uint32_t								unk198;						// 198-
	float								flt19C;						// 19C
	uint32_t								unk1A0;						// 1A0-
	uint32_t								unk1A4;						// 1A4-
	uint32_t								unk1A8;						// 1A8-
	uint32_t								unk1AC;						// 1AC-
	uint8_t								byte1B0;					// 1B0-
	bool								forceHit;					// 1B1-
	uint8_t								byte1B2;					// 1B2
	uint8_t								byte1B3;					// 1B3

	// OBSE: unk1 looks like quantity, usu. 1; ignored for ammo (equips entire stack). In NVSE, pretty much always forced internally to 1
	// OBSE: itemExtraList is NULL as the container changes entry is not resolved before the call
	// NVSE: Default values are those used by the vanilla script functions.
	void EquipItem(TESForm *objType, uint32_t equipCount = 1, ExtraDataList *itemExtraList = NULL, bool unk3 = 1, bool lockEquip = false, bool unk5 = 1);	// unk3 apply enchantment on player differently
	void UnequipItem(TESForm *objType, uint32_t unequipCount = 1, ExtraDataList *itemExtraList = NULL, bool unk3 = 1, bool lockUnequip_unused = false, bool unk5 = 1);

	//EquippedItemsList GetEquippedItems();
	//ExtraContainerDataArray GetEquippedEntryDataList();
	//ExtraContainerExtendDataArray GetEquippedExtendDataList();

	bool GetDead() {return (lifeState == 1) || (lifeState == 2);}
	bool GetRestrained() {return lifeState == 5;}

	TESActorBase *GetActorBase();
	bool GetLOS(Actor *target);
	char GetCurrentAIPackage();
	char GetCurrentAIProcedure();
	bool IsFleeing();
	TESObjectWEAP *GetEquippedWeapon() const;
	bool IsItemEquipped(TESForm *item);
	bool GetEquippedItemData(uint32_t slotIndex, ItemEntryData &itemData);
	uint8_t EquippedWeaponHasMod(uint8_t modID);
	bool IsSneaking();
	void StopCombat();

	// if "this" is the player, won't work properly!
	// See JIP's Cmd_IsInCombatWith
	bool __fastcall IsInCombatWith(Actor* target) const;

	int GetDetectionValue(Actor *detected);
	TESPackage *GetStablePackage();
	PackageInfo *GetPackageInfo();
	TESObjectREFR *GetPackageTarget();
	TESCombatStyle *GetCombatStyle();
	bool GetKnockedState();
	bool IsWeaponOut();
	void UpdateActiveEffects(MagicItem *magicItem, EffectItem *effItem, ActiveEffectCreate callback, bool addNew);
	bool GetIsGhost();
	float GetRadiationLevel();
	BackUpPackage *AddBackUpPackage(TESObjectREFR *targetRef, TESObjectCELL *targetCell, uint32_t flags);
	void TurnToFaceObject(TESObjectREFR *target);
	void TurnAngle(float angle);
	void SetAnimActionAndSequence(int32_t animAction, BSAnimGroupSequence *animGroupSeq);
	void PlayIdle(TESIdleForm *idleAnim);
	uint32_t GetLevel();
	float GetKillXP();
	void DismemberLimb(uint32_t bodyPartID, bool explode);
	void EquipItemAlt(ExtraContainerChanges::EntryData *itemEntry, bool noUnequip, bool noMessage);
	void EquipContainer(TESContainer *container);
	bool HasNoPath();
	bool IsInvisible();
	int32_t GetDetectionLevelAlt(Actor* target, bool calculateSneakLevel);
	bool Detects(Actor* target);
	float GetHealthEffectsSum();
	void Kill(Actor* killer);
	bool GetShouldAttack(Actor* target);
	void SetWantsWeaponOut(bool wantsWeaponOut);
	bool IsInReloadAnim();
	bool IsDoingAttackAnimation() const;
};

// 1C0
class Creature : public Actor
{
public:
	Creature();
	~Creature();

	virtual void	Unk_137(void);

	uint32_t			unk1B4[3];			// 1B4
};

// 1C8
class Character : public Actor
{
public:
	Character();
	~Character();

	virtual void	Unk_137(void);
	virtual void	Unk_138(void);

	ValidBip01Names	*validBip01Names;	// 1B4
	float			totalArmorDR;		// 1B8
	float			totalArmorDT;		// 1BC
	uint8_t			isTrespassing;		// 1C0
	uint8_t			byt1C1;				// 1C1
	uint16_t			unk1C2;				// 1C2
	float			unk1C4;				// 1C4
};

class bhkRigidBody;
struct ParentSpaceNode;
struct TeleportLink;
struct ItemChange;
class NiObject;
struct MusicMarker;

struct PerkRank
{
	BGSPerk		*perk;
	uint8_t		rank;
	uint8_t		pad05[3];
};

// E50
class PlayerCharacter : public Character
{
public:
	PlayerCharacter();
	~PlayerCharacter();

	// used to flag controls as disabled in disabledControlFlags
	enum
	{
		kControlFlag_Movement		= 1 << 0,
		kControlFlag_Look			= 1 << 1,
		kControlFlag_Pipboy			= 1 << 2,
		kControlFlag_Fight			= 1 << 3,
		kControlFlag_POVSwitch		= 1 << 4,
		kControlFlag_RolloverText	= 1 << 5,
		kControlFlag_Sneak			= 1 << 6,
	};

	virtual void		Unk_139(void);
	virtual void		Unk_13A(void);

	struct MapMarkerInfo
	{
		ExtraMapMarker::MarkerData	*markerData;
		TESObjectREFR				*markerRef;
	};

	struct CompassTarget
	{
		Actor		*target;
		uint8_t		isHostile;
		uint8_t		isDetected;
		uint8_t		pad06[2];

		CompassTarget(Actor* target, bool isHostile, bool isDetected)
			: target(target), isHostile(isHostile), isDetected(isDetected)
		{}
		CompassTarget(const CompassTarget& t) : target(t.target), isHostile(t.isHostile), isDetected(t.isDetected)
		{}
	};

	uint32_t								unk1C8[16];				// 1C8	208 could be a DialogPackage
	TESForm								*form208;				// 208 TESTING
	void								*unk20C;				// 20C
	tList<ActiveEffect>					*activeEffects;			// 210
	TESForm								*form214;				// 214 TESTING
	TESForm								*form218;				// 218 TESTING
	void								*unk21C;				// 21C
	uint32_t								unk220[8];				// 220	224 is a package of type 1C
	bool								showQuestItems;			// 240
	uint8_t								byte241;				// 241
	uint8_t								byte242;				// 242
	uint8_t								byte243;				// 243
	float								unk244[77];				// 244	have to be a set of ActorValue
	float								permAVMods[77];			// 378	have to be a set of ActorValue
	float								flt4AC;					// 4AC
	float								actorValues4B0[77];		// 4B0	have to be a set of ActorValue
	tList<BGSNote>						notes;					// 5E4
	ImageSpaceModifierInstanceDOF		*unk5EC;				// 5EC
	ImageSpaceModifierInstanceDOF		*unk5F0;				// 5F0
	ImageSpaceModifierInstanceDRB		*unk5F4;				// 5F4
	uint8_t								byte5F8;				// 5F8
	uint8_t								byte5F9;				// 5F9
	uint8_t								byte5FA;				// 5FA
	uint8_t								byte5FB;				// 5FB
	tList<TESObjectREFR>				teammates;				// 5FC
	TESObjectREFR						*lastExteriorDoor;		// 604
	void								*unk608;				// 608
	void								*unk60C;				// 60C
	void								*unk610;				// 610
	tList<TESCaravanCard>				*caravanCards1;			// 614
	tList<TESCaravanCard>				*caravanCards2;			// 618
	uint32_t								unk61C[7];				// 61C
	TESObjectREFR						*grabbedRef;			// 638
	uint32_t								unk63C[3];				// 63C
	uint8_t								byte648;				// 648
	uint8_t								byte649;				// 649
	bool								is3rdPersonVisible;		// 64A	= not FirstPerson, credits to lStewieAl for the name.
	bool								is3rdPerson;			// 64B, credits to lStewieAl
	bool								bThirdPerson;			// 64C, credits to lStewieAl
	uint8_t								byte64D;				// 64D
	uint8_t								byte64E;				// 64E
	bool								isUsingScope;			// 64F
	uint8_t								byte650;				// 650
	bool								alwaysRun;				// 651
	bool								autoMove;				// 652
	uint8_t								byte653;				// 653
	uint32_t								sleepHours;				// 654
	uint8_t								byte658;				// 658	Something to do with SleepDeprivation; see 0x969DCF
	uint8_t								byte659;				// 659
	uint8_t								byte65A;				// 65A
	uint8_t								byte65B;				// 65B
	uint32_t								unk65C[4];				// 65C
	uint8_t								byte66C;				// 66C
	uint8_t								byte66D;				// 66D  Something to do with Fast Travel flags
	uint8_t								canSleepWait;			// 66E
	uint8_t								byte66F;				// 66F
	//uint32_t								unk670[4];				// 670
	float                               worldFOV;				//670
	float								firstPersonFOV;
	float								unk678;
	float								unk67C;
	uint8_t								pcControlFlags;			// 680
	uint8_t								isWaitingForOpenContainerAnim;	// 681, credits to lStewieAl
	uint8_t								byte682;				// 682
	uint8_t								byte683;				// 683
	uint32_t								waitingForContainerOpenAnimPreventActivateTimer; // 684, credits to lStewieAl
	TESObjectREFR						*activatedDoor;			// 688, credits to lStewieAl
	ValidBip01Names						*VB01N1stPerson;		// 68C
	void								*unk690;				// 690
	NiNode								*playerNode;			// 694 used as node if unk64A is true
	uint32_t								unk698[4];				// 698
	tList<TESTopic>						topicList;				// 6A8
	uint32_t								unk6B0[2];				// 6B0
	TESQuest							*activeQuest;			// 6B8
	tList<BGSQuestObjective>			questObjectiveList;		// 6BC
	tList<BGSQuestObjective::Target>	questTargetList;		// 6C4
	uint8_t								byte6CC;				// 6CC
	uint8_t								pad6CD[3];				// 6CD
	uint32_t								time6D0;				// 6D0
	uint32_t								unk6D4;					// 6D4
	bool								bIsAMurderer;			// 6D8
	uint8_t								pad6D9;					// 6D9
	uint8_t								pad6DA;					// 6DA
	uint8_t								pad6DB;					// 6DB
	uint32_t								amountSoldStolen;		// 6DC
	uint32_t								sortActorDistanceListTimer; // 6E0
	uint32_t								seatedRotation;			// 6E4
	uint8_t								byte6E8;				// 6E8
	uint8_t								byte6E9;				// 6E9
	uint8_t								byte6EA;				// 6EA
	uint8_t								byte6EB;				// 6EB
	TESForm								*form6EC;				// 6EC TESTING
	TESForm								*form6F0;				// 6F0 TESTING
	uint32_t								unk6F4;					// 6F4;
	BSSimpleArray<ParentSpaceNode>		parentSpaceNodes;		// 6F8
	BSSimpleArray<TeleportLink>			teleportLinks;			// 708
	uint32_t								unk718[9];				// 718
	TESForm								*form73C;				// 73C	TESTING
	uint32_t								unk740[6];				// 740
	TESForm								*tempWeaponPoisonRef;	// 758 TESTING
	bool								inCharGen;				// 75C
	uint8_t								byte75D;				// 75D
	uint8_t								byte75E;				// 75E
	uint8_t								byte75F;				// 75F
	TESRegion							*currentRegion;			// 760
	TESRegionList						regionsList;			// 764
	uint32_t								unk774;					// 774
	uint32_t								unk778;					// 778
	uint32_t								heartBeatSound[7];		// 77C
	bool								isInSleepPackage;		// 798
	bool								isTransitioningSeatedState; // 799
	bool								isDrinkingPlacedWater;	// 79A
	bool								isPackagePlaying;		// 79B
	uint8_t								byte79C;				// 79C
	uint8_t								byte79D;				// 79D
	uint8_t								gap79E[2];				// 79E
	uint32_t								unk7A0[3];				// 7A0
	TESForm								*pcWorldOrCell;			// 7AC
	uint32_t								unk7B0;					// 7B0
	BGSMusicType						*musicType;				// 7B4
	uint8_t								gameDifficulty;			// 7B8
	uint8_t								byte7B9;				// 7B9
	uint8_t								byte7BA;				// 7BA
	uint8_t								byte7BB;				// 7BB
	bool								isHardcore;				// 7BC
	uint8_t								byte7BD;				// 7BD
	uint8_t								byte7BE;				// 7BE
	uint8_t								byte7BF;				// 7BF
	uint32_t								killCamMode;			// 7C0
	uint8_t								byte7C4;				// 7C4
	uint8_t								byte7C5;				// 7C5
	bool								isToddler;				// 7C6
	bool								canUsePA;				// 7C7
	tList<MapMarkerInfo>				mapMarkers;				// 7C8
	TESWorldSpace						*worldSpc7D0;			// 7D0
	tList<MusicMarker>					musicMarkers;			// 7D4
	MusicMarker							*currMusicMarker;		// 7DC
	uint32_t								unk7E0[39];				// 7E0
	tList<PerkRank>						perkRanksPC;			// 87C
	tList<BGSEntryPointPerkEntry>		perkEntriesPC[74];		// 884
	tList<PerkRank>						perkRanksTM;			// AD4
	tList<BGSEntryPointPerkEntry>		perkEntriesTM[74];		// ADC
	uint32_t								unkD2C[4];				// D2C
	NiObject							*unkD3C;				// D3C
	uint32_t								unkD40;					// D40
	Actor								*reticleActor;			// D44
	tList<CompassTarget>				*compassTargets;		// D48
	uint32_t								pipboyLightHeldTime;	// D4C
	uint32_t								ammoSwapTimer;			// D50
	bool								shouldOpenPipboy;		// D54
	uint8_t								byteD55;				// D55
	uint8_t								byteD56;				// D56
	uint8_t								byteD57;				// D57
	uint32_t*								ptD58[3];				// D58
	CombatActors						*combatActors;			// D64
	uint32_t								teammateCount;			// D68
	uint32_t								unkD6C[5];				// D6C
	NiNode								*niNodeD80;				// D80
	uint32_t								unkD84[12];				// D84
	NiNode								*niNodeDB4;				// DB4
	uint32_t								unkDB8[7];				// DB8
	NiVector3							vectorDD4;				// DD4
	NiVector3							cameraPos;				// DE0
	bhkRigidBody						*rigidBody;				// DEC
	bool								pcInCombat;				// DF0
	bool								pcUnseen;				// DF1
	uint8_t								byteDF2;				// DF2
	uint8_t								byteDF3;				// DF3
	BSSimpleArray<ItemChange>			itemChanges;			// DF4
	uint32_t								rockItLauncherWeight;   // E04
	bool								hasNightVisionApplied;	// E08
	uint8_t								byteE09; 				// E09
	uint8_t								byteE0A;				// E0A
	uint8_t								byteE0B;				// E0B
	uint32_t								modifiedReputation;		// E0C
	uint32_t								unkE10;        			// E10
	uint32_t								unkE14;       			// E14
	float								killCamTimer;			// E18
	float								killCamCooldown;		// E1C
	uint8_t								byteE20;				// E20
	bool								isUsingTurbo;			// E21
	uint8_t								byteE22;				// E22
	uint8_t								byteE23;				// E23
	float								lastHelloTime;			// E24
	float								counterAttackTimer;		// E28
	uint8_t								byteE2C;				// E2C
	bool								isCateyeEnabled;		// E2D
	bool								isSpottingImprovedActive;//E2E
	uint8_t								byteE2F;				// E2F
	float								itemDetectionTimer;		// E30
	NiNode								*ironSightNode;			// E34
	bool								noHardcoreTracking;		// E38	Appears to be unused
	bool								skipHCNeedsUpdate;		// E39
	uint8_t								byteE3A;				// E3A
	uint8_t								byteE3B;				// E3B
	BSSimpleArray<TESAmmo*>				pcAmmo;					// E3C
	uint32_t								unkE4C;					// E4C
		// 7C6 is a boolean meaning toddler,
		// 7C7 byte bool PCCanUsePowerArmor, Byt0E39 referenced during LoadGame
		// Used by TFC : 7E8/EC/F0 stores Pos, 7F0 adjusted by scaledHeight , 7E0 stores RotZ, 7E4 RotX
		// Quest Stage LogEntry at 6B0.
		// tList at 6C4 is cleared when there is no current quest. There is another NiNode at 069C
		// 086C is cleared after equipement change.

	bool IsThirdPerson() { return bThirdPerson ? true : false; }
	uint32_t GetMovementFlags() { return actorMover->GetMovementFlags(); }	// 11: IsSwimming, 9: IsSneaking, 8: IsRunning, 7: IsWalking, 0: keep moving
	bool IsPlayerSwimming() { return (GetMovementFlags()  >> 11) & 1; }

	static PlayerCharacter*	GetSingleton();
	bool SetSkeletonPath(const char* newPath);
	static void UpdateHead(void);

	bool ToggleFirstPerson(bool toggleON);
	char GetDetectionState();

	// Credits to lStewieAl
	void UpdateCamera(bool isCalledFromFunc21, bool _zero_skipUpdateLOD);
};
static_assert(sizeof(PlayerCharacter) == 0xE50);

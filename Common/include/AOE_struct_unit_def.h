
#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_empires_dat.h>
#include <gameVersion.h>

using namespace AOE_CONST_FUNC;


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size=6. Included in unit definition struct (*3)
	class STRUCT_COST {
	public:
		AOE_CONST_FUNC::RESOURCE_TYPES costType; // 2-bytes (word)
		short int costAmount;
		short int costPaid; // sometimes used as a byte. Warning: pop headroom is a non paid cost, but used anyway !
	};
	static_assert(sizeof(STRUCT_COST) == 6, "STRUCT_COST size");


	struct STRUCT_ARMOR_OR_ATTACK {
		AOE_CONST_FUNC::ATTACK_CLASS classId; // "class" is reserved !
		short int amount;
	};
	static_assert(sizeof(STRUCT_ARMOR_OR_ATTACK) == 4, "STRUCT_ARMOR_OR_ATTACK size");


	// Size = 0x4C
	// ReadFromFile=0x4E74F0
	class STRUCT_DEF_UNIT_COMMAND {
	public:
		short int alwaysOne;
		short int unitCommandIndex; // +02. index of this unitCommandDef element
		char unknown_04; // Unknown value
		char unknown_05; // unused ?
		AOE_CONST_FUNC::UNIT_ACTION_ID commandType; // +6
		short int classId; // +08
		short int unitId; // +0A.
		char selectionEnabler; // +0C
		char unknown_0D;
		short int terrainId; // +0E.
		// 0x10
		char selectionMode;
		char rightClickMode; // +11. what is this ?
		char unknown_12;
		char unknown_13;
		short int resourceTypeIn; // +0x14.
		short int resourceProductivityMultiplier;
		short int resourceTypeOut;
		short int resource;
		float workRateMultiplier; // or quantity ?
		// 0x20
		float executionRadius;
		float extraRange;
		char unknown_28;
		char unknown_29; // unused ?
		char unknown_2A; // unused ?
		char unknown_2B; // unused ?
		float unknown_2C;
		// 0x30
		short int plunderSource; // What is this ? 0=from resource, 1=from player (AGE3 tooltip)
		short int unknown_32;
		unsigned long int *ptrToolGraphic; // graphics NOT carrying resource
		unsigned long int *ptrProceedingGraphic; // +38.
		unsigned long int *ptrActionGraphic;
		// 0x40
		unsigned long int ptrCarryingGraphic; // graphics carrying resource
		unsigned long int *executionSound; // +44
		unsigned long int *resourceDepositSound; // +48
		// END of structure
	};



	// Size = 0x0C (for both parent/child classes). Constructor=0x4E7AB0 (no parameter).
	// Parent class checksum=E4 98 54 00, for birds/type50/projectiles
	// Child class checksum=AC 99 54 00 for living/building
	// 0x4E7B60=unitCommandHeader.readFromFile(fileId, arg2, arg3)
	class STRUCT_DEF_UNIT_COMMAND_HEADER {
	public:
		unsigned long int checksum; // AC 99 54 00 + E4 98 54 00 (parent class)
		STRUCT_DEF_UNIT_COMMAND **ptrCommandArray; // +4. The pointed list contains "commandCount" pointers to STRUCT_DEF_UNIT_COMMAND objects
		short int commandCount; // +8.
		unsigned short int unused_0A;

		bool IsCheckSumValid() { return (this->checksum == 0x005499AC) || (this->checksum == 0x005498E4); }
	};


	// Size = 08. Deserialized in 0x441775 (for example)
	class STRUCT_DAMAGE_GRAPHIC {
	public:
		unsigned long int *ptrGraphic;
		char fromDamagePercent; // +4 minimum % of HP from which this graphic is used
		char oldApplyMode; // +5. unused?
		char applyMode; // 0,1=add to graphic (flames), 2=replace graphic(walls)
		char unused_07;
	};
	static_assert(sizeof(STRUCT_DAMAGE_GRAPHIC) == 0x8, "STRUCT_DAMAGE_GRAPHIC size");

	// 34 45 54 00 = Eye candy (type10) - size=0xB8 - Constructor 0x441040
	class STRUCT_UNITDEF_BASE {
	public:
		unsigned long int checksum;
		char unitType; // +04. $46=70=Living unit, 80=building... On 4 bytes... Sometimes these values are 2-bytes, be careful
		char unused_003[3];
		char *ptrUnitName; // +08. Internal name. Technically, length is not limited, size is part of serialized data.
		short int languageDLLID_Name; // If =0, then "ptrUnitName" string will be displayed instead (useful for custom units whose name does not exist in language.dll).
		short int languageDLLID_Creation;
		// 0x10
		short int DAT_ID1; // "Base unit" id from empires.dat. Ex=cavalry
		short int DAT_ID2; // "Upgraded unit" id from empires.dat. Ex=heavy cavalry
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType; // +14. "Class". TribeAIGroupXXX - 3=building, 18=priest...
		char *ptrStandingGraphics; // +18. starts with SLP name.
		unsigned long int *ptrDyingGraphic; // +1C. starts with SLP name.
		// 0x20
		unsigned long int *ptrDyingGraphic2; // +20. Unused ?
		short int deathMode;
		short int totalHitPoints; // +026. Including upgrades !
		float lineOfSight; // +028
		char garrisonCapacity; // +02C
		char unused_02D[3];
		// 0x30
		float sizeRadiusY; // For Y axis
		float sizeRadiusX; // For X axis
		float sizeRadiusZ; // +38.
		unsigned long int *ptrSelectionSound; // +3C
		// 0x40
		unsigned long int *ptrTrainSound;
		unsigned long int *ptrDyingSound; // +44
		short int deadUnitId; // +48
		char placementMode; // +4A
		char airMode; // +4B
		short int iconId; // +4C
		char hideInEditor;
		char unknown_04F; // unused ?
		// 0x50
		short int unknown_050; // unknown1 in AGE3. It IS a word value (0x4414D2)
		char availableForPlayer; // +52. according to tree+researches (+requires enable in empires.dat?). 1="can be trained". 0 does not prevent from having one (scenario, conversion)
		char unknown_053;
		short int placementBypassTerrain1; // +54. Side terrain id ?
		short int placementBypassTerrain2; // +56. Side terrain id ?
		short int placementTerrain1; // +58
		short int placementTerrain2; // +5A
		float editorRadius1; // for Y axis
		// 0x60
		float editorRadius2; // for X axis
		char hillMode; // 0=no restriction, 2 for buildings ?
		char visibleInFog; // +65. Can be 0,1, 3 ("inverted visibility" in AGE3, but not exact. smoke has 3).
		short int terrainRestriction;
		char flyMode;
		char unknown_069;
		short int resourceStorageType_1; // +6A. A resource Id.
		short int resourceStorageType_2;
		short int resourceStorageType_3;
		// 0x70
		float resourceStorageAmount_1;
		float resourceStorageAmount_2;
		float resourceStorageAmount_3;
		long int resourceCapacity;
		// 0x80
		long int resourceDecay; // Amount of resource that is lost per second
		float unknown_084; // unknown3A in AGE3. 0x4415D4
		char resourceStorageEnableMode_1; // +88. type=RESOURCE_TYPES but on 1 byte only
		char resourceStorageEnableMode_2;
		char resourceStorageEnableMode_3;
		char blastArmorLevel;
		char triggerType; // same as class ? TribeAIGroupCivilian = 4, etc?
		AOE_CONST_FUNC::INTERACTION_MODES interactionMode; // +8D. 1-byte.
		char minimapMode; // +8E.
		AOE_CONST_FUNC::COMMAND_ATTRIBUTES commandAttribute; // +8F.
		// 0x90
		char minimapColor;
		char attackMode;
		char edibleMeat; // +92
		char damageGraphicCount; // +93. Count for damageGraphicsArray.
		STRUCT_DAMAGE_GRAPHIC *damageGraphicsArray; // +94. Point to an array of pointers to damage graphics (cf damageGraphicCount)
		long int languageDLLHelp;
		long int languageDLLHotKeyText;
		// 0xA0
		long int hotKey;
		char unselectable; // +A4
		char isSpottableResource; // +A5. 1 for resources other than animals and farms. Such units are added to some player list, but what for ?
		char unknownSpottableResourceAttribute_A6; // +A6. 2 for trees? Is this used ?
		PLAYER_GATHERABLE_RESOURCE_CATEGORIES spottableResourceCategory; // Unknown8 in AGE3. cf 517166.
		char selectionEffect;
		char editorSelectionColor;
		char unknown_0AA;
		char unknown_0AB;
		float selectionRadiusY;
		// 0xB0
		float selectionRadiusX;
		float selectionRadiusZ; // +B4

		bool IsCheckSumValid() const { return (this->checksum == 0x00544534); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_EYE_CANDY); }
		bool IsCheckSumValidForAUnitClass() const {
			return ((this->checksum == 0x00544534) || // eye candy - 10
				(this->checksum == 0x0054440C) || // Flag - type 20
				(this->checksum == 0x00544484) || // Doppleganger - 25
				(this->checksum == 0x005444FC) || // Dead/fish - 30
				(this->checksum == 0x005443CC) || // Birds - 40
				(this->checksum == 0x00544444) || // - 50
				(this->checksum == 0x005444C0) || // projectile - 60
				(this->checksum == 0x00549970) || // living - 70
				(this->checksum == 0x00549930) || // Building - 80
				(this->checksum == 0x005499BC) // Tree - 90
				);
		}
		// Returns true if the unit definition is a flag or a child class (all but eye candy and trees)
		bool DerivesFromFlag() { return (this->unitType >= (char)AOE_CONST_FUNC::GUT_FLAGS) || (this->unitType <= (char)AOE_CONST_FUNC::GUT_BUILDING); } // all but 10 and 90
		// Returns true if the unit definition is dead/fish or a child class
		bool DerivesFromDead_fish() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_DEAD_UNITS) || (this->DerivesFromBird()); }
		// Returns true if the unit definition is a bird or a child class
		bool DerivesFromBird() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_BIRD) || (this->DerivesFromType50()); }
		// Returns true if the unit definition is a living unit or a child class (building)
		bool DerivesFromLiving() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_LIVING_UNIT) || (this->unitType == (char)AOE_CONST_FUNC::GUT_BUILDING); }
		// Returns true if the unit definition is type50 or one of its child classes (projectile, living/building).
		bool DerivesFromType50() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_TYPE50) || (this->unitType == (char)AOE_CONST_FUNC::GUT_PROJECTILE) || (this->DerivesFromLiving()); }
		unsigned long int GetCopyConstructorAddress() { return 0x440FF0; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_BASE) == 0xB8, "STRUCT_UNITDEF_BASE size");

	// 0C 44 54 00 = flag (type20) - size=0xBC - Constructor 0x43E9B0
	class STRUCT_UNITDEF_FLAG : public STRUCT_UNITDEF_BASE {
	public:
		float speed; // 0xB8
		bool IsCheckSumValid() const { return (this->checksum == 0x0054440C); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_FLAGS); }
		unsigned long int GetCopyConstructorAddress() { return 0x43E930; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_FLAG) == 0xBC, "STRUCT_UNITDEF_FLAG size");

	// 84 44 54 00 = Doppleganger - size 0xBC. Constructor(createCopy)=0x4402C0
	class STRUCT_UNITDEF_DOPPLEGANGER : public STRUCT_UNITDEF_FLAG {
	public:
		// 0xBC - no additional fields ?
		bool IsCheckSumValid() const { return (this->checksum == 0x00544484); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_DOPPLEGANGER); }
		unsigned long int GetCopyConstructorAddress() { return 0x43EC40; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_DOPPLEGANGER) == 0xBC, "STRUCT_UNITDEF_DOPPLEGANGER size");

	// FC 44 54 00 = Dead/fish (type30) - size=0xD8 - Constructor 0x440990
	class STRUCT_UNITDEF_DEAD_FISH : public STRUCT_UNITDEF_FLAG {
	public:
		unsigned long int ptrWalkingGraphic1;
		// 0xC0
		unsigned long int ptrWalkingGraphic2;
		float rotationSpeed; // +C4
		char unknown_0C8;
		char unknown_0C9;
		short int trackingUnit; // +CA.
		char trackingUnitUsed; // +CC.
		char unknown_0CD;
		char unknown_0CE;
		char unknown_0CF;
		// 0xD0
		float trackingUnitDensity;
		char unknown_0D4; // unknown16 in AGE3
		char unknown_0D5[3]; // unused ?

		bool IsCheckSumValid() const { return (this->checksum == 0x005444FC); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_DEAD_UNITS); }
		unsigned long int GetCopyConstructorAddress() { return 0x440910; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_DEAD_FISH) == 0xD8, "STRUCT_UNITDEF_DEAD_FISH size");

	// CC 43 54 00 = Bird (type40) - size=0xFC - Constructor 0x43E090.
	// Deserialize=0x43E230 unitDef.ReadFromFile_bird_40(internalFileRef, ptrGraphicsList, ptrSoundsList)
	class STRUCT_UNITDEF_BIRD : public STRUCT_UNITDEF_DEAD_FISH {
	public:
		STRUCT_DEF_UNIT_COMMAND_HEADER *ptrUnitCommandHeader; // +D8
		short int whenBeingSeenCommandIndex; // +DC. For artefacts, discoveries but also animals
		short int unknown_0DE;
		// 0xE0
		float searchRadius;
		float workRate; // including upgrades. Ex for priest: base = 1, with astrology = 1.3
		short int dropSite1;
		short int dropSite2;
		char villagerMode; // +EC. An id for "group" of units that can switch definition ?
		char unknown_0ED;
		char unknown_0EE;
		char unknown_0EF;
		// 0xF0
		unsigned long int *attackSound; // Unknown struct
		unsigned long int *moveSound; // +F4. Unknown struct
		char animalMode;
		char unknown_0F9;
		char unknown_0FA;
		char unknown_0FB;

		bool IsCheckSumValid() const { return (this->checksum == 0x005443CC); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BIRD); }
		unsigned long int GetCopyConstructorAddress() { return 0x43E010; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_BIRD) == 0xFC, "STRUCT_UNITDEF_BIRD size");


	// 44 44 54 00 = type50 (type50) - size=0x148 - Constructor 0x43EE10
	class STRUCT_UNITDEF_TYPE50 : public STRUCT_UNITDEF_BIRD {
	public:
		unsigned long int *ptrAttackGraphic; // +FC. Unknown struct
		// 0x100
		char defaultArmor; // Used when there is no armor in ptrArmorsList?
		char unknown_101; // default attack ???
		short int armorsCount;
		STRUCT_ARMOR_OR_ATTACK *ptrArmorsList; // +104. ElemSize=0x04. +0=class,+2=amount
		short int attacksCount; // +108
		short int unknown_10A;
		STRUCT_ARMOR_OR_ATTACK *ptrAttacksList; // +10C. ElemSize=0x04. +0=class,+2=amount
		// 0x110
		TERRAIN_RESTRICTION armorTerrainRestriction; // Provide a multiplier for each terrain
		short int unknown_112;
		float maxRange; // Total range (8 if 7+1 is displayed). displayed range is the number before the "+" (7 in the example).
		float blastRadius; // +118. Distance blast damage applies. 0 means there is no blast damage.
		AOE_CONST_FUNC::BLAST_LEVELS blastLevel; // +11C. PLEASE always check if blastRadius>0. A lot of units have "wrongly" blastLevel=0
		char unknown_11D;
		char unknown_11E;
		char unknown_11F;
		// 0x120
		float reloadTime1;
		short int projectileUnitId; // +124. Can be used to determine if a military unit is melee (<0) or ranged (>=0)
		short int accuracyPercent;
		char towerMode; // +128
		char unknown_129; // unused ?
		short int frameDelay;
		float graphicDisplacement1;
		// 0x130
		float graphicDisplacement2;
		float graphicDisplacement3;
		unsigned long int minRange; // +138.
		short int displayedArmor; // The number before the "+". For a total 6 armor which is 2+4, displayedArmor=2. total armor is guessed from default armor ?
		short int displayedAttack;  // The number before the "+". For a total 6 attack which is 2+4, displayedAttack=2. total attack is guessed from ???
		// 0x140
		float displayedRange;
		float reloadTime2;

		bool IsCheckSumValid() const { return (this->checksum == 0x00544444); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TYPE50); }
		inline unsigned long int GetCopyConstructorAddress() const { return 0x43ED90; } // Address of AOE method to create a copy.
		inline bool HasBlastDamage() const { return (this->blastRadius > 0); }
	};
	static_assert(sizeof(STRUCT_UNITDEF_TYPE50) == 0x148, "STRUCT_UNITDEF_TYPE50 size");


	// C0 44 54 00 = Projectile (type60) - size=0x154 - Constructor 0x4403D0
	class STRUCT_UNITDEF_PROJECTILE : public STRUCT_UNITDEF_TYPE50 {
	public:
		PROJECTILE_TRAJECTORY_TYPE trajectoryType; // +148. Type of trajectory. Almost all projectiles have default one. Note: if graphics do not hit target, shooting fails.
		char intelligentProjectile; // +149.
		char penetrationMode; // +14A. If 1, projectile does not stop on target but continues (no impact and misses). Could be used (only) for units with blast damage ?
		char unknown_14B; // +14B. Unknown, but it is actually a field in structure.
		char unknown_14C; // +14C. Unknown, but it is actually a field in structure.
		char unknown_14D[3]; // Unused.
		// 0x150: Projectile movement arc. <=0.1 for standard projectiles (including ballista bolts), 0.25 for slinger stones, >=0.5 for catapult stones (0.4. boat cats)
		float projectileArc; // a 0+ value. Does NOT affect "time till impact". Only unit speed does. More than 10 may have the projectile go too high for screen (then come back though)

		bool IsCheckSumValid() const { return (this->checksum == 0x005444C0); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_PROJECTILE); }
		unsigned long int GetCopyConstructorAddress() { return 0x440350; } // Address of AOE method to create a copy. CopyFrom=0x4404C0
	};
	static_assert(sizeof(STRUCT_UNITDEF_PROJECTILE) == 0x154, "STRUCT_UNITDEF_PROJECTILE size");


	// 70 99 54 00 = living unit (type70) - size=0x164. Constructor = 0x4ECA90
	class STRUCT_UNITDEF_LIVING : public STRUCT_UNITDEF_TYPE50 {
	public:
		STRUCT_COST costs[3]; // +148, 3*6 bytes (3 words each)
		short int trainTime; // +15A.
		short int trainLocation; // (for buildings it is builder=0x76)
		char trainButton; // +15E.
		char unknown_15F; // +15F. Unused ?
		// 0x160
		short int displayedPierceArmor;
		short int unknown_162;

		bool IsCheckSumValid() const { return (this->checksum == 0x00549970); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT); }
		unsigned long int GetCopyConstructorAddress() { return 0x4ECA10; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_LIVING) == 0x164, "STRUCT_UNITDEF_LIVING size");


	// 30 99 54 00 = building (type80) - size=0x17C. Constructor = 0x4EC180
	class STRUCT_UNITDEF_BUILDING : public STRUCT_UNITDEF_LIVING {
	public:
		unsigned long int *ptrConstructionSound; // +164.
		unsigned long int *ptrConstructionGraphic; // +168. Graphics while building is under construction
		char multiplePlacement; // +16C. For building placement (put several at once)
		char unknown_16D;
		short int graphicsAngle; // +16E. constructionStep (for unfinished) or "angle" (different standing graphic frame?)
		// 0x170
		char disappearWhenBuilt;
		char unknown_171;
		short int stackUnitId; // +172. Additional building added on top on this one ?
		short int placementTerrainId; // +174.
		short int oldTerrainId; // Obsolete information ? Used to build roads in early versions (according to AGE3 tooltip)
		short int initiatesResearch;
		short int unknown_17A; // unused ?

		bool IsCheckSumValid() const { return (this->checksum == 0x00549930); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING); }
		unsigned long int GetCopyConstructorAddress() { return 0x4EC100; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_BUILDING) == 0x17C, "STRUCT_UNITDEF_BUILDING size");

	// BC 99 54 00 = tree (type90) - size=0xB8 - Constructor 0x4ED2C0
	class STRUCT_UNITDEF_TREE : public STRUCT_UNITDEF_BASE {
	public:
		bool IsCheckSumValid() const { return (this->checksum == 0x005499BC); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TREE); }
		unsigned long int GetCopyConstructorAddress() { return 0x4ED220; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_TREE) == 0xB8, "STRUCT_UNITDEF_TREE size");



	/*
	34 45 54 00 = Eye candy (type10) - size=0xB8 - Constructor 0x441040
	0C 44 54 00 = flag (type20) - size=0xBC - Constructor 0x43E9B0
	84 44 54 00 = Doppleganger (type25) - size=0xD8 - Constructor 0x440050
	FC 44 54 00 = Dead/fish (type30) - size=0xD8 - Constructor 0x440990
	CC 43 54 00 = Bird (type40) - size=0xFC - Constructor 0x43E090
	44 44 54 00 = type50 (type50) - size=0x148 - Constructor 0x43EE10
	C0 44 54 00 = Projectile? (type60) - size=0x154 - Constructor 0x4403D0
	70 99 54 00 = living unit (type70) - size=0x164. Constructor = 0x4ECA90
	30 99 54 00 = building (type80) - size=0x17C. Constructor = 0x4EC180
	BC 99 54 00 = tree (type90) - size=0xB8 - Constructor 0x4ED2C0
	*/
	// Old common class for unit definitions.
	class STRUCT_DEF_UNIT {
	public:
		unsigned long int checksum;
		char unitType; // +04. $46=70=Living unit, 80=building... On 4 bytes... Sometimes these values are 2-bytes, be careful
		char unused_003[3];
		char *ptrUnitName; // +08.
		short int languageDLLID_Name; // If =0, then "ptrUnitName" string will be displayed instead (useful for custom units whose name does not exist in language.dll).
		short int languageDLLID_Creation;
		// 0x10
		short int DAT_ID1; // "Base unit" id from empires.dat. Ex=cavalry
		short int DAT_ID2; // "Upgraded unit" id from empires.dat. Ex=heavy cavalry
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType; // +14. "Class". TribeAIGroupXXX - 3=building, 18=priest...
		char *ptrStandingGraphics; // starts with SLP name.
		unsigned long int ptrUnknownGraphic_01C_dying; // +1C
		// 0x20
		unsigned long int ptrUnknownGraphic_020;
		short int deathMode;
		short int totalHitPoints; // +026. Including upgrades !
		float lineOfSight; // +028
		char garrisonCapacity; // +02C
		char unused_02D[3];
		// 0x30
		float sizeRadius1; // For Y axis
		float sizeRadius2; // For X axis
		float HPBarHeight1;
		unsigned long int unsure_ptrTrainSound; // To confirm is it a ptr or a word (+2 unused bytes)? or selection sound ?
		// 0x40
		unsigned long int ptrUnknownGraphic_040;
		unsigned long int unknown_044;
		short int deadUnitId; // +48
		char placementMode; // +4A
		char airMode; // +4B
		short int iconId; // +4C
		char hideInEditor;
		char unknown_04F; // unused ?
		// 0x50
		short int unknown_050; // unknown1 in AGE3
		char availableForPlayer; // +52. according to tree+researches (+requires enable in empires.dat?). 1="can be trained". 0 does not prevent from having one (scenario, conversion)
		char unknown_053;
		short int placementBypassTerrain1;
		short int placementBypassTerrain2;
		short int placementTerrain1;
		short int placementTerrain2;
		float editorRadius1; // for Y axis
		// 0x60
		float editorRadius2; // for X axis
		char hillMode; // 0=no restriction, 2 for buildings ?
		char visibleInFog; // +65. Can be 0,1, 3 ("inverted visibility" in AGE3, but not exact. smoke has 3).
		short int terrainRestriction;
		char flyMode;
		char unknown_069;
		short int resourceStorageType_1; // +6A. A resource Id.
		short int resourceStorageType_2;
		short int resourceStorageType_3;
		// 0x70
		float resourceStorageAmount_1;
		float resourceStorageAmount_2;
		float resourceStorageAmount_3;
		unsigned long int resourceCapacity;
		// 0x80
		unsigned long int resourceDecay; // Amount of resource that is lost per second
		float unknown_084; // unknown3A in AGE3
		char resourceStorageEnableMode_1; // +88. type=RESOURCE_TYPES but on 1 byte only
		char resourceStorageEnableMode_2;
		char resourceStorageEnableMode_3;
		char blastArmorLevel;
		char triggerType; // See age3 (sub type). 2=building, 3=civilian, 4=military, 5=priest
		AOE_CONST_FUNC::INTERACTION_MODES interactionMode; // +8D. 1-byte.
		char minimapMode; // +8E.
		AOE_CONST_FUNC::COMMAND_ATTRIBUTES commandAttribute; // +8F.
		// 0x90
		char minimapColor;
		char attackMode;
		char edibleMeat;
		char damageGraphicCount;
		unsigned long int *damageGraphicsArray; // Point to an array of pointers to damage graphics (cf damageGraphicCount)
		unsigned long int languageDLLHelp;
		unsigned long int languageDLLHotKeyText;
		// 0xA0
		unsigned long int hotKey;
		char unselectable;
		char unknown_0A5; // Unknown6 in AGE3
		char unknown_selectionMode;
		char unknown_0A7; // Unknown8 in AGE3
		char selectionEffect;
		char editorSelectionColor;
		char unknown_0AA;
		char unknown_0AB;
		float selectionRadius1;
		// 0xB0
		float selectionRadius2;
		float HPBarHeight2;
		float speed; // +B8
		unsigned long int ptrWalkingGraphic1;
		// 0xC0
		unsigned long int ptrWalkingGraphic2;
		float rotationSpeed;
		char unknown_0C8;
		char unknown_0C9;
		short int trackingUnit;
		char trackingUnitUsed;
		char unknown_0CD;
		char unknown_0CE;
		char unknown_0CF;
		// 0xD0
		float trackingUnitDensity;
		char unknown_0D4; // unknown16 in AGE3
		char unknown_0D5[3];
		STRUCT_DEF_UNIT_COMMAND_HEADER *ptrUnitCommandHeader; // +D8
		short int whenBeingSeenCommandIndex; // +DC. For artefacts, discoveries but also animals (works with creatable too)
		short int unknown_0DE;
		// 0xE0
		float searchRadius;
		float workRate; // including upgrades. Ex: priest => 1, with astrology => 1.3
		short int dropSite1;
		short int dropSite2;
		char villagerMode;
		char unknown_0ED;
		char unknown_0EE;
		char unknown_0EF;
		// 0xF0
		unsigned long int ptrUnknownGraphic_0F0; // or sound ?
		unsigned long int ptrUnknownGraphic_0F4; // or sound ?
		char animalMode;
		char unknown_0F9;
		char unknown_0FA;
		char unknown_0FB;
		unsigned long int ptrAttackGraphic;
		// 0x100
		char defaultArmor; // Used when there is no armor in ptrArmorsList? For display ?
		char unknown_101; // default attack ???
		short int armorsCount;
		STRUCT_ARMOR_OR_ATTACK *ptrArmorsList; // ElemSize=0x04. +0=class,+2=amount
		short int attacksCount;
		short int unknown_10A;
		STRUCT_ARMOR_OR_ATTACK *ptrAttacksList; // ElemSize=0x04. +0=class,+2=amount
		// 0x110
		short int unknown_110_terrainRestrictionForDamage; // cf tech attribute 18. 0x43FBE1
		short int unknown_112;
		float maxRange; // Total range (8 if 7+1 is displayed). displayed range is the number before the "+" (7 in the example).
		float blastRadius; // +118
		char blastLevel;
		char unknown_11D;
		char unknown_11E;
		char unknown_11F;
		// 0x120
		float reloadTime1;
		short int projectileUnitId;
		short int accuracyPercent; // +126.
		char towerMode;
		char unknown_129;
		short int frameDelay;
		float graphicDisplacement1;
		// 0x130
		float graphicDisplacement2;
		float graphicDisplacement3;
		unsigned long int minRange; // +138.
		short int displayedArmor; // The number before the "+". For a total 6 armor which is 2+4, displayedArmor=2. total armor is guessed from default armor ?
		short int displayedAttack;  // The number before the "+". For a total 6 attack which is 2+4, displayedAttack=2. total attack is guessed from ???
		// 0x140
		float displayedRange;
		float reloadTime2;
		STRUCT_COST costs[3]; // +148, 3*6 bytes (3 words each)
		short int trainTime; // +15A.
		short int trainLocation; // (for buildings it is builder=0x76)
		char trainButton; // +15E.
		char unknown_15F; // unused ?
		// 0x160
		short int displayerPierceArmor;
		short int unknown_162;
		unsigned long int ptrUnknownSound_164;
		unsigned long int ptrConstructionGraphic;
		char unknown_16C;
		char unknown_16D;
		short int unknown_16E;
		// 0x170
		char unknown_170;
		char unknown_171;
		short int unknown_172;
		short int unknown_174;
		short int unknown_176;
		short int initiatesResearch;
		short int unknown_17A;
		unsigned long int unknown_17C;

		bool IsCheckSumValid() {
			return ((this->checksum == 0x00544534) || // eye candy - 10
				(this->checksum == 0x0054440C) || // Flag - type 20
				(this->checksum == 0x00544484) || // Doppleganger - 25
				(this->checksum == 0x005444FC) || // Dead/fish - 30
				(this->checksum == 0x005443CC) || // Birds - 40
				// - 50
				(this->checksum == 0x005444C0) || // projectile - 60
				(this->checksum == 0x00549970) || // living - 70
				(this->checksum == 0x00549930) || // Building - 80
				(this->checksum == 0x005499BC) // Tree - 90
				);
		}
	};


	// Returns a unit definition structure's size according to its actual type
	static long int GetUnitDefStructRealSize(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		if (unitDef->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BASE);
		if (((STRUCT_UNITDEF_BIRD*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BIRD);
		if (((STRUCT_UNITDEF_FLAG*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_FLAG);
		if (((STRUCT_UNITDEF_DEAD_FISH*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DEAD_FISH);
		if (((STRUCT_UNITDEF_DOPPLEGANGER*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER);
		if (((STRUCT_UNITDEF_LIVING*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_LIVING);
		if (((STRUCT_UNITDEF_PROJECTILE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE);
		if (((STRUCT_UNITDEF_TREE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BIRD);
		if (((STRUCT_UNITDEF_TYPE50*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50);
		if (((STRUCT_UNITDEF_BUILDING*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING);
		// If not found, try again using only checksum ? There is no reason here (just a copy paste from unit ^^)
		if (unitDef->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BASE);
		if (((STRUCT_UNITDEF_BIRD*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BIRD);
		if (((STRUCT_UNITDEF_FLAG*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_FLAG);
		if (((STRUCT_UNITDEF_DEAD_FISH*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DEAD_FISH);
		if (((STRUCT_UNITDEF_DOPPLEGANGER*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER);
		if (((STRUCT_UNITDEF_LIVING*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_LIVING);
		if (((STRUCT_UNITDEF_PROJECTILE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE);
		if (((STRUCT_UNITDEF_TREE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BIRD);
		if (((STRUCT_UNITDEF_TYPE50*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50);
		if (((STRUCT_UNITDEF_BUILDING*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING);
		return 0;
	}

}


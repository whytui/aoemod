
#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_graphics.h>
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


	// Size 4. Defines armor/attack amount for a specific class.
	struct STRUCT_ARMOR_OR_ATTACK {
		AOE_CONST_FUNC::ATTACK_CLASS classId; // "class" is reserved !
		short int amount;
	};
	static_assert(sizeof(STRUCT_ARMOR_OR_ATTACK) == 4, "STRUCT_ARMOR_OR_ATTACK size");


	// Size = 0x4C. "Task". Definition of unit commands (cf empires.dat)
	// Constructor = 0x4E73B0.
	// ReadFromFile=0x4E74F0
	class STRUCT_UNIT_COMMAND_DEF {
	public:
		short int alwaysOne;
		short int unitCommandIndex; // +02. index of this unitCommandDef element
		char unknown_04; // Unknown value
		char unknown_05; // unused ?
		AOE_CONST_FUNC::UNIT_ACTION_ID commandType; // +6
		short int classId; // +08
		short int unitDefId; // +0A. A unit definition id, or -1 if not specified.
		char selectionEnabler; // +0C // combat_level?
		char unknown_0D; // combat_level_flag?
		short int terrainId; // +0E.
		// 0x10
		char selectionMode; // owner_type ?
		char rightClickMode; // +11. what is this ? "holding_attr" ?
		char unknown_12; // state_building ???
		char unknown_13;
		short int resourceTypeIn; // +0x14.
		short int resourceProductivityMultiplier;
		short int resourceTypeOut;
		short int resource;
		float workRateMultiplier; // or quantity ? "work_val1"
		// 0x20
		float executionRadius; // "work_val2"
		float extraRange; // "work_range"
		char unknown_28; // "search_flag"
		char unknown_29; // unused ? "search_wait_time" ?
		char unknown_2A; // unused ? "work_flag" ?
		char unknown_2B; // unused ? " work_flag2" ?
		float unknown_2C;
		// 0x30
		short int plunderSource; // What is this ? 0=from resource, 1=from player (AGE3 tooltip)
		short int unknown_32;
		unsigned long int *ptrToolGraphic; // graphics NOT carrying resource "move_sprite"
		unsigned long int *ptrProceedingGraphic; // +38. "work_sprite"
		unsigned long int *ptrActionGraphic; // +3C. "work_sprite2"
		// 0x40
		unsigned long int ptrCarryingGraphic; // graphics carrying resource
		STRUCT_DAT_SOUND *executionSound; // +44 "work_sound"
		STRUCT_DAT_SOUND *resourceDepositSound; // +48 "work_sound2"
		// END of structure
	};



	// Size = 0x0C (for both parent/child classes). Constructor=0x4E7AB0 (no parameter).
	// Parent class checksum=E4 98 54 00, for birds/type50/projectiles
	// Child class checksum=AC 99 54 00 for living/building
	// 0x4E7B60=unitCommandHeader.readFromFile(fileId, arg2, arg3)
	// +0x00 = unitDefCmdHeader.createUnitDefCmd(arg1, arg2)
	// +0x04 = unitDefCmdHeader.FindRelevantCommand(ptrActorUnit, ptrTargetUnit, f_posY, f_posX, posZ)
	// +0x08 = unitDefCmdHeader.IsCommandConvertOrAttack(commandInfo)
	// +0x0C = unitDefCmdHeader.xx(unitCmdDef, actorUnit, targetUnit, arg4) [Last]
	class STRUCT_UNIT_COMMAND_DEF_HEADER {
	public:
		unsigned long int checksum; // AC 99 54 00 + E4 98 54 00 (parent class)
		STRUCT_UNIT_COMMAND_DEF **ptrCommandArray; // +4. The pointed list contains "commandCount" pointers to STRUCT_DEF_UNIT_COMMAND objects
		short int commandCount; // +8.
		unsigned short int unused_0A;

		bool IsCheckSumValid() { return (this->checksum == 0x005499AC) || (this->checksum == 0x005498E4); }
	};


	// Size = 08. Deserialized in 0x441775 (for example)
	class STRUCT_DAMAGE_GRAPHIC {
	public:
		STRUCT_GRAPHICS *ptrGraphic;
		char fromDamagePercent; // +4 minimum % of HP from which this graphic is used
		char oldApplyMode; // +5. unused?
		char applyMode; // 0,1=add to graphic (flames), 2=replace graphic(walls)
		char unused_07;
	};
	static_assert(sizeof(STRUCT_DAMAGE_GRAPHIC) == 0x8, "STRUCT_DAMAGE_GRAPHIC size");


	// 34 45 54 00 = Eye candy (type10) - size=0xB8 - Constructor 0x441040, 0x441070, 0x440FF0
	// +0x00 = unit.destructor(do_free)
	// +0x04 = unitDef.copyObj(unitDef)?
	// +0x08 = unitDef.applySetEffect(f_value, attribute)
	// +0x0C = unitDef.applyAddEffect(f_value, attribute)
	// +0x10 = unitDef.applyMultEffect(f_value, attribute)
	// +0x14 = unitDef.save(internalFileId)
	// +0x18 = unitDef.createUnit(player, posY, posX, posZ)
	// +0x1C = unitDef.createCopy()
	// +0x20 = unitDef.GetErrorForUnitCreationAtLocation(player, posY, posX, pOutPosInfo, checkVisibility, hillMode, arg7, editorMode, checkAirModeAndHPBar, checkConflictingUnits) "checkPlacement"
	// +0x24 = unsigned char RGE_Master_Static_Object::alignment(float &,float &, global, unsigned char)
	// +0x28 = ? unitDef.getDamageValueFromOtherUnitDef(unitDef)
	// +0x2C = unitDef.playAttackSound()
	// +0x30 = unitDef.playMoveSound?
	// +0x34 = unitDef.draw(TDrawArea *,short,short,RGE_Color_Table *,long,long) [last for base, flag, movable, tree]
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
		STRUCT_GRAPHICS *ptrStandingGraphics; // +18. starts with SLP name.
		STRUCT_GRAPHICS *ptrDyingGraphic; // +1C. starts with SLP name.
		// 0x20
		STRUCT_GRAPHICS *ptrDyingGraphic2; // +20. Used only when deathMode = true ("undead")
		short int deathMode; // +024. If 1, then unit status 6 is used. Otherwise, units die as soon as their own resources are depleted. ("undead flag")
		short int totalHitPoints; // +026. Including upgrades !
		float lineOfSight; // +028
		char garrisonCapacity; // +02C
		char unused_02D[3];
		// 0x30
		float sizeRadiusY; // For Y axis
		float sizeRadiusX; // For X axis
		float sizeRadiusZ; // +38.
		STRUCT_DAT_SOUND *ptrSelectionSound; // +3C
		// 0x40
		STRUCT_DAT_SOUND *ptrTrainSound; // +40. "Created" sound
		STRUCT_DAT_SOUND *ptrDyingSound; // +44
		short int deadUnitId; // +48
		char placementMode; // +4A. "sort_number"
		char airMode; // +4B. "can_be_built_on"
		short int iconId; // +4C
		char hideInEditor;
		char unknown_04F; // unused ?
		// 0x50
		short int unknown_050; // unknown1 in AGE3. It IS a word value (0x4414D2) "portrait_pict" ?
		char availableForPlayer; // +52. according to tree+researches (+requires enable in empires.dat?). 1="can be trained". 0 does not prevent from having one (scenario, conversion)
		char unknown_053;
		short int placementBypassTerrain1; // +54. Side terrain id ? "tile_req1"
		short int placementBypassTerrain2; // +56. Side terrain id ? "tile_req2"
		short int placementTerrain1; // +58 "center_tile_req1"
		short int placementTerrain2; // +5A "center_tile_req2"
		float editorRadiusY; // for Y axis "construction_radius". Used if placementTerrainId>=0, for example ?
		// 0x60
		float editorRadiusX; // for X axis "construction_radius". Used if placementTerrainId>=0, for example ?
		char hillMode; // 0=no restriction, 2 for buildings ? "elevation_flag"
		char visibleInFog; // +65. Can be 0,1, 3 ("inverted visibility" in AGE3, but not exact. smoke has 3). >0 = always visible for others. Note: this never provides visibility, unit is not selectable, but is visible (greyed) through fog (like buildings)
		short int terrainRestriction; // +66
		char flyMode; // +68. "movement_type"
		char unknown_069;
		short int resourceStorageType_1; // +6A. A resource Id. "attribute_type_held"
		short int resourceStorageType_2;
		short int resourceStorageType_3;
		// 0x70
		float resourceStorageAmount_1; // "attribute_amount_held"
		float resourceStorageAmount_2;
		float resourceStorageAmount_3;
		long int resourceCapacity;
		// 0x80
		float resourceDecay; // Amount of resource that is lost per second in unit status 5.
		float unknown_084; // unknown3A in AGE3. 0x4415D4 "multiple_attribute_mod"
		char resourceStorageEnableMode_1; // +88. type=RESOURCE_TYPES but on 1 byte only "attribute_flag"
		char resourceStorageEnableMode_2;
		char resourceStorageEnableMode_3; // +8A
		char blastArmorLevel; // +8B. "area_effect_object_level"
		char triggerType; // +8C. Same as class ? TribeAIGroupCivilian = 4, etc? "combat_level"
		AOE_CONST_FUNC::INTERACTION_MODES interactionMode; // +8D. 1-byte. "select_level"
		char minimapMode; // +8E. "map_draw_level"
		AOE_CONST_FUNC::COMMAND_ATTRIBUTES commandAttribute; // +8F. "unit_level"
		// 0x90
		char minimapColor;
		char attackMode; // "attack_reaction"
		char edibleMeat; // +92 "convert_terrain_flag" ?
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
		char selectionEffect; // +A8
		char editorSelectionColor; // +A9
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
		// Returns true if the unit definition is a flag (20) or a child class (all but eye candy and trees). This does not control the checksum.
		bool DerivesFromFlag() { return (this->unitType >= (char)AOE_CONST_FUNC::GUT_FLAGS) || (this->unitType <= (char)AOE_CONST_FUNC::GUT_BUILDING); } // all but 10 and 90
		// Returns true if the unit definition is dead/fish (30) or a child class. This does not control the checksum.
		bool DerivesFromMovable() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_MOVABLE) || (this->DerivesFromCommandable()); }
		// Returns true if the unit definition is a bird (40) or a child class. This does not control the checksum.
		bool DerivesFromCommandable() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_COMMANDABLE) || (this->DerivesFromAttackable()); }
		// Returns true if the unit definition is a living unit (70) or a child class (building). This does not control the checksum.
		bool DerivesFromTrainable() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_TRAINABLE) || (this->unitType == (char)AOE_CONST_FUNC::GUT_BUILDING); }
		// Returns true if the unit definition is type50 or one of its child classes (projectile, living/building). This does not control the checksum.
		bool DerivesFromAttackable() { return (this->unitType == (char)AOE_CONST_FUNC::GUT_ATTACKABLE) || (this->unitType == (char)AOE_CONST_FUNC::GUT_PROJECTILE) || (this->DerivesFromTrainable()); }
		unsigned long int GetCopyConstructorAddress() { return 0x440FF0; } // Address of AOE method to create a copy
	};
	static_assert(sizeof(STRUCT_UNITDEF_BASE) == 0xB8, "STRUCT_UNITDEF_BASE size");

	// 0C 44 54 00 = flag (type20) - size=0xBC - Constructor 0x43E9B0. Max method=+0x34. "MasterAnimatedObject".
	class STRUCT_UNITDEF_FLAG : public STRUCT_UNITDEF_BASE {
	public:
		float speed; // 0xB8. Unit movement speed.
		bool IsCheckSumValid() const { return (this->checksum == 0x0054440C); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_FLAGS); }
		unsigned long int GetCopyConstructorAddress() { return 0x43E930; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_FLAG) == 0xBC, "STRUCT_UNITDEF_FLAG size");

	// 84 44 54 00 = Doppleganger - size 0xBC. Constructor(createCopy)=0x4402C0. Max method=+0x38
	// +0x38 = unitDefDoppleganger.xxx(player, f_pos, pos, pos)? create unit?
	class STRUCT_UNITDEF_DOPPLEGANGER : public STRUCT_UNITDEF_FLAG {
	public:
		// 0xBC - no additional fields ?
		bool IsCheckSumValid() const { return (this->checksum == 0x00544484); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_DOPPLEGANGER); }
		unsigned long int GetCopyConstructorAddress() { return 0x43EC40; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_DOPPLEGANGER) == 0xBC, "STRUCT_UNITDEF_DOPPLEGANGER size");


	// FC 44 54 00 = Movable (type30 - dead/fish in AGE3) - size=0xD8 - Constructor 0x440990. Max method=+0x34. "MasterMovingObject"
	class STRUCT_UNITDEF_MOVABLE : public STRUCT_UNITDEF_FLAG {
	public:
		STRUCT_GRAPHICS *ptrWalkingGraphic1;
		// 0xC0
		STRUCT_GRAPHICS *ptrWalkingGraphic2;
		float rotationSpeed; // +C4
		char unknown_0C8;
		char unknown_0C9;
		short int trackingUnit; // +CA. Used to define the "smoke" unit to leave behind "me". Used for "flame bolts", etc.
		char trackingUnitUsed; // +CC. 0=disable, 1=?, 2=?
		char unknown_0CD;
		char unknown_0CE;
		char unknown_0CF;
		// 0xD0
		float trackingUnitDensity;
		char unknown_0D4; // unknown16 in AGE3
		char unknown_0D5[3]; // unused ?

		bool IsCheckSumValid() const { return (this->checksum == 0x005444FC); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_MOVABLE); }
		unsigned long int GetCopyConstructorAddress() { return 0x440910; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_MOVABLE) == 0xD8, "STRUCT_UNITDEF_MOVABLE size");


	// CC 43 54 00 = Commandable (type40 - bird in AGE3) - size=0xFC - Constructor 0x43E090. "MasterActionObject"
	// Deserialize=0x43E230 unitDef.ReadFromFile_40(internalFileRef, ptrGraphicsList, ptrSoundsList). Max method=+0x38
	// +0x38 = unitDef.GetNewDefUnitCommandHeader() ("createTaskList") [Last for commandable, attackable, projectile, trainable]
	class STRUCT_UNITDEF_COMMANDABLE : public STRUCT_UNITDEF_MOVABLE {
	public:
		STRUCT_UNIT_COMMAND_DEF_HEADER *ptrUnitCommandHeader; // +D8
		short int whenIdleCommandIndex; // +DC. Unit-command to execute when unit is idle (and status=2). Used for artefacts, discoveries, animals, farms, trade units... "convert herd" in old AGE3 versions.
		short int unknown_0DE;
		// 0xE0
		float searchRadius; // The distance unit will seek to auto-attack enemy units.
		float workRate; // including upgrades. Ex for priest: base = 1, with astrology = 1.3
		short int dropSite1; // +E8. Storage building/deposit unit 1.
		short int dropSite2; // +EA. Storage building/deposit unit 2.
		char unitDefinitionSwitchGroupId; // +EC. 0=none. An id for "group" of units that can switch definition. id=1 for villagers (builder, forager, etc).
		char unknown_0ED;
		char unknown_0EE;
		char unknown_0EF;
		// 0xF0
		STRUCT_DAT_SOUND *attackSound; // +F0.
		STRUCT_DAT_SOUND *moveSound; // +F4. Unknown struct
		char animalMode; // +F8.
		char unknown_0F9;
		char unknown_0FA;
		char unknown_0FB;

		bool IsCheckSumValid() const { return (this->checksum == 0x005443CC); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_COMMANDABLE); }
		unsigned long int GetCopyConstructorAddress() { return 0x43E010; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_COMMANDABLE) == 0xFC, "STRUCT_UNITDEF_COMMANDABLE size");


	// 44 44 54 00 = type50 (type50) - size=0x148 - Constructor 0x43EE10(readFromFile). Max method=+0x38
	// "RGE_MasterCombatObject"
	class STRUCT_UNITDEF_ATTACKABLE : public STRUCT_UNITDEF_COMMANDABLE {
	public:
		STRUCT_GRAPHICS *ptrAttackGraphic; // +FC.
		// 0x100
		char defaultArmor; // Used when there is no armor in ptrArmorsList?
		char unknown_101; // default attack ???
		short int armorsCount;
		STRUCT_ARMOR_OR_ATTACK *ptrArmorsList; // +104. ElemSize=0x04. +0=class,+2=amount
		short int attacksCount; // +108
		short int unknown_10A;
		STRUCT_ARMOR_OR_ATTACK *ptrAttacksList; // +10C. ElemSize=0x04. +0=class,+2=amount. The greatest attack value from list is the total displayed attack in unit info zone.
		// 0x110
		TERRAIN_RESTRICTION armorTerrainRestriction; // Provide a multiplier for each terrain
		short int unknown_112;
		float maxRange; // +114. Total range (8 if 7+1 is displayed). displayed range is the number before the "+" (7 in the example). "Weapon range"
		float blastRadius; // +118. Distance blast damage applies. 0 means there is no blast damage.
		AOE_CONST_FUNC::BLAST_LEVELS blastLevel; // +11C. PLEASE always check if blastRadius>0. A lot of units have "wrongly" blastLevel=0
		char unknown_11D;
		char unknown_11E;
		char unknown_11F;
		float reloadTime1; // +0x120. Rate of fire.
		short int projectileUnitId; // +124. Can be used to determine if a military unit is melee (<0) or ranged (>=0). Warning: priests would be "melee" with such criteria.
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
		short int displayedAttack; // The number before the "+". For a total 6 attack which is 2+4, displayedAttack=2. total attack is the greatest attack value from attacks list
		// 0x140
		float displayedRange; // The number before the "+". See also maxRange(=total range)
		float reloadTime2;

		bool IsCheckSumValid() const { return (this->checksum == 0x00544444); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_ATTACKABLE); }
		inline unsigned long int GetCopyConstructorAddress() const { return 0x43ED90; } // Address of AOE method to create a copy, method arg1=pUnitDefToCopy, arg2=1 for "initial call" (0 for inherited sub-calls)
		inline bool HasBlastDamage() const { return (this->blastRadius > 0) && (this->blastLevel < CST_BL_DAMAGE_TARGET_ONLY); }
	};
	static_assert(sizeof(STRUCT_UNITDEF_ATTACKABLE) == 0x148, "STRUCT_UNITDEF_ATTACKABLE size");


	// C0 44 54 00 = Projectile (type60) - size=0x154 - Constructor 0x4403D0. Max method=+0x38
	class STRUCT_UNITDEF_PROJECTILE : public STRUCT_UNITDEF_ATTACKABLE {
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


	// 70 99 54 00 = Trainable (type70 - living in AGE3) - size=0x164. Constructor = 0x4ECA90. Max method=+0x38. "TribeMasterCombatObject"
	class STRUCT_UNITDEF_TRAINABLE : public STRUCT_UNITDEF_ATTACKABLE {
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
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TRAINABLE); }
		unsigned long int GetCopyConstructorAddress() { return 0x4ECA10; } // Address of AOE method to create a copy.
	};
	static_assert(sizeof(STRUCT_UNITDEF_TRAINABLE) == 0x164, "STRUCT_UNITDEF_TRAINABLE size");


	// 30 99 54 00 = building (type80) - size=0x17C. Constructor = 0x4EC180. Max method=+0x3C
	// +0x3C = unitDefBld.createUnit(arg1, arg2)
	class STRUCT_UNITDEF_BUILDING : public STRUCT_UNITDEF_TRAINABLE {
	public:
		STRUCT_DAT_SOUND *ptrConstructionSound; // +164.
		STRUCT_GRAPHICS *ptrConstructionGraphic; // +168. Graphics while building is under construction
		char multiplePlacement; // +16C. For building placement (put several at once)
		char unknown_16D;
		short int graphicsAngle; // +16E. constructionStep (for unfinished) or "angle" (different standing graphic frame?)
		char dieWhenBuilt; // +170. If true, unit is killed (by unit.killAndHandleSacrifice()) immediately when fully built.
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


	// Returns a unit definition structure's size according to its actual type
	static long int GetUnitDefStructRealSize(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		if (unitDef->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BASE);
		if (((STRUCT_UNITDEF_COMMANDABLE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE);
		if (((STRUCT_UNITDEF_FLAG*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_FLAG);
		if (((STRUCT_UNITDEF_MOVABLE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE);
		if (((STRUCT_UNITDEF_DOPPLEGANGER*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER);
		if (((STRUCT_UNITDEF_TRAINABLE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE);
		if (((STRUCT_UNITDEF_PROJECTILE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE);
		if (((STRUCT_UNITDEF_TREE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TREE);
		if (((STRUCT_UNITDEF_ATTACKABLE*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE);
		if (((STRUCT_UNITDEF_BUILDING*)unitDef)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING);
		// If not found, try again using only checksum ? There is no reason here (just a copy paste from unit ^^)
		if (unitDef->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BASE);
		if (((STRUCT_UNITDEF_COMMANDABLE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE);
		if (((STRUCT_UNITDEF_FLAG*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_FLAG);
		if (((STRUCT_UNITDEF_MOVABLE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE);
		if (((STRUCT_UNITDEF_DOPPLEGANGER*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER);
		if (((STRUCT_UNITDEF_TRAINABLE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE);
		if (((STRUCT_UNITDEF_PROJECTILE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE);
		if (((STRUCT_UNITDEF_TREE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TREE);
		if (((STRUCT_UNITDEF_ATTACKABLE*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE);
		if (((STRUCT_UNITDEF_BUILDING*)unitDef)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING);
		return 0;
	}

}


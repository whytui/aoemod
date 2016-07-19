
#pragma once

using namespace AOE_CONST_FUNC;

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace ROR_STRUCTURES_10C {


	// Size =  0x0C
	class STRUCT_TECH_DEF_EFFECT {
	public:
		TECH_DEF_EFFECTS effectType; // 1 byte.
		char unused_01;
		short int effectUnit; // +02. Unit ID or Resource ID or...
		short int effectClass; // +04. Class or ToUnit (upgrade unit) or Mode (for enable unit, resource modifier) or...
		short int effectAttribute; // +06. see TECH_UNIT_ATTRIBUTES (not always this enum but always 2 bytes)
		float effectValue; // +08. Sometimes unused (enable unit, upgrade unit, etc). Sometimes includes 2 info using modulo !

		float GetValue() const {
			// Attribute modifier (set/add/mult) on armor or attack: exception (need to apply a modulo on value)
			if (((this->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
				(this->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
				(this->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT)) &&
				((this->effectAttribute == (short)TECH_UNIT_ATTRIBUTES::TUA_ARMOR) || (this->effectAttribute == (short)TECH_UNIT_ATTRIBUTES::TUA_ATTACK))) { // armor&attack (8,9) attributes: 2 info stored in the float...
				return fmodf(this->effectValue, 256); // Holy WTF !!!
				//AOE_CONST_FUNC::ATTACK_CLASS attackClass = (AOE_CONST_FUNC::ATTACK_CLASS)(this->effectValue / 256);
			}
			// ALL other cases: just use float value as is.
			return this->effectValue;
		}
		// Returns true if tech effect has a valid and "effective" effect (not disabled, not adding 0, etc)
		bool HasValidEffect() const {
			switch (this->effectType) {
			case TECH_DEF_EFFECTS::TDE_INVALID:
				return false;
			case TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET:
			case TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH:
				return true;
			case TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET:
			case TECH_DEF_EFFECTS::TDE_RESEARCH_COST_MODIFIER:
			case TECH_DEF_EFFECTS::TDE_RESEARCH_TIME_MODIFIER:
				if (this->effectClass == 0) { // mode 0 = set
					return true;
				} else { // mode 1 = add
					return (this->GetValue() != 0);
				}
			case TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT:
			case TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT:
				return (this->effectUnit >= 0);
			case TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD:
				return (this->GetValue() != 0);
			case TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT:
			case TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT:
				return (this->GetValue() != 1);
			}
			return true;
		}
		bool IsEnableUnit(short int unitDefId) const {
			return (this->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (this->effectClass == 1) && (this->effectUnit == unitDefId);
		}
		bool IsDisableUnit(short int unitDefId) const {
			return (this->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (this->effectClass == 0) && (this->effectUnit == unitDefId);
		}
		bool IsDisableResearch(short int researchId) const {
			return (this->effectType == TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH) && ((int)this->effectValue == researchId);
		}
		// Returns true if effect is an attribute modifier (set/multiply or add)
		bool IsAttributeModifier() const {
			return (this->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) || (this->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) || (this->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT);
		}
		// Return target unit ("TO") for "upgrade unit" effects. Returns -1 if invalid (not an upgrade unit effect)
		short int UpgradeUnitGetTargetUnit() const {
			if (this->effectType != TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) { return -1; }
			return this->effectClass;
		}
	};
	static_assert(sizeof(STRUCT_TECH_DEF_EFFECT) == 0x0C, "STRUCT_TECH_DEF_EFFECT size");


	// Size = 0x2C
	class STRUCT_TECH_DEF {
	public:
		char unknown_00[2]; // Is this used ?
		char techName[0x22]; // +02. Almost never valued ?
		short int effectCount; // +24
		short int unknown_26; // probably unused
		STRUCT_TECH_DEF_EFFECT *ptrEffects; // +28. Array of effects. Count = effectCount.
	};
	static_assert(sizeof(STRUCT_TECH_DEF) == 0x2C, "STRUCT_TECH_DEF size");


	// Size = 0x0C, alloc in 0x50A1CB
	class STRUCT_TECH_DEF_INFO {
	public:
		unsigned long int checksum; // 20 99 54 00
		STRUCT_TECH_DEF *ptrTechDefArray;
		long int technologyCount;

		bool IsCheckSumValid() const { return this->checksum == 0x00549920; }
		STRUCT_TECH_DEF *GetTechDef(short int techDefId) const {
			if ((techDefId < 0) || (techDefId >= this->technologyCount)) { return NULL; }
			return &this->ptrTechDefArray[techDefId];
		}
	};
	static_assert(sizeof(STRUCT_TECH_DEF_INFO) == 0x0C, "STRUCT_TECH_DEF_INFO size");

	// size = 0x3C
	class STRUCT_RESEARCH_DEF {
	public:
		char *researchName;
		short int requiredResearchId[4]; // +04. 4 possible research IDs (required researches)
		short int minRequiredResearchesCount; // +0C. Min number of "requiredResearchId" that need to be satisfied to enable this research. NOT number of used elements in array !
		short int costType1;
		// 0x10
		short int costType2;
		short int costType3;
		short int costAmount1; // +14
		short int costAmount2;
		short int costAmount3; // +18
		char costUsed1;
		char costUsed2;
		char costUsed3; // +1C
		char unused1D0;
		short int researchTime; // 1E
		// 0x20
		short int technologyId;
		short int researchType; // 1=dock, 2=granary,etc (see AGE3 tooltip)
		short int iconId; // 24
		char buttonId; // 26. 1-5 for first row, 6-10 for second row, 11-15 and 16-20 for 2nd page (2 rows)
		char unknown_27;
		short int researchLocation; // 28
		short int languageDLLName;
		short int languageDLLDescription; // 2C
		unsigned short int unknown_2E; // "pointer3" ??
		// 0x30
		long int languageDLLCreation;
		long int languageDLLHelp; // +34
		unsigned short int unknown_38;
		unsigned short int unknown_3A;
		//end
	};


	// Returns <0 if this attribute's values are reverse (lower value are better/stronger)
	// Returns >0 if this attribute's values are standard (higher = better)
	// Returns 0 if not applicable (values represent a IDs or something else...)
	static int AttributeValueHasPositiveEffect(TECH_UNIT_ATTRIBUTES attribute) {
		switch (attribute) {
		case AOE_CONST_FUNC::TUA_HP:
		case AOE_CONST_FUNC::TUA_LOS:
		case AOE_CONST_FUNC::TUA_GARRISON_CAPACITY:
		case AOE_CONST_FUNC::TUA_SPEED:
		case AOE_CONST_FUNC::TUA_ARMOR:
		case AOE_CONST_FUNC::TUA_ATTACK:
		case AOE_CONST_FUNC::TUA_ACCURACY_PERCENT:
		case AOE_CONST_FUNC::TUA_RANGE:
		case AOE_CONST_FUNC::TUA_WORK_RATE:
		case AOE_CONST_FUNC::TUA_RESOURCE_CAPACITY:
		case AOE_CONST_FUNC::TUA_DEFAULT_ARMOR:
		case AOE_CONST_FUNC::TUA_INTELLIGENT_PROJECTILE: // 1 is better than 0 (not completely true for slow projectiles ?)
			return 1;
		case AOE_CONST_FUNC::TUA_ATTACK_RELOAD_TIME: // Longer reload time = shoots less
		case AOE_CONST_FUNC::TUA_SIZE_RADIUS1: // Bigger = not an advantage (generally)
		case AOE_CONST_FUNC::TUA_SIZE_RADIUS2: // Bigger = not an advantage (generally)
		case AOE_CONST_FUNC::TUA_ROTATION_SPEED: // most units have 0. other values are worst (bigdaddy): have more difficulties to move (turn)
		case AOE_CONST_FUNC::TUA_POPULATION_COST: // For logistics: decreasing the cost is better
		case AOE_CONST_FUNC::TUA_ADD_COST_AMOUNT:
			return -1;
		// Not relevant for those ones :
		case AOE_CONST_FUNC::TUA_UNKNOWN_07:
		case AOE_CONST_FUNC::TUA_PROJECTILE_UNIT:
		case AOE_CONST_FUNC::TUA_ANGLE:
		case AOE_CONST_FUNC::TUA_TERRAIN_RESTRICTION_FOR_DAMAGE:
		case AOE_CONST_FUNC::TUA_NONE:
		default:
			return 0;
		}
	}
}

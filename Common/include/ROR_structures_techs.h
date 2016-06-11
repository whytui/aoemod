
#pragma once

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
		AOE_CONST_FUNC::TECH_DEF_EFFECTS effectType; // 1 byte.
		char unused_01;
		short int effectUnit; // +02. Unit ID or Resource ID or...
		short int effectClass; // +04. Class or ToUnit (upgrade unit) or Mode (for enable unit, resource modifier) or...
		short int effectAttribute; // +06
		float effectValue; // +08. Sometimes unused (enable unit, upgrade unit, etc). Sometimes includes 2 info using modulo !

		float GetValue() const {
			// Attribute modifier (set/add/mult) on armor or attack: exception (need to apply a modulo on value)
			if (((this->effectType == AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
				(this->effectType == AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
				(this->effectType == AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT)) &&
				((this->effectAttribute == 8) || (this->effectAttribute == 9))) { // armor&attack attributes: 2 info stored in the float...
				return fmodf(this->effectValue, 256); // Holy WTF !!!
				//AOE_CONST_FUNC::ATTACK_CLASS attackClass = (AOE_CONST_FUNC::ATTACK_CLASS)(this->effectValue / 256);
			}
			// ALL other cases: just use float value as is.
			return this->effectValue;
		}
		// Returns true if tech effect has a valid and "effective" effect (not disabled, not adding 0, etc)
		bool HasValidEffect() const {
			switch (this->effectType) {
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_INVALID:
				return false;
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET:
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH:
				return true;
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET:
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESEARCH_COST_MODIFIER:
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESEARCH_TIME_MODIFIER:
				if (this->effectClass == 0) { // mode 0 = set
					return true;
				} else { // mode 1 = add
					return (this->GetValue() != 0);
				}
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT:
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT:
				return (this->effectUnit >= 0);
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD:
				return (this->GetValue() != 0);
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT:
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT:
				return (this->GetValue() != 1);
			}
			return true;
		}
		bool IsEnableUnit(short int unitDefId) const {
			return (this->effectType == AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (this->effectClass == 1) && (this->effectUnit == unitDefId);
		}
		bool IsDisableUnit(short int unitDefId) const {
			return (this->effectType == AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (this->effectClass == 0) && (this->effectUnit == unitDefId);
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
		STRUCT_TECH_DEF *ptrTechDefArray; // unsure
		long int technologyCount;

		bool IsCheckSumValid() const { return this->checksum == 0x00549920; }
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

}
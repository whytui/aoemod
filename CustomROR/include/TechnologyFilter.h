#pragma once

#include <string>
#include <assert.h>
#include <ROR_structures.h>
#include <AOE_const_functional.h>


using namespace AOE_STRUCTURES;
using namespace AOE_CONST_FUNC;

namespace AOE_TECHNOLOGIES {
	// Technologies to ignore for automatic technology adding/detection, because they have drawbacks or unused by AI
	// Ages and tech trees are in ignore list too
	/*const short int LST_TECHS_TO_IGNORE[] = { CST_TCH_STONE_AGE, CST_TCH_TOOL_AGE, CST_TCH_BRONZE_AGE, CST_TCH_IRON_AGE,
		CST_TCH_TECH_TREE_ALL_TECHS, CST_TCH_TECH_TREE_ASSYRIAN, CST_TCH_TECH_TREE_BABYLONIAN, CST_TCH_TECH_TREE_CARTHAGE,
		CST_TCH_TECH_TREE_CHOSON, CST_TCH_TECH_TREE_EGYPTIAN, CST_TCH_TECH_TREE_GREEK, CST_TCH_TECH_TREE_HITTITE,
		CST_TCH_TECH_TREE_MACEDONIAN, CST_TCH_TECH_TREE_MINOAN, CST_TCH_TECH_TREE_PALMYRA, CST_TCH_TECH_TREE_PERSIAN,
		CST_TCH_TECH_TREE_PHOENICIAN, CST_TCH_TECH_TREE_ROMAN, CST_TCH_TECH_TREE_SHANG, CST_TCH_TECH_TREE_SUMERIAN, CST_TCH_TECH_TREE_YAMATO,
		CST_TCH_JIHAD, CST_TCH_MARTYRDOM,
		CST_TCH_BALLISTA_TOWER // because researching ballista tower in RM is not such a good idea (bad against villagers)
	};*/

	const short int LST_TECHS_TREES[] = {
		CST_TCH_TECH_TREE_ALL_TECHS, CST_TCH_TECH_TREE_ASSYRIAN, CST_TCH_TECH_TREE_BABYLONIAN, CST_TCH_TECH_TREE_CARTHAGE,
		CST_TCH_TECH_TREE_CHOSON, CST_TCH_TECH_TREE_EGYPTIAN, CST_TCH_TECH_TREE_GREEK, CST_TCH_TECH_TREE_HITTITE,
		CST_TCH_TECH_TREE_MACEDONIAN, CST_TCH_TECH_TREE_MINOAN, CST_TCH_TECH_TREE_PALMYRA, CST_TCH_TECH_TREE_PERSIAN,
		CST_TCH_TECH_TREE_PHOENICIAN, CST_TCH_TECH_TREE_ROMAN, CST_TCH_TECH_TREE_SHANG, CST_TCH_TECH_TREE_SUMERIAN, CST_TCH_TECH_TREE_YAMATO
	};

	class TechnologyFilterBase {
	public:
		static bool IsResourceModifier(STRUCT_TECH_DEF_EFFECT *techEffect) {
			return (techEffect->effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) ||
				(techEffect->effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT);
		}

		// Returns true if an effect should be ignored (typically, effect with no real impact)
		virtual bool IgnoreEffect(STRUCT_TECH_DEF_EFFECT *techEffect) const {
			return (IsResourceModifier(techEffect) && (techEffect->effectUnit == CST_RES_ORDER_TECHNOLOGIES)); // Ignore the "add 1 to technologies count" effect
		}
		// Returns true if an effect should have the whole tech be ignored (effect with potential drawback, or that make the whole technology be irrelevant according to some specific criteria)
		virtual bool IgnoreWholeTech(STRUCT_TECH_DEF_EFFECT *techEffect) const {
			return false;
		}
	};

	class TechFilterNoTechTreeNoAge : public TechnologyFilterBase {
	public:
		bool IgnoreEffect(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreEffect(techEffect)) { return true; }
			/*for (int i = 0; i < CST_TCH_TECH_TREE_COUNT; i++) {
				if (techEffect->)
			}*/
			if (IsResourceModifier(techEffect) && (techEffect->effectUnit == RESOURCE_TYPES::CST_RES_ORDER_CURRENT_AGE)) {
				return true;
			}
		}
		bool IgnoreWholeTech(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreWholeTech(techEffect)) { return true; }
			/*for (int i = 0; i < CST_TCH_TECH_TREE_COUNT; i++) {
			if (techEffect->)
			}*/
			if (IsResourceModifier(techEffect) && (techEffect->effectUnit == RESOURCE_TYPES::CST_RES_ORDER_CURRENT_AGE)) {
				return true;
			}
		}
	};

	// Filter all effects that corresponds to features/behaviours that AI doesn't use/know about (martyrdom...)
	class TechFilterExcludeNotAISupported : public TechnologyFilterBase {
	public:
		bool IgnoreEffect(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreEffect(techEffect)) { return true; }
			// Ignore martyrdom
			if (IsResourceModifier(techEffect) && (techEffect->effectUnit == CST_RES_ORDER_PRIEST_SACRIFICE)) {
				return true;
			}
			return false;
		}
		bool IgnoreWholeTech(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreWholeTech(techEffect)) { return true; }
			// Ignore martyrdom
			if (IsResourceModifier(techEffect) && (techEffect->effectUnit == CST_RES_ORDER_PRIEST_SACRIFICE)) {
				return true;
			}
			return false;
		}
	};

	// Filter all effects that corresponds to features/behaviours that AI doesn't use/know about (martyrdom...)
	// Filter all effects that have drawbacks: slower projectiles (ballista tower...), less productive villagers (jihad)
	class TechFilterExcludeTechsWithDrawbacks : public TechFilterExcludeNotAISupported {
	public:
		bool IgnoreEffect(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreEffect(techEffect)) { return true; }
			return this->InternalIgnoreTech(techEffect);
		}
		bool IgnoreWholeTech(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreWholeTech(techEffect)) { return true; }
			return this->InternalIgnoreTech(techEffect);
		}
	private:
		bool InternalIgnoreTech(STRUCT_TECH_DEF_EFFECT *techEffect) const {
			// Ignore jihad
			if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) {
				bool positiveEffect = ((techEffect->effectValue < 0) && (AttributeValueHasPositiveEffect((TECH_UNIT_ATTRIBUTES)techEffect->effectAttribute) < 0)) ||
					((techEffect->effectValue > 0) && (AttributeValueHasPositiveEffect((TECH_UNIT_ATTRIBUTES)techEffect->effectAttribute) > 0));
				return !positiveEffect;
			}
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT) &&
				(techEffect->effectValue < 1) &&
				(AttributeValueHasPositiveEffect((TECH_UNIT_ATTRIBUTES)techEffect->effectAttribute) < 0)) {
				return true;
			}
			// TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET : no way to know, depends on initial (previous) value
			// Ballista tower:
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) &&
				(techEffect->effectClass == CST_UNITID_BALLISTA_TOWER)) {
				return true; // TODO: remove hardcoded criterion
			}
			// TODO: Need player struct to find out if "set" is positive or negative
			return false;
		}
	};

	// Filter all effects that corresponds to features/behaviours that AI doesn't use/know about (martyrdom...)
	// Filter all effects that have drawbacks
	// Filter all effects that are "light" and that impact all/a lot of units (LOS for macedonian)
	class TechFilterExcludeDrawbacksAndDistributedEffects : public TechFilterExcludeNotAISupported {
	public:
		bool IgnoreEffect(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreEffect(techEffect)) { return true; }
			if (techEffect->IsAttributeModifier() && (techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_LOS)) {
				// TODO: this criterion is not really good. Temporary trick for macedonian tech tree
				return true; // Ignore effects on LOS. The actually useful effects are the ones that improve range !
			}
			return false;
		}
		bool IgnoreWholeTech(STRUCT_TECH_DEF_EFFECT *techEffect) const override {
			if (__super::IgnoreWholeTech(techEffect)) { return true; }
			return false;
		}
		// TODO : ignore tech def, not at effect level (need parent techdef)
	};
}

#include "../include/triggerDefinition.h"

namespace CR_TRIGGERS {

	// This unique instance of object will be shared by all classes/files
	ALL_TRIGGER_DEF allTriggerDef;


	// Static Initializations


	// List all existing parameters
	TRIGGER_PARAM_DEF ALL_TRIGGER_DEF::all_params[] {
		TRIGGER_PARAM_DEF(KW_NONE, TPT_NONE, "none", "Invalid parameter"),
		TRIGGER_PARAM_DEF(KW_NAME, TPT_STRING, "name", "Always provide 1 unique name for 1 TRIGGER"),
		TRIGGER_PARAM_DEF(KW_RUN_BEFORE, TPT_STRING, "run_before_me", "A Trigger (name) that will be executed just BEFORE the trigger this action is included with"),
		TRIGGER_PARAM_DEF(KW_RUN_AFTER, TPT_STRING, "run_after_me", "A Trigger (name) that will be executed just AFTER the trigger this action is included with"),
		TRIGGER_PARAM_DEF(KW_EVENT_TYPE, TPT_STRING, "event_type", "The type of Event (condition) that is used in order for the chosen Action (effect) to become active"), // Special: event type (always required)
		TRIGGER_PARAM_DEF(KW_DIFF_LEVEL, TPT_INTEGER, "diff_level"), // 0=hardest - 4=easiest
		TRIGGER_PARAM_DEF(KW_TIMER_VALUE, TPT_INTEGER, "timer_value", "# of seconds"),
		TRIGGER_PARAM_DEF(KW_TIMER_INTERVAL, TPT_INTEGER, "timer_interval", "# of seconds"),
		TRIGGER_PARAM_DEF(KW_EVENT_UNIT_ID, TPT_INTEGER, "event_unit_id", "A unit instance ID. This is NOT AGE3 unitID"),
		TRIGGER_PARAM_DEF(KW_EVENT_PLAYER_ID, TPT_INTEGER, "event_player_id", "A player ID 0-8"),
		TRIGGER_PARAM_DEF(KW_EVENT_RESEARCH_ID, TPT_INTEGER, "event_research_id", "A research id, see AGE3"),
		TRIGGER_PARAM_DEF(KW_EVENT_RESOURCE_ID, TPT_INTEGER, "event_resource_id", "A resource id, see AGE3 (food=0, wood=1, stone=2, gold=3)."),
		TRIGGER_PARAM_DEF(KW_EVENT_RESOURCE_VALUE, TPT_FLOAT, "event_resource_value", "Defines a certain amount of resource."),
		TRIGGER_PARAM_DEF(KW_ACTION_TYPE, TPT_STRING, "action_type", "The type of Action (effect) that is used to create the desired effect"), // Special: action type (always required)
		TRIGGER_PARAM_DEF(KW_MESSAGE, TPT_STRING, "message", "A free message, that cannot contain chariot return"),
		TRIGGER_PARAM_DEF(KW_ACTION_PLAYER_ID, TPT_INTEGER, "action_player_id"), // "actor" player (the one directly involved in trigger action)
		TRIGGER_PARAM_DEF(KW_ACTION_TARGET_PLAYER_ID, TPT_INTEGER, "target_player_id"), // "secondary" player for actions like diplomacy change.
		TRIGGER_PARAM_DEF(KW_DIPLOMACY_VALUE, TPT_INTEGER, "dipl_value", "Diplomacy can be 0=ally, 1=neutral, 3=enemy. Other values are invalid."), // Corresponds to "diplomacy stances" internal enum
		TRIGGER_PARAM_DEF(KW_RESOURCE_ID, TPT_INTEGER, "resource_id", "You can refer to AGE3 about resource IDs, for example in civilizations tab"),
		TRIGGER_PARAM_DEF(KW_RESOURCE_VALUE, TPT_FLOAT, "resource_value"),
		TRIGGER_PARAM_DEF(KW_ACTION_UNIT_ID, TPT_INTEGER, "action_unit_id", "A unit instance ID. This is NOT AGE3 unitID"),
		TRIGGER_PARAM_DEF(KW_ACTION_RESEARCH_ID, TPT_INTEGER, "action_research_id"),
		TRIGGER_PARAM_DEF(KW_TECHNOLOGY_ID, TPT_INTEGER, "technology_id", "A technology ID, see AGE3"),
		TRIGGER_PARAM_DEF(KW_ACTION_OBJ_PROPERTY, TPT_STRING, "action_obj_property"),
		TRIGGER_PARAM_DEF(KW_ACTION_OBJ_PROPERTY_VALUE, TPT_STRING, "action_obj_property_value"),
		TRIGGER_PARAM_DEF(KW_TARGET_TRIGGER_NAME, TPT_STRING, "target_trigger_name"),
		TRIGGER_PARAM_DEF(KW_POS_X, TPT_FLOAT, "x", "X coordinate in map, in [0-255] (also depending on map size)"),
		TRIGGER_PARAM_DEF(KW_POS_Y, TPT_FLOAT, "y", "Y coordinate in map, in [0-255] (also depending on map size)"),
		TRIGGER_PARAM_DEF(KW_MIN_POS_X, TPT_INTEGER, "min_x", "Minimum X position of a map zone"),
		TRIGGER_PARAM_DEF(KW_MIN_POS_Y, TPT_INTEGER, "min_y", "Minimum Y position of a map zone"),
		TRIGGER_PARAM_DEF(KW_MAX_POS_X, TPT_INTEGER, "max_x", "Maximum X position of a map zone"),
		TRIGGER_PARAM_DEF(KW_MAX_POS_Y, TPT_INTEGER, "max_y", "Maximum Y position of a map zone"),
		TRIGGER_PARAM_DEF(KW_ELEVATION_LEVEL, TPT_INTEGER, "elevation", "Elevation values are between 0 and 7 (included)"),
		TRIGGER_PARAM_DEF(KW_TERRAIN_ID, TPT_INTEGER, "terrain_id", "A terrain ID, see AGE3"),
		TRIGGER_PARAM_DEF(KW_ADD_HP, TPT_FLOAT, "add_hp"),
		TRIGGER_PARAM_DEF(KW_SET_HP, TPT_FLOAT, "set_hp"),
		TRIGGER_PARAM_DEF(KW_ADD_RESOURCE, TPT_FLOAT, "add_resource"),
		TRIGGER_PARAM_DEF(KW_SET_RESOURCE, TPT_FLOAT, "set_resource"),
		TRIGGER_PARAM_DEF(KW_ACTION_UNIT_DEF_ID, TPT_INTEGER, "action_unit_def_id", "Unit definition ID (from AGE3)"),
		TRIGGER_PARAM_DEF(KW_UNIT_GRAPHICS_AGE, TPT_INTEGER, "unit_graphics_age", "0 for stone/tool, 1 for bronze, 2 for iron. [Not working]"),
		TRIGGER_PARAM_DEF(KW_UNIT_ORIENTATION_INDEX, TPT_INTEGER, "unit_orientation_index", "Also impacts in-progress buildings \"status\". Values are 0-7, clockwise."),
		TRIGGER_PARAM_DEF(KW_FROM_UNIT_DEF_ID, TPT_INTEGER, "from_unit_def_id", "(AGE3) unit definition ID to copy to create a new unit \"definition\"."),
		TRIGGER_PARAM_DEF(KW_UNIT_NAME, TPT_STRING, "unit_name", "Unit definition name (like AGE3 unit name)"),
		TRIGGER_PARAM_DEF(KW_TOTAL_HP, TPT_INTEGER, "total_hp", "Total hit points (including upgrades)"),
		TRIGGER_PARAM_DEF(KW_RANGE, TPT_FLOAT, "range", "Range/line of sight value"),
		TRIGGER_PARAM_DEF(KW_SPEED, TPT_FLOAT, "speed", "Total speed (including upgrades)"),
		TRIGGER_PARAM_DEF(KW_ROTATION_SPEED, TPT_FLOAT, "rotation_speed", "Unit rotation speed"),
		TRIGGER_PARAM_DEF(KW_WORK_RATE, TPT_FLOAT, "work_rate", "Work rate (including upgrades)"),
		TRIGGER_PARAM_DEF(KW_ACCURACY_PERCENT, TPT_INTEGER, "accuracy_percent", "Accuracy percent, almost all units have 100"),
		TRIGGER_PARAM_DEF(KW_DISPLAYED_ARMOR, TPT_INTEGER, "displayed_armor", "Displayed armor value, this does NOT affect actual armor points"),
		TRIGGER_PARAM_DEF(KW_DISPLAYED_ATTACK, TPT_INTEGER, "displayed_attack", "Displayed attack value, this does NOT affect actual attack points"),
		TRIGGER_PARAM_DEF(KW_VISIBLE_IN_FOG, TPT_INTEGER, "visible_in_fog", "0/1 value to determine if other players can see such units through fog"),
		TRIGGER_PARAM_DEF(KW_OWNER_PLAYER_ID, TPT_INTEGER, "owner_player_id", "A playerId 0-8 that will take ownership of a unit."),
		TRIGGER_PARAM_DEF(KW_ACTION_TARGET_UNIT_ID, TPT_INTEGER, "action_target_unit_id", "A unit instance ID that will be the target of an action. This is NOT AGE3 unitID"),
		TRIGGER_PARAM_DEF(KW_ENABLE, TPT_INTEGER, "enable", "Set to 1 to enable, 0 to disable"),
		TRIGGER_PARAM_DEF(KW_MUTUAL, TPT_INTEGER, "mutual", "If true, the action is done in both ways (mutual) between actor and target. Set to 1 to enable, 0 to disable")
	};


	// All existing events
	TRIGGER_EVENT_DEF ALL_TRIGGER_DEF::events[] = {
		TRIGGER_EVENT_DEF(EVENT_NONE, "none", std::vector<TRIGGER_KWID_PARAMS>({}), "Invalid event"),
		TRIGGER_EVENT_DEF(EVENT_FROM_OTHER_TRIGGER, "from_other_trigger", std::vector<TRIGGER_KWID_PARAMS>({}), "There is no event that will execute this trigger. \nIt can only be executed from another trigger with special actions: run_after_me or run_before_me"),
		TRIGGER_EVENT_DEF(EVENT_GAME_START, "game_start", std::vector<TRIGGER_KWID_PARAMS>({}), "Activate when the Scenario starts"),
		TRIGGER_EVENT_DEF(EVENT_TIMER, "timer", std::vector<TRIGGER_KWID_PARAMS>({ KW_TIMER_VALUE, KW_TIMER_INTERVAL }), "If a certain number of seconds have passed. \ntimer_interval is optional (reoccuring interval), if not set or <0, the trigger will only be run once. A reoccuring interval can be stopped by disable_trigger."),
		TRIGGER_EVENT_DEF(EVENT_UNIT_LOSS, "unit_loss", std::vector<TRIGGER_KWID_PARAMS>({ KW_EVENT_UNIT_ID }), "If a certain unit is lost (included on conversion by another player)"),
		TRIGGER_EVENT_DEF(EVENT_RESEARCH_COMPLETED, "research_completed", std::vector<TRIGGER_KWID_PARAMS>({ KW_EVENT_PLAYER_ID, KW_EVENT_RESEARCH_ID }), "If a research is completed for a certain player"),
		TRIGGER_EVENT_DEF(EVENT_DISCOVER_GAIA_UNIT, "find_gaia_unit", std::vector<TRIGGER_KWID_PARAMS>({ KW_EVENT_PLAYER_ID, KW_EVENT_UNIT_ID }), "If a gaia unit has been discovered by a player."),
		TRIGGER_EVENT_DEF(EVENT_RESOURCE_VALUE_MORE_THAN, "resource_more_than", std::vector<TRIGGER_KWID_PARAMS>({ KW_EVENT_PLAYER_ID, KW_EVENT_RESOURCE_ID, KW_EVENT_RESOURCE_VALUE }), "If a player has more than a certain amount of a given resource."),
		TRIGGER_EVENT_DEF(EVENT_RESOURCE_VALUE_LESS_THAN, "resource_less_than", std::vector<TRIGGER_KWID_PARAMS>({ KW_EVENT_PLAYER_ID, KW_EVENT_RESOURCE_ID, KW_EVENT_RESOURCE_VALUE }), "If a player has less than a certain amount of a given resource.")
	};


	// All existing actions
	TRIGGER_ACTION_DEF ALL_TRIGGER_DEF::actions[] = {
		TRIGGER_ACTION_DEF(TRIGGER_ACTION_TYPES::TYPE_NONE, "none", std::vector<TRIGGER_KWID_PARAMS>({}), "Invalid action"),
		TRIGGER_ACTION_DEF(TYPE_WRITE_CHAT, "write_chat", std::vector<TRIGGER_KWID_PARAMS>({ KW_MESSAGE }), "Displays normal chat message. \nYou can have spaces in the message but no chariot return"),
		TRIGGER_ACTION_DEF(TYPE_WRITE_CENTERED_MESSAGE, "write_center_chat", std::vector<TRIGGER_KWID_PARAMS>({ KW_MESSAGE }), "Displays a chat message in red color at the center-bottom of the screen.\nDisappears if clicked anywhere"),
		TRIGGER_ACTION_DEF(TYPE_DIPL_CHANGE, "dipl_change", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_ACTION_TARGET_PLAYER_ID, KW_DIPLOMACY_VALUE, KW_MUTUAL }), "Diplomacy change. Use the \"mutual\" keyword to update both player's diplomacy."),
		TRIGGER_ACTION_DEF(TYPE_ADD_RESOURCE, "add_resource", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_RESOURCE_ID, KW_RESOURCE_VALUE }), "Add an amount of selected resource to selected player, for example 100 food"),
		TRIGGER_ACTION_DEF(TYPE_SET_RESOURCE, "set_resource", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_RESOURCE_ID, KW_RESOURCE_VALUE }), "Sets an amount of selected resource to selected player, for example 100 food. \nUnlike add_resource this SETS and does NOT add to your stockpile"),
		TRIGGER_ACTION_DEF(TYPE_ENABLE_RESEARCH, "enable_research", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_ACTION_RESEARCH_ID }), "Enables a research for selected player, for example Watch Tower at the Granary"),
		TRIGGER_ACTION_DEF(TYPE_DISABLE_RESEARCH, "disable_research", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_ACTION_RESEARCH_ID }), "Disables a research for selected player, for example Small Wall at the Granary"),
		TRIGGER_ACTION_DEF(TYPE_APPLY_RESEARCH, "apply_research", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_ACTION_RESEARCH_ID }), "Applies a research for selected player, for example Watch Tower at the Granary"),
		TRIGGER_ACTION_DEF(TYPE_APPLY_TECH, "apply_tech", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_TECHNOLOGY_ID }), "Applies a technology for selected player, for example the Lengthen Combat Mode (Tech ID = 100). \nPlease use apply_research instead, when possible, because it is the standard way how this is used in AoERoR"),
		TRIGGER_ACTION_DEF(TYPE_DISABLE_TRIGGER, "disable_trigger", std::vector<TRIGGER_KWID_PARAMS>({ KW_TARGET_TRIGGER_NAME }), "Disables selected trigger, this is useful when having an interval for example"),
		TRIGGER_ACTION_DEF(TYPE_SET_TERRAIN, "set_terrain", std::vector<TRIGGER_KWID_PARAMS>({ KW_TERRAIN_ID, KW_ELEVATION_LEVEL, KW_MIN_POS_X, KW_MIN_POS_Y, KW_MAX_POS_X, KW_MAX_POS_Y }), "Does not work very well"),
		TRIGGER_ACTION_DEF(TYPE_MODIFY_UNIT, "modify_unit", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_UNIT_ID, KW_ADD_HP, KW_SET_HP, KW_ADD_RESOURCE, KW_SET_RESOURCE, KW_UNIT_GRAPHICS_AGE, KW_UNIT_ORIENTATION_INDEX, KW_POS_X, KW_POS_Y, KW_OWNER_PLAYER_ID }),
			"Modifies a unit's attributes, such as HP (Hit Points). If HP <=0 then the unit dies. \nResource value can be applied to gatherable units such as Trees, Berry bush, Gazelle etc"),
		TRIGGER_ACTION_DEF(TYPE_ADD_UNIT_INSTANCE, "spawn_unit", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID, KW_ACTION_UNIT_DEF_ID, KW_UNIT_NAME, KW_POS_X, KW_POS_Y }), "Spawns a unit, given its ID (see AGE3), for a given player, at provided location. \nFor unit \"definitions\" that are added by another trigger, please provide unit_name instead of ID. \nWarning: the \"unit name\" param is only used to find unit when it has been created by another trigger."),
		TRIGGER_ACTION_DEF(TYPE_ADD_UNIT_DEF, "add_unit_def", std::vector<TRIGGER_KWID_PARAMS>(
		{ KW_ACTION_PLAYER_ID, KW_FROM_UNIT_DEF_ID, KW_UNIT_NAME, KW_TOTAL_HP, KW_RANGE, KW_SPEED, KW_ROTATION_SPEED, KW_WORK_RATE, KW_ACCURACY_PERCENT, KW_DISPLAYED_ARMOR, KW_DISPLAYED_ATTACK, KW_VISIBLE_IN_FOG }),
			"Adds a new unit definition, copied from an existing unit definition, for a given player. \nYou can then spawn one or several units based on this definition.\nA unit definition is specific to a player.\nBy default, all attribute are the same as the unit you copied's ones. You can change some attributes using the supplied keywords. \nAll unit attributes are optional except unit name"),
		TRIGGER_ACTION_DEF(TYPE_SET_UNIT_TARGET, "set_unit_target", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_UNIT_ID, KW_ACTION_TARGET_UNIT_ID }), "Tell a unit to interact with a target unit (like right click).\nIt can result in gather/attack, etc, depending on both unit types.\nWarning: AI may give another order that will cancel this one, especially for villagers."),
		TRIGGER_ACTION_DEF(TYPE_SET_UNIT_DESTINATION, "set_unit_destination", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_UNIT_ID , KW_POS_X, KW_POS_Y}), "Tell a unit to move to a position on the map."),
		TRIGGER_ACTION_DEF(TYPE_SET_GAIA_CAPTURABLE, "set_gaia_capturable", std::vector<TRIGGER_KWID_PARAMS>({ KW_ENABLE }), "Make gaia units capturable by human player or not.\nA typical use is to disable (set to 0) at game start.\nWarning: a gaia unit can only be captured when it BECOMES visible.\nIf you switch from disabled to enabled, a gaia unit must get covered by fog then \"be seen\" again to get captured."),
		TRIGGER_ACTION_DEF(TYPE_SET_VIEWED_PLAYER, "set_viewed_player", std::vector<TRIGGER_KWID_PARAMS>({ KW_ACTION_PLAYER_ID }), "Change the playerID the human player will see and control.\nThis does NOT change which players AI controls, so a player might be controlled by both human AND AI. Works with player 0-8.")
	};



	// ---------------------------------------------------------------------------------------------------
	// Methods
	// This section generally does not need to be modified when adding new triggers/events/actions.



	// This method is an ugly workaround for Visual Studio bug: it does not want to compile (and run static_assert) on private members !
	// This is ridiculous, as these are only STATIC checks !
	void ALL_TRIGGER_DEF::static_check() {
		// Here are 3 static checks to make sure event/action/params list are consistent between enumeration and the setup above.
		static_assert(sizeof(allTriggerDef.events) / sizeof(CR_TRIGGERS::TRIGGER_EVENT_DEF) == TRIGGER_EVENTS_TYPES_COUNT, "Event list does not match: enum vs definition.");
		static_assert(sizeof(allTriggerDef.actions) / sizeof(CR_TRIGGERS::TRIGGER_ACTION_DEF) == TRIGGER_ACTION_TYPES_COUNT, "Action list does not match: enum vs definition.");
		static_assert(sizeof(allTriggerDef.all_params) / sizeof(CR_TRIGGERS::TRIGGER_PARAM_DEF) == KW_PARAMS_COUNT, "Parameters list does not match: enum vs definition.");
	}



	// Securely get an event from its id.
	// Returns NULL if invalid.
	TRIGGER_EVENT_DEF *ALL_TRIGGER_DEF::GetEventOrNULL(TRIGGER_EVENT_TYPES eventId) const {
		if ((eventId < TRIGGER_EVENT_TYPES::EVENT_NONE) || (eventId >= TRIGGER_EVENT_TYPES::TRIGGER_EVENTS_TYPES_COUNT)) {
			return NULL;
		}
		return &this->events[eventId];
	}
	// Returns NULL if invalid.
	TRIGGER_ACTION_DEF *ALL_TRIGGER_DEF::GetActionOrNULL(TRIGGER_ACTION_TYPES actionId) const {
		if ((actionId < TRIGGER_ACTION_TYPES::TYPE_NONE) || (actionId >= TRIGGER_ACTION_TYPES::TRIGGER_ACTION_TYPES_COUNT)) {
			return NULL;
		}
		return &this->actions[actionId];
	}
	// Returns NULL if invalid.
	TRIGGER_PARAM_DEF *ALL_TRIGGER_DEF::GetParamOrNULL(TRIGGER_KWID_PARAMS paramId) const {
		if ((paramId < TRIGGER_KWID_PARAMS::KW_NONE) || (paramId >= TRIGGER_KWID_PARAMS::KW_PARAMS_COUNT)) {
			return NULL;
		}
		return &this->all_params[paramId];
	}

	// Securely get an event from its id.
	// Returns "NONE" object if invalid.
	TRIGGER_EVENT_DEF *ALL_TRIGGER_DEF::GetEvent(TRIGGER_EVENT_TYPES eventId) const {
		if ((eventId < TRIGGER_EVENT_TYPES::EVENT_NONE) || (eventId >= TRIGGER_EVENT_TYPES::TRIGGER_EVENTS_TYPES_COUNT)) {
			return &this->events[EVENT_NONE];;
		}
		return &this->events[eventId];
	}
	// Returns "NONE" object if invalid.
	TRIGGER_ACTION_DEF *ALL_TRIGGER_DEF::GetAction(TRIGGER_ACTION_TYPES actionId) const {
		if ((actionId < TRIGGER_ACTION_TYPES::TYPE_NONE) || (actionId >= TRIGGER_ACTION_TYPES::TRIGGER_ACTION_TYPES_COUNT)) {
			return &this->actions[TYPE_NONE];
		}
		return &this->actions[actionId];
	}
	// Returns "NONE" object if invalid.
	TRIGGER_PARAM_DEF *ALL_TRIGGER_DEF::GetParam(TRIGGER_KWID_PARAMS paramId) const {
		if ((paramId < TRIGGER_KWID_PARAMS::KW_NONE) || (paramId >= TRIGGER_KWID_PARAMS::KW_PARAMS_COUNT)) {
			return &this->all_params[KW_NONE];
		}
		return &this->all_params[paramId];
	}


	// Returns (char*) string that corresponds to provided action type.
	const char *TriggerActionTypeToText(TRIGGER_ACTION_TYPES value) {
		assert(value < TRIGGER_ACTION_TYPES::TRIGGER_ACTION_TYPES_COUNT);
		assert((int)value >= 0);
		assert(allTriggerDef.GetAction(value) != NULL);
		return allTriggerDef.GetAction(value)->keyword;
	}
	// Returns (char*) string that corresponds to provided event type.
	const char *TriggerEventTypeToText(TRIGGER_EVENT_TYPES value) {
		assert((int)value >= 0);
		assert(value < TRIGGER_EVENTS_TYPES_COUNT);
		assert(allTriggerDef.GetEvent(value) != NULL);
		return allTriggerDef.GetEvent(value)->keyword;
	}
	// Returns (char*) string that corresponds to provided event type.
	const char *TriggerParamKeywordIdToText(TRIGGER_KWID_PARAMS paramId) {
		assert((int)paramId >= 0);
		assert(paramId < TRIGGER_KWID_PARAMS::KW_PARAMS_COUNT);
		assert(allTriggerDef.GetParam(paramId) != NULL);
		return allTriggerDef.GetParam(paramId)->keyword;
	}

	// Returns the TRIGGER_ACTION_TYPES that matches provided text. Returns TYPE_NONE if not found.
	TRIGGER_ACTION_TYPES GetTriggerActionTypeFromText(char *value) {
		for (TRIGGER_ACTION_TYPES i = TYPE_NONE; i < TRIGGER_ACTION_TYPES::TRIGGER_ACTION_TYPES_COUNT; i = TRIGGER_ACTION_TYPES(i + 1)) {
			if (_stricmp(value, TriggerActionTypeToText(i)) == 0) { return i; }
		}
		return TRIGGER_ACTION_TYPES::TYPE_NONE;
	}

	// Returns the TRIGGER_EVENT_TYPES that matches provided text. Returns EVENT_NONE if not found.
	TRIGGER_EVENT_TYPES GetTriggerEventTypeFromText(char *value) {
		for (TRIGGER_EVENT_TYPES i = EVENT_NONE; i < TRIGGER_EVENT_TYPES::TRIGGER_EVENTS_TYPES_COUNT; i = TRIGGER_EVENT_TYPES(i + 1)) {
			if (_stricmp(value, TriggerEventTypeToText(i)) == 0) { return i; }
		}
		return TRIGGER_EVENT_TYPES::EVENT_NONE;
	}

	// Returns the TRIGGER_KWID_PARAMS that matches provided keyword. Returns KW_NONE if not found.
	TRIGGER_KWID_PARAMS GetTriggerParamKeywordIdFromText(char *value) {
		for (TRIGGER_KWID_PARAMS i = TRIGGER_KWID_PARAMS::KW_NONE; i < TRIGGER_KWID_PARAMS::KW_PARAMS_COUNT; i = TRIGGER_KWID_PARAMS(i + 1)) {
			if (_stricmp(value, TriggerParamKeywordIdToText(i)) == 0) { return i; }
		}
		return TRIGGER_KWID_PARAMS::KW_NONE;
	}


	// Returns the (char*) "special" keyword that corresponds to provided index.
	// Returns "" if index is not in bounds.
	const char *GetTriggerSpecialKeyword(TRIGGER_KWID_SPECIAL kwIndex) {
		if ((kwIndex < 0) || (kwIndex >= TRIGGER_KWID_SPECIAL::KW_SPECIAL_COUNT)) { return ""; }
		return KEYWORDS_SPECIAL[kwIndex];
	}
	// Returns the (char*) "param" keyword that corresponds to provided index.
	// Returns "" if index is not in bounds.
	const char *GetTriggerParamKeyword(TRIGGER_KWID_PARAMS kwIndex) {
		if ((kwIndex < 0) || (kwIndex >= TRIGGER_KWID_PARAMS::KW_PARAMS_COUNT)) { return ""; }
		return allTriggerDef.GetParam(kwIndex)->keyword;
	}


	// Returns true if the text corresponding to a keyword (given its index) is equals to provided string (as a char*)
	// The function is case-sensitive
	bool SpecialKeyword_equals(char *value, TRIGGER_KWID_PARAMS kwIndex) {
		return (_stricmp(value, GetTriggerParamKeyword(kwIndex)) == 0);
	}
	bool SpecialKeyword_equals(char *value, TRIGGER_KWID_SPECIAL kwIndex) {
		return (_stricmp(value, GetTriggerSpecialKeyword(kwIndex)) == 0);
	}
	// Get a keyword length
	int SpecialKeyword_getLength(TRIGGER_KWID_PARAMS kwIndex) {
		return strlen(GetTriggerParamKeyword(kwIndex));
	}
	int SpecialKeyword_getLength(TRIGGER_KWID_SPECIAL kwIndex) {
		return strlen(GetTriggerSpecialKeyword(kwIndex));
	}


	// Generate a HTML-formatted string corresponding to a list of parameters.
	std::string ParamListToString(std::vector<TRIGGER_KWID_PARAMS> v) {
		std::string result = "";
		std::vector<TRIGGER_KWID_PARAMS>::const_iterator it;
		for (it = v.begin(); it != v.end(); ++it) {
			TRIGGER_KWID_PARAMS p = (*it);
			if (!result.empty()) { result += "<br/>"; }
			result += "<b>";
			result += allTriggerDef.GetParam(*it)->keyword;
			result += "</b>=<i>{";
			result += TRIGGER_PARAM_TYPE_STRING[allTriggerDef.GetParam(*it)->paramType];
			result += "}</i>";
			result += " - ";
			result += allTriggerDef.GetParam(*it)->comment;
		}
		return result;
	}

	std::string TriggersDefinitionToHTML() {
		std::string result = "<html><head><title>Trigger Events and Actions List - CustomizeAoE</title>\n"
			"<meta http-equiv=\"content-type\" content=\"text/html;charset=windows-1252\" />\n"
			"<style type=\"text/css\">\n"
"body {\n"
"background:#DEAF83;\n"
"}\n"
".tg  {\n"
"width:100%;\n"
"border-collapse:collapse;\n"
"border-spacing:0;\n"
"text-align:left;\n"
"}\n"
".tg td{\n"
"font-family:Arial, sans-serif;\n"
"font-size:14px;\n"
"padding:10px 5px;\n"
"border-style:solid;\n"
"border-width:1px;\n"
"overflow:hidden;\n"
"word-break:normal;\n"
"}\n"
".tg th{font-family:Arial, sans-serif;\n"
"font-size:14px;\n"
"font-weight:normal;\n"
"padding:10px 5px;\n"
"border-style:solid;\n"
"border-width:1px;\n"
"overflow:hidden;\n"
"word-break:normal;\n"
"}\n"
".tg .tg-head{\n"
"background:#EF8713;\n"
"vertical-align:top;\n"
"}\n"
".tg .tg-1{\n"
"background:#DED3B7;\n"
"vertical-align:top;\n"
"}\n"
".tg .tg-2{\n"
"background:#B09898;\n"
"vertical-align:top;\n"
"}\n"
"</style>\n"
			"</head><body>\n"
			"<h2>Instructions</h2>\n<hr/>\n"
			"<table class=\"tg\">\n<tbody>\n<tr>\n"
			"<td class=\"tg-1\">\n"
			"Welcome to the instructions and the <a href=\"#list\"><b>trigger list</b></a> for the CustomizeAoE/RoR trigger system. "
			"Before getting started, it is recommended that you download <b><a href=\"http://aoe.heavengames.com/dl-php/showfile.php?fileid=2544\">AGE3</a></b>, "
			"which provides the necessary values needed for certain parameters, for example, a research ID or a resource ID.<br/>\n"
			"You can use CustomizeAOE \"Triggers/Generate trigger sample\" menu to get started.\n"
			"</td>\n</tr>\n"
			"<tr>\n<td class=\"tg-1\">\n"
			"<b><u>Here is a short explanation of how a Trigger works:</u></b><br><br>\n"
			"<b>TRIGGER</b> - Initiates a trigger. Must be capitalized and left alone.<br>\n"
			"<b>name</b> -always provide 1 <i>unique name</i> for 1 TRIGGER.<br>\n"
			"<b>event_type=(event type)</b> - The type of Event <i>(condition)</i> that is used in order for the chosen Action (<i>effect</i>) to become active.<br>\n"
			"<b>some_param1=(value)</b> - a parameter that is related to this event type. Some parameters are mandatory.<br>\n"
			"<b>action_type=(action type)</b> - The type of Action <i>(effect)</i> that is used to create the desired effect.<br>\n"
			"<b>some_param2=(value)</b> - a parameter that is related to this action type. Some parameters are mandatory.<br>\n"
			"</td>\n</tr>\n"
			"<tr>\n<td class=\"tg-1\">\n"
			"<b>Important:</b><br>\n<ul>\n"
			"<li>There are some <a href=\"#globalActions\">Global actions</a> that can be defined only once in the trigger file.</li>\n"
			"<li>1 TRIGGER can have only 1 event, 1 action and 1 of either <a href=\"#specialActions\">Special actions</a>.</li>\n"
			"<li>Leave no space/tabs/other characters in the lines \"xxx=yyy\".</li>\n"
			"<li>You can add comments by adding 2 forward slashes: //This is a comment. - useful for separating \"sections\" of Triggers for example.</li>\n"
			"<li>All the keywords are case-sensitive.</li>\n"
			"</ul>\n</td>\n</tr>\n"
			"</tbody></table>\n"
			"<a name=\"list\"/>\n"
			"<h2>Event Types and Parameters</h2>\n"
			"<table class=\"tg\"><tr>\n<th class=\"tg-head\"><b>Event type=</b></th>\n"
			"<th class=\"tg-head\"><b>Parameters</b></th>\n"
			"<th class=\"tg-head\"><b>Description</b></th></tr>\n";
		bool even = false;
		std::string s1 = "<td class=\"tg-1\">";
		std::string s2 = "<td class=\"tg-2\">";
		// Start at 1 to avoid the empty event
		for (TRIGGER_EVENT_TYPES i = TRIGGER_EVENT_TYPES(EVENT_NONE + 1); i < TRIGGER_EVENTS_TYPES_COUNT; i = TRIGGER_EVENT_TYPES(i + 1)) {
			if (allTriggerDef.GetEvent(i) != NULL) {
				result += "<tr>\n";
				result += even ? s2 : s1;
				result += allTriggerDef.GetEvent(i)->keyword;
				result += "</td>\n";
				result += even ? s2 : s1;
				result += ParamListToString(allTriggerDef.GetEvent(i)->eventParameters);
				result += "</td>\n";
				result += even ? s2 : s1;
				result += "\n<pre>";
				result += allTriggerDef.GetEvent(i)->description;
				result += "</pre></td>\n</tr>\n";
				even = !even;
			}
		}
		result += "</table><hr/><h2>Action Types and Parameters</h2>"
			"<table class=\"tg\"><tr>\n<th class=\"tg-head\"><b>Action type=</b></th>\n"
			"<th class=\"tg-head\"><b>Parameters</b></th>\n"
			"<th class=\"tg-head\"><b>Description</b></th></tr>\n";
		// Start at 1 to avoid the empty action
		for (TRIGGER_ACTION_TYPES i = TRIGGER_ACTION_TYPES(TYPE_NONE + 1); i < TRIGGER_ACTION_TYPES_COUNT; i = TRIGGER_ACTION_TYPES(i+1)) {
			if (allTriggerDef.GetAction(i) != NULL) {
				result += "<tr>\n";
				result += even ? s2 : s1;
				result += allTriggerDef.GetAction(i)->keyword;
				result += "</td>\n";
				result += even ? s2 : s1;
				result += ParamListToString(allTriggerDef.GetAction(i)->actionParameters);
				result += "</td>\n";
				result += even ? s2 : s1;
				result += "\n<pre>";
				result += allTriggerDef.GetAction(i)->description;
				result += "</pre>\n</td>\n</tr>\n";
				even = !even;
			}
		}
		result += "</table>\n"
			"<h2>Special Actions</h2>\n<hr/>\n"
			"<a name=\"specialActions\"</a>\n"
			"<table class=\"tg\">\n"
			"<tbody><tr>\n"
			"<th class=\"tg-head\"><b>Special Action</b></th>\n"
			"<th class=\"tg-head\"><b>Parameters</b></th>\n"
			"<th class=\"tg-head\"><b>Description</b></th>\n"
			"</tr><tr>\n"
			"<td class=\"tg-2\">run_before_me=(trigger name)</td>\n"
			"<td class=\"tg-2\">None</td>\n"
			"<td class=\"tg-2\">A Trigger (name) that will be executed just BEFORE the trigger this action is included with.</td>\n"
			"</tr><tr>\n"
			"<td class=\"tg-1\">run_after_me=(trigger name)</td>\n"
			"<td class=\"tg-1\">None</td>\n"
			"<td class=\"tg-1\">A Trigger (name) that will be executed just AFTER the trigger this action is included with.</td>\n"
			"</tr>\n"
			"</tbody>\n"
			"</table/>\n<hr/>\n"
			"</table>\n"
			"<h2>Global Actions</h2>\n<hr/>\n"
			// Global keyword (not related to 1 trigger but to the whole set)
			"<a name=\"globalActions\"</a>\n"
			"<table class=\"tg\">\n"
			"<tbody><tr>\n"
			"<th class=\"tg-head\"><b>Global Action</b></th>\n"
			"<th class=\"tg-head\"><b>Parameters</b></th>\n"
			"<th class=\"tg-head\"><b>Description</b></th>\n"
			"</tr><tr>\n"
			"<td class=\"tg-2\">disableUnitDefinitions_p#=(...)</td>\n"
			"<td class=\"tg-2\">None. Replace # by a playerId 1-8</td>\n"
			"<td class=\"tg-2\">Comma-separated list of unit definition IDs (cf AGE3) to disable for this player.\n<br/>"
			"Example: disableUnitDefinitions_p1=0,12 to disable academy and barracks for player 1.\n<br/>"
			"<b>Warning</b>: this is based on technologies/researches mechanisms.\n<br/>"
			"Units that are enabled by a technology must be defined so that the technology has no other effect.\n<br/>"
			"The existing units that do not respect this requirement are farm, house, villager, fishing&trade ships, but they are supported anyway (there is a hardcoded fix in customROR)</td>\n"
			"</tr><tr>\n"
			"<td class=\"tg-1\">disableResearches_p#=(...)</td>\n"
			"<td class=\"tg-1\">None. Replace # by a playerId 1-8</td>\n"
			"<td class=\"tg-1\">Comma-separated list of research IDs (cf AGE3) to disable for this player.</td>\n"
			"</tr>\n"
			"</tbody>\n"
			"</table/>\n<hr/>\n"
			"<p>This documentation was generated automatically by \n"
			"<a href=\"http://aoe.heavengames.com/dl-php/showfile.php?fileid=2464\">CustomizeAOE</a> - author AOEModder</p>\n"
			"<a href=\"#top\"><b>Back to top</b></a>\n"
			"</body></html>\n";
		return result;
	}

	// Generates a string with all necessary parameters for a given event.
	std::string GenerateEventTriggerString(TRIGGER_EVENT_TYPES eventId, bool HTMLFormat) {
		CR_TRIGGERS::TRIGGER_EVENT_DEF *event = allTriggerDef.GetEvent(eventId);
		if (!event) { return ""; }
		std::string result = "";
		if (HTMLFormat) {
			result += "<b>";
		}
		result += GetTriggerParamKeyword(KW_EVENT_TYPE);
		result += "=";
		if (HTMLFormat) {
			result += "</b>";
		}
		result += CR_TRIGGERS::TriggerEventTypeToText(eventId);
		std::vector<TRIGGER_KWID_PARAMS>::const_iterator it;
		for (it = event->eventParameters.begin(); it != event->eventParameters.end(); it++) {
			TRIGGER_KWID_PARAMS p = (*it);
			result += "\r\n";
			if (HTMLFormat) {
				result += "<b>";
			}
			result += allTriggerDef.GetParam(*it)->keyword;
			if (HTMLFormat) {
				result += "</b>";
			}
			result += "=(";
			result += TRIGGER_PARAM_TYPE_STRING[allTriggerDef.GetParam(*it)->paramType];
			result += ")";
		}
		return result;
	}

	// Generates a string with all necessary parameters for a given action.
	std::string GenerateActionTriggerString(TRIGGER_ACTION_TYPES actionId, bool HTMLFormat) {
		CR_TRIGGERS::TRIGGER_ACTION_DEF *action = allTriggerDef.GetAction(actionId);
		std::string result = "";
		if (HTMLFormat) {
			result += "<b>";
		}
		result += GetTriggerParamKeyword(KW_ACTION_TYPE);
		if (HTMLFormat) {
			result += "</b>";
		}
		result += "=";
		result += CR_TRIGGERS::TriggerActionTypeToText(actionId);
		if (!action) { return ""; }
		std::vector<TRIGGER_KWID_PARAMS>::const_iterator it;
		for (it = action->actionParameters.begin(); it != action->actionParameters.end(); it++) {
			TRIGGER_KWID_PARAMS p = (*it);
			result += "\r\n";
			if (HTMLFormat) {
				result += "<b>";
			}
			result += allTriggerDef.GetParam(*it)->keyword;
			if (HTMLFormat) {
				result += "</b>";
			}
			result += "=(";
			result += TRIGGER_PARAM_TYPE_STRING[allTriggerDef.GetParam(*it)->paramType];
			result += ")";
		}
		return result;
	}
}


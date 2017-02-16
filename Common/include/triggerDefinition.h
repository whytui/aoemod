#pragma once

#include <vector>
#include <string>
#include <assert.h>

// This files provides object definitions for custom triggers:
// Specify internal constants and keywords that will be used in trigger text format.
// Remark: it is better to use CRLF (\r\n) instead of \n because ROR's textarea component does not support \n in edit mode.



// Note for myself : http://aok.heavengames.com/university/design/trigger-tutorial/


// A limitation for all strings + line length concerning trigger files.
#define TRIGGER_FILE_LINE_MAX_LENGTH 100


namespace CR_TRIGGERS {

	// Trigger events (WHAT can trigger an event/action)
	enum TRIGGER_EVENT_TYPES : long int {
		EVENT_NONE = 0,
		EVENT_FROM_OTHER_TRIGGER, // A "joker" type that is triggered by no event but other triggers' "before/after" dependencies.
		EVENT_GAME_START,
		EVENT_TIMER,
		EVENT_UNIT_LOSS, // Killed or converted
		EVENT_RESEARCH_COMPLETED,
		EVENT_DISCOVER_GAIA_UNIT,
		EVENT_RESOURCE_VALUE_MORE_THAN,
		EVENT_RESOURCE_VALUE_LESS_THAN
		,
		TRIGGER_EVENTS_TYPES_COUNT
	};


	// Trigger action types (what can be DONE)
	enum TRIGGER_ACTION_TYPES : long int {
		TYPE_NONE = 0,
		TYPE_WRITE_CHAT,
		TYPE_WRITE_CENTERED_MESSAGE,
		TYPE_DIPL_CHANGE,
		TYPE_ADD_RESOURCE,
		TYPE_SET_RESOURCE,
		TYPE_ENABLE_RESEARCH,
		TYPE_DISABLE_RESEARCH,
		TYPE_APPLY_RESEARCH,
		TYPE_APPLY_TECH, // Warning: better use TYPE_APPLY_RESEARCH when possible.
		TYPE_DISABLE_TRIGGER, // Disable another trigger. Useful to stop repeatable timers.
		TYPE_SET_TERRAIN, // Change terrainID & elevation in a zone minXY->maxXY.
		TYPE_MODIFY_UNIT, // Change some unit properties (HP...)
		TYPE_ADD_UNIT_INSTANCE,
		TYPE_ADD_UNIT_DEF,
		TYPE_MODIFY_UNIT_DEF,
		TYPE_MAKE_UNIQUE, // Make a unit unique, with (possibly) custom name, attributes...
		TYPE_SET_UNIT_TARGET,
		TYPE_SET_UNIT_DESTINATION,
		TYPE_SET_GAIA_CAPTURABLE, // Make gaia units capturable (or not)
		TYPE_SET_VIEWED_PLAYER // Change human-controlled player value, but *not* AI control ON/OFF
		,
		TRIGGER_ACTION_TYPES_COUNT
	};


	// Trigger files syntax : all special keywords IDs (other than xxx=yyy)
	enum TRIGGER_KWID_SPECIAL : long int {
		KW_SPECIAL_NONE = 0, KW_HEADER, KW_END_LINE, KW_TRIGGER_SECTION,
		KW_SPECIAL_DISABLE_UNITDEFID_LIST_1, KW_SPECIAL_DISABLE_UNITDEFID_LIST_2, 
		KW_SPECIAL_DISABLE_UNITDEFID_LIST_3, KW_SPECIAL_DISABLE_UNITDEFID_LIST_4,
		KW_SPECIAL_DISABLE_UNITDEFID_LIST_5, KW_SPECIAL_DISABLE_UNITDEFID_LIST_6,
		KW_SPECIAL_DISABLE_UNITDEFID_LIST_7, KW_SPECIAL_DISABLE_UNITDEFID_LIST_8,
		KW_SPECIAL_DISABLE_RESEARCHES_LIST_1, KW_SPECIAL_DISABLE_RESEARCHES_LIST_2,
		KW_SPECIAL_DISABLE_RESEARCHES_LIST_3, KW_SPECIAL_DISABLE_RESEARCHES_LIST_4,
		KW_SPECIAL_DISABLE_RESEARCHES_LIST_5, KW_SPECIAL_DISABLE_RESEARCHES_LIST_6,
		KW_SPECIAL_DISABLE_RESEARCHES_LIST_7, KW_SPECIAL_DISABLE_RESEARCHES_LIST_8,
		KW_SPECIAL_COUNT // Leave this in last position
	};

	static const char *KEYWORDS_SPECIAL[] = {
		// Special keywords
		"", "//RockNRor_Trigger_file\r\n//Please do NOT remove the END line.\r\nEND\r\n", "END\r\n", "TRIGGER",
		"disableUnitDefinitions_p1", "disableUnitDefinitions_p2",
		"disableUnitDefinitions_p3", "disableUnitDefinitions_p4",
		"disableUnitDefinitions_p5", "disableUnitDefinitions_p6",
		"disableUnitDefinitions_p7", "disableUnitDefinitions_p8",
		"disableResearches_p1", "disableResearches_p2",
		"disableResearches_p3", "disableResearches_p4",
		"disableResearches_p5", "disableResearches_p6",
		"disableResearches_p7", "disableResearches_p8"
	};
	static_assert((sizeof(KEYWORDS_SPECIAL) / sizeof(char*) ) == KW_SPECIAL_COUNT, "special keywords arrays consistency");

	// Trigger files syntax : param keywords IDs (params are like xxx in xxx=yyy)
	// Please refer to .cpp file to update corresponding setup.
	enum TRIGGER_KWID_PARAMS : long int {
		// Special keywords
		KW_NONE, KW_NAME /*trigger name*/, KW_RUN_BEFORE, KW_RUN_AFTER,
		// Event keywords
		KW_EVENT_TYPE, KW_DIFF_LEVEL, KW_TIMER_VALUE, KW_TIMER_INTERVAL, KW_EVENT_UNIT_ID, KW_EVENT_PLAYER_ID, KW_EVENT_RESEARCH_ID,
		KW_EVENT_RESOURCE_ID, KW_EVENT_RESOURCE_VALUE,
		// Action keywords
		KW_ACTION_TYPE, KW_MESSAGE, KW_ACTION_PLAYER_ID, KW_ACTION_TARGET_PLAYER_ID, KW_DIPLOMACY_VALUE, KW_RESOURCE_ID,
		KW_RESOURCE_VALUE, KW_ACTION_UNIT_ID, KW_ACTION_RESEARCH_ID, KW_TECHNOLOGY_ID, KW_ACTION_OBJ_PROPERTY, KW_ACTION_OBJ_PROPERTY_VALUE,
		KW_TARGET_TRIGGER_NAME, KW_POS_X, KW_POS_Y, KW_MIN_POS_X, KW_MIN_POS_Y, KW_MAX_POS_X, KW_MAX_POS_Y, KW_ELEVATION_LEVEL,
		KW_TERRAIN_ID, KW_ADD_HP, KW_SET_HP, KW_ADD_RESOURCE, KW_SET_RESOURCE, KW_ACTION_UNIT_DEF_ID,
		KW_UNIT_GRAPHICS_AGE, KW_UNIT_ORIENTATION_INDEX,
		KW_FROM_UNIT_DEF_ID, KW_UNIT_NAME, KW_TOTAL_HP, KW_RANGE, KW_SPEED, KW_ROTATION_SPEED, KW_WORK_RATE, KW_ACCURACY_PERCENT,
		KW_DISPLAYED_ARMOR, KW_DISPLAYED_ATTACK, KW_VISIBLE_IN_FOG,
		KW_OWNER_PLAYER_ID, KW_ACTION_TARGET_UNIT_ID, KW_ENABLE, KW_MUTUAL,
		KW_USE_INITIAL_BUILDING_STATUS,

		KW_PARAMS_COUNT // Leave this in last position
	};



	
	// --------------------------------------------------------------------------------------------------
	// When adding triggers/parameters, you generally don't need to modify the code below.
	// --------------------------------------------------------------------------------------------------




	// Warning, to limit memory usage, this is an union (we always only need one of the members)
	union U_TRIGGER_PARAM_VALUE {
		char *text = NULL;
		long int number_int;
		float number_float;
	};
	// Compiler bug: Calling GetParamUnionValue<typeVar> (providing the type to use) does not work and it raises a stupid compilation error.
	// So we add useless argument foo as a workaround to force type.
	// Get the value for relevant type from parameter value (union) object. You need to force usage of relevant type: GetParamUnionValue<long|float|char*>(...)
	template<typename T> typename long int GetParamUnionValue(const U_TRIGGER_PARAM_VALUE *u, long int foo) { return u->number_int; }
	// Get the value for relevant type from parameter value (union) object. You need to force usage of relevant type: GetParamUnionValue<long|float|char*>(...)
	template<typename T> typename float GetParamUnionValue(const U_TRIGGER_PARAM_VALUE *u, float foo) { return u->number_float; }
	// Get the value for relevant type from parameter value (union) object. You need to force usage of relevant type: GetParamUnionValue<long|float|char*>(...)
	template<typename T> typename char* GetParamUnionValue(const U_TRIGGER_PARAM_VALUE *u, char *foo) { return u->text; }
	// Set the value for relevant type from parameter value (union) object.
	template<typename T> typename void SetParamUnionValue(U_TRIGGER_PARAM_VALUE *u, typename long int value) { u->number_int = value; }
	template<typename T> typename void SetParamUnionValue(U_TRIGGER_PARAM_VALUE *u, typename short int value) { u->number_int = value; }
	// Set the value for relevant type from parameter value (union) object.
	template<typename T> typename void SetParamUnionValue(U_TRIGGER_PARAM_VALUE *u, typename float value) { u->number_float = value; }
	// Set the value for relevant type from parameter value (union) object. Never call this if u contains data that is not relevant for "text" member (free is called on it).
	template<typename T> typename void SetParamUnionValue(U_TRIGGER_PARAM_VALUE *u, typename char *value) {
		if (u->text) { free(u->text); }
		if (!value) {
			u->text = NULL;
			return;
		}
		size_t sz = strlen(value) + 1; // +1 to include ending \0
		char *newValue = (char*)malloc(sz);
		strcpy_s(newValue, sz, value);
		u->text = newValue;
	}

	// Describes the possible data types in trigger parameters
	enum TRIGGER_PARAM_TYPE {
		TPT_NONE = 0,
		TPT_STRING,
		TPT_INTEGER,
		TPT_FLOAT,
		TPT_COUNT
	};
	const static char *TRIGGER_PARAM_TYPE_STRING[] {
		"", "String", "Integer", "Float"
	};


	// Describes an trigger parameter. Each trigger event/action can contain 0 or n parameters.
	class TRIGGER_PARAM_DEF {
	public:
		TRIGGER_PARAM_DEF() {
			this->kwIndex = KW_NONE;
			this->paramType = TRIGGER_PARAM_TYPE::TPT_NONE;
			this->keyword = "";
			this->comment = "";
		}
		TRIGGER_PARAM_DEF(TRIGGER_KWID_PARAMS kwIndex, TRIGGER_PARAM_TYPE paramType, char *keyword, char *comment = "") {
			this->kwIndex = kwIndex;
			if ((paramType < 0) || (paramType >= TRIGGER_PARAM_TYPE::TPT_COUNT)) { paramType = TRIGGER_PARAM_TYPE::TPT_NONE; }
			this->paramType = paramType;
			this->keyword = keyword;
			this->comment = comment;
		}
		TRIGGER_KWID_PARAMS kwIndex;
		TRIGGER_PARAM_TYPE paramType;
		char *keyword;
		char *comment;
		// +Default value as string?
	};


	// Describes an event an its possible parameters.
	class TRIGGER_EVENT_DEF {
	public:
		TRIGGER_EVENT_DEF() {
			this->eventType = EVENT_NONE;
			this->keyword = "";
			this->description = "";
		}
		TRIGGER_EVENT_DEF(TRIGGER_EVENT_TYPES eventType, char *keyword, std::vector<TRIGGER_KWID_PARAMS> eventParameters, char *description) {
			this->eventType = eventType;
			this->eventParameters = eventParameters;
			if (!description) { description = ""; }
			if (!keyword) { keyword = ""; }
			this->keyword = keyword;
			this->description = description;
		}
		TRIGGER_EVENT_TYPES eventType;
		std::vector<TRIGGER_KWID_PARAMS> eventParameters;
		char *keyword;
		char *description;

		bool UsesParameter(TRIGGER_KWID_PARAMS paramId) {
			for (std::vector<TRIGGER_KWID_PARAMS>::iterator it = this->eventParameters.begin();
				it != this->eventParameters.end(); it++) {
				if (paramId == *it) {
					return true;
				}
			}
			return false;
		}
	};

	// Describes an event an its possible parameters.
	class TRIGGER_ACTION_DEF {
	public:
		TRIGGER_ACTION_DEF() {
			this->actionType = TRIGGER_ACTION_TYPES::TYPE_NONE;
			this->keyword = "";
			this->description = "";
		}
		TRIGGER_ACTION_DEF(TRIGGER_ACTION_TYPES actionType, char *keyword, std::vector<TRIGGER_KWID_PARAMS> actionParameters, char *description = "") {
			this->actionType = actionType;
			if (!description) { description = ""; }
			if (!keyword) { keyword = ""; }
			this->keyword = keyword;
			this->actionParameters = actionParameters;
			this->description = description;
		}
		TRIGGER_ACTION_TYPES actionType;
		std::vector<TRIGGER_KWID_PARAMS> actionParameters;
		char *keyword;
		char *description;

		bool UsesParameter(TRIGGER_KWID_PARAMS paramId) {
			for (std::vector<TRIGGER_KWID_PARAMS>::iterator it = this->actionParameters.begin();
				it != this->actionParameters.end(); it++) {
				if (paramId == *it) {
					return true;
				}
			}
			return false;
		}
	};


	class ALL_TRIGGER_DEF {
	public:
		ALL_TRIGGER_DEF(){};
		int GetEventsCount() const { return TRIGGER_EVENTS_TYPES_COUNT; }
		int GetActionsCount() const { return TRIGGER_ACTION_TYPES_COUNT; }
		int GetParamsCount() const { return KW_PARAMS_COUNT; }
		// Returns NULL if invalid.
		TRIGGER_EVENT_DEF *GetEventOrNULL(TRIGGER_EVENT_TYPES eventId) const;
		// Returns "NONE" object if invalid.
		TRIGGER_EVENT_DEF *GetEvent(TRIGGER_EVENT_TYPES eventId) const;
		// Returns NULL if invalid.
		TRIGGER_ACTION_DEF *GetActionOrNULL(TRIGGER_ACTION_TYPES actionId) const;
		// Returns "NONE" object if invalid.
		TRIGGER_ACTION_DEF *GetAction(TRIGGER_ACTION_TYPES actionId) const;
		// Returns NULL if invalid.
		TRIGGER_PARAM_DEF *GetParamOrNULL(TRIGGER_KWID_PARAMS paramId) const;
		// Returns "NONE" object if invalid.
		TRIGGER_PARAM_DEF *GetParam(TRIGGER_KWID_PARAMS paramId) const;
		// Returns true if value type matches parameter definition (value type).
		template<typename T> bool CheckValueType(TRIGGER_KWID_PARAMS paramId, typename long int value) {
			TRIGGER_PARAM_DEF *p = this->GetParam(paramId);
			if (!p) { return false; }
			return (p->paramType == TRIGGER_PARAM_TYPE::TPT_INTEGER);
		}
		template<typename T> bool CheckValueType(TRIGGER_KWID_PARAMS paramId, typename short int value) {
			return CheckValueType<long int>(paramId, (long int)value);
		}
		// Returns true if value type matches parameter definition (value type).
		template<typename T> bool CheckValueType(TRIGGER_KWID_PARAMS paramId, typename float value) {
			TRIGGER_PARAM_DEF *p = this->GetParam(paramId);
			if (!p) { return false; }
			return (p->paramType == TRIGGER_PARAM_TYPE::TPT_FLOAT);
		}
		// Returns true if value type matches parameter definition (value type).
		template<typename T> bool CheckValueType(TRIGGER_KWID_PARAMS paramId, typename char *value) {
			TRIGGER_PARAM_DEF *p = this->GetParam(paramId);
			if (!p) { return false; }
			return (p->paramType == TRIGGER_PARAM_TYPE::TPT_STRING);
		}
	private:
		// Note: do not explicitely provide array sizes => useful for static checks
		static TRIGGER_EVENT_DEF events[];
		static TRIGGER_ACTION_DEF actions[];
		static TRIGGER_PARAM_DEF all_params[];

		void static_check();
	};


	// Define it only once (in .cpp file) so all files will use the same instance.
	extern ALL_TRIGGER_DEF allTriggerDef;


	// Contains necessary information to describe events context
	// This class is used when an event occurs: we store information about event in this. All members are not used, depending on event type.
	class EVENT_INFO_FOR_TRIGGER {
	public:
		EVENT_INFO_FOR_TRIGGER() { memset(this, -1, sizeof(EVENT_INFO_FOR_TRIGGER)); }
		long int difficultyLevel; // Always needs to be set.
		long int currentGameTime_s; // In seconds. Always needs to be set.
		// Other members are optional (according to event type). We must always only access members that are relevant for current event type.
		long int playerId;
		long int unitId;
		long int unitDefId;
		long int researchId;
		long int posX, posY;
		long int resourceId;
		float resourceValue;
	};



	// ---------------------------------------------------------------------------------------------------
	// Methods
	// This section generally does not need to be modified when adding new triggers/events/actions.

	// Returns (char*) string that corresponds to provided action type.
	const char *TriggerActionTypeToText(TRIGGER_ACTION_TYPES value);
	// Returns (char*) string that corresponds to provided event type.
	const char *TriggerEventTypeToText(TRIGGER_EVENT_TYPES value);
	// Returns (char*) string that corresponds to provided event type.
	const char *TriggerParamKeywordIdToText(TRIGGER_KWID_PARAMS paramId);

	// Returns the TRIGGER_ACTION_TYPES that matches provided text. Returns TYPE_NONE if not found.
	TRIGGER_ACTION_TYPES GetTriggerActionTypeFromText(char *value);
	// Returns the TRIGGER_EVENT_TYPES that matches provided text. Returns EVENT_NONE if not found.
	TRIGGER_EVENT_TYPES GetTriggerEventTypeFromText(char *value);
	// Returns the TRIGGER_KWID_PARAMS that matches provided keyword. Returns KW_NONE if not found.
	TRIGGER_KWID_PARAMS GetTriggerParamKeywordIdFromText(char *value);
	

	// Returns the (char*) "special" keyword that corresponds to provided index.
	// Returns "" if index is not in bounds.
	const char *GetTriggerSpecialKeyword(TRIGGER_KWID_SPECIAL kwIndex);
	// Returns the (char*) "param" keyword that corresponds to provided index.
	// Returns "" if index is not in bounds.
	const char *GetTriggerParamKeyword(TRIGGER_KWID_PARAMS kwIndex);


	// Useful to get a "property=value\r\n" string.
	static std::string GetPropertyLine(char *propertyName, std::string value) {
		std::string result = propertyName;
		result += "=";
		result += value;
		result += "\r\n";
		return result;
	}
	static std::string GetPropertyLine(char *propertyName, const char *value) {
		return GetPropertyLine(propertyName, std::string(value));
	}
	static std::string GetPropertyLine(char *propertyName, char *value) {
		return GetPropertyLine(propertyName, std::string(value));
	}
	// Overload for numeric types
	template<typename valueType> static std::string GetPropertyLine(char *propertyName, valueType value) {
		std::string result = propertyName;
		result += "=";
		result += std::to_string(value);
		result += "\r\n";
		return result;
	}
	// Overload for const char*
	template<typename valueType> static std::string GetPropertyLine(const char *propertyName, valueType value) {
		return GetPropertyLine((char*)propertyName, value);
	}
	// Overload to use directly with keyword list enum
	template<typename valueType> static std::string GetPropertyLine(TRIGGER_KWID_PARAMS kwIndex, valueType value) {
		return GetPropertyLine(GetTriggerParamKeyword(kwIndex), value);
	}

	// Returns true if the text corresponding to a keyword (given its index) is equals to provided string (as a char*)
	// The function is case-sensitive
	bool SpecialKeyword_equals(char *value, TRIGGER_KWID_PARAMS kwIndex);
	bool SpecialKeyword_equals(char *value, TRIGGER_KWID_SPECIAL kwIndex);
	// Get a keyword length
	int SpecialKeyword_getLength(TRIGGER_KWID_PARAMS kwIndex);
	int SpecialKeyword_getLength(TRIGGER_KWID_SPECIAL kwIndex);

	// Generate a HTML-formatted string corresponding to a list of parameters.
	std::string ParamListToString(std::vector<TRIGGER_KWID_PARAMS> v);

	// Exports triggers setup documentation to HTML-formatted string
	std::string TriggersDefinitionToHTML();

	// Generates a string with all necessary parameters for a given event.
	std::string GenerateEventTriggerString(TRIGGER_EVENT_TYPES eventId, bool HTMLFormat = false);

	// Generates a string with all necessary parameters for a given action.
	std::string GenerateActionTriggerString(TRIGGER_ACTION_TYPES actionId, bool HTMLFormat = false);



	// Returns true if supplied player ID is acceptable for trigger parameters.
	// Note: some trigger params accept playerId==-1 (as a joker) so we allow -1 here.
	static bool IsPlayerIdValidForTriggerParam(long int playerId) {
		return ((playerId >= -1) && (playerId < 9));
	}
	// Returns true if supplied player ID is acceptable for trigger parameters.
	// Joker is not allowed here (-1 is NOT a valid playerId)
	static bool IsPlayerIdValidForTriggerParam_noJoker(long int playerId) {
		return ((playerId >= 0) && (playerId < 9));
	}

	// Returns true if supplied coordinate (X or Y) is valid for trigger params definition (0-255)
	static bool IsPositionValueValidForTriggerParam(long int pos) {
		return ((pos >= 0) && (pos < 255));
	}
	// Returns true if supplied coordinate (X or Y) is valid for trigger params definition (0-255)
	static bool IsPositionValueValidForTriggerParam(float pos) {
		return ((pos >= 0) && (pos < 255));
	}
}


#include "../include/crTrigger.h"

using namespace CR_TRIGGERS;


crTrigger::crTrigger() {
	this->Reset();
}

crTrigger::crTrigger(std::string name) {
	this->Reset();
	this->triggerName = name;
}

crTrigger::~crTrigger() {
	this->RemoveAllParameters(); // To free all map underlying objects
}


void crTrigger::Reset() {
	this->triggerName = "";
	this->enabled = true;
	this->triggerActionType = TYPE_NONE;
	this->triggerEventType = EVENT_NONE;
	this->difficultyLevel = -1;
	this->triggerNameToRunBefore = "";
	this->triggerNameToRunAfter = "";
}


// Returns true if trigger data is valid/consistent/complete and can be used properly.
bool crTrigger::IsValid() const {
	if (this->triggerName.empty()) {
		return false;
	}
	std::string missingKeywordMsg = "A keyword is missing or invalid for trigger ";
	missingKeywordMsg += "\"" + this->triggerName + "\": ";

	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_NONE) {
		missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_TYPE);
		traceMessageHandler.WriteMessage(missingKeywordMsg);
		return false;
	}
	if (this->triggerEventType == TRIGGER_EVENT_TYPES::EVENT_NONE) {
		missingKeywordMsg += GetTriggerParamKeyword(KW_EVENT_TYPE);
		traceMessageHandler.WriteMessage(missingKeywordMsg);
		return false;
	}

	CR_TRIGGERS::TRIGGER_EVENT_DEF *eventDef = allTriggerDef.GetEvent(this->triggerEventType);
	CR_TRIGGERS::TRIGGER_ACTION_DEF *actionDef = allTriggerDef.GetAction(this->triggerActionType);
	bool requiresAllEventKeywords = true; // Set this to false for triggers that have optional event keywords.
	bool requiresAllActionKeywords = true; // Set this to false for triggers that have optional action keywords.

	// Start with specific checks
	// Checks on Event parameters
	if (this->triggerEventType == TRIGGER_EVENT_TYPES::EVENT_TIMER) {
		if (this->GetParameterValue(KW_TIMER_VALUE, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_TIMER_VALUE);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		requiresAllEventKeywords = false; // interval is optional
	}
	if (this->triggerEventType == TRIGGER_EVENT_TYPES::EVENT_UNIT_LOSS) {
		if (this->GetParameterValue(KW_EVENT_UNIT_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_EVENT_UNIT_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerEventType == TRIGGER_EVENT_TYPES::EVENT_RESEARCH_COMPLETED) {
		if (this->GetParameterValue(KW_EVENT_RESEARCH_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_EVENT_RESEARCH_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_EVENT_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_EVENT_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerEventType == TRIGGER_EVENT_TYPES::EVENT_DISCOVER_GAIA_UNIT) {
		if (this->GetParameterValue(KW_EVENT_UNIT_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_EVENT_UNIT_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_EVENT_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_EVENT_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if ((this->triggerEventType == TRIGGER_EVENT_TYPES::EVENT_RESOURCE_VALUE_MORE_THAN) ||
		(this->triggerEventType == TRIGGER_EVENT_TYPES::EVENT_RESOURCE_VALUE_LESS_THAN)){
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_EVENT_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_EVENT_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (this->GetParameterValue(KW_RESOURCE_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_RESOURCE_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!this->IsParameterDefined(KW_RESOURCE_VALUE)) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_RESOURCE_VALUE);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	// Checks on Action parameters
	if ((this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_WRITE_CHAT) || (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_WRITE_CENTERED_MESSAGE)) {
		if (!this->IsParameterDefined(KW_MESSAGE)) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_MESSAGE);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if ((this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_ADD_RESOURCE) || (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_SET_RESOURCE)) {
		if (this->GetParameterValue(KW_RESOURCE_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_RESOURCE_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!this->IsParameterDefined(KW_RESOURCE_VALUE)) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_RESOURCE_VALUE);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_ACTION_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if ((this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_ENABLE_RESEARCH) || (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_DISABLE_RESEARCH) ||
		(this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_APPLY_RESEARCH)) {
		if (this->GetParameterValue(KW_ACTION_RESEARCH_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_RESEARCH_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_ACTION_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_APPLY_TECH) {
		if (this->GetParameterValue(KW_TECHNOLOGY_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_TECHNOLOGY_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		// Note apply tech allows joker for playerid
		if (!IsPlayerIdValidForTriggerParam(this->GetParameterValue(KW_ACTION_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_DIPL_CHANGE) {
		if (this->GetParameterValue(KW_DIPLOMACY_VALUE, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_DIPLOMACY_VALUE);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_ACTION_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_ACTION_TARGET_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_TARGET_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		// KW_MUTUAL is optional.
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_DISABLE_TRIGGER) {
		if (!this->IsParameterDefined(KW_TARGET_TRIGGER_NAME)) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_TARGET_TRIGGER_NAME);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_SET_TERRAIN) {
		if ((!IsPositionValueValidForTriggerParam(this->GetParameterValue(KW_MIN_POS_X, -1))) ||
			(!IsPositionValueValidForTriggerParam(this->GetParameterValue(KW_MAX_POS_X, -1))) ||
			(!IsPositionValueValidForTriggerParam(this->GetParameterValue(KW_MIN_POS_Y, -1))) ||
			(!IsPositionValueValidForTriggerParam(this->GetParameterValue(KW_MAX_POS_Y, -1)))
			) {
			return false;
		}
		// Only one among terrain and elevation is required
		if ((this->GetParameterValue(KW_TERRAIN_ID, -1) < 0) && (this->GetParameterValue(KW_ELEVATION_LEVEL, -1) < 0)) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_TERRAIN_ID);
			missingKeywordMsg += std::string(" or ") + GetTriggerParamKeyword(KW_ELEVATION_LEVEL);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		requiresAllActionKeywords = false;
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_MODIFY_UNIT) {
		if (this->GetParameterValue(KW_ACTION_UNIT_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_UNIT_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		// Check at least 1 modification attribute is set => damn it, it is not necessary, we'll see at runtime. Worst case, it juste does nothing.
		/*if ((this->GetParameterValue<float>(KW_ADD_HP, 0.f) == 0) &&
			(this->GetParameterValue<float>(KW_ADD_RESOURCE, 0.f) == 0) &&
			(this->GetParameterValue<float>(KW_SET_HP, -2.f) == -2) && // we're prudent in case one day -1 has a special meaning or if users want to use it
			(this->GetParameterValue<float>(KW_SET_RESOURCE, -2.f) == -2) && // same thing here
			(this->GetParameterValue(KW_UNIT_GRAPHICS_AGE, -1) == -1) &&
			(this->GetParameterValue(KW_UNIT_ORIENTATION_INDEX, -1) == -1) &&
			(this->GetParameterValue<float>(KW_POS_X, -1.f) == -1.f) &&
			(this->GetParameterValue<float>(KW_POS_Y, -1.f) == -1.f)
			// + new owner, etc
			) {
			return false;
		}*/
		requiresAllActionKeywords = false; // we don't need all associated keywords, eg. user can set HP and not resource.
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_INSTANCE) {
		if (!IsPositionValueValidForTriggerParam(this->GetParameterValue<float>(KW_POS_X, -2.f))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_POS_X);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPositionValueValidForTriggerParam(this->GetParameterValue<float>(KW_POS_Y, -2.f))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_POS_Y);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_ACTION_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		// Only 1 of UnitDefId and unit_name is required. (unit name is used for trigger-generated unit defs).
		if ((this->GetParameterValue(KW_ACTION_UNIT_DEF_ID, -1) < 0) && (!this->IsParameterDefined(KW_UNIT_NAME))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_UNIT_DEF_ID);
			missingKeywordMsg += std::string(" or ") + GetTriggerParamKeyword(KW_UNIT_NAME);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false; // None of the 2 parameters is defined/valid
		}
		requiresAllActionKeywords = false;
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_DEF) {
		// Check only key values: unitdef id to copy
		if (this->GetParameterValue(KW_FROM_UNIT_DEF_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_FROM_UNIT_DEF_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		// No check on other values. They all are optional.
		requiresAllActionKeywords = false;
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_MODIFY_UNIT_DEF) {
		// Check only key values: unitdef id to modify and player ID
		if (!IsPlayerIdValidForTriggerParam(this->GetParameterValue(KW_ACTION_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (this->GetParameterValue(KW_ACTION_UNIT_DEF_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_UNIT_DEF_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		// No check on other values. They all are optional.
		requiresAllActionKeywords = false;
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_MAKE_UNIQUE) {
		// Check only key values: identify the unit to modify and give it a new name.
		if (!this->IsParameterDefined(KW_ACTION_UNIT_ID)) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_UNIT_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		// No check on other values. They all are optional.
		requiresAllActionKeywords = false;
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_SET_UNIT_TARGET) {
		if (this->GetParameterValue(KW_ACTION_UNIT_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_UNIT_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false; 
		}
		if (this->GetParameterValue(KW_ACTION_TARGET_UNIT_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_TARGET_UNIT_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_SET_UNIT_DESTINATION) {
		if (this->GetParameterValue(KW_ACTION_UNIT_ID, -1) < 0) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_UNIT_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPositionValueValidForTriggerParam(this->GetParameterValue<float>(KW_POS_X, -2.f))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_POS_X);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
		if (!IsPositionValueValidForTriggerParam(this->GetParameterValue<float>(KW_POS_Y, -2.f))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_POS_Y);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_SET_GAIA_CAPTURABLE) {
		long int v_enable = this->GetParameterValue(KW_ENABLE, -1);
		if ((v_enable < 0) || (v_enable > 1)) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ENABLE);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}
	if (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_SET_VIEWED_PLAYER) {
		// Check only key values: identify the unit to duplicate and give it a new name.
		if (!IsPlayerIdValidForTriggerParam_noJoker(this->GetParameterValue(KW_ACTION_PLAYER_ID, -2))) {
			missingKeywordMsg += GetTriggerParamKeyword(KW_ACTION_PLAYER_ID);
			traceMessageHandler.WriteMessage(missingKeywordMsg);
			return false;
		}
	}


	// -------------------------
	// End of specific/custom checks. Now run generic tests.
	// You can use requiresAllEventKeywords and requiresAllActionKeywords to disable the checks below.

	// Default check: test if all event/action parameters are set.
	std::vector<TRIGGER_KWID_PARAMS> expectedParams = eventDef->eventParameters;
	if (requiresAllEventKeywords) {
		// This loop returns false if 1 parameter (from event definition) has NOT been defined.
		for (std::vector<TRIGGER_KWID_PARAMS>::iterator it = expectedParams.begin();
			it != expectedParams.end(); it++) {
			if (!this->IsParameterDefined(*it)) {
				missingKeywordMsg += GetTriggerParamKeyword(*it);
				traceMessageHandler.WriteMessage(missingKeywordMsg);
				return false;
			}
		}
	}
	if (requiresAllActionKeywords) {
		// This loop returns false if 1 parameter (from action definition) has NOT been defined.
		expectedParams = actionDef->actionParameters;
		for (std::vector<TRIGGER_KWID_PARAMS>::iterator it = expectedParams.begin();
			it != expectedParams.end(); it++) {
			if (!this->IsParameterDefined(*it)) {
				missingKeywordMsg += GetTriggerParamKeyword(*it);
				traceMessageHandler.WriteMessage(missingKeywordMsg);
				return false;
			}
		}
	}
	return true;
}


// Returns true if the trigger can be executed now, according to trigger prerequisites (and event information)
bool crTrigger::CanExecuteNow(TRIGGER_EVENT_TYPES evtType, EVENT_INFO_FOR_TRIGGER &evtInfo) {
	if ((this->triggerEventType != evtType) || (!this->enabled) || (evtType == EVENT_NONE)) { return false; }
	if ((this->difficultyLevel >= 0) && (this->difficultyLevel < evtInfo.difficultyLevel)) {
		// Trigger difficulty level is "harder" than actual difficulty level. (warning: 0=hardest, 4=easiest)
		// We disable the trigger as it will NEVER be OK for execution.
		this->enabled = false;
		return false;
	}
	// Check trigger event types additional specific conditions
	if (evtType == TRIGGER_EVENT_TYPES::EVENT_TIMER) {
		return (evtInfo.currentGameTime_s >= this->GetParameterValue(KW_TIMER_VALUE, -1)); // True if trigger time is reached
	}
	if (evtType == TRIGGER_EVENT_TYPES::EVENT_UNIT_LOSS) {
		return (evtInfo.unitId == this->GetParameterValue(KW_EVENT_UNIT_ID, -1));
	}
	if (evtType == TRIGGER_EVENT_TYPES::EVENT_RESEARCH_COMPLETED) {
		return (evtInfo.playerId == this->GetParameterValue(KW_EVENT_PLAYER_ID, -1)) && (evtInfo.researchId == this->GetParameterValue(KW_EVENT_RESEARCH_ID, -1));
	}
	if (evtType == TRIGGER_EVENT_TYPES::EVENT_DISCOVER_GAIA_UNIT) {
		return (evtInfo.playerId == this->GetParameterValue(KW_EVENT_PLAYER_ID, -1)) && (evtInfo.unitId == this->GetParameterValue(KW_EVENT_UNIT_ID, -1));
	}
	if (evtType == TRIGGER_EVENT_TYPES::EVENT_RESOURCE_VALUE_MORE_THAN) {
		return ( (evtInfo.playerId == this->GetParameterValue(KW_EVENT_PLAYER_ID, -1)) &&
			(evtInfo.resourceId == this->GetParameterValue(KW_EVENT_RESOURCE_ID, -1)) &&
			(evtInfo.resourceValue >= this->GetParameterValue<float>(KW_EVENT_RESOURCE_VALUE, -1))
			);
	}
	if (evtType == TRIGGER_EVENT_TYPES::EVENT_RESOURCE_VALUE_LESS_THAN) {
		return ((evtInfo.playerId == this->GetParameterValue(KW_EVENT_PLAYER_ID, -1)) &&
			(evtInfo.resourceId == this->GetParameterValue(KW_EVENT_RESOURCE_ID, -1)) &&
			(evtInfo.resourceValue <= this->GetParameterValue<float>(KW_EVENT_RESOURCE_VALUE, -1))
			);
	}

	return true; // For events that have no special condition
}


// Read a property text and its value and updates members accordingly.
// Returns true if successful.
bool crTrigger::ApplyTextProperty(char *propertyName, char *value) {
	if (!propertyName || !value) { return false; }

	TRIGGER_KWID_PARAMS paramId = GetTriggerParamKeywordIdFromText(propertyName);

	// Common properties: we always store name, action/event type, before/after triggers in member variables.
	// They are used in all triggers so it's relevant to have them in dedicated variables (and do not add them into params map)

	// Trigger name.
	if (SpecialKeyword_equals(propertyName, KW_NAME)) {
		this->triggerName = value;
		return true;
	}
	// Event type
	if (paramId == KW_EVENT_TYPE) {
		this->triggerEventType = GetTriggerEventTypeFromText(value);
		if (this->triggerEventType == CR_TRIGGERS::EVENT_NONE) { return false; }
		return true;
	}
	// Action type
	if (paramId == KW_ACTION_TYPE) {
		this->triggerActionType = GetTriggerActionTypeFromText(value);
		if (this->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE) { return false; }
		return true;
	}
	// Trigger to run before
	if (paramId == KW_RUN_BEFORE) {
		this->triggerNameToRunBefore = value;
		return true;
	}
	// Trigger to run after
	if (paramId == KW_RUN_AFTER) {
		this->triggerNameToRunAfter = value;
		return true;
	}
	// Difficulty level
	if (paramId == KW_DIFF_LEVEL) {
		this->difficultyLevel = atoi(value);
		return true;
	}

	// Specific controls on player IDs
	if ((paramId == KW_ACTION_PLAYER_ID) || (paramId == KW_ACTION_TARGET_PLAYER_ID) || (paramId == KW_EVENT_PLAYER_ID)) {
		int intValue = atoi(value);
		if (!IsPlayerIdValidForTriggerParam(intValue)) {
			traceMessageHandler.WriteMessage(std::string("Invalid player id: ") + value);
			return false;
		}
	}
	// It is possible to add custom controls here and return false is they fail.

	// Default: add parameter+value in internal map
	this->SetParameterValue_guessType(GetTriggerParamKeywordIdFromText(propertyName), value);

	return true;
}







// -----------------------
// The code below should NOT require to be updated when adding new triggers / trigger properties.
// -----------------------




// Set a parameter value using the relevant type, when value is formatted as a char* (may be containing int, float or text value).
void crTrigger::SetParameterValue_guessType(TRIGGER_KWID_PARAMS parameterIndex, char *valueAsText) {
	switch (allTriggerDef.GetParam(parameterIndex)->paramType) {
	case TRIGGER_PARAM_TYPE::TPT_INTEGER:
		this->SetParameterValue(parameterIndex, atoi(valueAsText));
		break;
	case TRIGGER_PARAM_TYPE::TPT_FLOAT:
		this->SetParameterValue(parameterIndex, (float)atof(valueAsText));
		break;
	case TRIGGER_PARAM_TYPE::TPT_STRING:
		this->SetParameterValue<char *>(parameterIndex, valueAsText);
		break;
	case TRIGGER_PARAM_TYPE::TPT_NONE:
		break;
	}
}

// Returns true if the trigger can be executed more than once (repeatable), like timer with interval
bool crTrigger::IsRepeatable() const {
	return ((this->triggerEventType == CR_TRIGGERS::EVENT_TIMER) && (this->GetParameterValue(KW_TIMER_INTERVAL, -1) >= 0));
}


std::string crTrigger::GetActionTypeName() const {
	return TriggerActionTypeToText(this->triggerActionType);
}


// Returns a xxx=yyy property line for supplied parameter ID. Returns "" if the parameter is not defined.
std::string crTrigger::GetParameterPropertyLine(const TRIGGER_KWID_PARAMS paramIndex) const {
	if (this->IsParameterDefined(paramIndex)) {
		// Note: the default values in GetParameterValue calls below are unused because we tested the parameter IS defined.
		switch (allTriggerDef.GetParam(paramIndex)->paramType) {
		case TRIGGER_PARAM_TYPE::TPT_INTEGER:
			return GetPropertyLine(paramIndex, this->GetParameterValue(paramIndex, 0));
			break;
		case TRIGGER_PARAM_TYPE::TPT_FLOAT:
			return GetPropertyLine<float>(paramIndex, this->GetParameterValue(paramIndex, (float)0));
			break;
		case TRIGGER_PARAM_TYPE::TPT_STRING:
			return GetPropertyLine(paramIndex, this->GetParameterValue(paramIndex, (char*)NULL));
			break;
		default:
			break;
		}
	}
	return "";
}


// Export trigger to formatted string
// Only relevant information (common parameters + parameters that have been set) is written.
std::string crTrigger::ToString() const {
	if ((this->triggerEventType == EVENT_NONE) || (this->triggerActionType == TRIGGER_ACTION_TYPES::TYPE_NONE)) { return ""; }
	std::string result = GetPropertyLine(CR_TRIGGERS::KW_NAME, this->triggerName);

	CR_TRIGGERS::TRIGGER_EVENT_DEF *eventDef = allTriggerDef.GetEvent(this->triggerEventType);
	CR_TRIGGERS::TRIGGER_ACTION_DEF *actionDef = allTriggerDef.GetAction(this->triggerActionType);

	// Event
	result += GetPropertyLine(CR_TRIGGERS::KW_EVENT_TYPE, TriggerEventTypeToText(this->triggerEventType));
	std::vector<TRIGGER_KWID_PARAMS> expectedParams = eventDef->eventParameters;
	for (std::vector<TRIGGER_KWID_PARAMS>::iterator it = expectedParams.begin();
		it != expectedParams.end(); it++) {
		result += this->GetParameterPropertyLine(*it);
	}

	// Action
	result += GetPropertyLine(CR_TRIGGERS::KW_ACTION_TYPE, TriggerActionTypeToText(this->triggerActionType));
	if (!this->triggerNameToRunBefore.empty()) {
		result += GetPropertyLine(CR_TRIGGERS::KW_RUN_BEFORE, this->triggerNameToRunBefore);
	}
	if (!this->triggerNameToRunAfter.empty()) {
		result += GetPropertyLine(CR_TRIGGERS::KW_RUN_AFTER, this->triggerNameToRunAfter);
	}

	expectedParams = actionDef->actionParameters;
	for (std::vector<TRIGGER_KWID_PARAMS>::iterator it = expectedParams.begin();
		it != expectedParams.end(); it++) {
		result += this->GetParameterPropertyLine(*it);
	}
	return result;
}


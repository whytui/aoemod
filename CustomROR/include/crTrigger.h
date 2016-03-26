#pragma once
#include <string>
#include <assert.h>
#include <map>
#include <triggerDefinition.h>
#include "traceMessage.h"

// This files provides object definitions for custom triggers


namespace CR_TRIGGERS {
	// Describes a basic trigger
	class crTrigger {
	public:
		crTrigger();
		crTrigger(std::string name);
		~crTrigger();

		// Members
		std::string triggerName;
		TRIGGER_ACTION_TYPES triggerActionType;
		TRIGGER_EVENT_TYPES triggerEventType;
		std::string triggerNameToRunBefore; // A trigger name to run before executing THIS trigger. Will only be executed if enabled (avoids infinite dependencies).
		std::string triggerNameToRunAfter; // A trigger name to run before executing THIS trigger. Will only be executed if enabled (avoids infinite dependencies).
		long int difficultyLevel; // Condition for event. Can be used for any event type.
		bool enabled;

	private:
		// Parameter type makes us know which member of the union is relevant. Keep private, use API methods to manipulate it.
		std::map<TRIGGER_KWID_PARAMS, U_TRIGGER_PARAM_VALUE*> allProperties;
	public:
		// Returns true if the parameter is relevant for this trigger, false if not.
		// Depends on the trigger event/action
		bool IsParameterRelevant(TRIGGER_KWID_PARAMS parameterIndex) const {
			// Common properties: always OK
			if ((parameterIndex == KW_EVENT_TYPE) || (parameterIndex == KW_ACTION_TYPE) || (parameterIndex == KW_RUN_BEFORE) ||
				(parameterIndex == KW_RUN_AFTER) || (parameterIndex == KW_DIFF_LEVEL) || (parameterIndex == KW_NAME)) {
				return true;
			}
			CR_TRIGGERS::TRIGGER_EVENT_DEF *evDef = CR_TRIGGERS::allTriggerDef.GetEvent(this->triggerEventType);
			CR_TRIGGERS::TRIGGER_ACTION_DEF *evAction = CR_TRIGGERS::allTriggerDef.GetAction(this->triggerActionType);
			return (evDef->UsesParameter(parameterIndex) || (evAction->UsesParameter(parameterIndex)));
		}

		// Get the number of parameters that have been defined.
		long int GetDefinedParametersCount() const { return this->allProperties.size(); }
		// Returns true if a parameter value has been set. False if no value has been supplied.
		bool IsParameterDefined(TRIGGER_KWID_PARAMS parameterIndex) const {
			if (!this->IsParameterRelevant(parameterIndex)) { return false; }
			return (this->allProperties.find(parameterIndex) != this->allProperties.end());
		}
		// Force usage of relevant overload (non-float numeric types=>long int) to avoid compilation errors.
		long int GetParameterValue(TRIGGER_KWID_PARAMS parameterIndex, int defaultValue) const {
			return GetParameterValue<long int>(parameterIndex, (long int)defaultValue);
		}
		float GetParameterValue(TRIGGER_KWID_PARAMS parameterIndex, double defaultValue) const {
			return GetParameterValue<float>(parameterIndex, (float)defaultValue);
		}
		char *GetParameterValue(TRIGGER_KWID_PARAMS parameterIndex, const char *defaultValue) const {
			return GetParameterValue<char*>(parameterIndex, (char*)defaultValue);
		}
		// Get a parameter's value. Returns defaultValue if not found (or invalid).
		// Runs an assert to check datatype consistency with trigger definitions.
		template<typename typeVar> typeVar GetParameterValue(TRIGGER_KWID_PARAMS parameterIndex, typeVar defaultValue) const {
			if (!this->IsParameterRelevant(parameterIndex)) {
				std::string msg = std::string("Used an irrelevant parameter=") + CR_TRIGGERS::GetTriggerParamKeyword(parameterIndex);
				msg += ". Trigger name=";
				msg += this->triggerName;
				traceMessageHandler.WriteMessage(msg);
				return defaultValue;
			}
			TRIGGER_PARAM_DEF *paramDef = allTriggerDef.GetParam(parameterIndex);
			// This check is crucial to identify type mismatches due to config or coding errors.
			bool valid = allTriggerDef.CheckValueType<typeVar>(parameterIndex, defaultValue);
			assert(valid); // Show a warning if runtime datatype does not match parameter definition.
			if (!valid) {
				std::string msg = std::string("Used an invalid data type for parameter=") + CR_TRIGGERS::GetTriggerParamKeyword(parameterIndex);
				msg += ". Trigger name=";
				msg += this->triggerName;
				traceMessageHandler.WriteMessage(msg);
				return defaultValue; // Use default instead of an arbitraty value.
			}
			if (!paramDef || !IsParameterDefined(parameterIndex)) {
				return defaultValue;
			}
			return GetParamUnionValue<typeVar>(this->allProperties.at(parameterIndex), defaultValue /*unused - workaround for compiler bug*/);
		}

		// Set a parameter's value. Overwrite the previous value that has been set (if any).
		// Runs an assert to check datatype consistency with trigger definitions.
		template<typename typeVar> void SetParameterValue(TRIGGER_KWID_PARAMS parameterIndex, typeVar value) {
			if (!this->IsParameterRelevant(parameterIndex)) {
				std::string msg = std::string("Used an irrelevant parameter=") + CR_TRIGGERS::GetTriggerParamKeyword(parameterIndex);
				msg += ". Trigger name=";
				msg += this->triggerName;
				traceMessageHandler.WriteMessage(msg);
				return;
			}
			TRIGGER_PARAM_DEF *paramDef = allTriggerDef.GetParam(parameterIndex);
			// This check is crucial to identify type mismatches due to config or coding errors.
			bool valid = allTriggerDef.CheckValueType<typeVar>(parameterIndex, value);
			assert(valid); // Show a warning if runtime datatype does not match parameter definition.
			if (!valid || !paramDef) { return; }
			U_TRIGGER_PARAM_VALUE *u = new U_TRIGGER_PARAM_VALUE;
			u->text = NULL;
			SetParamUnionValue<typeVar>(u, value);
			this->allProperties[parameterIndex] = u;
		}
		// Force usage of relevant overload (non-float numeric types=>long int) to avoid compilation errors.
		void SetParameterValue(TRIGGER_KWID_PARAMS parameterIndex, int value) { this->SetParameterValue(parameterIndex, (long int)value); }
		// Same for float numeric types (loses precision, but we're not using double precision here).
		//void SetParameterValue(TRIGGER_KWID_PARAMS parameterIndex, double value) { this->SetParameterValue(parameterIndex, (float)value); }

		// Set a parameter value using the relevant type, when value is formatted as a char* (may be containing int, float or text value).
		void SetParameterValue_guessType(TRIGGER_KWID_PARAMS parameterIndex, char *valueAsText);

		// Removes a parameter from internal list. Returns true if a parameter has actually been removed.
		bool RemoveParameterValue(TRIGGER_KWID_PARAMS parameterIndex) {
			if (!this->IsParameterRelevant(parameterIndex)) {
				std::string msg = std::string("Used an irrelevant parameter=") + CR_TRIGGERS::GetTriggerParamKeyword(parameterIndex);
				msg += ". Trigger name=";
				msg += this->triggerName;
				traceMessageHandler.WriteMessage(msg);
				return false;
			}
			U_TRIGGER_PARAM_VALUE *u = this->allProperties[parameterIndex];
			// Strings are copied (allocated), so we have to free them when freeing union.
			if (allTriggerDef.GetParam(parameterIndex)->paramType == TRIGGER_PARAM_TYPE::TPT_STRING) {
				free(u->text);
			}
			delete u;
			this->allProperties.erase(parameterIndex);
			return true;
		}
		// Use this to clear properties map. Please call this in destructor to free all underlying objects !
		void RemoveAllParameters() {
			// First delete all underlying objects
			for (std::map<TRIGGER_KWID_PARAMS, U_TRIGGER_PARAM_VALUE*>::iterator it = this->allProperties.begin();
				it != this->allProperties.end(); it++) {
				U_TRIGGER_PARAM_VALUE *u = it->second;
				// Strings are copied (allocated), so we have to free them when freeing union.
				if (allTriggerDef.GetParam(it->first)->paramType == TRIGGER_PARAM_TYPE::TPT_STRING) {
					free(u->text);
				}
				delete u;
				it->second = NULL;
			}
			this->allProperties.clear(); // Now clear the map.
		}

		void Reset();
		std::string GetActionTypeName() const;

		// Export trigger to formatted string
		// Only relevant information is written.
		std::string ToString() const;
		// Returns true if trigger data is valid/consistent/complete and can be used properly.
		bool IsValid() const;
		// Read a property text and its value and updates members accordingly.
		// Returns true if successful.
		bool ApplyTextProperty(char *propertyName, char *value);
		// Returns true if the trigger can be executed now, according trigger prerequisites (and event information)
		bool CanExecuteNow(TRIGGER_EVENT_TYPES evtType, EVENT_INFO_FOR_TRIGGER &evtInfo);
		// Returns true if the trigger can be executed more than once (repeatable), like timer with interval
		bool IsRepeatable() const;

	private:
		// Returns a xxx=yyy property line for supplied parameter ID. Returns "" if the parameter is not defined.
		std::string GetParameterPropertyLine(const TRIGGER_KWID_PARAMS paramIndex) const;
	};

}


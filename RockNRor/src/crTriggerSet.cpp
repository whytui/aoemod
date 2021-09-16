#include "../include/crTriggerSet.h"

using namespace CR_TRIGGERS;


crTriggerListElem::crTriggerListElem() {
	this->next = NULL;
	this->previous = NULL;
	this->trigger = NULL;
}

crTriggerListElem::crTriggerListElem(crTrigger *trigger) {
	this->next = NULL;
	this->previous = NULL;
	this->trigger = trigger;
}


crTriggerListElem::~crTriggerListElem() {
	if (this->trigger) {
		delete this->trigger;
	}
	if (this->previous) {
		this->previous->next = this->next;
	}
	if (this->next) {
		this->next->previous = this->previous;
	}
}


crTriggerListElem *crTriggerListElem::AddAfter(crTrigger *trigger) {
	crTriggerListElem *newElem = new crTriggerListElem(trigger); // next/previous are initialized to NULL
	crTriggerListElem *nextElem = this->next;
	newElem->previous = this;
	this->next = newElem; // now link between "me" and "new" is ok on both sides
	// If I didn't have a "next": new will not have a next.
	if (nextElem) { // If I had a"next", manage link on both side (next & new)
		nextElem->previous = newElem;
		newElem->next = nextElem;
	}
	return newElem;
}


std::string crTriggerListElem::ToString(bool onlyIfEnabled) const {
	if (!this->trigger) { return ""; }
	if (onlyIfEnabled && !this->trigger->enabled) { return ""; }
	return this->trigger->ToString();
}


crTriggerSet::crTriggerSet() {
	this->firstTriggerElem = NULL;
	this->iteratorCurrent = NULL;
	for (int i = 0; i < TRIGGERSET_TOTAL_NUMBER_OF_PLAYERS; i++) {
		this->disable_unitDefIDTextList[i] = "";
		this->disable_researchIDTextList[i] = "";
	}
}

crTriggerSet::~crTriggerSet() {
	this->Reset(); // Delete all underlying trigger objects
}


// Removes all underlying triggers
void crTriggerSet::Reset() {
	crTriggerListElem *elem = this->firstTriggerElem;
	while (elem) {
		crTriggerListElem *oldelem = elem;
		elem = elem->next;
		delete oldelem; // also deletes underlying trigger object
		if (elem) { elem->previous = NULL; }
	}
	this->firstTriggerElem = NULL;
	this->iteratorCurrent = NULL;
}


// Returns true if the trigger set contains no trigger.
bool crTriggerSet::IsEmpty() const {
	return (this->firstTriggerElem == NULL);
}


// Add a trigger in the list.
// Returns NULL if the operation fails.
// Note: it is not allowed to have 2 triggers with the same name
// Warning: Do NOT free trigger yourself and don't use it afterwards.
crTriggerListElem *crTriggerSet::AddTriggerInList(crTrigger *trigger) {
	if (!trigger) {
		return NULL;
	}
	if (this->GetTrigger(trigger->triggerName) != NULL) { return NULL; } // A trigger already exists with this name.
	if (!this->firstTriggerElem) {
		this->firstTriggerElem = new crTriggerListElem();
		this->firstTriggerElem->trigger = trigger;
		return this->firstTriggerElem;
	} else {
		return this->firstTriggerElem->AddAfter(trigger);
	}
}


// Similar to AddTriggerInList, but runs preliminary checks before adding.
// Returns NULL if the operation fails / if the trigger is invalid.
crTriggerListElem *crTriggerSet::AddTriggerInListIfValid(crTrigger *trigger) {
	if (!trigger || !trigger->IsValid()) { return NULL; }
	return this->AddTriggerInList(trigger);
}


// Removes a trigger list element and handles list links.
bool crTriggerSet::RemoveListElem(crTriggerListElem *elem) {
	if (!elem) { return false; }
	if (elem == this->iteratorCurrent) {
		this->iteratorCurrent = elem->next; // Make sure iterator pointer is never an invalid pointer
	}
	delete elem; // Destuctor automatically handles links.
	return true;
}


// Returns a trigger according to its name. Returns NULL if not found.
crTrigger *crTriggerSet::GetTrigger(std::string triggerName) const {
	if (triggerName.empty()) { return NULL; }
	crTriggerListElem *elem = this->firstTriggerElem;
	while (elem) {
		if ((elem->trigger) && (elem->trigger->triggerName == triggerName)) {
			return elem->trigger;
		}
		elem = elem->next;
	}
	return NULL;
}


// Returns a string containing all underlying triggers formatted information.
std::string crTriggerSet::ToString(bool onlyEnabledTriggers) const {
	std::string result = "";
	crTriggerListElem *elem = this->firstTriggerElem;
	while (elem) {
		result += "\r\n";
		result += GetTriggerSpecialKeyword(KW_TRIGGER_SECTION);
		result += "\r\n";
		result += elem->ToString(onlyEnabledTriggers);
		elem = elem->next;
	}
	return result;
}


// Returns a "trigger file"-formatted string containing all information from the trigger set.
std::string crTriggerSet::GetStringForScenarioInfo(bool onlyEnabledTriggers) const {
	// This header ensures the game will never crash if trying to read as a PER file (END tag will make it stop reading the file).
	// This should never occur as we're using an unused player ID.
	std::string result = GetTriggerSpecialKeyword(CR_TRIGGERS::TRIGGER_KWID_SPECIAL::KW_HEADER);
	result += this->ToString(onlyEnabledTriggers);
	return result;
}


// Read a file content (a multi-line string) and parse it to create triggers.
// This does NOT reset existing triggers. Call Reset() first if you need.
// Limitation: each line's length must be < TRIGGER_FILE_LINE_MAX_LENGTH char.
bool crTriggerSet::ReadFromFileContent(char *triggersFile) {
	if (!triggersFile) { return false; }
	assert(TRIGGER_FILE_LINE_MAX_LENGTH > 10);
	char *currentPos = triggersFile;
	char currentLine[TRIGGER_FILE_LINE_MAX_LENGTH + 1]; // we need an extra char
	CR_TRIGGERS::crTrigger *currentTrigger = new crTrigger();
	while (*currentPos != 0) {
		char *nextLinePos = currentPos;
		int lineSize = 0;
		bool validLine = false;
		bool reachedEndOfLine = false;
		bool isEndOfFile = false;
		int equalsSignPosition = -1;
		while ((*nextLinePos != 0) && !reachedEndOfLine && (lineSize < TRIGGER_FILE_LINE_MAX_LENGTH - 1)) {
			if ((equalsSignPosition == -1) && (*nextLinePos == '=')) {
				equalsSignPosition = lineSize; // Save (first) = sign position
			}
			if (*nextLinePos == '\n') {
				reachedEndOfLine = true; // in this case, we still increase enOfLine so that it refers to 1st char of next line.
			} else {
				lineSize++;
			}
			nextLinePos++;
		}
		// Here endOfLine must refer to final \0 or first char of next line, if any.
		if (*nextLinePos == 0) { isEndOfFile = true; }
		
		// Copy current line to a dedicated buffer (currentLine) so we can do anything we want with it.
		if (lineSize > 0) {
			validLine = true;
			errno_t err;
			err = memcpy_s(currentLine, lineSize, currentPos, lineSize);
			assert(!err);
			currentLine[lineSize] = 0; // Unnecessary if we reached string end, useful if we just ended a line (replace \n by \0).
			if ((lineSize > 0) && (currentLine[lineSize - 1] == '\r')) {
				currentLine[lineSize - 1] = 0; // Fu**ing windows' CRLF !!!! Damn them for that stupid invention.
			}
		} else { 
			validLine = false;
		}
		// Ignore the "END" line (this is a security that will interrupt ROR PER file parsing.
		if (validLine && (currentLine[0] == 'E') && (currentLine[1] == 'N') && (currentLine[2] == 'D')) {
			validLine = false;
		}
		// Ignore comments
		if (validLine && (currentLine[0] == '/') && (currentLine[1] == '/')) {
			validLine = false;
		}
		// Read current line data, if valid
		if (validLine) {
			assert((equalsSignPosition < lineSize) && currentTrigger); // Note: equalsSignPosition<0 is possible, don't raise an error for that.
			if ((equalsSignPosition >= 0) && (equalsSignPosition < lineSize) && currentTrigger) {
				char *value = currentLine + equalsSignPosition + 1;
				currentLine[equalsSignPosition] = 0; // force string end after property name to split property name from value.
				this->ApplyTextProperty(currentTrigger, currentLine, value); // Load information into trigger object
			}
		}
		// Change section ? If yes, save current section's data
		if (isEndOfFile || (validLine && (_strnicmp(currentLine, GetTriggerSpecialKeyword(KW_TRIGGER_SECTION), lineSize) == 0))) {
			if (this->AddTriggerInListIfValid(currentTrigger) != NULL) {
				currentTrigger = new crTrigger(); // Restart with a brand new trigger. Don't modify previous one !
			} else {
				// Adding the trigger failed (was invalid/incomplete). Clean it before re-using the object.
				currentTrigger->Reset();
			}
		}
		
		assert(currentPos < nextLinePos);
		if (currentPos >= nextLinePos) {
			return false; // This is a big error case (probably an infinite loop).
		}
		currentPos = nextLinePos; // next line
	}
	return true;
}


// Interprets a line from trigger file: updates "triggerToUpdate" if the line contains a "single trigger object"-related data
// Returns true if successful.
bool crTriggerSet::ApplyTextProperty(crTrigger *triggerToUpdate, char *propertyName, char *value) {
	// Manage global-level instructions...
	bool isDisableUnitDef = false;
	bool isDisableResearch = false;
	long int playerId = -1;
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_1)) { isDisableUnitDef = true; playerId = 1; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_2)) { isDisableUnitDef = true; playerId = 2; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_3)) { isDisableUnitDef = true; playerId = 3; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_4)) { isDisableUnitDef = true; playerId = 4; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_5)) { isDisableUnitDef = true; playerId = 5; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_6)) { isDisableUnitDef = true; playerId = 6; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_7)) { isDisableUnitDef = true; playerId = 7; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_UNITDEFID_LIST_8)) { isDisableUnitDef = true; playerId = 8; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_1)) { isDisableResearch = true; playerId = 1; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_2)) { isDisableResearch = true; playerId = 2; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_3)) { isDisableResearch = true; playerId = 3; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_4)) { isDisableResearch = true; playerId = 4; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_5)) { isDisableResearch = true; playerId = 5; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_6)) { isDisableResearch = true; playerId = 6; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_7)) { isDisableResearch = true; playerId = 7; }
	if (SpecialKeyword_equals(propertyName, TRIGGER_KWID_SPECIAL::KW_SPECIAL_DISABLE_RESEARCHES_LIST_8)) { isDisableResearch = true; playerId = 8; }

	if (isDisableUnitDef) {
		if ((playerId < 0) || (playerId > 8)) { return false; }
		this->disable_unitDefIDTextList[playerId] = value;
		return true;
	}
	if (isDisableResearch) {
		if ((playerId < 0) || (playerId > 8)) { return false; }
		this->disable_researchIDTextList[playerId] = value;
		return true;
	}

	// If the line is not a global-level instruction, then apply it to the trigger object.
	return triggerToUpdate->ApplyTextProperty(propertyName, value); // Load information into trigger object
}


bool crTriggerSet::RemoveFromList(crTrigger *trigger) {
	if (!trigger) { return false; }
	crTriggerListElem *elem = this->firstTriggerElem;
	while (elem) {
		if (trigger == elem->trigger) {
			return this->RemoveListElem(elem);
		}
		elem = elem->next;
	}
	return false;
}


// Returns a "valid" trigger from current iterator position. Does not change iterator position unless it is invalid (see below).
// Ignores empty crTriggerListElem (ignore crTriggerListElem if its trigger is NULL):
//  retries with next list element (this MOVES iterator to next valid element).
// Returns NULL if not trigger was found (end of list is reached)
crTrigger *crTriggerSet::IteratorGetFirstValidTriggerFromCurrentPosition() {
	while (this->iteratorCurrent && (!this->iteratorCurrent->trigger)) {
		// ignore "empty" list elements (should NOT occur, we don't want list elem to have an NULL trigger object)
		assert(false);
		this->iteratorCurrent = this->iteratorCurrent->next;
	}
	if (this->iteratorCurrent) {
		return this->iteratorCurrent->trigger;
	}
	return NULL;
}

// Reset iterator and get first trigger element. Returns NULL when end has been reached (list is empty).
crTrigger *crTriggerSet::IteratorFirst() {
	this->iteratorCurrent = this->firstTriggerElem;
	return this->IteratorGetFirstValidTriggerFromCurrentPosition();
}

// Get next trigger using iterator. Returns NULL when end has been reached.
crTrigger *crTriggerSet::IteratorNext() {
	if (!this->iteratorCurrent) { return NULL; }
	this->iteratorCurrent = this->iteratorCurrent->next;
	return this->IteratorGetFirstValidTriggerFromCurrentPosition();
}

// Get current trigger using iterator position. Returns NULL when end has been reached or if iterator is not initialized.
crTrigger *crTriggerSet::IteratorCurrent() {
	return this->IteratorGetFirstValidTriggerFromCurrentPosition();
}

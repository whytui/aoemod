#pragma once
#include <string>
#include <assert.h>
#include <triggerDefinition.h>
#include "crTrigger.h"

// This files provides object definitions for custom triggers

// A limitation for all strings + line length concerning trigger files.
#define TRIGGER_FILE_LINE_MAX_LENGTH 100
#define TRIGGERSET_TOTAL_NUMBER_OF_PLAYERS 9

namespace CR_TRIGGERS {

	// Intermediate class to manage a list of triggers.
	// Do not use outside of crTriggerSet class.
	class crTriggerListElem {
	public:
		crTriggerListElem();
		crTriggerListElem(crTrigger *trigger);
		// The destructor automatically handles links for previous/next elements
		~crTriggerListElem();
		crTriggerListElem *AddAfter(crTrigger *trigger);
		std::string ToString(bool onlyIfEnabled) const;
		// Members
		// trigger object is NOT supposed to be NULL, please avoid this case.
		crTrigger *trigger;
		crTriggerListElem *previous;
		crTriggerListElem *next;
	};


	// Global class to manage all triggers.
	class crTriggerSet {
	public:
		crTriggerSet();
		~crTriggerSet();

		// Comma-separated list of unitDefID to disable at game startup (1 list per player)
		std::string disable_unitDefIDTextList[TRIGGERSET_TOTAL_NUMBER_OF_PLAYERS];
		// Comma-separated list of researchID to disable at game startup (1 list per player)
		std::string disable_researchIDTextList[TRIGGERSET_TOTAL_NUMBER_OF_PLAYERS];

		// Removes all underlying triggers
		void Reset();

		// Returns true if the trigger set contains no trigger.
		bool IsEmpty() const;

		// Add a trigger in the list.
		// Returns NULL if the operation fails.
		// Note: it is not allowed to have 2 triggers with the same name
		crTriggerListElem *AddTriggerInList(crTrigger *trigger);

		// Similar to AddTriggerInList, but runs preliminary checks before adding.
		crTriggerListElem *AddTriggerInListIfValid(crTrigger *trigger);

		// Remove a trigger from the list (remove all underlying objects)
		bool RemoveFromList(crTrigger *trigger);

		// Removes a trigger list element and handles list links.
		bool RemoveListElem(crTriggerListElem *elem);

		// Returns a trigger according to its name. Returns NULL if not found.
		crTrigger *GetTrigger(std::string triggerName) const;

		// Returns a string containing all underlying triggers formatted information.
		std::string ToString(bool onlyEnabledTriggers) const;

		// Returns a "trigger file"-formatted string containing all information from the trigger set.
		std::string GetStringForScenarioInfo(bool onlyEnabledTriggers) const;

		// Read a file content (a multi-line string) and parse it to create triggers.
		// This does NOT reset existing triggers. Call Reset() first if you need.
		bool ReadFromFileContent(char *triggersFile);

		// Reset iterator and get first trigger element. Returns NULL when end has been reached (list is empty).
		crTrigger *IteratorFirst();
		// Get next trigger using iterator. Returns NULL when end has been reached.
		crTrigger *IteratorNext();
		// Get current trigger using iterator position. Returns NULL when end has been reached or if iterator is not initialized.
		crTrigger *IteratorCurrent();

	private:
		crTriggerListElem *firstTriggerElem; //  list, 1st elem has no "previous", last has no "next".
		crTriggerListElem *iteratorCurrent; // Always make sure it is never an invalid pointer when removing elements !
		
		// Returns a "valid" trigger from current iterator position. Does not change iterator position unless it is invalid (see below).
		// Ignores empty crTriggerListElem (ignore crTriggerListElem if its trigger is NULL):
		//  retries with next list element (this MOVES iterator to next valid element).
		// Returns NULL if not trigger was found (end of list is reached)
		crTrigger *IteratorGetFirstValidTriggerFromCurrentPosition();

		// Interprets a line from trigger file: updates "triggerToUpdate" if the line contains a "single trigger object"-related data
		// Returns true if successful.
		bool ApplyTextProperty(crTrigger *triggerToUpdate, char *propertyName, char *value);
	};

}


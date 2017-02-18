#include "../include/GameTriggerTechInterfacing.h"


namespace ROCKNROR {
namespace TRIGGER {
;

// Returns a pointer to trigger data
char *GetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo) {
	if (!scInfo) { return NULL; }
	return scInfo->personalityFileContent[UNUSED_PLID_FOR_TRIGGERS - 1];
}
// Set trigger data pointer. Returns false if failed.
bool SetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, char *newDataPtr) {
	if (!scInfo) { return false; }
	scInfo->personalityFileContent[UNUSED_PLID_FOR_TRIGGERS - 1] = newDataPtr;
	return true;
}
// Returns a pointer to trigger data size
long int GetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo) {
	if (!scInfo) { return 0; }
	return scInfo->personalityFileSize[UNUSED_PLID_FOR_TRIGGERS - 1];
}
// Set trigger data size. Returns false if failed.
bool SetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, long int sizeToSet) {
	if (!scInfo) { return false; }
	scInfo->personalityFileSize[UNUSED_PLID_FOR_TRIGGERS - 1] = sizeToSet;
	return true;
}


// Returns true if provided trigger text contains the END tag before any trigger information.
bool TriggerTextContainsENDTagAtBeginning(char *triggerText) {
	if (!triggerText) { return false; }
	char *pos = triggerText;
	char *lineBeginning = triggerText;
	bool isComment = false;
	while (*pos != 0) {
		if ((pos == lineBeginning) && (pos[0] == 'E') && (pos[1] == 'N') && (pos[2] == 'D')) {
			return true; // Found a line starting by END
		}
		if ((pos == lineBeginning) && (pos[0] == '/') && (pos[1] == '/')) {
			isComment = true; // We ignore comments
		}
		if ((pos == lineBeginning) && SpecialKeyword_equals(pos, CR_TRIGGERS::KW_TRIGGER_SECTION)) {
			return false; // Found TRIGGER keyword before END tag.
		}
		if ((*pos == '=') && (!isComment)) {
			return false; // Found "=" in a line that is not a comment
		}
		if (*pos == '\n') { // Note there might be a preceding \r, it has no impact in this function.
			lineBeginning = pos + 1;
			isComment = false;
		}
		pos++;
	}
	return false;
}



}
}

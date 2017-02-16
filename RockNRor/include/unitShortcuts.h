#pragma once

#include <WinUser.h> // For virtual keys

/* This files handles custom unit shortcuts
 Here is the meaning of unit.shortcutNumber field
 - Values [1-9] are used as shortcut IDs, cf 1-9 keys
 - Value 10 (0x0A) works as shortcut "0", but has no graphical representation.
 - Values [11-127] are used as "unit group IDs". Do not mess with those values.
 - Value 0 = no group AND no shortcut
 - Value -1 = joker for groups (do not mess with this value)
 - Values <-1 are unused.

 In RockNRor, we use <-1 values to handle more shortcuts, without messing with unit groups.
 - Values -11 to -2 represent shortcuts 10-19 (numpad keys).
 */

// Converts a unit.shortcutNumber value into a REAL shortcut ID (1-10 in standard game, 1-20 in RockNRor)
// Returns 0 if value is invalid (no shortcut, or shortcut refers to a unit group, not a shortcutID)
static char GetShortcutDisplayValueFromInternalValue(char shortcutValue) {
	if ((shortcutValue >= 1) && (shortcutValue <= 10)) {
		return shortcutValue;
	}
	if (shortcutValue > 10) {
		return 0;
	}
	if (shortcutValue < -11) {
		return 0;
	}
	// Here: shortcutValue is in [-11;-2] = custom shortcuts
	return shortcutValue + 22; // returns a shortcut ID in [11-20]
}

// Converts a shortcut number (1-20) into the internal value to store in unit.shortcutNumber field.
// Returns 0 if input shortcut number is not a valid shortcut.
static char GetShortcutInternalValueFromDisplayValue(char shortcutNumber) {
	if ((shortcutNumber < 1) || (shortcutNumber > 20)) {
		return 0; // No shortcut
	}
	if (shortcutNumber <= 10) {
		return shortcutNumber; // [1;10] : return as is
	}
	return shortcutNumber - 22; // Other values : shortcuts [11;20] => values [-11;-2]
}


// Returns a shortcut display value corresponding to a keyboard shortcut key
// Returns 1-20 for valid shortcuts
// Returns 0 for groups / no shortcut / invalid values
static char GetShortcutDisplayValueFromKey(int key) {
	if (key == VK_NUMPAD0) {
		return 20;
	}
	if ((key >= VK_NUMPAD1) && (key <= VK_NUMPAD9)) {
		return (char)(key - VK_NUMPAD0 + 10);
	}
	if (key == '0') {
		return 10;
	}
	if ((key >= '1') && (key <= '9')) {
		return key - '0';
	}
	return 0;
}

// Returns a shortcut internal value corresponding to a keyboard shortcut key
static char GetShortcutInternalValueFromKey(int key) {
	char s = GetShortcutDisplayValueFromKey(key);
	return GetShortcutInternalValueFromDisplayValue(s);
}

// Returns true if provided value (a "unit.shortcutNumber" value) corresponds to a valid shortcut,
// Custom shortcuts (-11 to -2) are considered valid.
static bool IsValidInternalShortcutNumber(char shortcutValue) {
	// -1, 0, >10 and <-11 are invalid
	return (
		(shortcutValue >= 1) && (shortcutValue <= 10)
		) ||
		(
		(shortcutValue >= -11) && (shortcutValue <= -2)
		);
}

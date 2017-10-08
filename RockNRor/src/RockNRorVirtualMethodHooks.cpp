#include "../include/RockNRorVirtualMethodHooks.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

namespace VIRTUAL_METHOD_HOOKS {

#ifdef _DEBUG
	// 'Private' objects for internal usage
	static unsigned long int execBeginTime_ms = 0;
	static int checkPerfCallConsistency = 0;
	static int skippedRecursiveCalls = 0;

	void RecordPerfBegin(unsigned long int callAddress) {
		if (checkPerfCallConsistency > 0) {
			skippedRecursiveCalls++; // we measure only 1 hook at once (there is already a pending perf record). Skip this "recursive" call
			return; // Ignore
		}
		execBeginTime_ms = AOE_METHODS::TimeGetTime();
		checkPerfCallConsistency++;
	}
	void RecordPerfEnd(unsigned long int callAddress) {
		if (skippedRecursiveCalls > 0) {
			skippedRecursiveCalls--;
			return;
		}
		long int endTime = AOE_METHODS::TimeGetTime();
		long int timeSpent = endTime - execBeginTime_ms;
		long int n = ROCKNROR::crInfo.executionCounts[callAddress]; // returns 0 when not set yet (which is good)
		ROCKNROR::crInfo.executionCounts[callAddress] = n + 1;
		long int prevValue = ROCKNROR::crInfo.longestTimes_ms[callAddress]; // returns 0 when not set yet (which is good)
		if (timeSpent > prevValue) {
			ROCKNROR::crInfo.longestTimes_ms[callAddress] = timeSpent;
		}
		checkPerfCallConsistency--;
		assert(checkPerfCallConsistency == 0);
	}
#endif

#ifdef _DEBUG
#define RECORD_PERF_BEGIN(addr) RecordPerfBegin(addr); // BEGIN statement to collect execution stats
#define RECORD_PERF_END(addr) RecordPerfEnd(addr); // END statement to collect execution stats
#else
#define RECORD_PERF_BEGIN(addr) ;
#define RECORD_PERF_END(addr) ;
#endif



	// Maps to store ORIGINAL call addresses. Index=class checksum, value=original method call address
	std::map<unsigned long int, unsigned long int> activityProcessNotifyCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> activityMoveToCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> playerProcessNotifyCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> unitTransformCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> uiMenuOnKeyDownCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> uiGameEditorOnKeyDownCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> uiDialogsOnKeyDownCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> uiGenericOnKeyDownCheckSumAndOriginalAddress;


	// Return value is an unknown enum. 2=ok, processed. (unitAI: EDX+0xCC call).
	long int __stdcall ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2) {
		unsigned long int originalCallAddr = activityProcessNotifyCheckSumAndOriginalAddress[activity->checksum];
		assert(activity && activity->IsCheckSumValid() && notifyEvent);
		RECORD_PERF_BEGIN(originalCallAddr);

		if (!activity || !activity->IsCheckSumValid() || !notifyEvent) {
			RECORD_PERF_END(originalCallAddr);
			return AOE_CONST_INTERNAL::ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION; // TODO: return which value ?
		}
		
		bool runStandardMethod = true;

		// Custom treatments
		int result = ROCKNROR::GAME_EVENTS::ActivityProcessNotify(activity, notifyEvent, arg2, runStandardMethod); // updates runStandardMethod

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, activity;
				MOV EDX, DS:[ECX];
				PUSH arg2;
				PUSH notifyEvent;
				// DO NOT CALL DS:[EDX+0xCC] because we changed the pointer !
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}
		RECORD_PERF_END(originalCallAddr);
		return result;
	}


	// Return value = 1 on success, 0 if failed
	long int __stdcall ActivityMoveToMoveAwayFrom(STRUCT_UNIT_ACTIVITY *activity, float posY, float posX, unsigned long int arg3, unsigned long int arg4, long int force) {
		unsigned long int originalCallAddr = activityMoveToCheckSumAndOriginalAddress[activity->checksum];
		assert(activity && activity->IsCheckSumValid());
		RECORD_PERF_BEGIN(originalCallAddr);
		long int result = 0;
		bool runStandardMethod = true;

		// Custom treatments
		if ((posX < 0) || (posY < 0)) {
#ifdef __DEBUG // restore to troubleshoot
#pragma message(__FILE__ " TEMP TROUBLESHOOTING")
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = activity->ptrUnit;
			assert(unit && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass());
			std::string msg = "Canceled invalid activity.MoveTo with negative position for ";
			msg += unit->unitDefinition->ptrUnitName;
			msg += " id=";
			msg += std::to_string(unit->unitInstanceId);
			traceMessageHandler.WriteMessageNoNotification(msg.c_str());
			if (unit->unitDefinition->DerivesFromTrainable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
				if ((unitDef->speed > 0) && (unitDef->unitAIType != TribeAIGroupPredatorAnimal)) {
					if (unit->ptrStructPlayer) {
						AOE_STRUCTURES::STRUCT_PLAYER *player = AOE_METHODS::PLAYER::ChangeControlledPlayer(unit->ptrStructPlayer->playerId, false);
						assert(player && player->IsCheckSumValid());
						AOE_STRUCTURES::PLAYER::SelectOneUnit(player, unit, true);
					}
					AOE_METHODS::SetGamePause(true); // seems better for stability.
					ROCKNROR::SYSTEM::StopExecution(widen(msg).c_str(), true, true);
				}
			}
#else
			traceMessageHandler.WriteMessageNoNotification("Canceled invalid activity.MoveTo with negative position");
#endif
			runStandardMethod = false;
			result = 0; // failed
		}

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, activity;
				MOV EDX, DS:[ECX];
				PUSH force;
				PUSH arg4;
				PUSH arg3;
				PUSH posX;
				PUSH posY;
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}

		RECORD_PERF_END(originalCallAddr);
		return result;
	}


	// Returns void. This hook handles player's notifications (EDX+0xE8 call).
	void __stdcall PlayerProcessNotify(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent) {
		unsigned long int originalCallAddr = playerProcessNotifyCheckSumAndOriginalAddress[player->checksum];
		RECORD_PERF_BEGIN(originalCallAddr);
		assert(player && player->IsCheckSumValid());
		if (!player || !player->IsCheckSumValid()) {
			RECORD_PERF_END(originalCallAddr);
			return; 
		}
		bool runStandardMethod = true;

		// Custom treatments
		runStandardMethod = !ROCKNROR::GAME_EVENTS::PlayerNotifyEvent(player, notifyEvent);

		if (runStandardMethod) {
			unsigned long int arg1 = notifyEvent.callerUnitId;
			unsigned long int arg2 = notifyEvent.recipientUnitId;
			unsigned long int arg3 = notifyEvent.eventId;
			unsigned long int arg4 = notifyEvent.genericParam4;
			unsigned long int arg5 = notifyEvent.genericParam5;
			unsigned long int arg6 = notifyEvent.genericParam6;
			_asm {
				MOV ECX, player;
				MOV EDX, DS:[ECX];
				PUSH arg6;
				PUSH arg5;
				PUSH arg4;
				PUSH arg3;
				PUSH arg2;
				PUSH arg1;
				// DO NOT CALL DS:[EDX+0xE8] because we changed the pointer !
				CALL originalCallAddr;
			}
		}
		RECORD_PERF_END(originalCallAddr);
	}


	// unit.addPositionToTargetPosArray(pDword_posYXZ, arg2) for ALL unit classes (base+children). Method offset=+1BC.
	// Returns 1 on success, 0 on failure
	long int __stdcall UnitAddPositionToTargetPosArray(STRUCT_UNIT_BASE *unit, STRUCT_UNIT_TARGET_POS *targetPos, long int arg2) {
		unsigned long int originalCallAddr = unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[unit->checksum];
		RECORD_PERF_BEGIN(originalCallAddr);
		bool runStandardMethod = true;
		long int result = 0;
		assert(unit && unit->IsCheckSumValidForAUnitClass());
		assert(targetPos != NULL);
		assert(unit->ptrStructPlayer != NULL);

		// Custom treatments
		// TODO check with largest map size if this can happen
		if ((targetPos->posX == 0xFF) || (targetPos->posY == 0xFF)) {
			runStandardMethod = false; // Prevent game crash
			std::string msg = "Error with targetPos in UnitAddPositionToTargetPosArray. Unit=";
			msg += std::to_string(unit->unitInstanceId);
			msg += ". @ret=";
			msg += GetHexStringAddress(GetLastVirtualMethodCallReturnAddress());
			traceMessageHandler.WriteMessageNoNotification(msg.c_str());
			if (unit->ptrStructPlayer) {
				AOE_STRUCTURES::STRUCT_PLAYER *player = AOE_METHODS::PLAYER::ChangeControlledPlayer(unit->ptrStructPlayer->playerId, false);
				assert(player && player->IsCheckSumValid());
				AOE_STRUCTURES::PLAYER::SelectOneUnit(player, unit, true);
			}
			AOE_METHODS::CallWriteText(msg.c_str());
			AOE_METHODS::SetGamePause(true); // seems better for stability.
			ROCKNROR::SYSTEM::StopExecution(_T("An error occurred. *FIRST* close this message, then attach a debugger or press ESC to continue."), true, true);
			// this seems to happen on priests whose MOVE action has NULL target and -1,-1 target pos. Need to find out why.
		}

		if (runStandardMethod) {
			unsigned long int arg1 = (unsigned int)targetPos;
			_asm {
				MOV ECX, unit;
				PUSH arg2;
				PUSH targetPos;
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}

		RECORD_PERF_END(originalCallAddr);
		return result;
	}


	// Transform a unit from its current "unit definition" to provided "unit definition" (EDX+0x54 call).
	// Typically used to switch tasks (villagers) : in such case, newUnitDef is player's unitDefinition for the target unitDefId (repair_man, etc)
	// Also used to assign a dedicated "unit definition" in conversion process: unit will have its own "unitDefinition" (with its own specs)
	// This is only allowed if unit.status <= 2 (ready) because BASE method fails if status>2 (unit->unitDefinition is NOT updated).
	void __stdcall UnitTransform(STRUCT_UNIT_BASE *unit, STRUCT_UNITDEF_BASE *newUnitDef) {
		unsigned long int originalCallAddr = unitTransformCheckSumAndOriginalAddress[unit->checksum];
		assert(unit && unit->IsCheckSumValidForAUnitClass());
		RECORD_PERF_BEGIN(originalCallAddr);
		bool runStandardMethod = true;

		// Fix a game crash (that occurs later) : if status>2, base method does NOT update unit->unitDefinition
		// but TRAINABLE-level transform method would free "old" unitDefinition anyway => unit->unitDefinition would then refer to invalid memory
		// Remark: this fix is only needed for TRAINABLE override of "transform" method. Note that BUILDING class also uses this overload.
		if (unit->unitStatus > GAME_UNIT_STATUS::GUS_2_READY) {
			std::string msg = "Unit->transform bug tried to use invalid memory. RockNRor prevented the game from crashing. (id=";
			msg += std::to_string(unit->unitInstanceId);
			msg += " with status ";
			msg += std::to_string(unit->unitStatus);
			msg += ")";
			traceMessageHandler.WriteMessageNoNotification(msg.c_str());
			runStandardMethod = false;
		}

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, unit;
				PUSH newUnitDef;
				CALL originalCallAddr;
			}
		}

		RECORD_PERF_END(originalCallAddr);
	}


	// OnKeyDown event handler for menus, excluding in-game and scenario editor menus
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	long int __stdcall UIMenuOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
		unsigned long int originalCallAddr = uiMenuOnKeyDownCheckSumAndOriginalAddress[uiObj->checksum];
		assert(uiObj && isAValidRORChecksum(uiObj->checksum));
		RECORD_PERF_BEGIN(originalCallAddr);
		bool runStandardMethod = true;
		long int result = 0;

#ifdef _DEBUG

		ROCKNROR::UI::RnrScreenBase *curRnrScreen = ROCKNROR::crInfo.rnrUIHelper->FindRnrScreen(uiObj->screenName);
		if (curRnrScreen) {
			// We have a custom screen
		}

		// TEST
		ROCKNROR::UI::RnrScreenBase *testScreen1 = ROCKNROR::crInfo.rnrUIHelper->FindRnrScreen("testscreen");
		ROCKNROR::UI::RnrScreenBase *testScreen2 = ROCKNROR::crInfo.rnrUIHelper->FindRnrScreen("testscreen2");
		if (testScreen1 && testScreen1->GetScreenStatus() == ROCKNROR::UI::RnrScreenBase::CLOSED) {
			delete testScreen1;
			testScreen1 = NULL;
		}
		if (testScreen2 && testScreen2->GetScreenStatus() == ROCKNROR::UI::RnrScreenBase::CLOSED) {
			delete testScreen2;
			testScreen2 = NULL;
		}

		if (keyCode == VK_F2) {
			if (testScreen1 == NULL) {
				testScreen1 = new ROCKNROR::UI::RnrScreenBaseTest("testscreen");
				testScreen1->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::RedTheme);
				//testScreen1->SetWindowed(50, 100, 400, 300);
				//testScreen1->SetScreenType(AOE_METHODS::UI_BASE::ScreenType::DIALOG_PANEL);
				testScreen1->CreateScreen(NULL);
			}
			runStandardMethod = false;
		}
#endif

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, uiObj;
				PUSH SHIFT;
				PUSH CTRL;
				PUSH ALT;
				PUSH repeatCount;
				PUSH keyCode;
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}

		RECORD_PERF_END(originalCallAddr);
		return result;
	}


	// OnKeyDown event handler for in-game and scenario editor screens, excluding dialogs
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	long int __stdcall UIGameEditorOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
		unsigned long int originalCallAddr = uiGameEditorOnKeyDownCheckSumAndOriginalAddress[uiObj->checksum];
		assert(uiObj && isAValidRORChecksum(uiObj->checksum));
		RECORD_PERF_BEGIN(originalCallAddr);
		bool runStandardMethod = true;
		long int result = 0;

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, uiObj;
				PUSH SHIFT;
				PUSH CTRL;
				PUSH ALT;
				PUSH repeatCount;
				PUSH keyCode;
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}

		RECORD_PERF_END(originalCallAddr);
		return result;
	}


	// OnKeyDown event handler for dialogs (excluding generic dialog class)
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	long int __stdcall UIDialogOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
		unsigned long int originalCallAddr = uiDialogsOnKeyDownCheckSumAndOriginalAddress[uiObj->checksum];
		assert(uiObj && isAValidRORChecksum(uiObj->checksum));
		RECORD_PERF_BEGIN(originalCallAddr);
		bool runStandardMethod = true;
		long int result = 0;

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, uiObj;
				PUSH SHIFT;
				PUSH CTRL;
				PUSH ALT;
				PUSH repeatCount;
				PUSH keyCode;
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}

		RECORD_PERF_END(originalCallAddr);
		return result;
	}


	// OnKeyDown event handler for generic classes (easyPanel, dialogPanel)
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	// Can be used for custom screens and ALSO for unhandled events from child classes
	long int __stdcall UIGenericOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
		unsigned long int originalCallAddr = uiGenericOnKeyDownCheckSumAndOriginalAddress[uiObj->checksum];
		assert(uiObj && isAValidRORChecksum(uiObj->checksum));
		RECORD_PERF_BEGIN(originalCallAddr);
		bool runStandardMethod = true;
		long int result = 0;

		// If current screen is a custom one, first run custom handlers for this event
		{ // fake scope to ensure rnrScreen is not used afterwards
			ROCKNROR::UI::RnrScreenBase *rnrScreen = ROCKNROR::crInfo.rnrUIHelper->GetCurrentRnrScreen();
			if (rnrScreen) {
				runStandardMethod = !rnrScreen->OnKeyDown(uiObj, keyCode, repeatCount, ALT, CTRL, SHIFT);
				ROCKNROR::crInfo.rnrUIHelper->PurgeClosedScreens();
				// No longer use rnrScreen ! (in case it's been closed/deleted)
			}
		}

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, uiObj;
				PUSH SHIFT;
				PUSH CTRL;
				PUSH ALT;
				PUSH repeatCount;
				PUSH keyCode;
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}

		RECORD_PERF_END(originalCallAddr);
		return result;
	}




	// Technical declarations for Hook methods (creates small asm hook methods to dispatch to specific methods

	DECLARE_VIRTUAL_METHOD_HANDLER(ActivityProcessNotify)
	DECLARE_VIRTUAL_METHOD_HANDLER(ActivityMoveToMoveAwayFrom)
	DECLARE_VIRTUAL_METHOD_HANDLER(PlayerProcessNotify)
	DECLARE_VIRTUAL_METHOD_HANDLER(UnitAddPositionToTargetPosArray)
	DECLARE_VIRTUAL_METHOD_HANDLER(UnitTransform)
	DECLARE_VIRTUAL_METHOD_HANDLER(UIMenuOnKeyDown)
	DECLARE_VIRTUAL_METHOD_HANDLER(UIGameEditorOnKeyDown)
	DECLARE_VIRTUAL_METHOD_HANDLER(UIDialogOnKeyDown)
	DECLARE_VIRTUAL_METHOD_HANDLER(UIGenericOnKeyDown)


	// Patches ROR process (.rdata section) to connect overloaded virtual methods
	// Don't forget to declare RockNRor entry point methods as "__stdcall"
	bool InstallVirtualMethodsPatches() {
		if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
			return true;
		}
		if (ROCKNROR::crInfo.configInfo.doNotUpdateVirtualMethods) {
			CR_DEBUG::AppendTextToLogFile("Did not update virtual methods because it is disabled in configuration", true);
			return false;
		}
		if (EnableWritingInRData(true)) {
			// This can cause a crash if memory is not writeable. That's why we use EnableWritingInRData and also a flag to disable this.

			// Activity (unitAI)
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_BASE, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_BASE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_BUILDING, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_BUILDING]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_CIVILIAN, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_CIVILIAN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_LION, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_LION]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_MILITARY, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_MILITARY]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PRIEST, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PRIEST]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TOWER, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TOWER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP]);

			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_BASE, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_BASE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_BUILDING, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_BUILDING]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_CIVILIAN, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_CIVILIAN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_LION, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_LION]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_MILITARY, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_MILITARY]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PRIEST, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PRIEST]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TOWER, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TOWER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP, 0x90, ActivityMoveToMoveAwayFrom, activityMoveToCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP]);

			// Player
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_PLAYER, 0xE8, PlayerProcessNotify, playerProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_PLAYER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_RGE_PLAYER, 0xE8, PlayerProcessNotify, playerProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_RGE_PLAYER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_GAIA_PLAYER, 0xE8, PlayerProcessNotify, playerProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_GAIA_PLAYER]);

			// Unit
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_BASE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_BASE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ATTACKABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_ATTACKABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_BUILDING, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_BUILDING]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_COMMANDABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_COMMANDABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_DOPPLEGANGER, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_DOPPLEGANGER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_FLAG, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_FLAG]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_MOVABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_MOVABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_PROJECTILE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_PROJECTILE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_TRAINABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_TRAINABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_TREE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_TREE]);

			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_TRAINABLE, 0x54, UnitTransform, unitTransformCheckSumAndOriginalAddress[CHECKSUM_UNIT_TRAINABLE]); // unit->tranform only needs to be fixed for TRAINABLE class

			// ----- UI -----
			// Key down in parent screen classes
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_EASY_PANEL, 0x58, UIGenericOnKeyDown, uiGenericOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_EASY_PANEL]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SCREEN_PANEL, 0x58, UIGenericOnKeyDown, uiGenericOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SCREEN_PANEL]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_DIALOG_BASE, 0x58, UIGenericOnKeyDown, uiGenericOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_DIALOG_BASE]);
			// Key down in menus
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_WELCOME_MAIN_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_WELCOME_MAIN_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SP_MENU_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SP_MENU_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_MP_SETUP_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_MP_SETUP_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_GAME_SETTINGS, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_GAME_SETTINGS]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_CAMPAIGN_EDITOR, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_CAMPAIGN_EDITOR]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_CAMPAIGN_SELECTION, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_CAMPAIGN_SELECTION]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SELECT_SCENARIO_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SELECT_SCENARIO_SCREEN]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_MP_STARTUP_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_MP_STARTUP_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_JOIN_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_JOIN_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SCENARIO_EDITOR_MAIN_MENU, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SCENARIO_EDITOR_MAIN_MENU]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_ACHIEVEMENTS_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_ACHIEVEMENTS_SCREEN]); // In-game or menu ?
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_CREDITS_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_CREDITS_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_LOAD_SAVED_GAME, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_LOAD_SAVED_GAME]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_MAIN_ERROR_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_MAIN_ERROR_SCREEN]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_MP_WAIT_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_MP_WAIT_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_NAME_SELECTION, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_NAME_SELECTION]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SAVE_AS_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SAVE_AS_SCREEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SC_EDITOR_OPEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SC_EDITOR_OPEN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_STATUS_MESSAGE, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_STATUS_MESSAGE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_MISSION_SCREEN, 0x58, UIMenuOnKeyDown, uiMenuOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_MISSION_SCREEN]);

			// Key down in game or editor
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_IN_GAME_MAIN, 0x58, UIGameEditorOnKeyDown, uiGameEditorOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_IN_GAME_MAIN]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SCENARIO_EDITOR_MAIN, 0x58, UIGameEditorOnKeyDown, uiGameEditorOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SCENARIO_EDITOR_MAIN]);

			// Key down in dialogs
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_IN_GAME_MENU, 0x58, UIDialogOnKeyDown, uiDialogsOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_IN_GAME_MENU]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_SCENARIO_EDITOR_MENU, 0x58, UIDialogOnKeyDown, uiDialogsOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_SCENARIO_EDITOR_MENU]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_TLISTDIALOG, 0x58, UIDialogOnKeyDown, uiDialogsOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_TLISTDIALOG]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_RGE_DIALOG_LIST, 0x58, UIDialogOnKeyDown, uiDialogsOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_RGE_DIALOG_LIST]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_MESSAGE_DIALOG, 0x58, UIDialogOnKeyDown, uiDialogsOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_MESSAGE_DIALOG]);
			//INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UI_NAME_DIALOG, 0x58, UIDialogOnKeyDown, uiDialogsOnKeyDownCheckSumAndOriginalAddress[CHECKSUM_UI_NAME_DIALOG]);


			CR_DEBUG::AppendTextToLogFile("Virtual methods APIs have been written", true);

			// Restore read-only access on .rdata section.
			EnableWritingInRData(false);
			return true;
		} else {
			CR_DEBUG::AppendTextToLogFile("Failed to enable writing in rdata", true);
			return false;
		}
	}

}
}

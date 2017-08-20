#include "../include/testROR_Structures.h"


// Returns true if object's member is at the correct offset
// trs_assert macro is better, please use it instead.
static bool cmp_obj_with_member_ptr(void *objPtr, int offset, void *memberPtr) {
	return (((char*)objPtr) + offset) == memberPtr;
}

// Do not call directly.
static void _assert_member_offset(void *objPtr, int offset, void *memberPtr, wchar_t *objName, const wchar_t *file, unsigned int line) {
#ifdef _DEBUG
	// Split into 3 instruction for debugging
	long int myOffset = (long int)(((char*)objPtr) + offset);
	long int memberOffset = (long int)memberPtr;
	long int shift = myOffset - memberOffset;
	if (shift == 0) { return; }
	std::wstring msg = _T("");
	wchar_t values[512];
	swprintf_s(values, _T("Object name=%s ; shift=%08X\noffset=%08X - memberAddr=%08X"), objName, shift, myOffset, memberOffset);
	msg += values;
	_wassert(msg.c_str(), file, line);
#endif
}


// This is too big to be used in stack
static AOE_STRUCTURES::STRUCT_PATHING_SYSTEM umapF04C;
static AOE_STRUCTURES::STRUCT_GAME_MAP_INFO gmapinfo;
static AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN scEditorUI;


// Run tests on ROR structures offset (and size)
// Run it after each modification on ROR objects definitions to make sure there is no regression.
static bool test_ror_structures() {
#ifdef _DEBUG
	AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO clinfo;
	trs_assert(&clinfo, 0x8E4, &clinfo.screenSizeY);
	AOE_STRUCTURES::STRUCT_SOUND_TDIGITAL sounddataelem;
	trs_assert(&sounddataelem, 0x38, &sounddataelem.unknown_38);
	AOE_STRUCTURES::STRUCT_SOUND_DRIVER mainsound;
	assert(sizeof(mainsound) == 0x69C);
	trs_assert(&mainsound, 0x1D0, &mainsound.mlc_cbStruct);
	trs_assert(&mainsound, 0x250, &mainsound.mc_dwMaximum);
	trs_assert(&mainsound, 0x698, &mainsound.stackedSoundsCount);
	AOE_STRUCTURES::STRUCT_MAIN_MUSIC mainmusic;
	assert(sizeof(mainmusic) == 0x3F8);
	trs_assert(&mainmusic, 0x3F4, &mainmusic.unknown_3F4);
	AOE_STRUCTURES::STRUCT_MP_COMMUNICATION u580DA8;
	assert(sizeof(u580DA8) == 0x1774);
	trs_assert(&u580DA8, 0x1706, &u580DA8.unknown_1706);
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO scinfo;
	assert(sizeof(scinfo) == 0x514C);
	trs_assert(&scinfo, 0x1990, &scinfo.startingResources);
	trs_assert(&scinfo, 0x1AA4, &scinfo.generalVictory_goldAmount);
	trs_assert(&scinfo, 0x4BA8, &scinfo.hasAlliedVictory);
	trs_assert(&scinfo, 0x4BF4, &scinfo.disableTechnologyFlags);
	trs_assert(&scinfo, 0x5148, &scinfo.generalVictory_timeAmount);
	AOE_STRUCTURES::STRUCT_SCORE_ELEM scoreElem;
	assert(sizeof(scoreElem) == 0x20);
	trs_assert(&scoreElem, 0x1C, &scoreElem.unknown_1C);
	AOE_STRUCTURES::STRUCT_SCORE_HEADER scoreHeader;
	trs_assert(&scoreHeader, 0x1C, &scoreHeader.currentTotalScore);
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT se;
	assert(sizeof(se) == 0xB0);
	trs_assert(&se, 0xAC, &se.permanentSkip);
	AOE_STRUCTURES::STRUCT_VILLAGER_TASKS_ELEM vte;
	assert(sizeof(vte) == 0x18);
	trs_assert(&vte, 0x14, &vte.timeGetTimeValue);
	AOE_STRUCTURES::STRUCT_PLANNED_RESOURCE_NEEDS prn;
	assert(sizeof(prn) == 0x70);
	trs_assert(&prn, 0x6C, &prn.resourceTypesCount);
	AOE_STRUCTURES::STRUCT_UNIT_GROUP uge;
	assert(sizeof(uge) == 0x330);
	trs_assert(&uge, 0x1D8, &uge.unknown_1D8);
	trs_assert(&uge, 0x32C, &uge.unknown_gameTime_ms);
	assert(sizeof(AOE_STRUCTURES::STRUCT_INF_AI_DETAILED_UNIT_INFO) == 0x24);

	// Actions
	AOE_STRUCTURES::STRUCT_ACTION_BASE actionbase;
	trs_assert(&actionbase, 0x030, &actionbase.command);
	assert(sizeof(AOE_STRUCTURES::STRUCT_ACTION_BASE) == 0x40);
	AOE_STRUCTURES::STRUCT_ACTION_CONVERSION aconv;
	assert(sizeof(aconv) == 0x48);
	trs_assert(&aconv, 0x40, &aconv.unknown_040);
	AOE_STRUCTURES::STRUCT_ACTION_ATTACK aa;
	trs_assert(&aa, 0x030, &aa.command);
	trs_assert(&aa, 0x050, &aa.minRange);
	assert(sizeof(AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT) == 0x50);
	assert(sizeof(AOE_STRUCTURES::STRUCT_ACTION_DISCOVERY_ARTEFACT) == 0x44);
	// AI Struct
	AOE_STRUCTURES::STRUCT_AI ai;
	assert(sizeof(ai.structBuildAI.commonAIObject) == 0xEC);
	trs_assert(&ai.structBuildAI, 0xF0, &ai.structBuildAI.fakeFirstStrategyElement);
	assert(sizeof(ai.structEmotionalAI) == 0x1C0);
	assert(sizeof(ai.structBuildAI) == 0x5C0);
	trs_assert(&ai, 0x104, &ai.structBuildAI);
	trs_assert(&ai, 0x6C4, &ai.structConAI);
	trs_assert(&ai, 0x9DC, &ai.structDiplAI);
	trs_assert(&ai, 0xCEC, &ai.structInfAI);
	trs_assert(&ai, 0x10D7C, &ai.structResourceAI);
	trs_assert(&ai, 0x10E74, &ai.structStrategyAI);
	trs_assert(&ai, 0x1146C, &ai.structTacAI);
	trs_assert(&ai, 0x1146C + 0x124, &ai.structTacAI.unknown_124);
	trs_assert(&ai, 0x1146C + 0x194, &ai.structTacAI.SNNumber);
	trs_assert(&ai, 0x1146C + 0xD78, &ai.structTacAI.gathererCount_desired);
	trs_assert(&ai, 0x1146C + 0x51C, &ai.structTacAI.gatherersTasks);
	trs_assert(&ai, 0x1146C + 0xF94, &ai.structTacAI.lastPanicModeStrategyUpdateTime);
	trs_assert(&ai, 0x12478, &ai.structTradeAI);
	trs_assert(&ai.structTradeAI, 0xF0, &ai.structTradeAI.ptrMainAI);
	trs_assert(&ai, 0x12568, &ai.structTradeAI.ptrMainAI);
	trs_assert(&ai, 0x1256C, &ai.player);

	AOE_STRUCTURES::STRUCT_WAYPOINT pstep;
	assert(sizeof(pstep) == 0x10);
	trs_assert(&pstep, 0x0C, &pstep.remainingSteps);
	AOE_STRUCTURES::STRUCT_PATH movinfo;
	assert(sizeof(movinfo) == 0x34);
	trs_assert(&movinfo, 0x30, &movinfo.intermediatePositionsCurrentIndex);
	AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF duc;
	trs_assert(&duc, 0x48, &duc.resourceDepositSound);
	assert(sizeof(duc) == 0x4C);
	AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER duch;
	trs_assert(&duch, 0x08, &duch.commandCount);
	AOE_STRUCTURES::STRUCT_CIVILIZATION_DEF dc;
	trs_assert(&dc, 0x2A, &dc.techTreeId);

	AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT tde;
	trs_assert(&tde, 0x08, &tde.effectValue);
	assert(sizeof(tde) == 0x0C);
	AOE_STRUCTURES::STRUCT_TECH_DEF td;
	trs_assert(&td, 0x28, &td.ptrEffects);
	assert(sizeof(td) == 0x2C);
	// TO DO
	assert(sizeof(AOE_STRUCTURES::STRUCT_COST) == 6);
	AOE_STRUCTURES::STRUCT_TECH_DEF_INFO tdi;
	trs_assert(&tdi, 0x08, &tdi.technologyCount);
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO pri;
	trs_assert(&pri, 0x0C, &pri.ptrPlayer);
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS prs;
	trs_assert(&prs, 0x06, &prs.unknown_06);
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF_INFO rdi;
	trs_assert(&rdi, 0x08, &rdi.ptrGlobalStruct);
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF rd;
	trs_assert(&rd, 0x38, &rd.unknown_38);
	assert(sizeof(rd) == 0x3C);	

	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY uay;
	trs_assert(&uay, 0x60, &uay.unitIDsThatAttackMe);
	trs_assert(&uay, 0xF0, &uay.wayPointQueueSize);
	trs_assert(&uay, 0x114, &uay.currentPosX);
	AOE_STRUCTURES::STRUCT_UNIT_ACTION_INFO uai;
	trs_assert(&uai, 0x08, &uai.ptrActionLink);

	assert(sizeof(AOE_STRUCTURES::STRUCT_PLAYER_NEW_EXPLORED_TILES) == 0x14);
	AOE_STRUCTURES::STRUCT_PLAYER p;
	trs_assert(&p, 0x238, &p.unknown_238);
	trs_assert(&p, 0x840, &p.populationHistoryInfo);
	trs_assert(&p, 0x85C, &p.custom_selectedUnits);
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL g;
	assert(sizeof(g) == 0x124);
	trs_assert(&g, 0x11E, &g.unknown_11E);
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS s;
	trs_assert(&s, 0xAC4, &s.chosenPlayerNum);
	trs_assert(&s, 0x1198, &s.civPlayerNameIsUsed);

	// MAP
	assert(sizeof(AOE_STRUCTURES::STRUCT_MAPGEN_ELEVATION_DATA) == 0xF80);
	assert(sizeof(AOE_STRUCTURES::STRUCT_MAPGEN_ELEVATION_LINK) == 0x8);
	assert(sizeof(AOE_STRUCTURES::STRUCT_RMM_ELEVATION_GENERATOR) == 0xFB0);
	assert(sizeof(AOE_STRUCTURES::STRUCT_TERRAIN_DEF) == 0x198);
	trs_assert(&umapF04C, 0xCE624, &umapF04C.unitObstructionMap);
	trs_assert(&umapF04C, 0x10DE28, &umapF04C.unknown_010DE28_misc);
	trs_assert(&umapF04C, 0x11DC28, &umapF04C.unknown_11DC28);
	trs_assert(&umapF04C, 0x11DCE4, &umapF04C.unknown_11DCE4);
	assert(sizeof(AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO) == 0x18);
	assert(sizeof(gmapinfo) == 0xB5F8);
	trs_assert(&gmapinfo, 0x8DF0, &gmapinfo.unknown_8DF0);
	AOE_STRUCTURES::STRUCT_PLAYER_MAP_INFO pmapinfo;
	assert(sizeof(pmapinfo) == 0x38);
	trs_assert(&pmapinfo, 0x20, &pmapinfo.totalTilesCount);
	assert(sizeof(AOE_STRUCTURES::STRUCT_MAPGEN_OBJECT_PLACER) == 0x30);
	AOE_STRUCTURES::STRUCT_MAPGEN_BASE_ZONE_INFO mbzi;
	assert(sizeof(AOE_STRUCTURES::STRUCT_MAPGEN_BASE_ZONE_INFO) == 0x2C);
	trs_assert(&mbzi, 0x28, &mbzi.unknown_28);
	assert(sizeof(AOE_STRUCTURES::STRUCT_MAPGEN_PLAYER_BASE_INFO) == 0x34);
	assert(sizeof(AOE_STRUCTURES::STRUCT_MAPGEN_MAP_TYPE) == 0x48);
	AOE_STRUCTURES::STRUCT_RMM_DB_CONTROLLER obj185D5400;
	trs_assert(&obj185D5400, 0x1470, &obj185D5400.borderUsagePercentage);

	// UI
	AOE_STRUCTURES::STRUCT_ANY_UI anyUI;
	assert(sizeof(anyUI) == 0xF4); // Will raise an error if changed. Don't forget to impact inherited objects.
	trs_assert(&anyUI, 0x44, &anyUI.unknown_044);

	AOE_STRUCTURES::STRUCT_UI_TEXTBOX textbox;
	trs_assert(&textbox, 0xFC, &textbox.pTypedText);

	AOE_STRUCTURES::STRUCT_UI_LABEL label;
	trs_assert(&label, 0x180, &label.unknown_180);
	assert(sizeof(label) == 0x188);

	assert(sizeof(AOE_STRUCTURES::STRUCT_UI_COMBOBOX) == 0x144);
	AOE_STRUCTURES::STRUCT_UI_LISTBOX lbx;
	trs_assert(&lbx, 0x102, &lbx.unknown_102);

	AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE playingzone;
	trs_assert(&playingzone, 0x100, &playingzone.controlledPlayer);
	trs_assert(&playingzone, 0x340, &playingzone.unknown_340_unitId);

	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN uiigm;
	trs_assert(&uiigm, 0x55C, &uiigm.ingameChatTextZone);
	trs_assert(&uiigm, 0x5E0, &uiigm.panelDisplayedButtonCount);
	trs_assert(&uiigm, 0x7C4, &uiigm.panelSelectedUnitId);

	AOE_STRUCTURES::STRUCT_UI_POPUP_DIALOG uidialog;
	assert(sizeof(uidialog) == 0x4A4);
	trs_assert(&uidialog, 0x4A0, &uidialog.btnCancel);

	AOE_STRUCTURES::STRUCT_UI_GAME_SETTINGS uigamesettings;
	assert(sizeof(uigamesettings) == 0x560);
	trs_assert(&uigamesettings, 0x55C, &uigamesettings.unknown_55C);

	AOE_STRUCTURES::STRUCT_UI_DIPLOMACY_DIALOG uidipldialog;
	assert(sizeof(uidipldialog) == 0x6B8);
	trs_assert(&uidipldialog, 0x62C, &uidipldialog.lblStoneTribute);

	AOE_STRUCTURES::STRUCT_UI_IN_GAME_OPTIONS uiingameoptions;
	assert(sizeof(uiingameoptions) == 0x564);
	trs_assert(&uiingameoptions, 0x560, &uiingameoptions.soundVolumeLevelValue);
	
	//assert(sizeof(scEditorUI) == );
	trs_assert(&scEditorUI, 0x500, &scEditorUI.btnMenu);
	trs_assert(&scEditorUI, 0x664, &scEditorUI.unitListElementCount);

	AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE uiunitinfozone;
	trs_assert(&uiunitinfozone, 0x1E0, &uiunitinfozone.currentUnitPlayer);

	AOE_STRUCTURES::STRUCT_TRIBE_GAME_OPTIONS mpho;
	trs_assert(&mpho, 0xCC, &mpho.tribeSpecificOptions.unknown_CC);
	trs_assert(&mpho, 0x105, &mpho.tribeSpecificOptions.allTechnologies);

#endif
	return true;
}



int _tmain(int argc, _TCHAR *argv[]) {
#ifndef _DEBUG
	MessageBox(NULL, _T("Please run this in DEBUG compilation mode."), _T("Unit test"), MB_ICONINFORMATION);
	return 0; // Test must be run in debug mode.
#endif
	test_ror_structures();
	test_randomizer();
	MessageBox(NULL, _T("End"), _T("Unit test"), MB_ICONINFORMATION);
	return 0;
}


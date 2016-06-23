#pragma once
#include "AOE_const_global.h"
#include "BinarySeqDefinition.h"


// Always use consecutive values for iterators
// Example: for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {...}
// This enum is used to regroup binary sequence definitions by theme.
enum BINSEQ_CATEGORIES { BC_NONE, BC_TECH_FIXES, BC_RESOLUTION, BC_WINDOWED_MODE, BC_VEG_WINDOWED_MODE, BC_SELECTED_UNITS, 
	BC_ROR_API, BC_OPTIONS, BC_MANAGE_AI, BC_OBSOLETES, BC_COUNT
};


// Just to identify setup exceptions
class AOE_binDataSetupException : public std::exception {
public:
	AOE_binDataSetupException(const char *m = "AOE binData custom exception") : std::exception(m) {}
	~AOE_binDataSetupException(void) {};
};


class AOE_binData {
public:
	// Throws AOE_binDataSetupException
	AOE_binData();
	~AOE_binData();

	BinarySeqDefSet resolution_10b;
	BinarySeqDefSet resolution_10c;
	BinarySeqDefSet windowedMode_10b;
	BinarySeqDefSet VEG_windowedMode_10b;
	BinarySeqDefSet windowedMode_10c;
	BinarySeqDefSet VEG_windowedMode_10c;
	BinarySeqDefSet techFixes_10b;
	BinarySeqDefSet techFixes_10c;
	BinarySeqDefSet ROR_API_10b;
	BinarySeqDefSet ROR_API_10c;
	BinarySeqDefSet options_10b;
	BinarySeqDefSet options_10c;
	BinarySeqDefSet selectedUnits_10b;
	BinarySeqDefSet selectedUnits_10c;
	BinarySeqDefSet obsoletes_10b;
	BinarySeqDefSet obsoletes_10c;
	BinarySeqDefSet manageAI_10b;
	BinarySeqDefSet manageAI_10c;

	AOE_FILE_VERSION GetCurrentVersion();
	void SetCurrentVersion(AOE_FILE_VERSION value);
	BinarySeqDefSet *GetSeqDefSet(AOE_FILE_VERSION version, BINSEQ_CATEGORIES category);

private:
	AOE_FILE_VERSION currentVersion;

	void InitData();
	void InitResolution_b();
	void InitResolution_c();
	void InitWindowedMode_b();
	void InitWindowedMode_c();
	void InitTechFixes_10b();
	void InitTechFixes_10c();
	void InitOptions_10b();
	void InitOptions_10c();
	void InitSelectedUnits();
	void InitObsoletes();
	void InitROR_API_10b();
	void InitROR_API_10c();
	void InitManageAI();
};

/*
namespace AOE_BIN_VALUES {
	// All constant names MUST be normalized to be used directly by macros.
	// prefix_xxx_OFFSET
	// prefix_xxx_SEQ0 to n (according to the number of possible sequences)
	// prefix_xxx_VARTYPE0 to n (if a variable is used)
	// prefix_xxx_VAROFFSET0 to n (if a variable is used)

	const static long _10B_CD_CHECK_OFFSET = 0x19010;
	const static unsigned char _10B_CD_CHECK_SEQ0[] = { 0x88, 0x08, 0x04, 0x00, 0x00, 0x85, 0xc9, 0x75, 0x12, 0xb8, 0x01, 0x00, 0x00, 0x00 };
	const static unsigned char _10B_CD_CHECK_SEQ1[] = { 0x88, 0x08, 0x04, 0x00, 0x00, 0x85, 0xc9, 0x75, 0x00, 0xb8, 0x01, 0x00, 0x00, 0x00 };

	const static long _10C_CD_CHECK_OFFSET = 0x1953d;
	const static unsigned char _10C_CD_CHECK_SEQ0[] = { 0x8B, 0x88, 0x08, 0x04, 0x00, 0x00, 0x85, 0xc9, 0x75, 0x10 };
	const static unsigned char _10C_CD_CHECK_SEQ1[] = { 0x8B, 0x88, 0x08, 0x04, 0x00, 0x00, 0x85, 0xc9, 0x75, 0x00 };

	const static long _10C_DEBUG_TO_FILE_OFFSET = 0xc1c4;
	const static unsigned char _10C_DEBUG_TO_FILE_SEQ0[] = { 0x74, 0x50, 0x85, 0xC0, 0x75, 0x63 };
	const static unsigned char _10C_DEBUG_TO_FILE_SEQ1[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };


	const static long _10C_LVL5_BONUS_AMOUNT_OFFSET = 0xf4edd;
	const static unsigned char _10C_LVL5_BONUS_AMOUNT_SEQ0[] = { 0xb8, 0xd0, 0x07, 00, 00 };
	const static SEQ_VAR_TYPES _10C_LVL5_BONUS_AMOUNT_VARTYPE0 = SVT_INT_4B;
	const static long _10C_LVL5_BONUS_AMOUNT_VAROFFSET0 = 1;



}
*/

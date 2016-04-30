
#pragma once

#include <string>
#include <assert.h>
#include "../../AOE_DataPatcher/include/AOE_binData.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"
#include <AOE_offsets_10c.h>
#include "EXESelfEditor.h"
#include "RORProcessEditor.h"


static AOE_binData x;



static void patcherExample() {
	try {
		RORProcessEditor e;
		x.SetCurrentVersion(AOE_FILE_VERSION::AOE_VERSION_1_0C);
		BinarySeqDefSet *sds = x.GetSeqDefSet(AOE_FILE_VERSION::AOE_VERSION_1_0C, BINSEQ_CATEGORIES::BC_OPTIONS);
		//x.options_10c.SetUserVarValue(21, (char)25);
		int index = sds->FindSeqDefinitionIndex(_T("PointsPerTemple"));
		sds->SetUserSelection(index, 0);
		char v = 25;
		sds->SetUserVarValue(index, &v);
		int res = e.WriteFromSequenceUsingValue(sds->GetBinSeqDefinition(index), 0, &v);
		//int res = WriteFromSequence(sds->GetBinSeqDefinition(index), 0); // issue: will use default var value
		//int res = WriteFromSequenceUsingValue(sds->GetBinSeqDefinition(index), 0, &v);
		if (res == 0) {
			traceMessageHandler.WriteMessageNoNotification("OK");
		} else {
			traceMessageHandler.WriteMessageNoNotification("failed");
		}
	}
	catch (exception e) {
		traceMessageHandler.WriteMessage(e.what());
	}
}


#pragma once

#include "AOE_binData.h"
#include "EmpiresX_API.h"


#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/treectrl.h>
#include <wx/hyperlink.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class AOEEditTreeItemData : public wxTreeItemData
{
public:
	AOEEditTreeItemData(BINSEQ_CATEGORIES category, bool isAdvanced);
	AOEEditTreeItemData(bool isPending);
	~AOEEditTreeItemData() {};
	BINSEQ_CATEGORIES category;
	bool isAdvanced; // true = is a "details" screen
	bool isPending; // true = is the "pending changes" node
	std::wstring labelText;
	std::wstring refSequenceName; // Name of the "reference" sequence to test current status in non-advanced screens.
};

class AdvancedSeqButtonInfo : public wxObjectRefData {
public:
	AdvancedSeqButtonInfo() {
		category = BC_NONE;
		seqDefIndex = -1;
		seqIndex = -1;
		hasOtherButton = false;
		otherButton = NULL;
		userSelLabel = NULL;
		fileSelLabel = NULL;
	}

	BINSEQ_CATEGORIES category;
	int seqDefIndex;
	int seqIndex; // to match with userChoice / fileSelection
	bool hasOtherButton;
	wxButton *otherButton;
	wxHyperlinkCtrl *userSelLabel;
	wxStaticText *fileSelLabel;
};


class AdvancedSeqVarLinkInfo : public wxObjectRefData {
public:
	AdvancedSeqVarLinkInfo() {
		category = BC_NONE;
		seqDefIndex = -1;
		variableType = SVT_NO_VARIABLE;
	}

	BINSEQ_CATEGORIES category;
	int seqDefIndex;
	SEQ_VAR_TYPES variableType;
};


#pragma warning(pop)

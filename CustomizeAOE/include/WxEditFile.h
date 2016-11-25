#pragma once
#include <Shlwapi.h> // PathFileExists
#include "Version.h"
#include "mystrings.h"
#include "EmpiresX_API.h"
#include "../../AOE_DataPatcher/include/AOE_binData.h"
#include "UI_infoObjects.h"

#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/treectrl.h>
#include <wx/numdlg.h> 
#include <wx/colour.h>
#include <wx/hyperlink.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "WxBinSeqWindow.h"

// Some float values are truncated when displayed (we don't want tons of zeros to be displayed for decimal numbers).
#define FLOAT_MAX_CHARS_TO_DISPLAY 8


// Common colors (RGB values)
static const long color_lightBlue = 0xE6D8AD;
static const long color_lightGrey = 0xCDCDCD;
static const long color_lightYellow = 0xCCFFFF;
static const long color_lightGreen = 0x90EE90;
static const long color_lightOrange = 0x1080FF;


class WxEditFile : public wxDialog
{
public:
	WxEditFile(wxWindow *parent, const wxString& title, const wxSize& size);
	WxEditFile(wxWindow *parent, const wxString& title, const wxSize& size, EmpiresX_API *api);
	~WxEditFile();
	void ConstructorInit(EmpiresX_API *api);
	void SetAPI(EmpiresX_API *value) { this->e_api = value; };
	EmpiresX_API *GetAPI() { return this->e_api; }

private:
	EmpiresX_API *e_api;
	
	wxControl *objectToFocusAfterClickOnTree; // A dirty workaround for wxWidget tree events bug

	// Global Sizers
	wxBoxSizer *MainArea;
	wxBoxSizer *TreeAndNodesArea;
	wxBoxSizer *TreeArea;
	wxBoxSizer *NodeContentArea;
	wxBoxSizer *NodeHeaderArea;
	wxBoxSizer *NodeCentralArea;
	wxBoxSizer *NodeFooterArea;
	wxBoxSizer *BottomButtonsArea;

	// Containers / Scrollbars / sizers for nodes content.
	wxBoxSizer *EmptyNodeArea; // A sizer to display a message for invalid/empty nodes (please choose another node...)
	//wxBoxSizer *NodeScrollContainer_Main[BC_COUNT]; // Table of containers (sizers that contain the scrollbars) for Main node screens
	wxBoxSizer *NodeScrollContainer_Detail[BC_COUNT]; // Table of containers (sizers that contain the scrollbars) for detailed node screens
	//wxScrolledWindow *NodeScroller_Main[BC_COUNT]; // Table of scrollbars for Main node screens
	wxScrolledWindow *NodeScroller_Detail[BC_COUNT]; // Table of scrollbars for Detailed node screens
	//wxBoxSizer *NodeScrolledSizer_Main[BC_COUNT]; // Table of sizers (under the scrollbars) for Main node screens. Put UI objects in there.
	wxFlexGridSizer *NodeScrolledSizer_Detailed[BC_COUNT]; // Table of sizers (under the scrollbars) for Detailed node screens. Put UI objects in there.
	wxBoxSizer *NodeSizer_Pending;
	wxBoxSizer *NodeSizer_General; // Common for all "general" nodes (with the Enable Feature checkbox).
	vector<wxButton*> NodeButtons0[BC_COUNT]; // For each category, store "button 0" (button for user choice 0). We can find button1 pointer in AdvancedSeqButtonInfo data.

	// Other UI objects
	wxCheckBox *cbxEnableFeature;
	wxTextCtrl *edtPendingChanges;
	wxButton *btnUndoAll;
	wxButton *btnSave;
	wxButton *btnClose;
	wxButton *btnResetToDefault;
	wxTreeCtrl *treeView;
	wxTreeItemId TI_Root;
	wxTreeItemId TI_TechFixes;
	wxTreeItemId TI_TechFixes_dtl;
	wxTreeItemId TI_Resolution;
	wxTreeItemId TI_Resolution_dtl;
	wxTreeItemId TI_WndMode;
	wxTreeItemId TI_WndMode_dtl;
	wxTreeItemId TI_SelectedUnits;
	wxTreeItemId TI_SelectedUnits_dtl;
	wxTreeItemId TI_ROR_API;
	wxTreeItemId TI_ROR_API_dtl;
	wxTreeItemId TI_ManageAI;
	wxTreeItemId TI_ManageAI_dtl;
	wxTreeItemId TI_AudioVideo;
	wxTreeItemId TI_AudioVideo_dtl;
	wxTreeItemId TI_Options;
	wxTreeItemId TI_Obsolete;
	wxTreeItemId TI_Obsolete_VEG;
	wxTreeItemId TI_Obsolete_dtl;
	wxTreeItemId TI_Pending;
	wxStaticText *lblNodeHeader;
	wxStaticText *lblSimpleNodeHelp;
	
	void CancelPending();
	void SaveToFile();
	void ResetNodeToDefault();
	void ChangeNode();
	void ShowAdvancedNode(BINSEQ_CATEGORIES category);
	// Show a node's content (switch selection in treeview, or proceed to refresh...)
	// Will set focus to "focusedControl" object if not NULL
	void ShowNodeContent(wxControl *focusedControl = NULL);
	void CreateNodesContent();
	void CreateDetailedNodeContent(BINSEQ_CATEGORIES category);
	void ComputeButtonColor(wxColour &col0, wxColour &col1, int userChoice, int fileSelection);
	void UpdateButtonColor(wxButton *btn, bool clickOnButton);
	void DoROR_API_checks();

	void OnClose(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnUndoAll(wxCommandEvent& event);
	void OnResetToDefault(wxCommandEvent& event);
	void OnTreeSelChange(wxCommandEvent& event);
	void OnTreeGetInfo(wxCommandEvent& event);
	void OnEnableFeatureClick(wxCommandEvent& event);
	void OnFuncMeaningButtonClick(wxCommandEvent& event);
	void OnValueLinkClick(wxCommandEvent& event);

	void OnContextMenu(wxContextMenuEvent& event);// For some reason this does not work. EVT_CONTEXT_MENU won't compile, wxCommandEventHandler(WxEditFile::OnContextMenu2)) neither
	void OnDescContextMenu_Fake(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

};


// EditFile component IDs
enum EF_ComponentIDs
{
	ID_CLOSE = 1,
	ID_SAVE = 2,
	ID_UNDO_ALL = 3,
	ID_RESET_TO_DEFAULT = 4,
	ID_TREE = 5,
	ID_CBX_ENABLE_FEATURE = 6,
	ID_EDT_PENDING = 7
};


#pragma warning(pop)

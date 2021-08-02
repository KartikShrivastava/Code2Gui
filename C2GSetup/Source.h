///     For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

///     for all others, include the necessary headers (this file is usually all you
///     need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

///     the application icon (under Windows it is in resources and even
///     though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif

#include "wx/simplebook.h"
#include "wx/stattext.h"

#include <fstream>

///     Version history
///     1.0.0
#define SETUP_VERSION "1.0.0"

#define MAXIMIZED_BUTTON_PAGE_NAME      "Maximized button"

/// <summary>
/// IDs for the controls and the menu commands
/// </summary>
enum ID_COMMANDS
{
    ID_NEXT_PAGE_1_TO_2 = wxID_HIGHEST + 1,
    ID_PREVIOUS_PAGE_2_TO_1,
    ID_CANCEL_SETUP,
    ID_INSTALL_SETUP,
    ID_FILE_BROWSER_1,
    ID_FILE_BROWSER_2,
    ID_CHECKBOX_X86,
    ID_CHECKBOX_X86_D,
    ID_CHECKBOX_X86_R,
    ID_CHECKBOX_X64,
    ID_CHECKBOX_X64_D,
    ID_CHECKBOX_X64_R
};

/// <summary>
/// Define a new application type, each program should derive a class from wxApp
/// </summary>
class SetupApp : public wxApp
{
private:

public:
    /// <summary>
    /// 'Main program' equivalent: the program execution "starts" here
    /// this one is called on application startup and is a good place for the app
    /// initialization (doing it here and not in the ctor allows to have an error
    /// return: if OnInit() returns false, the application terminates)
    /// </summary>
    virtual bool OnInit() wxOVERRIDE;

    ~SetupApp();
};

/// <summary>
/// Define a new frame type: this is going to be our main frame
/// </summary>
class SetupFrame : public wxFrame
{
private:
    bool incompleteFields = false;

public:
    wxPanel* panelMain;
    wxBookCtrlBase* bookCtrl;
    wxBoxSizer* sizerFrame;
    wxImageList* imageList;
    wxButton* next;
    wxTextCtrl* textCtrlVcxprojPath;
    wxTextCtrl* textCtrlCppPath;
    wxTextCtrl* textCtrlTitle;
    wxCheckBox* checkBoxX86;
    wxCheckBox* checkBoxX86Release;
    wxCheckBox* checkBoxX86Debug;
    wxCheckBox* checkBoxX64;
    wxCheckBox* checkBoxX64Release;
    wxCheckBox* checkBoxX64Debug;

    /// <summary>
    /// Frame constructor
    /// </summary>
    /// <param name="title">Window title</param>
    SetupFrame(const wxString& title);

    ~SetupFrame();

    /// <summary>
    /// Event handler
    /// </summary>
    /// <param name="WXUNUSED">Predefined event handle</param>
    void OnQuit(wxCommandEvent& event);

    /// <summary>
    /// Event handler
    /// </summary>
    /// <param name="WXUNUSED">Predefined event handle</param>
    void OnAbout(wxCommandEvent& event);

    void CreateUi();

    void RecreateBook();

    int GetIconIndex(wxBookCtrlBase* bookCtrl);

    wxPanel* CreatePage(wxBookCtrlBase* parent, const wxString& pageName);

    void CreateInitialPages(wxBookCtrlBase* parent);

    wxPanel* CreateFirstPage(wxBookCtrlBase* parent);

    wxPanel* CreateSecondPage(wxBookCtrlBase* parent);

    void OnNextPage(wxCommandEvent& event);

    wxBookCtrlBase* GetCurrentBook() const { return bookCtrl; }
    
    void OnCancelSetup(wxCommandEvent& event);
    
    void OnPreviousPage(wxCommandEvent& event);
    
    void OnInstall(wxCommandEvent& event);

    void OnFileBrowser(wxCommandEvent& event);

    std::string GetExtension(const std::string& path);

    void OnCheckCheckBoxX86(wxCommandEvent& event);
    void OnCheckCheckBoxX86Debug(wxCommandEvent& event);
    void OnCheckCheckBoxX86Release(wxCommandEvent& event);

    void OnCheckCheckBoxX64(wxCommandEvent& event);
    void OnCheckCheckBoxX64Debug(wxCommandEvent& event);
    void OnCheckCheckBoxX64Release(wxCommandEvent& event);

    std::string RemoveSpaces(const std::string& line);

    std::string GetDiretoryFromPath(const std::string& path);

    ///     Any class wishing to process wxWidgets events must use this macro
    //wxDECLARE_EVENT_TABLE();
};

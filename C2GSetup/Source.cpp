#include "Source.h"
#include "wx/artprov.h"
#include "wx/treectrl.h" 


///     -----------------------------------------------------------
///     event tables and other macros for wxWidgets
///     -----------------------------------------------------------

///     The event tables connect the wxWidgets events with the functions (event
///     handlers) which process them. It can be also done at run-time, but for the
///     simple menu events like this the static method is much simpler.

///     Create a new application object: this macro will allow wxWidgets to create
///     the application object during program execution (it's better than using a
///     static object for many reasons) and also implements the accessor function
///     wxGetApp() which will return the reference of the right type (i.e. MyApp and
///     not wxApp)
wxIMPLEMENT_APP(SetupApp);


///     ------------------------------------------------------------
///     SetupApp(wxApp) class definitions
///     ------------------------------------------------------------

bool SetupApp::OnInit() {
    // Call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if (!wxApp::OnInit())
        return false;

    // Create the main application window
    SetupFrame* frame = new SetupFrame("CodeToGui " + wxString(SETUP_VERSION) + " Setup");
    // And show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);
    //SetTopWindow(frame);
    // Success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

SetupApp::~SetupApp() {
}

///     ------------------------------------------------------------
///     SetupFrame(wxFrame) class definitions
///     ------------------------------------------------------------

SetupFrame::SetupFrame(const wxString& title) : wxFrame(NULL, -1, title, wxDefaultPosition, { 400, 500 },
    wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCAPTION | wxCLIP_CHILDREN) {
    ///     Set the frame icon
    //SetIcon(wxICON(sample));

    #if wxUSE_STATUSBAR
    ///     Create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
    SetStatusText("Welcome to CodeToGui setup!");
    #endif ///  wxUSE_STATUSBAR

    CreateUi();
    //CreateFirstPage(bookCtrl);
    //Connect(wxEVT_CLOSE_WINDOW, wxCommandEventHandler(SetupFrame::OnQuit));
}

SetupFrame::~SetupFrame() {
}

void SetupFrame::OnQuit(wxCommandEvent& event) {
    ///     True is to force the frame to close
    Close(true);
}

void SetupFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageBox(wxString::Format
    (
        "Welcome to %s!\n"
        "\n"
        "This is the minimal wxWidgets sample\n"
        "running under %s.",
        wxVERSION_STRING,
        wxGetOsDescription()
    ),
        "About wxWidgets minimal sample",
        wxOK | wxICON_INFORMATION,
        this);
}

void SetupFrame::CreateUi() {

    // create a dummy image list with a few icons
    //const wxSize imageSize(32, 32);
    //
    //imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
    //imageList->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, imageSize));
    //imageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
    //imageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
    //imageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));

    panelMain = new wxPanel(this, -1);
    sizerFrame = new wxBoxSizer(wxVERTICAL);

    bookCtrl = NULL;

    RecreateBook();

    panelMain->SetSizer(sizerFrame);
    Centre();
}

void SetupFrame::RecreateBook() {
    int flags;
    flags = wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCAPTION | wxCLIP_CHILDREN;

    wxBookCtrlBase* oldBook = bookCtrl;

    bookCtrl = NULL;

    if (!bookCtrl) {
        bookCtrl = new wxSimplebook(panelMain, wxID_ANY, wxDefaultPosition, { 400, 500 },
            wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCAPTION | wxCLIP_CHILDREN);
    }

    if (!bookCtrl)
        return;

    bookCtrl->Hide();

    if (oldBook) {
        const int count = oldBook->GetPageCount();
        for (int n = 0; n < count; n++) {
            const int image = GetIconIndex(bookCtrl);
            const wxString str = oldBook->GetPageText(n);

            wxWindow* page = CreatePage(bookCtrl, str);

            bookCtrl->AddPage(page, str, false, image);
        }

        const int sel = oldBook->GetSelection();
        if (sel != wxNOT_FOUND)
            bookCtrl->SetSelection(sel);

        sizerFrame->Detach(oldBook);
        delete oldBook;
    }
    else
        CreateInitialPages(bookCtrl);

    sizerFrame->Insert(0, bookCtrl);

    sizerFrame->Show(bookCtrl);
    //sizerFrame->Layout();
}

int SetupFrame::GetIconIndex(wxBookCtrlBase* bookCtrl) {
    if (bookCtrl && bookCtrl->GetImageList())
    {
        int nImages = bookCtrl->GetImageList()->GetImageCount();
        if (nImages > 0)
        {
            return bookCtrl->GetPageCount() % nImages;
        }
    }

    return -1;
}

wxPanel* SetupFrame::CreatePage(wxBookCtrlBase* parent, const wxString& pageName) {
    //if (pageName == MAXIMIZED_BUTTON_PAGE_NAME)
    //    return CreateBigButtonPage(parent);

    wxFAIL_MSG("unknown page name");

    return NULL;
}

void SetupFrame::CreateInitialPages(wxBookCtrlBase* parent) {
    ///     Create and add some panels to the notebook

    wxPanel* panelTmp = CreateFirstPage(parent);
    parent->AddPage(panelTmp, MAXIMIZED_BUTTON_PAGE_NAME, false, GetIconIndex(parent));

    panelTmp = CreateSecondPage(parent);
    parent->AddPage(panelTmp, MAXIMIZED_BUTTON_PAGE_NAME, false, GetIconIndex(parent));

    parent->SetSelection(0);
}

wxPanel* SetupFrame::CreateFirstPage(wxBookCtrlBase* parent) {
    wxColour col1, col2, col3;
    col1.Set(wxT("#4f5049"));
    col2.Set(wxT("#ededed"));
    col3.Set(wxT("#ffffff"));

    wxPanel* panelMain = new wxPanel(parent);

    wxBoxSizer* vboxMain = new wxBoxSizer(wxVERTICAL);
        wxPanel* panel1 = new wxPanel(panelMain, wxID_ANY);
            wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
            vbox1->Add(-1, 5);
                wxStaticText* stText1 = new wxStaticText(panel1, wxID_ANY, wxT("CodeToGui project integration"));
                    wxFont font1 = stText1->GetFont();
                    font1.SetPointSize(9);
                    font1.SetWeight(wxFONTWEIGHT_BOLD);
                    stText1->SetFont(font1);
            vbox1->Add(stText1,0,wxLEFT,10);
            vbox1->Add(-1, 5);
                wxStaticText* stText2 = new wxStaticText(panel1, wxID_ANY, 
                    wxT("Visual Studio Project file(.vcxproj) will be edited\n"
                        "for auto integration.\n"
                        "Following edits in project will take place:\n"
                        "  - Addition of a source file for GUI compilation.\n"
                        "  - Addition of a text file to store metadata.\n"
                        "  - \"Property Sheets\" updation to add WxWidgets."));
            vbox1->Add(stText2, 0, wxLEFT, 10);
            vbox1->Add(-1, 10);
        panel1->SetBackgroundColour(col3);
        panel1->SetSizer(vbox1);
    vboxMain->Add(panel1, 2, wxEXPAND);
        wxStaticBox* staticBox1 = new wxStaticBox(panelMain, wxID_ANY, "http://codetogui.com");
            wxFont font2 = staticBox1->GetFont();
            font2.SetPointSize(8);
            font2.SetStyle(wxFONTSTYLE_ITALIC);
            font2.SetWeight(wxFONTWEIGHT_THIN);
            staticBox1->SetFont(font2);
        wxStaticBoxSizer* vbox2 = new wxStaticBoxSizer(staticBox1, wxVERTICAL);
            wxStaticText* stText3 = new wxStaticText(panelMain, wxID_ANY, wxT("Specify .vcxproj file path to add CodeToGui functionality in it:"));
            wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
                textCtrlVcxprojPath = new wxTextCtrl(panelMain, wxID_ANY);
                wxButton* btn1 = new wxButton(panelMain, ID_COMMANDS::ID_FILE_BROWSER_1, wxT("..."), wxDefaultPosition, { 30,wxDefaultSize.y });
                btn1->Connect(ID_COMMANDS::ID_FILE_BROWSER_1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnFileBrowser), NULL, this);
            hbox2->Add(textCtrlVcxprojPath, 1);
            hbox2->Add(btn1);
        vbox2->Add(stText3,0,wxLEFT|wxRIGHT|wxTOP,10);
        vbox2->Add(-1, 5);
        vbox2->Add(hbox2, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
        vbox2->Add(-1, 10);
            wxStaticText* stText4 = new wxStaticText(panelMain, wxID_ANY, wxT("Specify path of cpp file which has main method in selected\n.vcxproj:"));
            wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
                textCtrlCppPath = new wxTextCtrl(panelMain, wxID_ANY);
                wxButton* btn2 = new wxButton(panelMain, ID_COMMANDS::ID_FILE_BROWSER_2, wxT("..."), wxDefaultPosition, { 30,wxDefaultSize.y });
                btn2->Connect(ID_COMMANDS::ID_FILE_BROWSER_2, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnFileBrowser), NULL, this);
            hbox3->Add(textCtrlCppPath, 1);
            hbox3->Add(btn2);
        vbox2->Add(stText4, 0, wxLEFT | wxRIGHT, 10);
        vbox2->Add(-1, 5);
        vbox2->Add(hbox3, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
        vbox2->Add(-1, 10);
            wxStaticText* stText5 = new wxStaticText(panelMain, wxID_ANY, wxT("Enter the title of test GUI:"));
            wxBoxSizer* hbox4 = new wxBoxSizer(wxHORIZONTAL);
                textCtrlTitle = new wxTextCtrl(panelMain, wxID_ANY);
            hbox4->Add(textCtrlTitle,1);
        vbox2->Add(stText5, 0, wxLEFT | wxRIGHT, 10);
        vbox2->Add(-1, 5);
        vbox2->Add(hbox4, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
        vbox2->Add(-1, 1, wxEXPAND);
            wxBoxSizer* hbox5 = new wxBoxSizer(wxHORIZONTAL);
                next = new wxButton(panelMain, ID_COMMANDS::ID_NEXT_PAGE_1_TO_2, wxT("Next"));
                next->Connect(ID_COMMANDS::ID_NEXT_PAGE_1_TO_2, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnNextPage), NULL, this);
                wxButton* cancel = new wxButton(panelMain, ID_COMMANDS::ID_CANCEL_SETUP, wxT("Cancel"));
                cancel->Connect(ID_COMMANDS::ID_CANCEL_SETUP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnCancelSetup), NULL, this);
            hbox5->Add(next,0,wxRIGHT, 10);
            hbox5->Add(cancel,0,wxRIGHT);
        vbox2->Add(hbox5,0,wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
    vboxMain->Add(vbox2,3, wxALIGN_TOP| wxEXPAND|wxALL, 10);

    panelMain->SetSizer(vboxMain);

    return panelMain;
}

wxPanel* SetupFrame::CreateSecondPage(wxBookCtrlBase* parent) {
    wxColour col1, col2, col3;
    col1.Set(wxT("#4f5049"));
    col2.Set(wxT("#ededed"));
    col3.Set(wxT("#ffffff"));
    
    wxPanel* panelMain = new wxPanel(parent);

    wxBoxSizer* vboxMain = new wxBoxSizer(wxVERTICAL);
        wxPanel* panel1 = new wxPanel(panelMain, wxID_ANY);
            wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
            vbox1->Add(-1, 5);
                wxStaticText* stText1 = new wxStaticText(panel1, wxID_ANY, wxT("Platform and Configration"));
                    wxFont font1 = stText1->GetFont();
                    font1.SetPointSize(9);
                    font1.SetWeight(wxFONTWEIGHT_BOLD);
                    stText1->SetFont(font1);
            vbox1->Add(stText1,0,wxLEFT,10);
            vbox1->Add(-1, 5);
                wxStaticText* stText2 = new wxStaticText(panel1, wxID_ANY, 
                    wxT("Un-check the platform and configuration for\n"
                        "which CodeToGui integration is not required.\n"
                        "\n"
                        "\n"
                        "\n"
                        ""));
            vbox1->Add(stText2, 0, wxLEFT, 10);
            vbox1->Add(-1, 10);
        panel1->SetBackgroundColour(col3);
        panel1->SetSizer(vbox1);
    vboxMain->Add(panel1, 1, wxEXPAND);
        wxStaticBox* staticBox1 = new wxStaticBox(panelMain, wxID_ANY, "http://codetogui.com");
            wxFont font2 = staticBox1->GetFont();
            font2.SetPointSize(8);
            font2.SetStyle(wxFONTSTYLE_ITALIC);
            font2.SetWeight(wxFONTWEIGHT_THIN);
            staticBox1->SetFont(font2);
        wxStaticBoxSizer* vbox2 = new wxStaticBoxSizer(staticBox1, wxVERTICAL);
            wxPanel* panel2 = new wxPanel(panelMain, -1,wxDefaultPosition,wxDefaultSize, wxSIMPLE_BORDER);
                wxBoxSizer* vbox3 = new wxBoxSizer(wxVERTICAL);
                    checkBoxX86 = new wxCheckBox(panel2, ID_COMMANDS::ID_CHECKBOX_X86, "x86", wxDefaultPosition,wxDefaultSize, wxCHK_3STATE);
                    checkBoxX86->Connect(ID_COMMANDS::ID_CHECKBOX_X86, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SetupFrame::OnCheckCheckBoxX86), NULL, this);
                    checkBoxX86->Set3StateValue(wxCHK_CHECKED);
                    checkBoxX86Debug = new wxCheckBox(panel2, ID_COMMANDS::ID_CHECKBOX_X86_D, "Debug");
                    checkBoxX86Debug->Connect(ID_COMMANDS::ID_CHECKBOX_X86_D, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SetupFrame::OnCheckCheckBoxX86Debug), NULL, this);
                    checkBoxX86Debug->SetValue(wxCHK_CHECKED);
                    checkBoxX86Release = new wxCheckBox(panel2, ID_COMMANDS::ID_CHECKBOX_X86_R, "Release");
                    checkBoxX86Release->Connect(ID_COMMANDS::ID_CHECKBOX_X86_R, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SetupFrame::OnCheckCheckBoxX86Release), NULL, this);
                    checkBoxX86Release->SetValue(wxCHK_CHECKED);
                    checkBoxX64 = new wxCheckBox(panel2, ID_COMMANDS::ID_CHECKBOX_X64, "x64", wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);
                    checkBoxX64->Connect(ID_COMMANDS::ID_CHECKBOX_X64, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SetupFrame::OnCheckCheckBoxX64), NULL, this);
                    checkBoxX64->Set3StateValue(wxCHK_CHECKED);
                    checkBoxX64Debug = new wxCheckBox(panel2, ID_COMMANDS::ID_CHECKBOX_X64_D, "Debug");
                    checkBoxX64Debug->Connect(ID_COMMANDS::ID_CHECKBOX_X64_D, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SetupFrame::OnCheckCheckBoxX64Debug), NULL, this);
                    checkBoxX64Debug->SetValue(wxCHK_CHECKED);
                    checkBoxX64Release = new wxCheckBox(panel2, ID_COMMANDS::ID_CHECKBOX_X64_R, "Release");
                    checkBoxX64Release->Connect(ID_COMMANDS::ID_CHECKBOX_X64_R, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SetupFrame::OnCheckCheckBoxX64Release), NULL, this);
                    checkBoxX64Release->SetValue(wxCHK_CHECKED);
                vbox3->Add(-1,20);
                vbox3->Add(checkBoxX86, 0, wxLEFT, 10);
                vbox3->Add(-1,5);
                vbox3->Add(checkBoxX86Debug, 0, wxLEFT, 30);
                vbox3->Add(-1,5);
                vbox3->Add(checkBoxX86Release, 0, wxLEFT, 30);
                vbox3->Add(-1,10);
                vbox3->Add(checkBoxX64, 0, wxLEFT, 10);
                vbox3->Add(-1,5);
                vbox3->Add(checkBoxX64Debug, 0, wxLEFT, 30);
                vbox3->Add(-1,5);
                vbox3->Add(checkBoxX64Release, 0, wxLEFT, 30);
                vbox3->Add(-1,20);
            panel2->SetBackgroundColour(col3);
            panel2->SetSizer(vbox3);
        vbox2->Add(panel2,1,wxEXPAND|wxALL, 10);
        vbox2->Add(-1, 1, wxEXPAND);
            wxBoxSizer* hbox5 = new wxBoxSizer(wxHORIZONTAL);
                wxButton* back = new wxButton(panelMain, ID_COMMANDS::ID_PREVIOUS_PAGE_2_TO_1, wxT("Back"));
                back->Connect(ID_COMMANDS::ID_PREVIOUS_PAGE_2_TO_1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnPreviousPage), NULL, this);
                wxButton* install = new wxButton(panelMain, ID_COMMANDS::ID_INSTALL_SETUP, wxT("Install"));
                install->Connect(ID_COMMANDS::ID_INSTALL_SETUP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnInstall), NULL, this);
                wxButton* cancel = new wxButton(panelMain, ID_COMMANDS::ID_CANCEL_SETUP, wxT("Cancel"));
                cancel->Connect(ID_COMMANDS::ID_CANCEL_SETUP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnCancelSetup), NULL, this);
            hbox5->Add(back);
            hbox5->Add(install,0,wxRIGHT, 10);
            hbox5->Add(cancel,0,wxRIGHT);
        vbox2->Add(hbox5,0,wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
    vboxMain->Add(vbox2, 3, wxEXPAND | wxALL, 10);

    panelMain->SetSizer(vboxMain);

    return panelMain;
}

void SetupFrame::OnNextPage(wxCommandEvent& event) {
    switch (event.GetId()) {
        case int(ID_NEXT_PAGE_1_TO_2) : {
            std::string extension = GetExtension(std::string(textCtrlVcxprojPath->GetValue()));
            incompleteFields = false;
            if (extension.compare(".vcxproj") != 0) {
                textCtrlVcxprojPath->SetHint(wxT(".vcxproj file path required"));
                incompleteFields = true;
            }
            extension = GetExtension(std::string(textCtrlCppPath->GetValue()));
            if (extension.compare(".cpp") != 0) {
                textCtrlCppPath->SetHint(wxT(".cpp file path required"));
                incompleteFields = true;
            }
            if (std::string(textCtrlTitle->GetValue()).compare("") == 0) {
                textCtrlTitle->SetHint(wxT("Title required"));
                incompleteFields = true;
            }
            if (!incompleteFields) {
                SetStatusText("Some fields are missing");
                return;
            }
            else {
                SetStatusText("Ready to go!");
            }
        }
        break;
        case int(ID_PREVIOUS_PAGE_2_TO_1) : {
            SetStatusText("Check and Update the fields if required");
        }
        break;
    default:
        break;
    }

    wxBookCtrlBase* currBook = GetCurrentBook();
    
    if (currBook)
        currBook->AdvanceSelection();
}

void SetupFrame::OnPreviousPage(wxCommandEvent& event) {
    wxBookCtrlBase* currBook = GetCurrentBook();

    switch (event.GetId()) {
    case int(ID_PREVIOUS_PAGE_2_TO_1) : {
        SetStatusText("Verify and update the fields if required");
    }
    break;
    default:
        break;
    }

    if (currBook)
        currBook->AdvanceSelection(false);
}

void SetupFrame::OnCancelSetup(wxCommandEvent& event) {
    if (wxMessageBox
    (
        "Are you sure you do not want to complete integration?\n"
        "",
        "Exit Setup",
        wxICON_QUESTION | wxYES_NO,
        this
    ) == wxYES)
    {
        Close(true);
    }
}

void SetupFrame::OnFileBrowser(wxCommandEvent& event) {
    switch (event.GetId()) {
        case int(ID_FILE_BROWSER_1) : {
            wxFileDialog openFileDialog(this, _("Open vcxproj file"), "", "",
                "vcxproj files (*.vcxproj)|*.vcxproj", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (openFileDialog.ShowModal() == wxID_CANCEL)
                return;
            *textCtrlVcxprojPath << openFileDialog.GetPath();
        }
        break;
        case int(ID_FILE_BROWSER_2):{
            wxFileDialog openFileDialog(this, _("Open cpp file"), "", "",
                "cpp files (*.cpp)|*.cpp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (openFileDialog.ShowModal() == wxID_CANCEL)
                return;
            *textCtrlCppPath << openFileDialog.GetPath();
        }
        break;
    default:
        break;
    }
}

std::string SetupFrame::GetExtension(const std::string& path) {
    size_t pos = path.find_last_of(".");
    return (pos == std::string::npos) ? "" : path.substr(pos, path.length() + 1);
}

void SetupFrame::OnCheckCheckBoxX86(wxCommandEvent& event) {
    if (checkBoxX86->Get3StateValue() == wxCHK_UNDETERMINED) {
        checkBoxX86Debug->SetValue(false);
        checkBoxX86Release->SetValue(false);
    }
    else if(checkBoxX86->Get3StateValue() == wxCHK_CHECKED) {
        checkBoxX86Debug->SetValue(true);
        checkBoxX86Release->SetValue(true);
    }
    else if (checkBoxX86->Get3StateValue() == wxCHK_UNCHECKED) {
        checkBoxX86Debug->SetValue(false);
        checkBoxX86Release->SetValue(false);
    }
}

void SetupFrame::OnCheckCheckBoxX86Debug(wxCommandEvent& event) {
    if (checkBoxX86Debug->GetValue() == wxCHK_UNCHECKED) {
        if (checkBoxX86Release->GetValue() == wxCHK_UNCHECKED)
            checkBoxX86->SetValue(false);
        else if (checkBoxX86Release->GetValue() == wxCHK_CHECKED)
            checkBoxX86->Set3StateValue(wxCHK_UNDETERMINED);
    }
    else if (checkBoxX86Debug->GetValue() == wxCHK_CHECKED) {
        if (checkBoxX86Release->GetValue() == wxCHK_CHECKED)
            checkBoxX86->SetValue(true);
        else if (checkBoxX86Release->GetValue() == wxCHK_UNCHECKED)
            checkBoxX86->Set3StateValue(wxCHK_UNDETERMINED);
    }
}

void SetupFrame::OnCheckCheckBoxX86Release(wxCommandEvent& event) {
    if (checkBoxX86Release->GetValue() == wxCHK_UNCHECKED) {
        if (checkBoxX86Debug->GetValue() == wxCHK_UNCHECKED)
            checkBoxX86->SetValue(false);
        else if (checkBoxX86Debug->GetValue() == wxCHK_CHECKED)
            checkBoxX86->Set3StateValue(wxCHK_UNDETERMINED);
    }
    else if (checkBoxX86Release->GetValue() == wxCHK_CHECKED) {
        if (checkBoxX86Debug->GetValue() == wxCHK_CHECKED)
            checkBoxX86->SetValue(true);
        else if (checkBoxX86Debug->GetValue() == wxCHK_UNCHECKED)
            checkBoxX86->Set3StateValue(wxCHK_UNDETERMINED);
    }
}

void SetupFrame::OnCheckCheckBoxX64(wxCommandEvent& event) {
    if (checkBoxX64->Get3StateValue() == wxCHK_UNDETERMINED) {
        checkBoxX64Debug->SetValue(false);
        checkBoxX64Release->SetValue(false);
    }
    else if (checkBoxX64->Get3StateValue() == wxCHK_CHECKED) {
        checkBoxX64Debug->SetValue(true);
        checkBoxX64Release->SetValue(true);
    }
    else if (checkBoxX64->Get3StateValue() == wxCHK_UNCHECKED) {
        checkBoxX64Debug->SetValue(false);
        checkBoxX64Release->SetValue(false);
    }
}

void SetupFrame::OnCheckCheckBoxX64Debug(wxCommandEvent& event) {
    if (checkBoxX64Debug->GetValue() == wxCHK_UNCHECKED) {
        if (checkBoxX64Release->GetValue() == wxCHK_UNCHECKED)
            checkBoxX64->SetValue(false);
        else if (checkBoxX64Release->GetValue() == wxCHK_CHECKED)
            checkBoxX64->Set3StateValue(wxCHK_UNDETERMINED);
    }
    else if (checkBoxX64Debug->GetValue() == wxCHK_CHECKED) {
        if (checkBoxX64Release->GetValue() == wxCHK_CHECKED)
            checkBoxX64->SetValue(true);
        else if (checkBoxX64Release->GetValue() == wxCHK_UNCHECKED)
            checkBoxX64->Set3StateValue(wxCHK_UNDETERMINED);
    }
}

void SetupFrame::OnCheckCheckBoxX64Release(wxCommandEvent& event) {
    if (checkBoxX64Release->GetValue() == wxCHK_UNCHECKED) {
        if (checkBoxX64Debug->GetValue() == wxCHK_UNCHECKED)
            checkBoxX64->SetValue(false);
        else if (checkBoxX64Debug->GetValue() == wxCHK_CHECKED)
            checkBoxX64->Set3StateValue(wxCHK_UNDETERMINED);
    }
    else if (checkBoxX64Release->GetValue() == wxCHK_CHECKED) {
        if (checkBoxX64Debug->GetValue() == wxCHK_CHECKED)
            checkBoxX64->SetValue(true);
        else if (checkBoxX64Debug->GetValue() == wxCHK_UNCHECKED)
            checkBoxX64->Set3StateValue(wxCHK_UNDETERMINED);
    }
}

std::string SetupFrame::RemoveSpaces(const std::string& line) {
    std::string stringNoSpaces = "";
    for (int i = 0; i < line.length(); ++i)
        if (line[i] != '\t' && line[i] != ' ')
            stringNoSpaces += line[i];
    return stringNoSpaces;
}

std::string SetupFrame::GetDiretoryFromPath(const std::string& path) {
    size_t pos = path.find_last_of("\\/");
    return (pos == std::string::npos) ? "" : path.substr(0, pos);
}

void SetupFrame::OnInstall(wxCommandEvent& event) {
    ///     Edit the .vcxproj and .vcxproj.filters file to add
    ///     1. wxwidgets property sheet
    ///     2. C2GMetadata.txt and C2GWxWidgets.cpp file

    ///     Add Custom Build Tool instructions to the C2GMetadata.txt
    
    ///     Add wxwidget dll path to user environment variable

}

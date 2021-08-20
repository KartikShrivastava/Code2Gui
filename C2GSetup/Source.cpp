#include "Source.h"
#include "pics/icon_64x64.xpm"

#include "wx/artprov.h"
#include "wx/treectrl.h" 

#include <fstream>
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

    frame->SetIcon(icon_codetogui64x64_xpm);
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
            wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
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
                        wxT("For auto-integration, CodeToGui will make\n"
                            "following changes in VC++ Project &\n"
                            "VC++ Project Filters files:\n"
                            "        - Add WxWidgets config to Property Sheets\n"
                            "        - Add C2GWxWidgets.cpp file to project\n"
                            "        - Add C2GMetadata.txt file to project"));
                vbox1->Add(stText2, 0, wxLEFT, 10);
                vbox1->Add(-1, 10);
            hbox1->Add(vbox1, 0, wxRIGHT,5);
                wxPNGHandler* handler = new wxPNGHandler;
                wxImage::AddHandler(handler);
                wxStaticBitmap* image;
                image = new wxStaticBitmap(panel1, wxID_ANY, wxBitmap("pics/logo_96x96.png", wxBITMAP_TYPE_PNG), wxPoint(-1, -1), wxSize(96, 96));
            hbox1->Add(image,0,wxTOP,7);
        panel1->SetBackgroundColour(col3);
        panel1->SetSizer(hbox1);
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
            wxStaticText* stText5 = new wxStaticText(panelMain, wxID_ANY, wxT("Enter GUI window title:"));
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
            wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
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
                            "NOTE:For CodeToGui integration at-least one\n"
                            "           of the following configuration should\n"
                            "           be present in your project."));
                vbox1->Add(stText2, 0, wxLEFT, 10);
                vbox1->Add(-1, 10);
            hbox1->Add(vbox1, 0, wxRIGHT,20);
                wxPNGHandler* handler = new wxPNGHandler;
                wxImage::AddHandler(handler);
                wxStaticBitmap* image;
                image = new wxStaticBitmap(panel1, wxID_ANY, wxBitmap("pics/logo_96x96.png", wxBITMAP_TYPE_PNG), wxPoint(-1, -1), wxSize(96, 96));
            hbox1->Add(image,0,wxTOP,7);
        panel1->SetBackgroundColour(col3);
        panel1->SetSizer(hbox1);
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
                    checkBoxX86 = new wxCheckBox(panel2, ID_COMMANDS::ID_CHECKBOX_X86, "x86", wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);
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
                installBtnRef = new wxButton(panelMain, ID_COMMANDS::ID_INSTALL_SETUP, wxT("Install"));
                installBtnRef->Connect(ID_COMMANDS::ID_INSTALL_SETUP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnInstall), NULL, this);
                cancelBtnRef = new wxButton(panelMain, ID_COMMANDS::ID_CANCEL_SETUP, wxT("Cancel"));
                cancelBtnRef->Connect(ID_COMMANDS::ID_CANCEL_SETUP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupFrame::OnCancelSetup), NULL, this);
            hbox5->Add(back);
            hbox5->Add(installBtnRef,0,wxRIGHT, 10);
            hbox5->Add(cancelBtnRef,0,wxRIGHT);
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

            if (incompleteFields) {
                SetStatusText("Some fields are missing");
                return;
            }
            else {
                bool fileExist = FindProjectConfigurations();
                if (fileExist == false) {
                    SetStatusText("Invalid vcxproj file, check path");
                    return;
                }
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
    if (integrationSuccessful) {
        Close(true);
    }
    else {
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

bool SetupFrame::FindProjectConfigurations() {
    std::string vcxprojFilePath = std::string(textCtrlVcxprojPath->GetValue());
    std::ifstream vcxprojFile(vcxprojFilePath);

    if (!vcxprojFile.is_open()) {
        return false;
    }

    std::string line;
    foundX86 = false;
    foundX86Debug = false;
    foundX86Release = false;
    foundX64 = false;
    foundX64Debug = false;
    foundX64Release = false;

    bool closingItemGroupTag = false;
    while (std::getline(vcxprojFile, line) && !closingItemGroupTag) {
        std::string lineNoSpaces = RemoveSpaces(line);
        if (strstr(lineNoSpaces.c_str(), "<ItemGroupLabel=\"ProjectConfigurations\">") != NULL) {
            while (!closingItemGroupTag) {
                std::getline(vcxprojFile, line);
                if(strstr(line.c_str(),"Debug|Win32") !=NULL)
                    foundX86Debug = true;
                if(strstr(line.c_str(),"Release|Win32") !=NULL)
                    foundX86Release = true;
                if(strstr(line.c_str(),"Debug|x64") !=NULL)
                    foundX64Debug = true;
                if(strstr(line.c_str(),"Release|x64") !=NULL)
                    foundX64Release = true;

                if (strstr(line.c_str(), "</ItemGroup>") != NULL)
                    closingItemGroupTag = true;
            }
        }
    }
    vcxprojFile.close();

    if (foundX86Debug || foundX86Release)
        foundX86 = true;

    if (foundX64Debug || foundX64Release)
        foundX64 = true;

    ///     Update the second page GUI
    if (!foundX86) {
        checkBoxX86->Enable(false);
        checkBoxX86Debug->Enable(false);
        checkBoxX86Release->Enable(false);
        checkBoxX86->Set3StateValue(wxCHK_UNCHECKED);
        checkBoxX86Debug->SetValue(wxCHK_UNCHECKED);
        checkBoxX86Release->SetValue(wxCHK_UNCHECKED);
    }
    else {
        if (!foundX86Debug) {
            checkBoxX86Debug->Enable(false);
            checkBoxX86Debug->SetValue(wxCHK_UNCHECKED);
            checkBoxX86->Set3StateValue(wxCHK_UNDETERMINED);
        }
        if (!foundX86Release) {
            checkBoxX86Release->Enable(false);
            checkBoxX86Release->SetValue(wxCHK_UNCHECKED);
            checkBoxX86->Set3StateValue(wxCHK_UNDETERMINED);
        }
    }

    if (!foundX64) {
        checkBoxX64->Enable(false);
        checkBoxX64Debug->Enable(false);
        checkBoxX64Release->Enable(false);
        checkBoxX64->Set3StateValue(wxCHK_UNCHECKED);
        checkBoxX64Debug->SetValue(wxCHK_UNCHECKED);
        checkBoxX64Release->SetValue(wxCHK_UNCHECKED);
    }
    else {

        if (!foundX64Debug) {
            checkBoxX64Debug->Enable(false);
            checkBoxX64Debug->SetValue(wxCHK_UNCHECKED);
            checkBoxX64->Set3StateValue(wxCHK_UNDETERMINED);
        }
        if (!foundX64Release) {
            checkBoxX64Release->Enable(false);
            checkBoxX64Release->SetValue(wxCHK_UNCHECKED);
            checkBoxX64->Set3StateValue(wxCHK_UNDETERMINED);
        }
    }

    ///     Disable installation if conditions are not met
    if (!(foundX86 || foundX64))
        installBtnRef->Enable(false);

    return true;
}

void SetupFrame::OnInstall(wxCommandEvent& event) {
    ///     Edit the .vcxproj and .vcxproj.filters file to add
    ///     1. After clicking next from first page traverse the .vcxproj file and
    ///         find the configurations mentioned inside "ProjectConfigurations" label.
    ///         Make a string of all the configurations and compare it with following four:
    ///             Debug|Win32 Release|Win32 Debug|x64 Release|x64
    ///     2. Update the second page according to the matched "ProjectConfigurations",
    ///         if none is found then on pressing next button on same page say that 
    ///             "Code2GUI integration alteast on of the [four confugurations]".
    ///         if some or all matches then let user select them and press next button.
    ///     3. On pressing install button on third page
    ///         Traverse .vcxproj file again and create a backup if it with *.backup extension
    ///         Note: If backup file alraedy exist then dont create another and mess it up!
    ///         look for "PropertySheets" label
    ///             In each of the found label add following line inside it
    ///             <Import Project="..\..\..\ProjectsArchive\wxClientDynamicBinaries\wxwidgets.props" />
    ///         While traversing also search for <ItemGroup> which contains following symbol
    ///             ClCompile Include
    ///             At the end of parent <ItemGroup> add <ClCompile Include="C2GWxWidgets.cpp" />
    ///         Finally start searching for these symbols
    ///         <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
    ///         <ImportGroup Label="ExtensionTargets">
    ///         If first symbol is found then add following lines according to detected "ProjectConfigurations"
    ///             before it
    ///         If second symbol is found then add following lines ...
    ///         If none of the above two symbols are found then add lines before final </Project> tag
    ///         <ItemGroup>
    ///            <CustomBuild Include="C2GMetadata.txt">
    ///              <Message Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">Generating WxWidgets code</Message>
    ///              <Outputs Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">$(OutDir)\test.txt</Outputs>
    ///              <Message Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">Generating WxWidgets code</Message>
    ///              <Outputs Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">$(OutDir)\test.txt</Outputs>
    ///              <Message Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">Generating WxWidgets code</Message>
    ///              <Outputs Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">$(OutDir)\test.txt</Outputs>
    ///              <Message Condition="'$(Configuration)|$(Platform)'=='Release|x64'">Generating WxWidgets code</Message>
    ///              <Outputs Condition="'$(Configuration)|$(Platform)'=='Release|x64'">$(OutDir)\test.txt</Outputs>
    ///              <Command Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">"C:\Home\Projects\Github\Code2Gui\binCode2Gui\x64\Debug\build.bat"</Command>
    ///              <Command Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">"C:\Home\Projects\Github\Code2Gui\binCode2Gui\x64\Debug\build.bat"</Command>
    ///              <Command Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">"C:\Home\Projects\Github\Code2Gui\binCode2Gui\x64\Debug\build.bat"</Command>
    ///              <Command Condition="'$(Configuration)|$(Platform)'=='Release|x64'">"C:\Home\Projects\Github\Code2Gui\binCode2Gui\x64\Debug\build.bat"</Command>
    ///            </CustomBuild>
    ///          </ItemGroup>
    ///         Also traverse .vcxproj.filters file and create a backup of it with .backup extension
    ///         Note: If backup file alraedy exist then dont create another and mess it up!
    ///         Search for parent <ItemGroup> which contains <ClCompile include= as child 
    ///             Add following lines at the end of that parent item group
    ///             <ClCompile Include="C2GWxWidgets.cpp">
    ///                  <Filter>Source Files</Filter>
    ///             </ClCompile>
    ///         Add following lines before closing </Project> tag
    ///             <ItemGroup>
    ///               <CustomBuild Include="C2GMetadata.txt" />
    ///             </ItemGroup>
    /// 
    /// 
    ///     4. Add wxwidget dll path to user environment variable

    ///     Note: Consider creating a folder for all code2gui related files
    ///     Add a checkbox with "I've closed my visual studio project"
    ///     Add a radio button with following options:
    ///         "I acknowledge that a new user environment variable will be created for integration(recommended)"
    ///         "I can do it manually"

    int result = -1;
    result = ProcessVcxprojFile();
    if (result == -1) {
        SetStatusText("Couldn't process vcxproj file. Setup stopped!");
        return;
    }

    result = ProcessFiltersFiles();

    if (result == -1) {
        ///     hmm I think it doesnt exist
    }

    result = CreateProjectFiles();

    result = CreateBatFile();

    result = CreateEnvironmentVariable();
    
    if (result != -1) {
        SetStatusText("Integration successful!");
        integrationSuccessful = true;
        cancelBtnRef->SetLabelText("Close");
    }
}

std::string SetupFrame::GetExecutablePath() {
   char rawPathName[MAX_PATH];
   GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
   return std::string(rawPathName);
}

std::string SetupFrame::GetFileName(const std::string& filePath) {
    size_t pos = filePath.find_last_of("\\/");
    size_t periodPos = filePath.find_first_of(".");
    return (pos == std::string::npos || periodPos == std::string::npos) ? "" : filePath.substr(pos + 1, periodPos - pos - 1);
}

int SetupFrame::ProcessVcxprojFile() {
    std::string vcxprojFilePath = std::string(textCtrlVcxprojPath->GetValue());
    std::string vcxprojDirPath = GetDiretoryFromPath(vcxprojFilePath);
    ///     Create a backup file, traverse it and re-create original file
    ///     with edits.
    ///     Do not create and overrite the existing backup file
    std::ifstream vcxprojFileIn(vcxprojFilePath);
    if (!vcxprojFileIn.good())
        return -1;

    ///     Prepare paths
    std::string setupExePath = GetExecutablePath();
    std::string setupExeDirPath = GetDiretoryFromPath(setupExePath);
    std::string projectTitleNoSpcs = GetFileName(vcxprojFilePath);// RemoveSpaces(std::string(textCtrlTitle->GetValue()));
    
    std::string batFilePath = setupExeDirPath + "\\bin\\build_" + projectTitleNoSpcs + ".bat";
    std::string wxPropsFilePath = setupExeDirPath + "\\bin\\wxClient\\wxwidgets.props";


    ///     Only update file if it(that update) does not exist
    std::string outString = "";
    std::string line;

    bool insidex86DPropShetGrp = false;
    bool insidex86RPropShetGrp = false;
    bool insidex64DPropShetGrp = false;
    bool insidex64RPropShetGrp = false;

    bool insideIncludeItmGrp = false;
    bool check1ForCstmBld = false;
    bool check2ForCstmBld = false;
    bool cstBldWritten = false;
    bool projCloseTagArrv = false;
    bool finalTag = false;

    bool propShtsx86DAlrdyWrtn = false;
    bool propShtsx86RAlrdyWrtn = false;
    bool propShtsx64DAlrdyWrtn = false;
    bool propShtsx64RAlrdyWrtn = false;

    bool includeAlreadyWritten = false;

    bool cstBldAlreadyWritten = false;

    while (std::getline(vcxprojFileIn, line)) {
        std::string lineNoSpaces = RemoveSpaces(line);

        if (strstr(lineNoSpaces.c_str(), "wxwidgets.props\"/>") != NULL) {
            if (insidex86DPropShetGrp)
                propShtsx86DAlrdyWrtn = true;
            if (insidex86RPropShetGrp)
                propShtsx86RAlrdyWrtn = true;
            if (insidex64DPropShetGrp)
                propShtsx64DAlrdyWrtn = true;
            if (insidex64RPropShetGrp)
                propShtsx64RAlrdyWrtn = true;
        }

        if (strstr(lineNoSpaces.c_str(), "<ClCompileInclude=\"C2GWxWidgets.cpp\"/>") != NULL) {
            includeAlreadyWritten = true;
        }

        if (strstr(lineNoSpaces.c_str(), "<CustomBuildInclude=\"C2GMetadata.txt\">") != NULL) {
            cstBldAlreadyWritten = true;
        }

        if (strstr(lineNoSpaces.c_str(), "<ImportGroupLabel=\"PropertySheets\"") != NULL) {
            if (strstr(lineNoSpaces.c_str(), "Debug|Win32") != NULL) {
                insidex86DPropShetGrp = true;
            }
            else if (strstr(lineNoSpaces.c_str(), "Release|Win32") != NULL) {
                insidex86RPropShetGrp = true;
            }
            else if (strstr(lineNoSpaces.c_str(), "Debug|x64") != NULL) {
                insidex64DPropShetGrp = true;
            }
            else if (strstr(lineNoSpaces.c_str(), "Release|x64") != NULL) {
                insidex64RPropShetGrp = true;
            }
        }

        if (strstr(lineNoSpaces.c_str(), "</ImportGroup>") != NULL) {
            if (insidex86DPropShetGrp) {
                if(propShtsx86DAlrdyWrtn == false && checkBoxX86Debug->GetValue())
                    outString += "    <Import Project=\"" + wxPropsFilePath + "\" />\n";
                insidex86DPropShetGrp = false;
            }
            if (insidex86RPropShetGrp) {
                if (propShtsx86RAlrdyWrtn == false && checkBoxX86Release->GetValue())
                    outString += "    <Import Project=\"" + wxPropsFilePath + "\" />\n";
                insidex86RPropShetGrp = false;
            }
            if (insidex64DPropShetGrp) {
                if (propShtsx64DAlrdyWrtn == false && checkBoxX64Debug->GetValue())
                    outString += "    <Import Project=\"" + wxPropsFilePath + "\" />\n";
                insidex64DPropShetGrp = false;
            }
            if (insidex64RPropShetGrp) {
                if (propShtsx64RAlrdyWrtn == false && checkBoxX64Release->GetValue())
                    outString += "    <Import Project=\"" + wxPropsFilePath + "\" />\n";
                insidex64RPropShetGrp = false;
            }
        }

        if (strstr(lineNoSpaces.c_str(), "<ClCompileInclude=") != NULL 
            && strstr(lineNoSpaces.c_str(), ".cpp\"/>") != NULL) {
            insideIncludeItmGrp = true;
        }

        if (strstr(lineNoSpaces.c_str(), "</ItemGroup>") != NULL) {
            if (insideIncludeItmGrp) {
                if(includeAlreadyWritten == false)
                    outString += "    <ClCompile Include=\"C2GWxWidgets.cpp\" />\n";
                insideIncludeItmGrp = false;
            }
        }

        if (strstr(lineNoSpaces.c_str(), "<ImportProject=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"/>") != NULL)
            check1ForCstmBld = true;

        if (strstr(lineNoSpaces.c_str(), "<ImportGroupLabel=\"ExtensionTargets\">") != NULL)
            check2ForCstmBld = true;

        if (strstr(lineNoSpaces.c_str(), "</Project>") != NULL) {
            finalTag = true;
            if (check1ForCstmBld == false && check2ForCstmBld == false)
                projCloseTagArrv = true;
        }

        if (check1ForCstmBld || check2ForCstmBld || projCloseTagArrv) {
            if (cstBldAlreadyWritten == false && cstBldWritten == false) {
                outString +=
                    "  <ItemGroup>\n"
                    "    <CustomBuild Include=\"C2GMetadata.txt\">\n";
                //if (checkBoxX86Debug->GetValue()) {
                    outString +=
                        "      <Message Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">Generating WxWidgets code</Message>\n"
                        "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">$(OutDir)\\test.txt</Outputs>\n";
                //}
                //if (checkBoxX86Release->GetValue()) {
                    outString +=
                        "      <Message Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">Generating WxWidgets code</Message>\n"
                        "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">$(OutDir)\\test.txt</Outputs>\n";
                //}
                //if (checkBoxX64Debug->GetValue()) {
                    outString +=
                        "      <Message Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">Generating WxWidgets code</Message>\n"
                        "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">$(OutDir)\\test.txt</Outputs>\n";
                //}
                //if (checkBoxX64Release->GetValue()) {
                    outString +=
                        "      <Message Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">Generating WxWidgets code</Message>\n"
                        "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">$(OutDir)\\test.txt</Outputs>\n";
                //}

                //if (checkBoxX86Debug->GetValue())
                    outString += "      <Command Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\"" + batFilePath + "\"</Command>\n";
                
                //if (checkBoxX86Release->GetValue())
                    outString += "      <Command Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\"" + batFilePath + "\"</Command>\n";
                
                //if (checkBoxX64Debug->GetValue())
                    outString += "      <Command Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\"" + batFilePath + "\"</Command>\n";
                
                //if (checkBoxX64Release->GetValue())
                    outString += "      <Command Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\"" + batFilePath + "\"</Command>\n";
                
                outString +=
                    "    </CustomBuild>\n"
                    "  </ItemGroup>\n";
                cstBldWritten = true;
            }
        }

        if(finalTag)
            outString += line;
        else
            outString += line + "\n";
    }
    
    vcxprojFileIn.close();

    std::ofstream vcxprojFileOut(vcxprojFilePath);
    vcxprojFileOut << outString;
    vcxprojFileOut.close();

    return 0;
}

int SetupFrame::ProcessFiltersFiles() {
    ///     Prepare paths
    std::string vcxprojFilePath = std::string(textCtrlVcxprojPath->GetValue());

    std::string vcxfiltersFilePath = vcxprojFilePath + ".filters";

    std::ifstream filtersFileIn(vcxfiltersFilePath);
    if (!filtersFileIn.good())
        return -1;

    bool insideIncludeItmGrp = false;
    bool includeAlreadyWritten = false;
    bool finalTag = false;
    bool metadataAlreadyWrtn = false;

    std::string outString;
    std::string line;

    while (std::getline(filtersFileIn, line)) {
        std::string lineNoSpaces = RemoveSpaces(line);

        if (strstr(lineNoSpaces.c_str(), "</Project>") != NULL)
            finalTag = true;

        if (strstr(lineNoSpaces.c_str(), "<ClCompileInclude=\"C2GWxWidgets.cpp\">") != NULL)
            includeAlreadyWritten = true;

        if (strstr(lineNoSpaces.c_str(), "<CustomBuildInclude=\"C2GMetadata.txt\"/>") != NULL)
            metadataAlreadyWrtn = true;

        if (strstr(lineNoSpaces.c_str(), "<ClCompileInclude=") != NULL
            && strstr(lineNoSpaces.c_str(), ".cpp\">") != NULL) {
            insideIncludeItmGrp = true;
        }

        if (strstr(lineNoSpaces.c_str(), "</ItemGroup>") != NULL) {
            if (insideIncludeItmGrp) {
                if (includeAlreadyWritten == false) {
                    outString +=
                        "    <ClCompile Include=\"C2GWxWidgets.cpp\">\n"
                        "      <Filter>Source Files</Filter>\n"
                        "    </ClCompile>\n";
                    includeAlreadyWritten = true;
                }
                insideIncludeItmGrp = false;
            }
        }

        if (finalTag) {
            if (metadataAlreadyWrtn == false) {
                outString +=
                    "  <ItemGroup>\n"
                    "    <CustomBuild Include=\"C2GMetadata.txt\" />\n"
                    "  </ItemGroup>\n";
            }
            outString += line;
        }
        else
            outString += line + "\n";
    }
    filtersFileIn.close();

    std::ofstream filtersFileOut(vcxfiltersFilePath);
    filtersFileOut << outString;
    filtersFileOut.close();

    return 0;
}

int SetupFrame::CreateProjectFiles() {
    ///     Prepare paths
    std::string vcxprojFilePath = std::string(textCtrlVcxprojPath->GetValue());
    std::string vcxprojDirPath = GetDiretoryFromPath(vcxprojFilePath);

    std::string c2gwxwidgetsCppFilePath = vcxprojDirPath + "\\C2GWxWidgets.cpp";
    std::string c2gmetadataTxtFilePath = vcxprojDirPath + "\\C2GMetadata.txt";

    std::ofstream cppfile(c2gwxwidgetsCppFilePath);
    cppfile.close();
    
    std::ofstream txtfile(c2gmetadataTxtFilePath);

    txtfile << "///		Update GUI window title by modifying text after colon':'\n";
    txtfile << "window_title        :Hello world\n";
    txtfile << "\n";
    txtfile << "///		Specify whether to rebuild GUI layout upon running application or not\n";
    txtfile << "rebuild_gui[yes/no] :yes\n";
    txtfile << "\n";
    txtfile << "///		Do not make any modifications below\n";
    txtfile << "entry_file_path     :" << std::string(textCtrlCppPath->GetValue()) << "\n";
    txtfile << "vcxproj_file_path   :" << std::string(textCtrlVcxprojPath->GetValue()) << "\n";
    txtfile << "codetogui_setup_path:" << GetExecutablePath() << "\n";

    txtfile.close();

    return 0;
}

int SetupFrame::CreateBatFile() {
    ///     Prepare paths
    std::string vcxprojFilePath = std::string(textCtrlVcxprojPath->GetValue());
    std::string vcxprojDirPath = GetDiretoryFromPath(vcxprojFilePath);

    std::string setupExePath = GetExecutablePath();
    std::string setupExeDirPath = GetDiretoryFromPath(setupExePath);
    std::string projectTitleNoSpcs = GetFileName(vcxprojFilePath);

    std::string batFilePath = setupExeDirPath + "\\bin\\build_" + projectTitleNoSpcs + ".bat";
    std::string wxPropsFilePath = setupExeDirPath + "\\bin\\wxClient\\wxwidgets.props";

    std::string ctagsFilePath = setupExeDirPath + "\\bin\\tags_" + projectTitleNoSpcs + ".txt";
    std::string ctagsExePath = setupExeDirPath + "\\bin\\ctags.exe";
    std::string code2GuiExePath = setupExeDirPath + "\\bin\\Code2Gui.exe";
    std::string projMainFilePath = std::string(textCtrlCppPath->GetValue());

    std::string c2gmetadataTxtFilePath = vcxprojDirPath + "\\C2GMetadata.txt";

    std::ofstream buildBatFile(batFilePath);
    buildBatFile << "\"" << ctagsExePath << "\" -f \"" << ctagsFilePath << "\" --c++-kinds=+cdefglmnpstuvx -R --fields=+afikKlmnsSzt --sort=0 \"" << projMainFilePath << "\"\n";
    buildBatFile << "\"" << code2GuiExePath << "\" \"" << c2gmetadataTxtFilePath << "\"\n";
    buildBatFile.close();

    return 0;
}

int SetupFrame::CreateEnvironmentVariable() {

    return 0;
}


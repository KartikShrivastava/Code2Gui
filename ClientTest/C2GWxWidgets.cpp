
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif
#include <wx/valnum.h>
#include <iostream>
using namespace std;
extern int Square(int x) ;
enum ID_COMMANDS {
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    Begin_User_Enum = wxID_HIGHEST + 1,
    Text_Ctrl_Log, 
    Button_0,
    StaticText_0,
    txtCtrl_num_id,
};
class MyApp : public wxApp {
private:
public:
    virtual bool OnInit() wxOVERRIDE;
};
class MyFrame : public wxFrame {
private:
    wxTextCtrl* textctrlLog;
    wxStreamToTextRedirector* redirector;
    int num;
    int result;
    wxTextCtrl* txtCtrl_num;

    void OnTextCtrlUpdate(wxCommandEvent& event) {
        if(event.GetId() == ID_COMMANDS::txtCtrl_num_id) {
            std::string val(txtCtrl_num->GetValue());
            num = std::stoi(val);
        }
    }
    void OnButton_0(wxCommandEvent& event) {
	result = Square(num);
	cout << "Square of number: " << result << endl;
    }
public:
    MyFrame(const wxString& title);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_COMMANDS::Minimal_Quit, MyFrame::OnQuit)
EVT_MENU(ID_COMMANDS::Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit() {
    if (!wxApp::OnInit())
        return false;
    MyFrame* frame = new MyFrame("CodeToGui");
    frame->Show(true);
    return true;
}
MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, {-1,-1}, { 300, 350 })
{
    SetIcon(wxICON(sample));
    #if wxUSE_MENUBAR
    wxMenu* fileMenu = new wxMenu;
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(ID_COMMANDS::Minimal_About, "&About	F1", "Show about dialog");
    fileMenu->Append(ID_COMMANDS::Minimal_Quit, "E&xit	Alt-X", "Quit this program");
    wxColour col1;
    col1.Set(wxT("#e3e3e3"));
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    wxPanel *panel = new wxPanel(this, -1);
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
        wxScrolledWindow* panel2 = new wxScrolledWindow(panel, -1);
            wxBoxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);
                float validateF;
                wxFloatingPointValidator<float> floatValidator(6, &validateF, wxNUM_VAL_ZERO_AS_BLANK);
                wxIntegerValidator<int> intValidator;
                wxTextValidator textValidator;
                floatValidator.SetRange(-FLT_MAX, FLT_MAX);
                intValidator.SetRange(INT_MIN, INT_MAX);
                wxStaticText* StaticText_0 = new wxStaticText(panel2, ID_COMMANDS::StaticText_0, "Awesome console app, huh?\nEnter a number: ");
            vbox2->Add(StaticText_0);
            vbox2->Add(-1,10);
                txtCtrl_num = new wxTextCtrl(panel2, ID_COMMANDS::txtCtrl_num_id, "", wxDefaultPosition, wxDefaultSize, 0, intValidator);
                txtCtrl_num->Connect(ID_COMMANDS::txtCtrl_num_id, wxEVT_COMMAND_TEXT_UPDATED, wxObjectEventFunction(&MyFrame::OnTextCtrlUpdate), NULL, this);
            vbox2->Add(txtCtrl_num);
            vbox2->Add(-1,10);
                wxButton* Button_0 = new wxButton(panel2, ID_COMMANDS::Button_0, "Button_0");
                Button_0->Connect(ID_COMMANDS::Button_0, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::OnButton_0), NULL, this);
            vbox2->Add(Button_0);
            vbox2->Add(-1,10);
		panel2->SetScrollRate(5, 5);
        panel2->SetSizer(vbox2);
    vbox->Add(panel2, 1, wxEXPAND | wxALL, 10);
    textctrlLog = new wxTextCtrl(panel, ID_COMMANDS::Text_Ctrl_Log, wxT(""), wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE|wxNO_BORDER);
    textctrlLog->SetEditable(false);
    textctrlLog->SetBackgroundColour(col1);
    vbox->Add(textctrlLog, 1, wxEXPAND, 0);
    panel->SetSizer(vbox);
    SetMenuBar(menuBar);
    #else // !wxUSE_MENUBAR
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
    #endif // wxUSE_MENUBAR/!wxUSE_MENUBAR
    #if wxUSE_STATUSBAR
    CreateStatusBar(1);
    SetStatusText("Welcome to CodeToGui demo!");
    #endif // wxUSE_STATUSBAR
    redirector = new wxStreamToTextRedirector(textctrlLog);
}
void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent & WXUNUSED(event)) {
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

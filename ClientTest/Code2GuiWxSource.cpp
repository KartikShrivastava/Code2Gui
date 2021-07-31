#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif
#include "wx/stattext.h"
#include "GuiApp.h"
#include <fstream>
class MyApp : public wxApp {
private:
public:
    virtual bool OnInit() wxOVERRIDE;
};
class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
private:
    wxDECLARE_EVENT_TABLE();
};
enum {
    Minimal_Quit = wxID_EXIT,
		Minimal_About = wxID_ABOUT,
		StaticText_test = wxID_HIGHEST + 1
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit() {
	if (!wxApp::OnInit())
		return false;
		MyFrame* frame = new MyFrame("windoow");
		frame->Show(true);
		return true;
}
MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title) {
	SetIcon(wxICON(sample));
#if wxUSE_MENUBAR
    wxMenu* fileMenu = new wxMenu;
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About	F1", "Show about dialog");
    fileMenu->Append(Minimal_Quit, "E&xit	Alt-X", "Quit this program");
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);
#else // !wxUSE_MENUBAR
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif // wxUSE_MENUBAR/!wxUSE_MENUBAR
#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR
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

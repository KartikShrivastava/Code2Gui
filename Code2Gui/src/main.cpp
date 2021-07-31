#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::string removeSpaces(const std::string& line) {
	std::string stringNoSpaces = "";
	for (int i = 0; i < line.length(); ++i)
		if(line[i] != '\t' && line[i] != ' ')
			stringNoSpaces += line[i];
	return stringNoSpaces;
}

std::string getDiretoryFromPath(const std::string& path) {
	size_t pos = path.find_last_of("\\/");
	return (pos == std::string::npos) ? "" : path.substr(0, pos);
}

int main(int argc, char** argv) {
	///		1. Traverse the file which contains int main(...) and
	///		   look for expressions containing "{" opening curly bracket.
	///		2. Start storing the code encountered after "{" line-by-line.
	///		4. End storing when "}" is encountered.
	///		5. Take the stored code and put it in a file named temp.cpp.

	///		argv[0]: path to main file
	///		argv[1]: window title
	
	if (argc != 3) {
		std::cout << "Usage:\targv[1]: absolute path of main file"
				  << "\n\targv[2]: title of gui window" << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	std::ifstream mainFile(argv[1]);
	if (mainFile.is_open() == false) {
		std::cout << "Code2Gui::Error:: Could not open file: "<< argv[1] << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}
	
	std::string line;
	bool mainSeen = false;

	///		Method to capture the left curly bracket of main method "{"
	///		Also helps in checking whether we're in right file or not
	while (std::getline(mainFile, line)) {
		std::string lineNoSpaces = removeSpaces(line);
		if (strstr(lineNoSpaces.c_str(), "main(") != NULL) {
			std::cout << "main seen" << std::endl;
			mainSeen = true;
			break;
		}
	}

	bool recordFunctionBody = false;
	std::string functionBody = "";
	std::vector<std::string> buttonCode;

	if (mainSeen) {
		while (std::getline(mainFile, line)) {
			std::string lineNoSpaces = removeSpaces(line);

			if (strstr(lineNoSpaces.c_str(), "{////") != NULL) {
				std::cout << line << std::endl;
				recordFunctionBody = true;
				functionBody = "";
			}
			else if (strstr(lineNoSpaces.c_str(), "}////") != NULL) {
				std::cout << line << std::endl;
				recordFunctionBody = false;
				buttonCode.push_back(functionBody);
			}
			else if (recordFunctionBody) {
				std::cout << line << std::endl;
				functionBody += line + "\n";
			}
		}
	}
	else {
		std::cout << "Code2Gui::Error:: \"main\" method entry point not found" << argv[1] << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	std::string widgetsBoilerPlate = "\
#include \"wx/wxprec.h\"\n\
#ifndef WX_PRECOMP\n\
#include \"wx/wx.h\"\n\
#endif\n\
#ifndef wxHAS_IMAGES_IN_RESOURCES\n\
#include \"../sample.xpm\"\n\
#endif\n\
#include \"wx/stattext.h\"\n\
#include \"GuiApp.h\"\n\
#include <fstream>\n\
class MyApp : public wxApp {\n\
private:\n\
public:\n\
    virtual bool OnInit() wxOVERRIDE;\n\
};\n\
class MyFrame : public wxFrame {\n\
public:\n\
    MyFrame(const wxString& title);\n\
    void OnQuit(wxCommandEvent& event);\n\
    void OnAbout(wxCommandEvent& event);\n\
private:\n\
    wxDECLARE_EVENT_TABLE();\n\
};\n\
enum {\n\
    Minimal_Quit = wxID_EXIT,\n\
		Minimal_About = wxID_ABOUT,\n\
		StaticText_test = wxID_HIGHEST + 1\n\
};\n\
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)\n\
EVT_MENU(Minimal_Quit, MyFrame::OnQuit)\n\
EVT_MENU(Minimal_About, MyFrame::OnAbout)\n\
wxEND_EVENT_TABLE()\n\
wxIMPLEMENT_APP(MyApp);\n\
bool MyApp::OnInit() {\n\
	if (!wxApp::OnInit())\n\
		return false;\n\
		MyFrame* frame = new MyFrame(\"windoow\");\n\
		frame->Show(true);\n\
		return true;\n\
}\n\
MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title) {\n\
	SetIcon(wxICON(sample));\n\
#if wxUSE_MENUBAR\n\
    wxMenu* fileMenu = new wxMenu;\n\
    wxMenu* helpMenu = new wxMenu;\n\
    helpMenu->Append(Minimal_About, \"&About\tF1\", \"Show about dialog\");\n\
    fileMenu->Append(Minimal_Quit, \"E&xit\tAlt-X\", \"Quit this program\");\n\
    wxMenuBar* menuBar = new wxMenuBar();\n\
    menuBar->Append(fileMenu, \"&File\");\n\
    menuBar->Append(helpMenu, \"&Help\");\n\
    SetMenuBar(menuBar);\n\
#else // !wxUSE_MENUBAR\n\
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);\n\
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, \"About...\");\n\
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);\n\
    sizer->Add(aboutBtn, wxSizerFlags().Center());\n\
    SetSizer(sizer);\n\
#endif // wxUSE_MENUBAR/!wxUSE_MENUBAR\n\
#if wxUSE_STATUSBAR\n\
    CreateStatusBar(2);\n\
    SetStatusText(\"Welcome to wxWidgets!\");\n\
#endif // wxUSE_STATUSBAR\n\
}\n\
void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {\n\
    Close(true);\n\
}\n\
void MyFrame::OnAbout(wxCommandEvent & WXUNUSED(event)) {\n\
    wxMessageBox(wxString::Format\n\
    (\n\
        \"Welcome to %s!\\n\"\n\
        \"\\n\"\n\
        \"This is the minimal wxWidgets sample\\n\"\n\
        \"running under %s.\",\n\
        wxVERSION_STRING,\n\
        wxGetOsDescription()\n\
    ),\n\
        \"About wxWidgets minimal sample\",\n\
        wxOK | wxICON_INFORMATION,\n\
        this);\n\
}";

	std::ofstream outWidgetFile(getDiretoryFromPath(argv[1]) + "/Code2GuiWxSource.cpp");
	outWidgetFile << widgetsBoilerPlate << std::endl;
	outWidgetFile.close();

	return 0;
}

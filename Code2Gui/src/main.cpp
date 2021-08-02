#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>

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

enum KindTag
{
	VARIABLE,
	FUNCTION,
	LOCAL,
};

struct TagDetail
{
	KindTag kind;
	int lineNum;
	bool definitionFound;
	///		In case of static variables of methods whole defintion can be multiline
	///		so we've to catch the whole definition. And this boolean is helpful in that.
	bool definitionCompleteIfStatic;
	std::string title;
	std::string declaration;
	std::string definition;
};

int main() {
	///		1. Traverse the file which contains int main(...) and
	///		   look for expressions containing "{" opening curly bracket.
	///		2. Start storing the code encountered after "{" line-by-line.
	///		4. End storing when "}" is encountered.
	///		5. Take the stored code and put it in a file named temp.cpp.

	///		argv[0]: path to main file
	///		argv[1]: window title
	
	/*std::ifstream c2gInternalMeta("c2g.meta");
	if (!c2gInternalMeta.is_open()) {
		std::cout << "Code2Gui::Error:: Could not locate internal metadata file." << std::endl;
		std::cout << "                  Try running the Code2Gui setup again." << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}*/
	std::string metadataLoc = "C2GMetadata.txt";
	/*std::getline(c2gInternalMeta, metadataLoc);
	if (metadataLoc.compare("") == 0) {
		std::cout << "Code2Gui::Error:: Internal metadata file got corrupted." << std::endl;
		std::cout << "                  Try running the Code2Gui setup again."<< std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}
	c2gInternalMeta.close();*/

	std::string windowTitle;
	std::string mainFilePath;
	std::string guiUpdateStr;
	bool guiUpdate;
	std::ifstream c2gUserMeta(metadataLoc);
	std::getline(c2gUserMeta, windowTitle);
	std::getline(c2gUserMeta, mainFilePath);
	std::getline(c2gUserMeta, guiUpdateStr);
	if (guiUpdateStr.compare("0") == 0)
		guiUpdate = false;
	else
		guiUpdate = true;
	c2gUserMeta.close();

	if (guiUpdate == false) {
		std::cout << "Code2Gui::Log:: GuiUpdation set to false." << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	std::ifstream ctagsFile("C:\\Home\\Projects\\Github\\Code2Gui\\binCode2Gui\\x64\\Debug\\tags.txt");
	std::string line = "";

	///		Array of line splits of those members which are unscoped and marked as static in main file
	///		These varibles need static re-declaration and re-definition in C2GWxWidgets file
	std::vector<TagDetail> staticUnscopedVars;

	///		Array of line splits of those members which are unscoped(and global) present in main file
	///		These varibles need extern keyword declaration in C2GWxWidgets file
	std::vector<TagDetail> globalUnscopeVars;
	
	///		Array of line splits of those methods/functions which are unscoped and marked as static in main file
	///		These methods need static re-declaration and re-definition in C2GWxWidgets file
	std::vector<TagDetail> staticUnscopedFunctions;
	
	///		Array of line splits of those methods/functions which are unscoped(and global) present in main file
	///		These methods need extern keyword declaration in C2GWxWidgets file
	std::vector<TagDetail> globalUnscopedFunctions;

	///		Array of line splits of those variables which are defined in main method
	///		Declaration of these variables need to be extracted and put into Frame class and
	///		Defintion of these variables need to be extracted and put into contructor of Frame class
	std::vector<TagDetail> mainScopedVars;

	///		Important Assumption: ctags.txt is sorted according to line number
	///		If it wont be sorted according to line numbers then code will break!
	bool mainSeenInTagFile = false;

	std::unordered_set<int> capturedLines;
	
	while (std::getline(ctagsFile, line)) {
		if (line[0] == '!')
			continue;

		std::istringstream iss(line);
		std::string token;
		std::vector<std::string> tokens;

		///		sample token line:
		///		main   C:\main.cpp   /^int main(int argc, char** argv)$/;"   kind:function   line:21   language:C++   signature:( int argc, char** argv)
		int idx = 0;
		int nameIdx = idx, declarationIdx = -1, kindIdx = -1, lineIdx = -1;
		while (std::getline(iss, token, '\t')) {// but we can specify a different one
			tokens.push_back(token);
			if (strstr(token.c_str(), "/^") != NULL)
				declarationIdx = idx;
			if (strstr(token.c_str(), "kind:") != NULL)
				kindIdx = idx;
			if (strstr(token.c_str(), "line:") != NULL)
				lineIdx = idx;

			++idx;
		}

		TagDetail tempTagDetail;
		if (tokens.size() && nameIdx != -1 && declarationIdx != -1 && kindIdx != -1 && lineIdx != -1) {
			///		Get the title of the tag
			tempTagDetail.title = tokens[nameIdx];

			///		Get the "kind"(type) of the tag
			if (tokens[kindIdx].compare("variable") == 0)
				tempTagDetail.kind = KindTag::VARIABLE;
			if (tokens[kindIdx].compare("function") == 0)
				tempTagDetail.kind = KindTag::FUNCTION;
			if (tokens[kindIdx].compare("local") == 0)
				tempTagDetail.kind = KindTag::LOCAL;

			///		Get the line number of the tag
			size_t colonIdx = tokens[declarationIdx].find_first_of(":");
			if (colonIdx != std::string::npos) {
				std::string lineNum = tokens[declarationIdx].substr(colonIdx+1, tokens[declarationIdx].length() - colonIdx);
				tempTagDetail.lineNum = std::stoi(lineNum);
			}
			///		If line number is not assinged then ultimately code will break
			else
				tempTagDetail.lineNum = -1;

			///		Do not store tag in case of invalid line number or repeated tag line numbers(can happen in ctags
			///		in case of statements with namespace or class scopes)
			if (tempTagDetail.lineNum == -1 || capturedLines.find(tempTagDetail.lineNum) != capturedLines.end())
				continue;
			else
				capturedLines.insert(tempTagDetail.lineNum);
			
			///		Check if main tag has encountered so far or not
			if (tempTagDetail.title.compare("main") == 0 && tempTagDetail.kind == KindTag::FUNCTION)
				mainSeenInTagFile = true;

			///		Extract the definition and declaration from the 2nd indexed token
			size_t startPos = tokens[declarationIdx].find_first_not_of("/^");
			size_t endPos = tokens[declarationIdx].find_last_of("$/;\"");

			///		What's happening here? Core stuff!
			///		Currect C2G supports unscoped static varibles
			///		Currect C2G supports unscoped non-static varibles
			///		Currect C2G supports unscoped static functions
			///		Currect C2G supports unscoped non-static functions
			///		Currect C2G supports main scoped static variables
			///		Currect C2G supports main scoped non-static variables
			///		Note: All above are extracted in corresponding if statements below (in order)
			///		      If main is not seen while traversing tag file then we assume that variables and functions
			///		      declared are unscoped.
			if (startPos != std::string::npos && endPos != std::string::npos) {
				std::string declarationNdDef = tokens[declarationIdx].substr(startPos + 1, endPos - startPos - 1);

				if (!mainSeenInTagFile) {
					if (tempTagDetail.kind == KindTag::VARIABLE) {
						///		[------------UNSCOPED STATIC VARIBLES------------]
						///		we'll separate declaration and definition and use declaration as Frame member in
						///		C2GWxWidgets.cpp and definition in Frame's constructor
						if (declarationNdDef.compare("static") == 0) {
							size_t equalstoIndexPos = declarationNdDef.find_first_of("=");

							///		If equals to '=' is present then definition is found
							if (equalstoIndexPos != std::string::npos) {
								///		e.g. 
								///			static int x = 10;
								if (declarationNdDef.compare(";") == 0) {
									tempTagDetail.definitionCompleteIfStatic = true;
									tempTagDetail.definition = tempTagDetail.title + " = " + declarationNdDef.substr(equalstoIndexPos, 
																declarationNdDef.length() - equalstoIndexPos + 1) + ";";
								}
								///		In case whole definition is written in multi-line, then make suitable flag
								///		false and while traversing main file, retrieve whole definition.
								///		e.g.
								///			static int x = 
								///			10;
								else {
									tempTagDetail.definitionFound = false;
									tempTagDetail.definitionCompleteIfStatic = false;
									tempTagDetail.definition = "";
								}
								tempTagDetail.definitionFound = true;
								tempTagDetail.declaration = declarationNdDef.substr(0, equalstoIndexPos) + ";";
							}
							///		e.g.
							///		static int x;
							else {
								tempTagDetail.definitionFound = false;
								tempTagDetail.definitionCompleteIfStatic = false;
								tempTagDetail.definition = "";
								tempTagDetail.declaration = declarationNdDef;
							}
							staticUnscopedVars.push_back(tempTagDetail);
						}
						///		[------------UNSCOPED NON-STATIC VARIBLES------------]
						///		In this case we need declaration (to make it extern in C2GWxWidgets.cpp,
						///		Dont care much about definition)
						else {
							size_t equalstoIndexPos = declarationNdDef.find_first_of("=");

							///		If equals to '=' is present then definition is found
							///		e.g. 
							///			int x = 10;
							///		or
							///			int x =
							///			10;
							if (equalstoIndexPos != std::string::npos)
								tempTagDetail.declaration = declarationNdDef.substr(0, equalstoIndexPos) + ";";
							///		e.g.
							///		int x;
							else
								tempTagDetail.declaration = declarationNdDef;

							tempTagDetail.definitionFound = false;
							tempTagDetail.definitionCompleteIfStatic = false;
							tempTagDetail.definition = "";
							globalUnscopeVars.push_back(tempTagDetail);
						}
					}
					else if(tempTagDetail.kind == KindTag::FUNCTION) {
						///		[------------UNSCOPED STATIC FUNCTIONS------------]
						///		we'll separate declaration and definition and use declaration as Frame member in
						///		C2GWxWidgets.cpp and definition in Frame's constructor
						if (declarationNdDef.compare("static") == 0) {
							size_t equalstoIndexPos = declarationNdDef.find_first_of("=");

							///		If equals to '=' is present then definition is found
							if (equalstoIndexPos != std::string::npos) {
								///		e.g. 
								///			static int x = 10;
								if (declarationNdDef.compare(";") == 0) {
									tempTagDetail.definitionCompleteIfStatic = true;
									tempTagDetail.definition = tempTagDetail.title + " = " + declarationNdDef.substr(equalstoIndexPos,
										declarationNdDef.length() - equalstoIndexPos + 1) + ";";
								}
								///		In case whole definition is written in multi-line, then make suitable flag
								///		false and while traversing main file, retrieve whole definition.
								///		e.g.
								///			static int x = 
								///			10;
								else {
									tempTagDetail.definitionFound = false;
									tempTagDetail.definitionCompleteIfStatic = false;
									tempTagDetail.definition = "";
								}
								tempTagDetail.definitionFound = true;
								tempTagDetail.declaration = declarationNdDef.substr(0, equalstoIndexPos) + ";";
							}
							///		e.g.
							///		static int x;
							else {
								tempTagDetail.definitionFound = false;
								tempTagDetail.definitionCompleteIfStatic = false;
								tempTagDetail.definition = "";
								tempTagDetail.declaration = declarationNdDef;
							}
							staticUnscopedVars.push_back(tempTagDetail);
						}
						///		[------------UNSCOPED NON-STATIC VARIBLES------------]
						///		In this case we need declaration (to make it extern in C2GWxWidgets.cpp,
						///		Dont care much about definition)
						else {
							size_t equalstoIndexPos = declarationNdDef.find_first_of("=");

							///		If equals to '=' is present then definition is found
							///		e.g. 
							///			int x = 10;
							///		or
							///			int x =
							///			10;
							if (equalstoIndexPos != std::string::npos)
								tempTagDetail.declaration = declarationNdDef.substr(0, equalstoIndexPos) + ";";
							///		e.g.
							///		int x;
							else
								tempTagDetail.declaration = declarationNdDef;

							tempTagDetail.definitionFound = false;
							tempTagDetail.definitionCompleteIfStatic = false;
							tempTagDetail.definition = "";
							globalUnscopeVars.push_back(tempTagDetail);
						}
					}

					tempTagDetail.definitionFound = false;
					tempTagDetail.declaration = declarationNdDef;
					tempTagDetail.definition = "";
					tempTagDetail.definitionCompleteIfStatic = false;
				}
				
				///		Check if variables are defined during declaration,
				///		If so then separate definition from declaration
				size_t equalstoIndexPos = declarationNdDef.find_first_of("=");
				if (equalstoIndexPos != std::string::npos) {
					tempTagDetail.definitionFound = true;
					tempTagDetail.definitionCompleteIfStatic = true;
					tempTagDetail.declaration = declarationNdDef.substr(0, equalstoIndexPos) + ";";
					tempTagDetail.definition = tempTagDetail.title + declarationNdDef.substr(equalstoIndexPos, declarationNdDef.length() - equalstoIndexPos + 1) + ";";
				}
				///		Else take the declaration and explicitly make definition empty string
				else {
					tempTagDetail.definitionFound = false;
					tempTagDetail.declaration = declarationNdDef;
					tempTagDetail.definition = "";
					tempTagDetail.definitionCompleteIfStatic = false;
				}
			}
			else {
				tempTagDetail.definitionFound = false;
				tempTagDetail.declaration = "";
				tempTagDetail.definition = "";
			}

			///		If main is not seen then fill unscoped TagDetails vectors
			if (!mainSeenInTagFile) {
				if (tempTagDetail.kind == KindTag::VARIABLE) {
					///		If variable is static
					if (tempTagDetail.declaration.compare("static") == 0)
						staticUnscopedVars.push_back(tempTagDetail);
					else
						globalUnscopeVars.push_back(tempTagDetail);
				}
				else if (tempTagDetail.kind == KindTag::FUNCTION) {
					///		If variable is static
					if (tempTagDetail.declaration.compare("static") == 0)
						staticUnscopedFunctions.push_back(tempTagDetail);
					else
						globalUnscopedFunctions.push_back(tempTagDetail);
				}
			}
			else
				if(tempTagDetail.kind == KindTag::VARIABLE)
					mainScopedVars.push_back(tempTagDetail);
		}
	}

	std::ifstream mainFile(mainFilePath);
	if (mainFile.is_open() == false) {
		std::cout << "Code2Gui::Error:: Could not open file: "<< mainFilePath << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}
	
	line = "";
	bool mainSeen = false;

	std::string includes = "";
	std::string usingNamespaces = "";

	///		Method to capture the left curly bracket of main method "{"
	///		Also helps in checking whether we're in right file or not
	while (std::getline(mainFile, line)) {
		if (strstr(line.c_str(), "#include") != NULL)
			includes += line + "\n";
		if (strstr(line.c_str(), "using namespace") != NULL)
			usingNamespaces += line + "\n";

		std::string lineNoSpaces = removeSpaces(line);
		if (strstr(lineNoSpaces.c_str(), "main(") != NULL) {
			std::cout << "main seen" << std::endl;
			mainSeen = true;
			break;
		}
	}

	bool recordFunctionBody = false;
	std::string functionBody = "";
	std::vector<std::pair<std::string,std::string>> buttonNameCodePair;

	///		Block of code between {//( and }//(  converts to a button
	///		Text specified after {//( makes up the button name, and if
	///		not specified then a default button name i.e. Buttom_[index] is applied.
	/// 
	///		Block of code between {//< and }//< converts to staticText

	if (mainSeen) {
		while (std::getline(mainFile, line)) {
			std::string lineNoSpaces = removeSpaces(line);

			if (recordFunctionBody==false && strstr(lineNoSpaces.c_str(), "{////") != NULL) {
				std::cout << line << std::endl;
				recordFunctionBody = true;
				functionBody = "";
			}
			else if (recordFunctionBody && strstr(lineNoSpaces.c_str(), "}////") != NULL) {
				std::cout << line << std::endl;
				recordFunctionBody = false;
				buttonNameCodePair.push_back({ "Button_" + std::to_string(buttonNameCodePair.size()), functionBody });
			}
			else if (recordFunctionBody) {
				std::cout << line << std::endl;
				functionBody += line + "\n";
			}
		}
	}
	else {
		std::cout << "Code2Gui::Error:: \"main\" method entry point not found" << mainFilePath << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}
	mainFile.close();

	std::string buttonDecsAndDefs = "";
	std::string buttonEnums = "";
	std::string buttonUI = "";
	
	for (int i = 0; i < buttonNameCodePair.size(); ++i) {
		buttonDecsAndDefs += "\tvoid On" + buttonNameCodePair[i].first + "(wxCommandEvent& event) {\n";
		buttonDecsAndDefs += buttonNameCodePair[i].second;
		buttonDecsAndDefs += "\t}\n";
		
		buttonEnums += "\t" + buttonNameCodePair[i].first + ",\n";

		buttonUI += "\t\t\t\twxButton* " + buttonNameCodePair[i].first + " = new wxButton(panel2, ID_COMMANDS::" + 
			buttonNameCodePair[i].first + ", \"" + buttonNameCodePair[i].first + "\");\n";
		buttonUI += "\t\t\t\t" + buttonNameCodePair[i].first + "->Connect(ID_COMMANDS::"+ buttonNameCodePair[i].first+
			", wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::On"+ buttonNameCodePair[i].first+"), NULL, this);\n";
		buttonUI += "\t\t\tvbox2->Add(" + buttonNameCodePair[i].first + ");\n";
	}

	std::string widgetsBoilerPlate = "\n"
	"#include \"wx/wxprec.h\"\n"
	"#ifndef WX_PRECOMP\n"
	"#include \"wx/wx.h\"\n"
	"#endif\n"
	"#ifndef wxHAS_IMAGES_IN_RESOURCES\n"
	"#include \"../sample.xpm\"\n"
	"#endif\n"
	"enum ID_COMMANDS {\n"
	"	Minimal_Quit = wxID_EXIT,\n"
	"	Minimal_About = wxID_ABOUT,\n"
	"	Begin_User_Enum = wxID_HIGHEST + 1,\n"
	"	Text_Ctrl_Log, \n";
	widgetsBoilerPlate += buttonEnums;
	widgetsBoilerPlate += "};\n"
	"class MyApp : public wxApp {\n"
	"private:\n"
	"public:\n"
	"    virtual bool OnInit() wxOVERRIDE;\n"
	"};\n"
	"class MyFrame : public wxFrame {\n"
	"private:\n"
	"	wxTextCtrl* textctrlLog;\n"
	"	wxStreamToTextRedirector* redirector;\n";
	widgetsBoilerPlate += buttonDecsAndDefs;
	widgetsBoilerPlate += "public:\n"
	"   MyFrame(const wxString& title);\n"
	"   void OnQuit(wxCommandEvent& event);\n"
	"   void OnAbout(wxCommandEvent& event);\n"
	"   wxDECLARE_EVENT_TABLE();\n"
	"};\n"
	"wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)\n"
	"EVT_MENU(ID_COMMANDS::Minimal_Quit, MyFrame::OnQuit)\n"
	"EVT_MENU(ID_COMMANDS::Minimal_About, MyFrame::OnAbout)\n"
	"wxEND_EVENT_TABLE()\n"
	"wxIMPLEMENT_APP(MyApp);\n"
	"bool MyApp::OnInit() {\n"
	"	if (!wxApp::OnInit())\n"
	"		return false;\n"
	"	MyFrame* frame = new MyFrame(\"windoow\");\n"
	"	frame->Show(true);\n"
	"	return true;\n"
	"}\n"
	"MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title) {\n"
	"	SetIcon(wxICON(sample));\n"
	"   #if wxUSE_MENUBAR\n"
	"   wxMenu* fileMenu = new wxMenu;\n"
	"   wxMenu* helpMenu = new wxMenu;\n"
	"   helpMenu->Append(ID_COMMANDS::Minimal_About, \"&About\tF1\", \"Show about dialog\");\n"
	"   fileMenu->Append(ID_COMMANDS::Minimal_Quit, \"E&xit\tAlt-X\", \"Quit this program\");\n"
	"   wxMenuBar* menuBar = new wxMenuBar();\n"
	"   menuBar->Append(fileMenu, \"&File\");\n"
	"   menuBar->Append(helpMenu, \"&Help\");\n"
	"	wxPanel *panel = new wxPanel(this, -1);\n"
	"	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);\n"
	"		wxScrolledWindow* panel2 = new wxScrolledWindow(panel, -1);\n"
	"			wxBoxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);\n";
	widgetsBoilerPlate += buttonUI;
	widgetsBoilerPlate += "\t\tpanel2->SetScrollRate(5, 5);\n"
	"		panel2->SetSizer(vbox2);\n"
	"	vbox->Add(panel2, 1, wxEXPAND | wxALL, 10);\n"
	"	textctrlLog = new wxTextCtrl(panel, ID_COMMANDS::Text_Ctrl_Log, wxT(\"\"), wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);\n"
	"	textctrlLog->SetEditable(false);\n"
	"	vbox->Add(textctrlLog, 1, wxEXPAND, 0);\n"
	"	panel->SetSizer(vbox);\n"
	"   SetMenuBar(menuBar);\n"
	"   #else // !wxUSE_MENUBAR\n"
	"   wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);\n"
	"   wxButton* aboutBtn = new wxButton(this, wxID_ANY, \"About...\");\n"
	"   aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);\n"
	"   sizer->Add(aboutBtn, wxSizerFlags().Center());\n"
	"   SetSizer(sizer);\n"
	"   #endif // wxUSE_MENUBAR/!wxUSE_MENUBAR\n"
	"   #if wxUSE_STATUSBAR\n"
	"   CreateStatusBar(2);\n"
	"   SetStatusText(\"Welcome to wxWidgets!\");\n"
	"   #endif // wxUSE_STATUSBAR\n"
	"	redirector = new wxStreamToTextRedirector(textctrlLog);\n"
	"}\n"
	"void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {\n"
	"   Close(true);\n"
	"}\n"
	"void MyFrame::OnAbout(wxCommandEvent & WXUNUSED(event)) {\n"
	"   wxMessageBox(wxString::Format\n"
	"   (\n"
	"       \"Welcome to %s!\\n\"\n"
	"       \"\\n\"\n"
	"       \"This is the minimal wxWidgets sample\\n\"\n"
	"       \"running under %s.\",\n"
	"       wxVERSION_STRING,\n"
	"       wxGetOsDescription()\n"
	"   ),\n"
	"   \"About wxWidgets minimal sample\",\n"
	"   wxOK | wxICON_INFORMATION,\n"
	"   this);\n"
	"}";

	std::ofstream outWidgetFile(getDiretoryFromPath(mainFilePath) + "/C2GWxWidgets.cpp");
	outWidgetFile << widgetsBoilerPlate << std::endl;
	outWidgetFile.close();

	return 0;
}

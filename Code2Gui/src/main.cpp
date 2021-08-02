#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <list>

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

	///		In case of static variables of methods whole defintion can be multiline
	///		so we've to catch the whole definition. And this boolean is helpful in that.
	//bool definitionCompleteIfStatic;
	//std::string definition;
struct GenericTagDetails
{
	KindTag kind;
	int lineNum;
	std::string title;
};

struct UnscopedStaticVars : GenericTagDetails
{
	bool definitionExists;
	std::string declarationNdDefinition;
};

struct UnscopedNonStaticVars : GenericTagDetails
{
	std::string declaration;
};

struct UnscopedStaticFunctions : GenericTagDetails
{
	bool definitionExists;
	std::string declarationNdDefinition;
};

struct UnscopedNonStaticFunctions : GenericTagDetails
{
	std::string declaration;
};

struct ScopedMainVars : GenericTagDetails
{
	bool definitionExists;
	std::string declaration;
	std::string definition;
};

int main() {
	std::string metadataLoc = "C2GMetadata.txt";

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
	std::vector<UnscopedStaticVars> staticUnscopedVars;
	///		List of pairs of lineNumbers and vector indices of the corresponding line number
	std::list<std::pair<int,int>> staticUnscpVarsNoDefLines;

	///		Array of line splits of those members which are unscoped(and global) present in main file
	///		These varibles need extern keyword declaration in C2GWxWidgets file
	std::vector<UnscopedNonStaticVars> globalUnscopeVars;
	
	///		Array of line splits of those methods/functions which are unscoped and marked as static in main file
	///		These methods need static re-declaration and re-definition in C2GWxWidgets file
	std::vector<UnscopedStaticFunctions> staticUnscopedFunctions;
	///		List of pairs of lineNumbers and vector indices of the corresponding line number
	std::list<std::pair<int, int>> staticUnscpFuncsNoDefLines;

	///		Array of line splits of those methods/functions which are unscoped(and global) present in main file
	///		These methods need extern keyword declaration in C2GWxWidgets file
	std::vector<UnscopedNonStaticFunctions> globalUnscopedFunctions;

	///		Array of line splits of those variables which are defined in main method
	///		Declaration of these variables need to be extracted and put into Frame class and
	///		Defintion of these variables need to be extracted and put into contructor of Frame class
	std::vector<ScopedMainVars> scopedMainVars;
	///		List of pairs of lineNumbers and vector indices of the corresponding line number
	std::list<std::pair<int, int>> scpMainVarsNoDefLines;

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

		GenericTagDetails tempTagDetail;
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
						///		we'll save decleration + definition(if possible) and put it at the top of *WxWidgets.cpp file
						if (declarationNdDef.compare("static") == 0) {
							UnscopedStaticVars tempUnscopedStaticVar;
							tempUnscopedStaticVar.kind = tempTagDetail.kind;
							tempUnscopedStaticVar.lineNum = tempTagDetail.lineNum;
							tempUnscopedStaticVar.title = tempTagDetail.title;

							size_t equalstoIndexPos = declarationNdDef.find_first_of("=");

							///		If equals to '=' is present then definition is probably found
							if (equalstoIndexPos != std::string::npos) {
								///		e.g.:
								///			static int x = 10;
								///		stays same:
								///			static int x = 10;
								if (declarationNdDef.compare(";") == 0)
									tempUnscopedStaticVar.definitionExists = true;
								///		In case whole definition is written in multi-line, then make suitable flag
								///		false and while traversing main file, try to retrieve whole definition.
								///		e.g.:
								///			static int x = 
								///		stays same:
								///			static int x =
								else {
									tempUnscopedStaticVar.definitionExists = false;
									staticUnscpVarsNoDefLines.push_back({ tempUnscopedStaticVar.lineNum,staticUnscopedVars.size() });
								}
							}
							///		e.g.:
							///			static int x;
							///		stays same but TODO: Write suitable implementation to find definition
							///			static int x;
							else {
								tempUnscopedStaticVar.definitionExists = false;
								//staticUnscpVarsNoDefLines.push_back(tempUnscopedStaticVar.lineNum);
							}
							tempUnscopedStaticVar.declarationNdDefinition = declarationNdDef;
							staticUnscopedVars.push_back(tempUnscopedStaticVar);
						}
						///		[------------UNSCOPED NON-STATIC VARIBLES------------]
						///		In this case we need declaration (to make it extern in C2GWxWidgets.cpp,
						///		Dont care much about definition)
						else {
							UnscopedNonStaticVars tempUnscopedNonStaticVar;
							tempUnscopedNonStaticVar.kind = tempTagDetail.kind;
							tempUnscopedNonStaticVar.lineNum = tempTagDetail.lineNum;
							tempUnscopedNonStaticVar.title = tempTagDetail.title;

							///		Handle case in which function declaration is already extern

							size_t equalstoIndexPos = declarationNdDef.find_first_of("=");

							///		If equals to '=' is present then remove definition
							if (equalstoIndexPos != std::string::npos) {
								///		e.g.:
								///			int x = 10;
								///		or
								///			int x = 
								///		converts to:
								///			extern int x;
								tempUnscopedNonStaticVar.declaration = "extern " + declarationNdDef.substr(0, equalstoIndexPos) + ";";
							}
							///		e.g.
							///			static int x;
							///		converts to:
							///			extern int x;
							else {
								tempUnscopedNonStaticVar.declaration = "extern " + declarationNdDef;
							}
							globalUnscopeVars.push_back(tempUnscopedNonStaticVar);
						}
					}
					else if(tempTagDetail.kind == KindTag::FUNCTION) {
						///		[------------UNSCOPED STATIC FUNCTIONS------------]
						///		we'll save decleration + definition(if possible) and put it at the top of *WxWidgets.cpp file
						if (declarationNdDef.compare("static") == 0) {
							UnscopedStaticFunctions tempUnscopedStaticFunctions;
							tempUnscopedStaticFunctions.kind = tempTagDetail.kind;
							tempUnscopedStaticFunctions.lineNum = tempTagDetail.lineNum;
							tempUnscopedStaticFunctions.title = tempTagDetail.title;

							size_t rightCurlyPos = declarationNdDef.find_last_of("}");

							///		If right curly bracket '|' is present then definition is found
							///		e.g.:
							///			static int foo() { return 10; }
							///		stays same:
							///			static int foo() { return 10; }
							if (rightCurlyPos != std::string::npos)
								tempUnscopedStaticFunctions.definitionExists = true;
							///		else we've to definitely complete the definition while traversing main
							///		e.g.:
							///			static int foo() { 
							///		stays same:
							///			static int foo() { 
							else {
								tempUnscopedStaticFunctions.definitionExists = false;
								staticUnscpFuncsNoDefLines.push_back({ tempUnscopedStaticFunctions.lineNum, staticUnscopedFunctions.size() });
							}

							tempUnscopedStaticFunctions.declarationNdDefinition = declarationNdDef;
							staticUnscopedFunctions.push_back(tempUnscopedStaticFunctions);
						}
						///		[------------UNSCOPED NON-STATIC VARIBLES------------]
						///		In this case we need declaration (and make it extern in *WxWidgets.cpp,
						///		we dont care much about definition)
						else {
							UnscopedNonStaticFunctions tempGlobalUnscopedFunctions;
							tempGlobalUnscopedFunctions.kind = tempTagDetail.kind;
							tempGlobalUnscopedFunctions.lineNum = tempTagDetail.lineNum;
							tempGlobalUnscopedFunctions.title = tempTagDetail.title;

							size_t rightCurlyPos = declarationNdDef.find_last_of("}");
							size_t leftCurlyPos = declarationNdDef.find_first_of("{");

							///		If right curly bracket is present then take declaration upto first
							///		left curly bracket
							///		e.g.:
							///			int foo() { return 10; }
							///		or
							///			int foo() { 
							///		converts to:
							///			extern int foo();
							if (rightCurlyPos != std::string::npos || leftCurlyPos != std::string::npos)
								tempGlobalUnscopedFunctions.declaration = "extern " + declarationNdDef.substr(0, leftCurlyPos) + ";";
							///		else take whole string terminated with ';'
							///		e.g.:
							///			int foo() 
							///		converts to:
							///			extern int foo();
							else
								tempGlobalUnscopedFunctions.declaration = "extern " + declarationNdDef + ";";

							globalUnscopedFunctions.push_back(tempGlobalUnscopedFunctions);
						}
					}
				}
				else {
					if (tempTagDetail.kind == KindTag::LOCAL) {
						///		[------------SCOPED MAIN LOCAL VARIBLES------------]
						///		we'll save decleration + definition(if possible) and put it at the 
						///		initializer list of *Frame constructor in *WxWidgets.cpp
						ScopedMainVars tempScopedMainVars;
						tempScopedMainVars.kind = tempTagDetail.kind;
						tempScopedMainVars.lineNum = tempTagDetail.lineNum;
						tempScopedMainVars.title = tempTagDetail.title;

						size_t equalstoPos = declarationNdDef.find_first_of("=");
						size_t semicolorPos = declarationNdDef.find_first_of(";");

						///		If equals to '=' and semicolon ';' is present then definition is found
						///		e.g.:
						///			int x = 10;
						///		converts to:
						///			declaration: int x;
						///			definition: x(10)
						if (equalstoPos != std::string::npos && semicolorPos != std::string::npos) {
							tempScopedMainVars.declaration = declarationNdDef.substr(0, equalstoPos) + ";";
							tempScopedMainVars.definition = tempScopedMainVars.title + "(" +
								declarationNdDef.substr(equalstoPos + 1, semicolorPos - equalstoPos - 1) +
								")";
							tempScopedMainVars.definitionExists = true;
						}
						///		If equals to is present/absent but semicolon is absent the definition is multiline
						///		so you've to retrieve it while traversing main method
						///		e.g.:
						///			std::string str = "this is a useful comment"+ 
						///		or
						///			std::string str;
						///			
						///		converts to:
						///			declaration: int x;
						///			definition: not yet found
						else {
							tempScopedMainVars.definition = "";
							tempScopedMainVars.definitionExists = false;
							scpMainVarsNoDefLines.push_back({ tempScopedMainVars.lineNum, scopedMainVars.size() });
							if (equalstoPos != std::string::npos) {
								tempScopedMainVars.declaration = declarationNdDef.substr(0, equalstoPos) + ";";
								tempScopedMainVars.definition = tempScopedMainVars.title + "(" + 
																declarationNdDef.substr(equalstoPos, declarationNdDef.length() - equalstoPos);
							}
							///		Assuming that semicolor is present
							else {
								tempScopedMainVars.declaration = declarationNdDef;
								tempScopedMainVars.definition = tempScopedMainVars.title + "(";
							}
						}
						scopedMainVars.push_back(tempScopedMainVars);
					}
				}
			}
		}
	}

	std::ifstream mainFile(mainFilePath);
	if (mainFile.is_open() == false) {
		std::cout << "Code2Gui::Error:: Could not open file: "<< mainFilePath << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}
	
	line = "";
	int lineNumber = 0;
	bool searchingSUVarDSemiColon = false,		///		SUVD: Static Unscoped Var Definition
		searchingSUFuncDRParanthesis = false;	///		SUFDR: Static Unscoped Function Definition Right
	bool mainSeen = false;

	std::string includesStr = "";
	std::string usingNamespacesStr = "";

	///		Method to capture the left curly bracket of main method "{"
	///		Also helps in checking whether we're in right file or not
	while (std::getline(mainFile, line)) {
		if (strstr(line.c_str(), "#include") != NULL)
			includesStr += line + "\n";
		else if (strstr(line.c_str(), "using namespace") != NULL)
			usingNamespacesStr += line + "\n";
		else if (searchingSUVarDSemiColon) {
			staticUnscopedVars[staticUnscpVarsNoDefLines.front().second].declarationNdDefinition += "\n" + line;
			if (strstr(line.c_str(), ";") != NULL) {
				searchingSUVarDSemiColon = false;
				staticUnscpVarsNoDefLines.pop_front();
			}
		}
		else if (searchingSUFuncDRParanthesis) {
			staticUnscopedFunctions[staticUnscpFuncsNoDefLines.front().second].declarationNdDefinition += "\n" + line;
			if (strstr(line.c_str(), "}") != NULL) {
				searchingSUFuncDRParanthesis = false;
				staticUnscpFuncsNoDefLines.pop_front();
			}
		}

		///		Retrieve everything starting after '=' to first encountered semicolon and add it to
		///		the suitable vector
		if (!searchingSUVarDSemiColon && staticUnscpVarsNoDefLines.size() && lineNumber == staticUnscpVarsNoDefLines.front().first)
			searchingSUVarDSemiColon = true;
		else if (!searchingSUFuncDRParanthesis && staticUnscpFuncsNoDefLines.size() && lineNumber == staticUnscpFuncsNoDefLines.front().first)
			searchingSUFuncDRParanthesis = true;

		++lineNumber;
		std::string lineNoSpaces = removeSpaces(line);
		if (strstr(lineNoSpaces.c_str(), "main(") != NULL) {
			std::cout << "main seen" << std::endl;
			mainSeen = true;
			break;
		}
	}

	bool recordFunctionBody = false;
	bool searchingSMVarDSemiColon = false;		///		SMVD: Scoped Main Var Definition
	std::string functionBody = "";
	std::vector<std::pair<std::string,std::string>> buttonNameCodePair;

	///		Block of code between {//( and }//(  converts to a button
	///		Text specified after {//( makes up the button name, and if
	///		not specified then a default button name i.e. Buttom_[index] is applied.
	/// 
	///		Block of code between {//< and }//< converts to staticText
	/// 
	///		Block of code between {//>int and }//>int converts to input

	if (mainSeen) {
		while (std::getline(mainFile, line)) {
			if (searchingSMVarDSemiColon) {
				size_t semicolonPos = line.find_last_of(";");
				if (semicolonPos != std::string::npos) {
					scopedMainVars[scpMainVarsNoDefLines.front().second].definition += line.substr(0, semicolonPos) + ")";
					searchingSMVarDSemiColon = false;
					scpMainVarsNoDefLines.pop_front();
				}
				else
					scopedMainVars[scpMainVarsNoDefLines.front().second].definition += line;
			}

			if (!searchingSMVarDSemiColon && scpMainVarsNoDefLines.size() && lineNumber == scpMainVarsNoDefLines.front().second)
				searchingSMVarDSemiColon = true;

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
			++lineNumber;
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
		"#endif\n";
	widgetsBoilerPlate += includesStr;
	widgetsBoilerPlate += usingNamespacesStr;
	for (int i = 0; i < staticUnscopedVars.size(); ++i)
		widgetsBoilerPlate += staticUnscopedVars[i].declarationNdDefinition + "\n";
	for (int i = 0; i < globalUnscopeVars.size(); ++i)
		widgetsBoilerPlate += globalUnscopeVars[i].declaration + "\n";
	for (int i = 0; i < staticUnscopedFunctions.size(); ++i)
		widgetsBoilerPlate += staticUnscopedFunctions[i].declarationNdDefinition + "\n";
	for (int i = 0; i < globalUnscopedFunctions.size(); ++i)
		widgetsBoilerPlate += globalUnscopedFunctions[i].declaration + "\n";
	widgetsBoilerPlate += "enum ID_COMMANDS {\n"
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
	for (int i = 0; i < scopedMainVars.size(); ++i)
		widgetsBoilerPlate += scopedMainVars[i].declaration + "\n";
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
		"MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title)\n";
	for (int i = 0; i < scopedMainVars.size(); ++i)
		widgetsBoilerPlate += "," + scopedMainVars[i].definition;
	widgetsBoilerPlate += "{\n"
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <list>
#include "CodeToGui.h"

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

int main() {
	int result = -1;
	
	CodeToGui codeToGui;
	result = codeToGui.LoadInfoFromMetadata("C:\\Home\\Projects\\Github\\Code2Gui\\ClientTest\\C2GMetadata.txt");

	if (result == -1) {
		std::cout << "Code2Gui::Log:: Unable to open/read C2GMetadata.txt" << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	if (codeToGui.guiUpdate == false) {
		std::cout << "Code2Gui::Log:: GuiUpdation set to false." << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	result = codeToGui.LoadInfoFromTagFile("C:\\Home\\Projects\\Github\\Code2Gui\\binCode2Gui\\x64\\Debug\\tags.txt");

	if (result == -1) {
		std::cout << "Code2Gui::Log:: Internal tag file missing/corrupted" << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	std::ifstream mainFile(mainFilePath);
	if (mainFile.is_open() == false) {
		std::cout << "Code2Gui::Error:: Could not open file: "<< mainFilePath << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}
	
	line = "";
	int lineNumber = 1;
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
				staticUnscopedVars[staticUnscpVarsNoDefLines.front().second].definitionExists = true;
				staticUnscpVarsNoDefLines.pop_front();
			}
		}
		else if (searchingSUFuncDRParanthesis) {
			staticUnscopedFunctions[staticUnscpFuncsNoDefLines.front().second].declarationNdDefinition += "\n" + line;
			if (strstr(line.c_str(), "}") != NULL) {
				searchingSUFuncDRParanthesis = false;
				staticUnscopedFunctions[staticUnscpFuncsNoDefLines.front().second].definitionExists += "}\n";
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

	bool searchingSMVarDSemiColon = false;		///		SMVD: Scoped Main Var Definition
	bool recordFunctionBody = false;
	bool recordStaticTextBody = false;
	bool recordTextCtrlVar = false;
	std::string functionBody = "";
	std::string staticTextBody = "";
	std::string txtCtrlType = "";
	std::string uiBodyCombined = "";			///		Helpful in maintaining order of ui elements
	std::vector<std::pair<std::string,std::string>> buttonName_CodePair;
	std::vector<std::pair<std::string,std::string>> statTxtName_BodyPair;
	///		vector of pair of reference variable name whose value needs to be updated and it's type
	std::vector<std::pair<std::string,std::string>> txtCtrlRefVarName_TypePair;

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
					scopedMainVars[scpMainVarsNoDefLines.front().second].definitionExists = true;
					searchingSMVarDSemiColon = false;
					scpMainVarsNoDefLines.pop_front();
				}
				else
					scopedMainVars[scpMainVarsNoDefLines.front().second].definition += line;
			}

			if (!searchingSMVarDSemiColon && scpMainVarsNoDefLines.size() && lineNumber == scpMainVarsNoDefLines.front().second)
				searchingSMVarDSemiColon = true;

			std::string lineNoSpaces = removeSpaces(line);

			if (recordFunctionBody == false && recordStaticTextBody==false && recordTextCtrlVar==false
				&& strstr(lineNoSpaces.c_str(), "{///btn") != NULL) {
				std::cout << line << std::endl;
				recordFunctionBody = true;
				functionBody = "";
			}
			else if (recordFunctionBody && recordStaticTextBody == false && recordTextCtrlVar == false
				&& strstr(lineNoSpaces.c_str(), "}///") != NULL) {
				std::cout << line << std::endl;
				recordFunctionBody = false;
				buttonName_CodePair.push_back({ "Button_" + std::to_string(buttonName_CodePair.size()), functionBody });

				int index = buttonName_CodePair.size() - 1;
				uiBodyCombined += "                wxButton* " + buttonName_CodePair[index].first + " = new wxButton(panel2, ID_COMMANDS::" +
					buttonName_CodePair[index].first + ", \"" + buttonName_CodePair[index].first + "\");\n";
				uiBodyCombined += "                " + buttonName_CodePair[index].first + "->Connect(ID_COMMANDS::" + buttonName_CodePair[index].first +
					", wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::On" + buttonName_CodePair[index].first + "), NULL, this);\n";
				uiBodyCombined += "            vbox2->Add(" + buttonName_CodePair[index].first + ");\n";
			}
			else if (recordFunctionBody && recordStaticTextBody == false && recordTextCtrlVar == false) {
				std::cout << line << std::endl;
				functionBody += line + "\n";
			}

			if (recordStaticTextBody == false && recordFunctionBody == false && recordTextCtrlVar == false
				&& strstr(lineNoSpaces.c_str(), "{///lbl") != NULL) {
				recordStaticTextBody = true;
				staticTextBody = "";
			}
			else if (recordStaticTextBody && recordFunctionBody == false && recordTextCtrlVar == false
				&& strstr(lineNoSpaces.c_str(), "}///") != NULL) {
				recordStaticTextBody = false;
				statTxtName_BodyPair.push_back({ "StaticText_" + std::to_string(statTxtName_BodyPair.size()), staticTextBody });

				int index = statTxtName_BodyPair.size() - 1;
				uiBodyCombined += "                wxStaticText* " + statTxtName_BodyPair[index].first + " = new wxStaticText(panel2, ID_COMMANDS::" +
					statTxtName_BodyPair[index].first + ", \"" + statTxtName_BodyPair[index].second + "\");\n";
				uiBodyCombined += "            vbox2->Add(" + statTxtName_BodyPair[index].first + ");\n";
			}
			else if (recordStaticTextBody && recordFunctionBody == false && recordTextCtrlVar == false) {
				std::cout << line << std::endl;
				size_t firstDQuote = line.find_first_of("\"");
				size_t lastDQuote = line.find_last_of("\"");
				if (staticTextBody == "")
					staticTextBody += line.substr(firstDQuote + 1, lastDQuote - firstDQuote - 1);
				else
					staticTextBody += "\n" + line.substr(firstDQuote+1,lastDQuote-firstDQuote-1);
			}

			if (recordTextCtrlVar == false && recordStaticTextBody == false && recordFunctionBody == false
				&& strstr(lineNoSpaces.c_str(), "{///get") != NULL) {
				size_t rightAngPos = line.find_last_of(">");
				txtCtrlType = line.substr(rightAngPos + 1, line.length() - rightAngPos - 1);
				recordTextCtrlVar = true;
			}
			else if (recordTextCtrlVar && recordStaticTextBody == false && recordFunctionBody == false
				&& strstr(lineNoSpaces.c_str(), "}///") != NULL) {
				recordTextCtrlVar = false;
			}
			else if (recordTextCtrlVar && recordStaticTextBody == false && recordFunctionBody == false) {
				std::cout << line << std::endl;
				size_t rightAngPos = line.find_last_of(">");
				size_t semicolonPos = line.find_last_of(";");
				if (rightAngPos != std::string::npos && semicolonPos != std::string::npos) {
					std::string refVarName = line.substr(rightAngPos + 1, semicolonPos - rightAngPos - 1);
					refVarName = removeSpaces(refVarName);
					txtCtrlRefVarName_TypePair.push_back({ refVarName,txtCtrlType });

					int index = txtCtrlRefVarName_TypePair.size() - 1;
					if (strstr(txtCtrlRefVarName_TypePair[index].second.c_str(), "int") != NULL) {
						uiBodyCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + " = new wxTextCtrl(panel2, ID_COMMANDS::" +
							"txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "_id, \"\", wxDefaultPosition, wxDefaultSize, 0, intValidator);\n";
					}
					else if (strstr(txtCtrlRefVarName_TypePair[index].second.c_str(), "float") != NULL) {
						uiBodyCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + " = new wxTextCtrl(panel2, ID_COMMANDS::" +
							"txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "_id, \"\", wxDefaultPosition, wxDefaultSize, 0, floatValidator);\n";
					}
					else {
						uiBodyCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + " = new wxTextCtrl(panel2, ID_COMMANDS::" +
							"txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "_id, \"\", wxDefaultPosition, wxDefaultSize, 0, textValidator);\n";
					}

					uiBodyCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "->Connect(ID_COMMANDS::" + "txtCtrl_" + 
						txtCtrlRefVarName_TypePair[index].first + "_id, wxEVT_COMMAND_TEXT_UPDATED, wxObjectEventFunction(&MyFrame::OnTextCtrlUpdate), NULL, this);\n";
					uiBodyCombined += "            vbox2->Add(txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + ");\n";
				}
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

	std::string buttonFuncDecsAndDefs = "";
	std::string buttonEnums = "";
	
	for (int i = 0; i < buttonName_CodePair.size(); ++i) {
		buttonFuncDecsAndDefs += "    void On" + buttonName_CodePair[i].first + "(wxCommandEvent& event) {\n";
		buttonFuncDecsAndDefs += buttonName_CodePair[i].second;
		buttonFuncDecsAndDefs += "    }\n";
		
		buttonEnums += "    " + buttonName_CodePair[i].first + ",\n";
	}

	std::string staticTextEnums = "";

	for (int i = 0; i < statTxtName_BodyPair.size(); ++i) {
		staticTextEnums += "    " + statTxtName_BodyPair[i].first + ",\n";
	}

	std::string textCtrlVarEnums = "";
	std::string textCtrlVarDeclarations = "";
	std::string textCtrlRefDefinitions = "";

	for (int i = 0; i < txtCtrlRefVarName_TypePair.size(); ++i) {
		textCtrlVarEnums += "    txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + "_id,\n";
		textCtrlVarDeclarations += "    wxTextCtrl* txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + ";\n";

		textCtrlRefDefinitions += "        if(event.GetId() == ID_COMMANDS::txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + "_id) {\n";
		textCtrlRefDefinitions += "            std::string val(txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + "->GetValue());\n";
		if (strstr(txtCtrlRefVarName_TypePair[i].second.c_str(), "int") != NULL) {
			textCtrlRefDefinitions += "            " + txtCtrlRefVarName_TypePair[i].first + " = std::stoi(val);\n";
		}
		else if (strstr(txtCtrlRefVarName_TypePair[i].second.c_str(), "float") != NULL) {
			textCtrlRefDefinitions += "            " + txtCtrlRefVarName_TypePair[i].first + " = std::stof(val);\n";
		}
		else {
			textCtrlRefDefinitions += "            " + txtCtrlRefVarName_TypePair[i].first + " = val;\n";
		}
		textCtrlRefDefinitions += "        }\n";
	}



	std::string widgetsBoilerPlate = "\n"
		"#include \"wx/wxprec.h\"\n"
		"#ifndef WX_PRECOMP\n"
		"#include \"wx/wx.h\"\n"
		"#endif\n"
		"#ifndef wxHAS_IMAGES_IN_RESOURCES\n"
		"#include \"../sample.xpm\"\n"
		"#endif\n"
		"#include <wx/valnum.h>\n";

	widgetsBoilerPlate += includesStr;
	
	widgetsBoilerPlate += usingNamespacesStr;

	for (int i = 0; i < staticUnscopedVars.size(); ++i)
		if(staticUnscopedVars[i].definitionExists)
			widgetsBoilerPlate += staticUnscopedVars[i].declarationNdDefinition + "\n";

	for (int i = 0; i < globalUnscopeVars.size(); ++i)
		widgetsBoilerPlate += globalUnscopeVars[i].declaration + "\n";
	
	for (int i = 0; i < staticUnscopedFunctions.size(); ++i)
		if(staticUnscopedFunctions[i].definitionExists)
			widgetsBoilerPlate += staticUnscopedFunctions[i].declarationNdDefinition + "\n";
	
	for (int i = 0; i < globalUnscopedFunctions.size(); ++i)
		widgetsBoilerPlate += globalUnscopedFunctions[i].declaration + "\n";
	
	widgetsBoilerPlate += "enum ID_COMMANDS {\n"
		"    Minimal_Quit = wxID_EXIT,\n"
		"    Minimal_About = wxID_ABOUT,\n"
		"    Begin_User_Enum = wxID_HIGHEST + 1,\n"
		"    Text_Ctrl_Log, \n";
	
	widgetsBoilerPlate += buttonEnums;
	widgetsBoilerPlate += staticTextEnums;
	widgetsBoilerPlate += textCtrlVarEnums;
	
	widgetsBoilerPlate += "};\n"
		"class MyApp : public wxApp {\n"
		"private:\n"
		"public:\n"
		"    virtual bool OnInit() wxOVERRIDE;\n"
		"};\n"
		"class MyFrame : public wxFrame {\n"
		"private:\n"
		"    wxTextCtrl* textctrlLog;\n"
		"    wxStreamToTextRedirector* redirector;\n";

	for (int i = 0; i < scopedMainVars.size(); ++i)
		widgetsBoilerPlate += scopedMainVars[i].declaration + "\n";

	widgetsBoilerPlate += textCtrlVarDeclarations + "\n";

	widgetsBoilerPlate += "    void OnTextCtrlUpdate(wxCommandEvent& event) {\n";
	widgetsBoilerPlate += textCtrlRefDefinitions;
	widgetsBoilerPlate += "    }\n";

	widgetsBoilerPlate += buttonFuncDecsAndDefs;
	
	//TODO:		Add text ctrl and static text boxes
	
	widgetsBoilerPlate += "public:\n"
		"    MyFrame(const wxString& title);\n"
		"    void OnQuit(wxCommandEvent& event);\n"
		"    void OnAbout(wxCommandEvent& event);\n"
		"    wxDECLARE_EVENT_TABLE();\n"
		"};\n"
		"wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)\n"
		"EVT_MENU(ID_COMMANDS::Minimal_Quit, MyFrame::OnQuit)\n"
		"EVT_MENU(ID_COMMANDS::Minimal_About, MyFrame::OnAbout)\n"
		"wxEND_EVENT_TABLE()\n"
		"wxIMPLEMENT_APP(MyApp);\n"
		"bool MyApp::OnInit() {\n"
		"    if (!wxApp::OnInit())\n"
		"        return false;\n"
		"    MyFrame* frame = new MyFrame(\"windoow\");\n"
		"    frame->Show(true);\n"
		"    return true;\n"
		"}\n"
		"MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title)\n";

	for (int i = 0; i < scopedMainVars.size(); ++i)
		if(scopedMainVars[i].definitionExists)
			widgetsBoilerPlate += "," + scopedMainVars[i].definition;
	
	widgetsBoilerPlate += "{\n"
		"    SetIcon(wxICON(sample));\n"
		"    #if wxUSE_MENUBAR\n"
		"    wxMenu* fileMenu = new wxMenu;\n"
		"    wxMenu* helpMenu = new wxMenu;\n"
		"    helpMenu->Append(ID_COMMANDS::Minimal_About, \"&About\tF1\", \"Show about dialog\");\n"
		"    fileMenu->Append(ID_COMMANDS::Minimal_Quit, \"E&xit\tAlt-X\", \"Quit this program\");\n"
		"    wxMenuBar* menuBar = new wxMenuBar();\n"
		"    menuBar->Append(fileMenu, \"&File\");\n"
		"    menuBar->Append(helpMenu, \"&Help\");\n"
		"    wxPanel *panel = new wxPanel(this, -1);\n"
		"    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);\n"
		"        wxScrolledWindow* panel2 = new wxScrolledWindow(panel, -1);\n"
		"            wxBoxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);\n"
		"                float validateF;\n"
		"                wxFloatingPointValidator<float> floatValidator(6, &validateF, wxNUM_VAL_ZERO_AS_BLANK);\n"
		"                wxIntegerValidator<int> intValidator;\n"
		"                wxTextValidator textValidator;\n"
		"                floatValidator.SetRange(-FLT_MAX, FLT_MAX);\n"
		"                intValidator.SetRange(INT_MIN, INT_MAX);\n";

	widgetsBoilerPlate += uiBodyCombined;
	
	widgetsBoilerPlate += "\t\tpanel2->SetScrollRate(5, 5);\n"
		"        panel2->SetSizer(vbox2);\n"
		"    vbox->Add(panel2, 1, wxEXPAND | wxALL, 10);\n"
		"    textctrlLog = new wxTextCtrl(panel, ID_COMMANDS::Text_Ctrl_Log, wxT(\"\"), wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);\n"
		"    textctrlLog->SetEditable(false);\n"
		"    vbox->Add(textctrlLog, 1, wxEXPAND, 0);\n"
		"    panel->SetSizer(vbox);\n"
		"    SetMenuBar(menuBar);\n"
		"    #else // !wxUSE_MENUBAR\n"
		"    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);\n"
		"    wxButton* aboutBtn = new wxButton(this, wxID_ANY, \"About...\");\n"
		"    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);\n"
		"    sizer->Add(aboutBtn, wxSizerFlags().Center());\n"
		"    SetSizer(sizer);\n"
		"    #endif // wxUSE_MENUBAR/!wxUSE_MENUBAR\n"
		"    #if wxUSE_STATUSBAR\n"
		"    CreateStatusBar(2);\n"
		"    SetStatusText(\"Welcome to wxWidgets!\");\n"
		"    #endif // wxUSE_STATUSBAR\n"
		"    redirector = new wxStreamToTextRedirector(textctrlLog);\n"
		"}\n"
		"void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {\n"
		"    Close(true);\n"
		"}\n"
		"void MyFrame::OnAbout(wxCommandEvent & WXUNUSED(event)) {\n"
		"    wxMessageBox(wxString::Format\n"
		"    (\n"
		"        \"Welcome to %s!\\n\"\n"
		"        \"\\n\"\n"
		"        \"This is the minimal wxWidgets sample\\n\"\n"
		"        \"running under %s.\",\n"
		"        wxVERSION_STRING,\n"
		"        wxGetOsDescription()\n"
		"    ),\n"
		"    \"About wxWidgets minimal sample\",\n"
		"    wxOK | wxICON_INFORMATION,\n"
		"    this);\n"
		"}";

	std::ofstream outWidgetFile(getDiretoryFromPath(mainFilePath) + "/C2GWxWidgets.cpp");
	outWidgetFile << widgetsBoilerPlate << std::endl;
	outWidgetFile.close();

	return 0;
}

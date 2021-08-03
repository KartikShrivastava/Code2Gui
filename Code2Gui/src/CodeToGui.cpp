#include "CodeToGui.h"

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <sstream>

CodeToGui::CodeToGui() {
	mainFileAllIncludes = "";
	mainFileAllUsingNamespaces = "";
	allWxButtonIdStr = "";
	allWxStaticTextIdStr = "";
	allWxTextCtrlIdStr = "";
	allWxTextCtrlMemberDeclarationStr = "";
	allWxTextCtrlReferedVarDefinitions = "";
	allWxButtonFuncDecsAndDefs = "";
	wxUiLayoutCombined = "";
}

bool CodeToGui::IsGuiUpdateAllowed() {
	return guiUpdateAllowed;
}

std::string CodeToGui::GetUserMainFilePath() {
	return userMainFilePath;
}

int CodeToGui::LoadInfoFromMetadataFile(const std::string& metadataFilePath) {
	std::string windowTitle;
	std::string guiUpdateStr;
	std::ifstream c2gUserMeta(metadataFilePath);
	if (!c2gUserMeta.is_open())
		return -1;

	std::getline(c2gUserMeta, windowTitle);
	std::getline(c2gUserMeta, userMainFilePath);
	std::getline(c2gUserMeta, guiUpdateStr);
	if (guiUpdateStr.compare("0") == 0)
		guiUpdateAllowed = false;
	else
		guiUpdateAllowed = true;
	c2gUserMeta.close();

	return 0;
}

int CodeToGui::LoadInfoFromTagFile(const std::string& metadataFilePath) {
	std::ifstream ctagsFile("C:\\Home\\Projects\\Github\\Code2Gui\\binCode2Gui\\x64\\Debug\\tags.txt");
	//std::ifstream ctagsFile("tags.txt");
	if (!ctagsFile.is_open())
		return -1;

	std::string line = "";

	///		Important Assumption: ctags.txt is sorted according to line number
	///		If it wont be sorted according to line numbers then code will break!
	bool mainSeenInTagFile = false;

	std::unordered_set<int> capturedLines;
	std::unordered_set<std::string> capturedTitle;

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
			if (strstr(token.c_str(), "$/;\"") != NULL)
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
			if (strstr(tokens[kindIdx].c_str(), "variable") != NULL)
				tempTagDetail.kind = KindTag::VARIABLE;
			else if (strstr(tokens[kindIdx].c_str(), "function") != NULL)
				tempTagDetail.kind = KindTag::FUNCTION;
			else if (strstr(tokens[kindIdx].c_str(), "local") != NULL)
				tempTagDetail.kind = KindTag::LOCAL;
			else
				tempTagDetail.kind = KindTag::UNSUPPORTED;

			///		Get the line number of the tag
			size_t colonIdx = tokens[lineIdx].find_first_of(":");
			if (colonIdx != std::string::npos) {
				std::string lineNum = tokens[lineIdx].substr(colonIdx + 1, tokens[declarationIdx].length() - colonIdx);
				tempTagDetail.lineNum = std::stoi(lineNum);
			}
			///		If line number is not assinged then ultimately code will break
			else
				tempTagDetail.lineNum = -1;

			///		Do not store tag in case of invalid line number or repeated tag line numbers(can happen in ctags
			///		in case of statements with namespace or class scopes)
			if (tempTagDetail.lineNum == -1 ||
				capturedLines.find(tempTagDetail.lineNum) != capturedLines.end() ||
				capturedTitle.find(tempTagDetail.title) != capturedTitle.end())
				continue;
			else {
				capturedLines.insert(tempTagDetail.lineNum);
				capturedTitle.insert(tempTagDetail.title);
			}

			///		Check if main tag has encountered so far or not
			if (tempTagDetail.title.compare("main") == 0 && tempTagDetail.kind == KindTag::FUNCTION)
				mainSeenInTagFile = true;

			///		Extract the definition and declaration from the 2nd indexed token
			size_t startPos = tokens[declarationIdx].find_first_of("^");
			size_t endPos = tokens[declarationIdx].find_first_of("$");

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
			if (endPos != std::string::npos) {
				std::string declarationNdDef;
				if (startPos != std::string::npos)
					declarationNdDef = tokens[declarationIdx].substr(startPos + 1, endPos - startPos - 1);
				else
					declarationNdDef = tokens[declarationIdx].substr(0, endPos - startPos - 1);

				if (!mainSeenInTagFile) {
					if (tempTagDetail.kind == KindTag::VARIABLE) {
						///		[------------UNSCOPED STATIC VARIBLES------------]
						///		we'll save decleration + definition(if possible) and put it at the top of *WxWidgets.cpp file
						if (strstr(declarationNdDef.c_str(), "static") != NULL) {
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
								if (strstr(declarationNdDef.c_str(), ";") != NULL)
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
					else if (tempTagDetail.kind == KindTag::FUNCTION) {
						///		[------------UNSCOPED STATIC FUNCTIONS------------]
						///		we'll save decleration + definition(if possible) and put it at the top of *WxWidgets.cpp file
						if (strstr(declarationNdDef.c_str(), "static") != NULL) {
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
							tempScopedMainVars.declaration = "    " + declarationNdDef.substr(0, equalstoPos) + ";";
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
								tempScopedMainVars.declaration = "    " + declarationNdDef.substr(0, equalstoPos) + ";";
								tempScopedMainVars.definition = tempScopedMainVars.title + "(" +
									declarationNdDef.substr(equalstoPos, declarationNdDef.length() - equalstoPos);
							}
							///		Assuming that semicolor is present
							else {
								tempScopedMainVars.declaration = "    " + declarationNdDef;
								tempScopedMainVars.definition = tempScopedMainVars.title + "(";
							}
						}
						scopedMainVars.push_back(tempScopedMainVars);
					}
				}
			}
		}
	}
	
	ctagsFile.close();

	return 0;
}

int CodeToGui::LoadInfoFromMainFile() {
	std::ifstream mainFile(userMainFilePath);
	if (mainFile.is_open() == false)
		return -1;

	std::string line = "";
	int lineNumber = 1;
	bool searchingSUVarDSemiColon = false,		///		SUVD: Static Unscoped Var Definition
		searchingSUFuncDRParanthesis = false;	///		SUFDR: Static Unscoped Function Definition Right
	bool mainSeen = false;

	///		Method to capture the left curly bracket of main method "{"
	///		Also helps in checking whether we're in right file or not
	while (std::getline(mainFile, line)) {
		if (strstr(line.c_str(), "#include") != NULL)
			mainFileAllIncludes += line + "\n";
		else if (strstr(line.c_str(), "using namespace") != NULL)
			mainFileAllUsingNamespaces += line + "\n";
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
				staticUnscopedFunctions[staticUnscpFuncsNoDefLines.front().second].definitionExists = true;
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
		std::string lineNoSpaces = RemoveSpaces(line);
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
	std::vector<std::pair<std::string, std::string>> buttonName_CodePair;
	std::vector<std::pair<std::string, std::string>> statTxtName_BodyPair;
	///		vector of pair of reference variable name whose value needs to be updated and it's type
	std::vector<std::pair<std::string, std::string>> txtCtrlRefVarName_TypePair;

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

			std::string lineNoSpaces = RemoveSpaces(line);

			if (recordFunctionBody == false && recordStaticTextBody == false && recordTextCtrlVar == false
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
				wxUiLayoutCombined += "                wxButton* " + buttonName_CodePair[index].first + " = new wxButton(panel2, ID_COMMANDS::" +
					buttonName_CodePair[index].first + ", \"" + buttonName_CodePair[index].first + "\");\n";
				wxUiLayoutCombined += "                " + buttonName_CodePair[index].first + "->Connect(ID_COMMANDS::" + buttonName_CodePair[index].first +
					", wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::On" + buttonName_CodePair[index].first + "), NULL, this);\n";
				wxUiLayoutCombined += "            vbox2->Add(" + buttonName_CodePair[index].first + ");\n";
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
				wxUiLayoutCombined += "                wxStaticText* " + statTxtName_BodyPair[index].first + " = new wxStaticText(panel2, ID_COMMANDS::" +
					statTxtName_BodyPair[index].first + ", \"" + statTxtName_BodyPair[index].second + "\");\n";
				wxUiLayoutCombined += "            vbox2->Add(" + statTxtName_BodyPair[index].first + ");\n";
			}
			else if (recordStaticTextBody && recordFunctionBody == false && recordTextCtrlVar == false) {
				std::cout << line << std::endl;
				size_t firstDQuote = line.find_first_of("\"");
				size_t lastDQuote = line.find_last_of("\"");
				if (staticTextBody == "")
					staticTextBody += line.substr(firstDQuote + 1, lastDQuote - firstDQuote - 1);
				else
					staticTextBody += "\n" + line.substr(firstDQuote + 1, lastDQuote - firstDQuote - 1);
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
					refVarName = RemoveSpaces(refVarName);
					txtCtrlRefVarName_TypePair.push_back({ refVarName,txtCtrlType });

					int index = txtCtrlRefVarName_TypePair.size() - 1;
					if (strstr(txtCtrlRefVarName_TypePair[index].second.c_str(), "int") != NULL) {
						wxUiLayoutCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + " = new wxTextCtrl(panel2, ID_COMMANDS::" +
							"txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "_id, \"\", wxDefaultPosition, wxDefaultSize, 0, intValidator);\n";
					}
					else if (strstr(txtCtrlRefVarName_TypePair[index].second.c_str(), "float") != NULL) {
						wxUiLayoutCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + " = new wxTextCtrl(panel2, ID_COMMANDS::" +
							"txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "_id, \"\", wxDefaultPosition, wxDefaultSize, 0, floatValidator);\n";
					}
					else {
						wxUiLayoutCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + " = new wxTextCtrl(panel2, ID_COMMANDS::" +
							"txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "_id, \"\", wxDefaultPosition, wxDefaultSize, 0, textValidator);\n";
					}

					wxUiLayoutCombined += "                txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + "->Connect(ID_COMMANDS::" + "txtCtrl_" +
						txtCtrlRefVarName_TypePair[index].first + "_id, wxEVT_COMMAND_TEXT_UPDATED, wxObjectEventFunction(&MyFrame::OnTextCtrlUpdate), NULL, this);\n";
					wxUiLayoutCombined += "            vbox2->Add(txtCtrl_" + txtCtrlRefVarName_TypePair[index].first + ");\n";
				}
			}

			++lineNumber;
		}
	}
	else {
		std::cout << "Code2Gui::Error:: \"main\" method entry point not found" << userMainFilePath << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	mainFile.close();

	SaveButtonIds(buttonName_CodePair);
	FormatButtonCallbacks(buttonName_CodePair);
	
	SaveStaticTextIds(statTxtName_BodyPair);
	
	SaveTextCtrlIds(txtCtrlRefVarName_TypePair);
	FormatTextCtrlDeclarations(txtCtrlRefVarName_TypePair);
	FormatTextCtrlCallbacks(txtCtrlRefVarName_TypePair);

	return 0;
}

std::string CodeToGui::RemoveSpaces(const std::string& line) {
	std::string stringNoSpaces = "";
	for (int i = 0; i < line.length(); ++i)
		if (line[i] != '\t' && line[i] != ' ')
			stringNoSpaces += line[i];
	return stringNoSpaces;
}

std::string CodeToGui::GetDiretoryFromPath(const std::string& path) {
	size_t pos = path.find_last_of("\\/");
	return (pos == std::string::npos) ? "" : path.substr(0, pos);
}

void CodeToGui::SaveButtonIds(const std::vector<std::pair<std::string, std::string>>& buttonName_CodePair) {
	for (int i = 0; i < buttonName_CodePair.size(); ++i) {
		allWxButtonIdStr += "    " + buttonName_CodePair[i].first + ",\n";
	}
}

void CodeToGui::FormatButtonCallbacks(const std::vector<std::pair<std::string, std::string>>& buttonName_CodePair) {
	for (int i = 0; i < buttonName_CodePair.size(); ++i) {
		allWxButtonFuncDecsAndDefs += "    void On" + buttonName_CodePair[i].first + "(wxCommandEvent& event) {\n";
		allWxButtonFuncDecsAndDefs += buttonName_CodePair[i].second;
		allWxButtonFuncDecsAndDefs += "    }\n";
	}
}

void CodeToGui::SaveStaticTextIds(const std::vector<std::pair<std::string, std::string>>& statTxtName_BodyPair) {
	for (int i = 0; i < statTxtName_BodyPair.size(); ++i) {
		allWxStaticTextIdStr += "    " + statTxtName_BodyPair[i].first + ",\n";
	}
}

void CodeToGui::SaveTextCtrlIds(const std::vector<std::pair<std::string, std::string>>& txtCtrlRefVarName_TypePair) {
	for (int i = 0; i < txtCtrlRefVarName_TypePair.size(); ++i) {
		allWxTextCtrlIdStr += "    txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + "_id,\n";
	}
}

void CodeToGui::FormatTextCtrlDeclarations(const std::vector<std::pair<std::string, std::string>>& txtCtrlRefVarName_TypePair) {
	for (int i = 0; i < txtCtrlRefVarName_TypePair.size(); ++i) {
		allWxTextCtrlMemberDeclarationStr += "    wxTextCtrl* txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + ";\n";
	}
}

void CodeToGui::FormatTextCtrlCallbacks(const std::vector<std::pair<std::string, std::string>>& txtCtrlRefVarName_TypePair) {
	for (int i = 0; i < txtCtrlRefVarName_TypePair.size(); ++i) {
		allWxTextCtrlReferedVarDefinitions += "        if(event.GetId() == ID_COMMANDS::txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + "_id) {\n";
		allWxTextCtrlReferedVarDefinitions += "            std::string val(txtCtrl_" + txtCtrlRefVarName_TypePair[i].first + "->GetValue());\n";
		if (strstr(txtCtrlRefVarName_TypePair[i].second.c_str(), "int") != NULL) {
			allWxTextCtrlReferedVarDefinitions += "            " + txtCtrlRefVarName_TypePair[i].first + " = std::stoi(val);\n";
		}
		else if (strstr(txtCtrlRefVarName_TypePair[i].second.c_str(), "float") != NULL) {
			allWxTextCtrlReferedVarDefinitions += "            " + txtCtrlRefVarName_TypePair[i].first + " = std::stof(val);\n";
		}
		else {
			allWxTextCtrlReferedVarDefinitions += "            " + txtCtrlRefVarName_TypePair[i].first + " = val;\n";
		}
		allWxTextCtrlReferedVarDefinitions += "        }\n";
	}
}

void CodeToGui::GenerateGuiBoilerplateCode() {
	std::string widgetsBoilerPlate = "\n"
		"#include \"wx/wxprec.h\"\n"
		"#ifndef WX_PRECOMP\n"
		"#include \"wx/wx.h\"\n"
		"#endif\n"
		"#ifndef wxHAS_IMAGES_IN_RESOURCES\n"
		"#include \"../sample.xpm\"\n"
		"#endif\n"
		"#include <wx/valnum.h>\n";

	widgetsBoilerPlate += mainFileAllIncludes;

	widgetsBoilerPlate += mainFileAllUsingNamespaces;

	for (int i = 0; i < staticUnscopedVars.size(); ++i)
		if (staticUnscopedVars[i].definitionExists)
			widgetsBoilerPlate += staticUnscopedVars[i].declarationNdDefinition + "\n";

	for (int i = 0; i < globalUnscopeVars.size(); ++i)
		widgetsBoilerPlate += globalUnscopeVars[i].declaration + "\n";

	for (int i = 0; i < staticUnscopedFunctions.size(); ++i)
		if (staticUnscopedFunctions[i].definitionExists)
			widgetsBoilerPlate += staticUnscopedFunctions[i].declarationNdDefinition + "\n";

	for (int i = 0; i < globalUnscopedFunctions.size(); ++i)
		widgetsBoilerPlate += globalUnscopedFunctions[i].declaration + "\n";

	widgetsBoilerPlate += "enum ID_COMMANDS {\n"
		"    Minimal_Quit = wxID_EXIT,\n"
		"    Minimal_About = wxID_ABOUT,\n"
		"    Begin_User_Enum = wxID_HIGHEST + 1,\n"
		"    Text_Ctrl_Log, \n";

	widgetsBoilerPlate += allWxButtonIdStr;
	widgetsBoilerPlate += allWxStaticTextIdStr;
	widgetsBoilerPlate += allWxTextCtrlIdStr;

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

	widgetsBoilerPlate += allWxTextCtrlMemberDeclarationStr + "\n";

	widgetsBoilerPlate += "    void OnTextCtrlUpdate(wxCommandEvent& event) {\n";
	widgetsBoilerPlate += allWxTextCtrlReferedVarDefinitions;
	widgetsBoilerPlate += "    }\n";

	widgetsBoilerPlate += allWxButtonFuncDecsAndDefs;

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
		if (scopedMainVars[i].definitionExists)
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

	widgetsBoilerPlate += wxUiLayoutCombined;

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

	std::ofstream outWidgetFile(GetDiretoryFromPath(userMainFilePath) + "/C2GWxWidgets.cpp");
	outWidgetFile << widgetsBoilerPlate << std::endl;
	outWidgetFile.close();
}

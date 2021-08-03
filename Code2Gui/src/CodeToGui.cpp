#include "CodeToGui.h"

#include <iostream>
#include <fstream>

CodeToGui::CodeToGui() {

}

int CodeToGui::LoadInfoFromMetadata(const std::string& metadataFilePath) {
	std::string windowTitle;
	std::string mainFilePath;
	std::string guiUpdateStr;
	std::ifstream c2gUserMeta(metadataFilePath);
	if (!c2gUserMeta.is_open())
		return -1;

	std::getline(c2gUserMeta, windowTitle);
	std::getline(c2gUserMeta, mainFilePath);
	std::getline(c2gUserMeta, guiUpdateStr);
	if (guiUpdateStr.compare("0") == 0)
		guiUpdate = false;
	else
		guiUpdate = true;
	c2gUserMeta.close();

	return 0;
}

int CodeToGui::LoadInfoFromTagFile(const std::string& metadataFilePath) {
	std::ifstream ctagsFile("C:\\Home\\Projects\\Github\\Code2Gui\\binCode2Gui\\x64\\Debug\\tags.txt");
	//std::ifstream ctagsFile("tags.txt");
	if (!ctagsFile.is_open())
		return -1;

	std::string line = "";

	///		Array of line splits of those members which are unscoped and marked as static in main file
	///		These varibles need static re-declaration and re-definition in C2GWxWidgets file
	std::vector<UnscopedStaticVars> staticUnscopedVars;
	///		List of pairs of lineNumbers and vector indices of the corresponding line number
	std::list<std::pair<int, int>> staticUnscpVarsNoDefLines;

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
	return 0;
}

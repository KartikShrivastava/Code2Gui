#pragma once

#include <string>
#include <vector>
#include <list>

enum KindTag
{
	VARIABLE,
	FUNCTION,
	LOCAL,
	UNSUPPORTED
};

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

class CodeToGui
{
private:
	std::string userMainFilePath;
	
	bool guiUpdateAllowed;

	///		Array of line splits of those members which are unscoped(and global) present in main file
	///		These varibles need extern keyword declaration in C2GWxWidgets file
	std::vector<UnscopedNonStaticVars> globalUnscopeVars;

	///		Array of line splits of those methods/functions which are unscoped(and global) present in main file
	///		These methods need extern keyword declaration in C2GWxWidgets file
	std::vector<UnscopedNonStaticFunctions> globalUnscopedFunctions;

	///		Array of line splits of those members which are unscoped and marked as static in main file
	///		These varibles need static re-declaration and re-definition in C2GWxWidgets file
	std::vector<UnscopedStaticVars> staticUnscopedVars;
	
	///		List of pairs of lineNumbers and vector indices of the corresponding line number
	std::list<std::pair<int, int>> staticUnscpVarsNoDefLines;

	///		Array of line splits of those methods/functions which are unscoped and marked as static in main file
	///		These methods need static re-declaration and re-definition in C2GWxWidgets file
	std::vector<UnscopedStaticFunctions> staticUnscopedFunctions;
	
	///		List of pairs of lineNumbers and vector indices of the corresponding line number
	std::list<std::pair<int, int>> staticUnscpFuncsNoDefLines;

	///		Array of line splits of those variables which are defined in main method
	///		Declaration of these variables need to be extracted and put into Frame class and
	///		Defintion of these variables need to be extracted and put into contructor of Frame class
	std::vector<ScopedMainVars> scopedMainVars;
	
	///		List of pairs of lineNumbers and vector indices of the corresponding line number
	std::list<std::pair<int, int>> scpMainVarsNoDefLines;

	std::string mainFileAllIncludes;

	std::string mainFileAllUsingNamespaces;

	std::string allWxButtonIdStr;

	std::string allWxStaticTextIdStr;

	std::string allWxTextCtrlIdStr;

	std::string allWxTextCtrlMemberDeclarationStr;

	std::string allWxTextCtrlReferedVarDefinitions;

	std::string allWxButtonFuncDecsAndDefs;

	///		Helpful in maintaining order of ui elements
	std::string wxUiLayoutCombined;

	std::string RemoveSpaces(const std::string& line);

	std::string GetDiretoryFromPath(const std::string& path);

	void SaveButtonIds(const std::vector<std::pair<std::string, std::string>>& buttonName_CodePair);

	void FormatButtonCallbacks(const std::vector<std::pair<std::string, std::string>>& buttonName_CodePair);

	void SaveStaticTextIds(const std::vector<std::pair<std::string, std::string>>& statTxtName_BodyPair);

	void SaveTextCtrlIds(const std::vector<std::pair<std::string, std::string>>& txtCtrlRefVarName_TypePair);

	void FormatTextCtrlCallbacks(const std::vector<std::pair<std::string, std::string>>& txtCtrlRefVarName_TypePair);

	void FormatTextCtrlDeclarations(const std::vector<std::pair<std::string, std::string>>& txtCtrlRefVarName_TypePair);

public:

	CodeToGui();

	bool IsGuiUpdateAllowed();
	std::string GetUserMainFilePath();

	int LoadInfoFromMetadataFile(const std::string& metadataFilePath);
	int LoadInfoFromTagFile(const std::string& metadataFilePath);
	int LoadInfoFromMainFile();

	void GenerateGuiBoilerplateCode();
};

#pragma once

#include <string>

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
public:
	bool guiUpdate;

	CodeToGui();

	int LoadInfoFromMetadata(const std::string& metadataFilePath);
	int LoadInfoFromTagFile(const std::string& metadataFilePath);

};
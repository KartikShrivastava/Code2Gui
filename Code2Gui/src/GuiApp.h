#pragma once

#include <string>

class GuiApp
{
public:
	std::string windowTitle = "Test main file access";

	///		Contains relative path of cpp file which contains main method.
	///		Note that relative path evaluation starts from solution(.sln)
	///		directory.
	std::string mainFilePath = "Code2Gui/src/main.cpp";
};
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <list>
#include "CodeToGui.h"

int main() {
	int result = -1;
	
	CodeToGui codeToGui;
	result = codeToGui.LoadInfoFromMetadataFile("C:\\Home\\Projects\\Github\\Code2Gui\\ClientTest\\C2GMetadata.txt");

	if (result == -1) {
		std::cout << "Code2Gui::Log:: Unable to open/read C2GMetadata.txt" << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	if (!codeToGui.IsGuiUpdateAllowed()) {
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

	result = codeToGui.LoadInfoFromMainFile();

	if (result == -1) {
		std::cout << "Code2Gui::Error:: Could not open file: " << codeToGui.GetUserMainFilePath() << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
	}

	codeToGui.GenerateGuiBoilerplateCode();

	return 0;
}

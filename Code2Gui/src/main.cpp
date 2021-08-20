#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <list>
#include "CodeToGui.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Code2Gui::Log:: Bat file corrupted." << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	int result = -1;
	
	CodeToGui codeToGui;
	result = codeToGui.LoadInfoFromMetadataFile(argv[1]);

	if (result == -1) {
		std::cout << "Code2Gui::Log:: C2GMetadata.txt file got corrupted." << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	if (!codeToGui.IsGuiUpdateAllowed()) {
		std::cout << "Code2Gui::Log:: GuiUpdation set to false." << std::endl;
		std::cout << "Code2Gui::Log:: Exiting..." << std::endl;
		return 0;
	}

	result = codeToGui.LoadInfoFromTagFile();

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

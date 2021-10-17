## CodeToGui is now open source
This repository aims to create a faster way to generate gui from code without making any additionaly changes in the code-flow.
Currently Supported Language: C++
Currently Supported Platform: Windows

## Warning: Crude unformatted development readme

EndGame 1: (Done)
Steps:
1. Add a file named Code2GuiWxSource.cpp in the directory which contains main entry point file.
2. Put block of code which needs to executed as a button call between "{////" and "}////" symbols.
3. Add a file named Code2GuiInfo.txt.
4. Right click Code2GuiInfo.txt and go to "Properties".
5. Under Configuration Properties -> General change "Item Type" to "Custom Build Tool".
6. Click on "Apply".
7. Now Under Configuration Properties -> Custom Build Tool.
8. *Adding wxwidgets dependencies.

*- Most crucial as there many ways to do it depending upon the platform so
	which way will work for most of the people.


EndGame 2: (Done)
Steps:
1. Find how to run Custom Build Tool without generating output
2. Make an executable for setting up ui in any visual studio project
3. Prompt user to close all instances of visual studio
4. Ask user to enter:
	- .vcxproj location
	- location of cpp file which contains main
	- Test window title
	- Ask user which configuratio and platforms should widgets support
5. Create two files in the directory where main.cpp is present
	- First file will contain boiler plate wxWidget code: C2GWxWidgets.cpp
	- Second file will contain Custom Build Tool instructions and paths to files entered by user during setup
6. Store the information entered by user in a text file named C2GMetadata.txt
7. Edit .vcxproj file and add following contents:
	- Add another import of wxwidgets.prop property sheet under <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'
	  given label.
	- Add new ItemGroup(direcly under Project tag) to include C2GMetadata.txt in the project and include a CustomBuildTool instruction with it
		<ItemGroup>
    		<CustomBuild Include="C2GMetadata.txt">
	- Add new ItemGroup to include C2GWxWidgets.cpp file under ItemGroup Tag which contains rest of cpp files		
		<ItemGroup>
			<ClCompile Include="Code2GuiWxSource.cpp" />

EndGame 1:
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


EndGame 2:
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

Hey devs,
I'd like to know your interest in the Code to Gui app I'm working on. It is made to
help begineer programmers in making their college project, not so boring!
First 10 visual studio user can grab a free copy of tool and can help in testing and finding
bugs.


working ctags:

--list-kinds
    c  classes
    d  macro definitions
    e  enumerators (values inside an enumeration)
    f  function definitions
    g  enumeration names
    l  local variables [off]
    m  class, struct, and union members
    n  namespaces
    p  function prototypes [off]
    s  structure names
    t  typedefs
    u  union names
    v  variable definitions
    x  external and forward variable declarations [off]

--fields	
a	Access (or export) of class members
f	File-restricted scoping [enabled]
i	Inheritance information
k	Kind of tag as a single letter [enabled]
K	Kind of tag as full name
l	Language of source file containing tag
m	Implementation information
n	Line number of tag definition
s	Scope of tag definition [enabled]
S	Signature of routine (e.g. prototype or parameter list)
z	Include the "kind:" key in kind field
t	Type and name of a variable or typedef as "typeref:" field [enabled]

ctags -R --c++-kinds=l -x main.cpp 					:Local Variable 
ctags -x --c++-types=f --extra=q --format=1 main.cpp			:Functions with class scope
ctags -x --c++-kinds=v --file-scope=no main.cpp				:Global Variable
ctags -x --c++-kinds=+p main.cpp					:Specifies most things
ctags -x --c++-kinds=+cdefglmnpstuvx --extra=q --totals=yes main.cpp	:Specifies all things with redundancy
ctags --list-kinds							:kinds parameters
current:
1> ctags.exe -f "C:\Home\Projects\Github\Code2Gui\binCode2Gui\x64\Debug\tags.txt" --c++-kinds=+cdefglmnpstuvx --extra=q --format=1 -R c:\home\projects\github\code2gui\ClientTest\main.cpp
*2> "C:\Home\Projects\Github\Code2Gui\binCode2Gui\x64\Debug\ctags.exe" -f "C:\Home\Projects\Github\Code2Gui\binCode2Gui\x64\Debug\tags.txt" --c++-kinds=+cdefglmnpstuvx -R --fields=+afikKlmnsSzt --sort=0 "C:\Home\Projects\Github\Code2Gui\ClientTest\main.cpp"

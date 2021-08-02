///		include all the files, irrespective of redundancy
#include <iostream>
#include <vector>
#include "MyClass.h"

///		include using namespace lines
using namespace std;

///		include function declarations as externs(if non-static)
vector<int> foo(int x) {
	return vector<int>{2};
}

///		include whole function definition(if static)

///		include global variables as externs(if non-static)
int glob = 1;

///		include whole variable definition(if non-static)
static int glob2 = 2;

int main( int argc, char** argv)
{
	std::cout << "Main entry point!" << std::endl;
	
	int xx = 1;
	vector<int> xz;
	MyClass obj(xx);

	xx = 1;

	{//(
		xz = foo(xx);
	}//(

	{//<
		std::cout << "Print hello world" << std::endl;
	}//<

	{//(
		for (int i = 0; i < xz.size(); ++i)
			std::cout << xz[i] << std::endl;
	}//(

	{//>

	}//>

	return 0;
}

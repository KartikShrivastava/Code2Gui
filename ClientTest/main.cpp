#include <iostream>
#include <vector>
#include "MyClass.h"

using namespace std;

vector<int> foo(int x)
{
	return vector<int>{2};
}

static void Print(int val) {
	std::cout << val << std::endl;
}

///		include global variables as externs(if non-static)
int glob = 1;

///		include whole variable definition(if non-static)
static int glob2 = 22121;

int main( int argc, char** argv)
{
	{///lbl
		std::cout << "Main entry point!" << std::endl;
	}///

	int xx = 1;
	vector<int> xzx;
	MyClass obj(1);

	xx = 1;

	{///btn
		xzx = foo(xx);
	}///

	int zz;

	{///get>int
		std::cin >> zz;
	}///

	{///lbl
		std::cout << "call it" << std::endl;
	}///

	{///btn
		Print(glob2);
	}///

	{///btn
		for (int i = 0; i < xzx.size(); ++i)
			std::cout << xzx[i] << std::endl;
	}///

	return 0;
}

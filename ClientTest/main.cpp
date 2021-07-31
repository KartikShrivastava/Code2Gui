#include "MyClass.h"

#include <iostream>
#include <memory>

int main ( int argc, char** argv)
{
	std::cout << "Main entry point executed" << std::endl;

	{////
			std::cout << "Print hello world" << std::endl;
	}////

	std::unique_ptr<MyClass>myClass; 
	myClass = std::make_unique<MyClass>();
	{////
			myClass->CallMyMethod();
	}////

	return 0;
}
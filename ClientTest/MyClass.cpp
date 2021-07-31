#include "MyClass.h"

#include <iostream>

MyClass::MyClass() {
	std::cout << "My class constructed executed" << std::endl;
}

void MyClass::CallMyMethod() {
	std::cout << "My class method executed" << std::endl;
}

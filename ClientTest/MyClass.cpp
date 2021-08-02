#include "MyClass.h"

#include <iostream>

MyClass::MyClass(int x) {
	std::cout << "My class constructed executed" << std::endl;
}

void MyClass::CallMyMethod() {
	std::cout << "My class method executed" << std::endl;
}

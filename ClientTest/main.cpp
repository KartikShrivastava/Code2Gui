#include <iostream>

using namespace std;

int Square(int x) {
	return x*x;
}

int main( int argc, char** argv) {
	{///lbl
	cout << "Awesome console app, huh?" << endl;
	cout << "Enter a number: " << endl;
	}///

	int num;
	{///get>int
	cin >> num;
	}///
	
	int result;

	{///btn
	result = Square(num);
	cout << "Square of number: " << result << endl;
	}///

	return 0;
}

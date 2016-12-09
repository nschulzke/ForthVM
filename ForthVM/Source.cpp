#include <iostream>
#include <iomanip>
#include <fstream>
#include "Machine.h"
using namespace std;

int fileSize(const string& file)
{
	ifstream stream(file, ios::binary);
	int begin = stream.tellg();
	stream.seekg(0, ios::end);
	int end = stream.tellg();
	stream.close();
	return end - begin;
}

int main(int argc, char* argv[])
{
	Machine machine;
	string fileName;

	if (argc <= 1)
		fileName = "prog.obj";
	else
		fileName = argv[1];

	cout << "Loading file: " << fileName << endl << endl;

	int size = fileSize(fileName);
	ifstream input(fileName, ios::binary);
	char* arr = new char[size];

	input.read(arr, size);

	machine.arrayToMem(arr, size);

	machine.run();
//	cout << setbase(16);
	machine.dumpStacks();

	cout << endl;
	system("PAUSE");
	return 0;
}
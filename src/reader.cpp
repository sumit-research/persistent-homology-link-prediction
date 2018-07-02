#include <iostream>
#include <string>
#include <fstream>
#include <utility>

using namespace std;

int main(int argc, char* argv[]){
	double i;

	if(argc < 2){
		cout << "[Usage:] ./decTobin inputFilename\nProgram aborted\n";
	}

	string inFile = argv[1];

	ifstream iFile;
	iFile.open(inFile);

	int temp = 0;
	double j = 0;
	string line;
	while(getline(iFile, line)){

		i = stod(line);
		cout << i << '\n';
		if(i == j) temp++;
		
	}

	cout << temp << '\n';
}
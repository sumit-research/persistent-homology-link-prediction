#include <iostream>
#include <string>
#include <fstream>


using namespace std;

ifstream iFile;
ofstream oFile;

int64_t swap_int64( int64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32)  | ((val >> 32) & 0xFFFFFFFFULL);
}

int main(int argc, char* argv[]){
	double i;

	if(argc < 3){
		cout << "[Usage:] ./decTobin inputFilename outputFilename\nProgram aborted\n";
	}

	string inFile = argv[1];

	iFile.open(inFile, ios::binary);
	int64_t temp = 0

	while(iFile.read(reinterpret_cast<const char*>(&i), sizeof(double))){
		if(i == 0){
			temp++;
		}
	}
	cout << temp << '\n';
}
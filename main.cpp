/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE (4|5)85 Final Project
*/

// Cache specs:
// 16MB, 64 byte lines, 8-way associative.
// Uses write allocate, MESI, and psuedo-LRU.

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "cache.cpp" 

using namespace std;

int main (int argc, char *argv[])
{
	//Get test file
	ifstream file;
	string filename;
	if (argc < 2){ //No filename provided.
		cout << "Please enter filename:" << endl;
		cin >> filename;
	}
	else{
		filename = argv[1];
	}
	file.open(filename);
	if (!file){
		cout << "File open error!" << endl;
		return 1;
	}

	//TEST print file
	string first;
	string second;
	while(getline(file, first, ' ')){
		getline(file, second);
		cout << "First: " << first << " Second: " << second << " Int: " << stoul(second, nullptr, 16) << endl;
	}

	file.close();
	return 0;
}

// ---------------
// EOF
// ---------------
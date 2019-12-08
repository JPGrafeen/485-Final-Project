/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE 485 Final Project
*
*   Standard "Main"
*/

// Cache specs:
// 16MB, 64 byte lines, 8-way associative.
// Uses write allocate, MESI, and psuedo-LRU.

#include "main.h"
#include "cache.cpp"

using namespace std;

int main (int argc, char *argv[])
{
	// 32b addresses assumed from code stub in project guide.
	if (sizeof(unsigned int) != 4)
	{
		cout << "Incompatible compiler used" << endl;
		return 3;
	}

	Tag_Array TagArray[NofIndex][CacheAssc];

	Cache my_cache(*TagArray);

	if(!my_cache)
	{
		cout << "Error creating cache" << endl;
		return 2
	}

	// Parse Arguments
	string filename;
	if (argc < 2){ //No filename provided.
		bool mode;
		cout << "Please enter filename:" << endl;
		cin >> filename;
		cout << "Debug mode? (1/0)" << endl;
		cin >> mode;
		my_cache.Set_DebugMode(mode);
	}
	else if (argv[1] == "--help"){
		cout << "Cache Simulator." << endl << "Takes file name/path of a trace file as the first argument." << endl << "Enter '--debug' as the second argument to enter debug mode.";
	}
	else{
		filename = argv[1];
		my_cache.Set_DebugMode(argv[2] == "--debug");
	}

	// Open file
	ifstream file;
	file.open(filename);
	if (!file){
		cout << "File open error!" << endl;
		return 1;
	}


	// Read line by line until eof and perform the trace operation for the trace address
	string sOperation;
	string sAddress;
	while( getline(filename,sOperation,' '))
	{
		getline(filename,sAddress);

		// Convert strings to appropriate data types
		TraceOp eOperation = sOperation - '0';
		unsigned int Address = stoul(sAddress, nullptr, 16);

		switch(eOperation)
		{
		case TRC_L1_DATA_READ:
			my_cache.L1_Data_Read(Address);
			break;
		case TRC_L1_DATA_WRITE:
			my_cache.L1_Data_Write(Address);
			break;
		case TRC_L1_INST_READ:
			my_cache.L1_Inst_Read(Address);
			break;
		case TRC_SNP_INVALIDATE:
			my_cache.SNP_Invalidate(Address);
			break;
		case TRC_SNP_READ:
			my_cache.SNP_Read(Address);
			break;
		case TRC_SNP_WRITE:
			my_cache.SNP_Write(Address);
			break;
		case TRC_SNP_RWIM:
			my_cache.SNP_RWIM(Address);
			break;
		case TRC_CLR_CACHE:
			my_cache.Clear_Cache();
			break;
		case TRC_PRNT_CACHE:
			my_cache.Print_Cache();
			break;
		}
	}
	file.close();

	for (int i = 0; i < 32; ++i)
	{
		cout << endl;
	}
	cout << "----------------------------------------------------------------" << endl << endl;
	cout << "Cache Statistics" << endl << endl << endl;
	cout << "Reads:  " << my_cache.Get_CacheRead()  << endl;
	cout << "Writes: " << my_cache.Get_CacheWrite() << endl;
	cout << "Hits:   " << my_cache.Get_CacheHit()   << endl;
	cout << "Misses: " << my_cache.Get_CacheMiss()  << endl;
	cout << "Ratio:  " << my_cache.Get_CacheRatio() << endl;

	return 0;
}

// ---------------
// EOF
// ---------------
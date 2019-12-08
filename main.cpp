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

	Cache my_cache;

	Tag_Array_Entry Tag_Array[NofIndex][CacheAssc];

	if(my_cache)
	{
		my_cache.Tag_Array = *Tag_Array;
	}
	else
	{
		cout << "Error Creating Cache" << endl;
		return 2;
	}

	// Parse Arguments
	string filename;
	if (argc < 2){ //No filename provided.
		cout << "Please enter filename:" << endl;
		cin >> filename;
		cout << "Debug mode? (1/0)" << endl;
		cin >> m_DebugMode;
	}
	else if (argv[1] == "--help"){
		cout << "Cache Simulator." << endl << "Takes file name/path of a trace file as the first argument." << endl << "Enter '--debug' as the second argument to enter debug mode.";
	}
	else{
		filename = argv[1];
		if (argv[2] == "--debug"){
			m_DebugMode = true;
		}
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
			my_cache.L1_Data_Read(Adress);
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

	cout <<  "Reads: "  << my_cache.m_CacheRead  << endl;
	cout <<  "Writes: " << my_cache.m_CacheWrite << endl;
	cout <<  "Hits: "   << my_cache.m_CacheHit   << endl;
	cout <<  "Misses: " << my_cache.m_CacheMiss  << endl;
	cout <<  "Ratio: "  << Cache_Ratio()         << endl;

	file.close();
	return 0;
}

// ---------------
// EOF
// ---------------
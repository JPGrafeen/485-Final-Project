/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly Makinster
*
*   ECE 485 Final Project
*
*   Standard "Main"
*/

// Cache specs:
// 16MB, 64 byte lines, 8-way associative.
// Uses write allocate, MESI, and psuedo-LRU.

#include "main.h"
#include "cache.h"

using namespace std;

int main (int argc, char *argv[])
{
	// 32b addresses assumed from code stub in project guide.
	if (sizeof(unsigned int) != 4)
	{
		cout << "Incompatible compiler used" << endl;
		return 2;
	}

	// create cache object and initialize.
	Cache my_cache;


	// Parse Arguments
	string filename;
	string mode;
	if (argc < 2)
	{ // No filename provided.
		bool mode;
		cout << "Please enter filename:" << endl;
		cin >> filename;
		cout << "Silent mode? (1/0)" << endl;
		cin >> mode;
		my_cache.Set_SilentMode(mode);
	}
	else if (argv[1])
	{
		filename = argv[1];
		if (filename == "--help")
		{
			cout << "Cache Simulator." << endl;
			cout << "Takes file name/path of a trace file as the first argument." << endl;
			cout << "Enter '--silent' as the second argument to enter silent mode." << endl << endl;
			cout << "Silent Mode: Simulation will only print to screen responses to a" << endl;
			cout << "           '9'  from the trace file and the usage statistics at the" << endl;
			cout << "            completion of the run." << endl << endl;
			cout << "Default Mode:   Displays bus operations, reported snoop results," << endl;
			cout << "           and communication messages to higher level cache (L1)" << endl << endl;
			return 0;
		}
		else if (filename == "--silent")
		{
			cout << "No file name given" << endl;
		}
		else if (argv[2])
		{
			mode = argv[2];
			my_cache.Set_SilentMode((mode == "--silent" ? true : false));
		}
	}


	// Open file
	ifstream file (filename);
	if (file.is_open())
	{
		// Read line by line until eof and perform the trace operation for the trace address
		string sOperation;
		string sAddress;
		while( getline(file, sOperation, ' '))
		{
			getline(file, sAddress);

			// Convert strings to appropriate data types
			TraceOp eOperation   = static_cast<TraceOp>(stoi(sOperation));
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

		// Lazy programming to clear the screen
		printf( "----------------------------------------------------------------\n\n" );
		printf( "Cache Statistics\n\n\n");
		printf( "Reads:  %d \n", my_cache.Get_CacheRead()  );
		printf( "Writes: %d \n", my_cache.Get_CacheWrite() );
		printf( "Hits:   %d \n", my_cache.Get_CacheHit()   );
		printf( "Misses: %d \n", my_cache.Get_CacheMiss()  );
		printf( "Ratio:  %.10f \n\n\n", my_cache.Get_CacheRatio() );

		file.close();
		return 0;
	}


	cout << "File open error!" << endl;
	return 1;
}

// ---------------
// EOF
// --------------
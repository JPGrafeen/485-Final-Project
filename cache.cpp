
//============================================================
// Cache.cpp
// Cache object for holding cache line data, does not hold actual data.
//
//============================================================

//Constants:
const int ASSOCIATIVITY = 8;
const int NUMSETS = 2^15;

#define	R_L1DATA 0 //read request from L1 data cache
#define	W_L1DATA 1 //write request from L1 data cache
#define	R_L1INS 2 //read request from L1 instruction cache
#define	S_INVAL 3 //snooped invalidate command
#define	S_READ 4 //snooped read request
#define	S_WRITE 5 //snooped write request
#define	S_READ_MOD 6 //snooped read with intent to modify request
#define	CLEAR_CACHE 8 //clear the cache and reset all state
#define	PRINT_CACHE 9 //print contents and state of each valid cache line (allow subsequent trace activity)

using namespace std;

class Cache{
public:
	Cache(){ //Constructor
		clearCache();
	}

	void takeRequest(int command, int address){
		switch (command)
		{
		case R_L1DATA:
			/* code */
			break;

		case W_L1DATA:
			/* code */
			break;

		case R_L1INS:
			/* code */
			break;
		
		case S_INVAL:
			/* code */
			break;

		case S_READ:
			/* code */
			break;

		case S_WRITE:
			/* code */
			break;
		
		case S_READ_MOD:
			/* code */
			break;

		case CLEAR_CACHE:
			clearCache();
			break;

		case PRINT_CACHE:
			printCache();
			break;
		
		default:
			cout << "Something broke! (takeRequest). Command: " << command << " Address: " << address << endl;
			break;
		}
	}

	void clearCache(){
		for (int set = 0; set < NUMSETS; set++){
			pLRU[set][0] = 0;
			pLRU[set][1] = 0;
			pLRU[set][2] = 0;
			for (int line = 0; line < ASSOCIATIVITY; line++){
				dirty[set][line] = 0;
				valid[set][line] = 0;
				// Don't need to clear tag.
			}
		}
		reads = 0;
		writes = 0;
		hits = 0;
		misses = 0;
	}

	void read(){
		//Check cache
		//Report hit/miss
		//If miss fill cache
	}

	void write(){
		//Check Cache
		//Report hit/miss
		//If miss, fill cache (write allocate)
	}

	void printCache(){
		for (int set = 0; set < NUMSETS; set++){
			for (int line = 0; line < ASSOCIATIVITY; line++){
				if (valid[set][line] == 1){
					cout << "Set: " << set << "Line: " << line << "Tag: " << tag[set][line] << "Dirty bit: " << dirty[set][line] << endl;
				}
			}
		}
	}

	void printStats(){
		cout <<  "Reads: " << reads << endl;
		cout <<  "Writes: " << writes << endl;
		cout <<  "Hits: " << hits << endl;
		cout <<  "Misses: " << misses << endl;
	}

	void updatePLRU(int set, int way){
		if (way <= 1)
		{
			pLRU[set][0] = false;
			if (way == 0)
			{
				pLRU[set][1] = false;
			}
			else
			{
				pLRU[set][1] = true;
			}
		}
		else{
			pLRU[set][0] = true;
			if (way == 2){
				pLRU[set][2] = false;
			}
			else{
				pLRU[set][2] = true;
			}
		}
	}

private:
	//Variables
	bool dirty[NUMSETS][ASSOCIATIVITY];
	bool valid[NUMSETS][ASSOCIATIVITY];
	int tag[NUMSETS][ASSOCIATIVITY];

	//  0
	// 1 2
	bool pLRU[NUMSETS][4];

	//Stats
	int reads = 0;
	int writes = 0;
	int hits = 0;
	int misses = 0;
};

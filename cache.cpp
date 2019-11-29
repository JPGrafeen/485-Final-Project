
//============================================================
// Cache.cpp
// Cache object for holding cache line data, does not hold actual data.
//
//============================================================

//Constants:
const int ASSOCIATIVITY = 8;
const int NUMSETS = 2^15;

class Cache{
public:
	Cache(){ //Constructor
		for (int x = 0; x < NUMSETS; x++){
			pLRU[x][0] = 0;
			pLRU[x][1] = 0;
			pLRU[x][2] = 0;
			for (int y = 0; x < ASSOCIATIVITY; y++){
				dirty[x][y] = 0;
				valid[x][y] = 0;
				tag[x][y] = 0;
			}
		}
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

	}

	void printStats(){

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
	int hits = 0;
	int misses = 0;
};

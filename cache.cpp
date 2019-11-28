
//============================================================
// Cache.cpp
// Cache object for holding cache line data, does not hold actual data.
//
//============================================================

class Cache {
	public:
	void Cache(){
		for(int x = 0; x<NUMSETS; x++){
			pLRU[x][0][0] = 0;
			pLRU[x][0][1] = 0;
			pLRU[x][1][0] = 0;
			pLRU[x][1][1] = 0;
			for(int y = 0; x<ASSOCIATIVITY; y++){
				dirty[x][y] = 0;
				valid[x][y] = 0;
				tag[x][y] = 0;
			}
		}
	}

	bool read(){
	}

	bool write(){
	}

	bool print(){
	}


	private:

	//Constants:
	const int ASSOCIATIVITY = 8;
	const int NUMSETS = 2^15;

	//Variables
	bool dirty [NUMSETS][ASSOCIATIVITY];
	bool valid [NUMSETS][ASSOCIATIVITY];
	int tag [NUMSETS][ASSOCIATIVITY];

	bool pLRU [NUMSETS][2][2];

}


//============================================================
// Cache.cpp
// Cache object for holding cache line data, does not hold actual data.
//
//============================================================

//Constants:
const int ASSOCIATIVITY = 8;
const int NUMSETS = 2^15;

/*
I was thinking this first part could look a bit more like this

#define	ADDRESS_BUS_WIDTH	32					
#define TOTAL_CACHE_SIZE	16,777,216‬ 
#define CACHE_LINE_SIZE		64					
#define WAYS_PER_SET		8					
#define NO_OF_SETS			(TOTAL_CACHE_SIZE/CACHE_LINE_SIZE/WAYS_PER_SET)

*/

#define TAG 12	
#define SET 14					
#define BYTE 6	
#define SETMASK 0x000FFFFF
#define BYTEMASK 0x0000003F

#define	R_L1DATA 0 //read request from L1 data cache
#define	W_L1DATA 1 //write request from L1 data cache
#define	R_L1INS 2 //read request from L1 instruction cache
#define	S_INVAL 3 //snooped invalidate command
#define	S_READ 4 //snooped read request
#define	S_WRITE 5 //snooped write request
#define	S_READ_MOD 6 //snooped read with intent to modify request
#define	CLEAR_CACHE 8 //clear the cache and reset all state
#define	PRINT_CACHE 9 //print contents and state of each valid cache line (allow subsequent trace activity)

#define NOHIT				0 /* No hit */
#define HIT					1 /* Hit */
#define HITM				2 /* Hit to modified line */

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

	/
	void clearCache()
	{
		for (int set = 0; set < NUMSETS; ++set)
		{
			for (int way = 0; way < ASSOCIATIVITY; ++way)
			{
				pLRU[set][way] = 0;	
			}

			for (int line = 0; line < ASSOCIATIVITY; line++)
			{
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


	//Check cache
	//Report hit/miss
	//If miss fill cache
	int read(unsigned int addr)
	{
		
	}

	void write()
	{
		//Check Cache
		//Report hit/miss
		//If miss, fill cache (write allocate)
	}

	void printCache()
	{
		for (int set = 0; set < NUMSETS; set++)
		{
			for (int line = 0; line < ASSOCIATIVITY; line++)
			{
				if (valid[set][line] == 1)
				{
					cout << "Set: " << set << "Line: " << line << "Tag: " << tag_array[set][line] << "Dirty bit: " << dirty[set][line] << endl;
				}
			}
		}
	}

	void printStats()
	{
		cout <<  "Reads: " << reads << endl;
		cout <<  "Writes: " << writes << endl;
		cout <<  "Hits: " << hits << endl;
		cout <<  "Misses: " << misses << endl;
	}

	//*******************************************
	//needs to be changed for 8 way cache
	//*******************************************
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
	int tag_array[NUMSETS][ASSOCIATIVITY];


	//  0
	// 1 2
	bool pLRU[NUMSETS][ASSOCIATIVITY];	//8 way for this project

	//Stats
	int reads = 0;
	int writes = 0;
	int hits = 0;
	int misses = 0;

	int writebacks = 0;
	int evictions = 0;


};


//=======================General Purpose Cache Functions======================

//name: CheckForMatch
//inputs:	index (cache index number)
//			tag (tag to compare to)
//			way (pointer so that it can potentialy be modified)
//desc: check for a tag match at a given index
//retn: bool
bool CheckForMatch(unsigned int index, unsigned int tag, unsigned int *way)
{
	bool match = FALSE;

	for (int i = 0; i < ASSOCIATIVITY; ++i)
	{
		if (valid[index][i] != INVALID)	//invalid needs to be changed to whatever we populate the empty cache with
										//also wasn't sure which array to search through
		{
			if (tag == tag_array[index][i])
			{
				//If there is a match, return true
				*way = i;
				match = TRUE;
				return match;
			}
		}
	}

	return match_stat;
}


//=======================BUS Operations==============================


//name: 	GetSnoopResult
//input: 	Address (from file)
//desc: 	Used to simulate the reported snoop results by other caches
//retn: 	int (the type of hit that goes with the snooped result)
int GetSnoopResult(unsigned int Address)
{
	int snoop_res = NOHIT;
	unsigned int snoop_param;

	snoop_param = (Address >> (INDEX_BITS + BYTE_BITS) & 0x03); //offset + mask to get snoop parameter
	if (snoop_param == 0)
		snoop_res = HIT;
	else if (snoop_param == 1)
		snoop_res = HITM;
	else	//snoop_param = 2 or 3					
		snoop_res = NOHIT;	

	return snoop_res;
}

//name: 	BusOperation:
//inputs: 	Bus operators (READ, WRITE, INVALIDATE, RWIM)
//			Address (from file)
//			SnoopResult (HIT, HITM, NOHIT)
//desc: 	Used to simulate a bus operation and to capture the snoop results of last
//			level caches of other processors.
//retn: 	void
void BusOperation(char BusOp, unsigned int Address, char *SnoopResult)
{
	*SnoopResult = GetSnoopResult(Address);
	
	//printf("BusOp: %u, Address : %x, Snoop Result : %u\n", BusOp, Address, *SnoopResult);
	
}


//Name: 	PutSnoopResult
//inputs: 	Address (from file)
//			SnoopResult (HIT, HITM, NOHIT)
//desc: 	Used to report the result of our snooping bus operations by other caches.
//			Used in debug mode
//retn: 	void
void PutSnoopResult(unsigned int Address, char SnoopResult)
{
	printf("SnoopResult: Address %x, SnoopResult : %u\n", Address, SnoopResult);
}

//name: 	MessageToL2Cache
//inputs: 	Bus operators (READ, WRITE, INVALIDATE, RWIM)
//			Address (from file)
//desc:		Used to simulate communication to our upper level cache.
//			Used in Debug mode
//retn: 	void
void MessageToL2Cache(char BusOp, unsigned int Address)
{
	printf("L2: %u %x\n", BusOp, Address);
}

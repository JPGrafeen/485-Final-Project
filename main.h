/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE 485 Final Project
*
*   Necessary data structures and definitions for standard program
*/


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#define CacheSize = 2^24   //16MiB
#define CacheLine = 2^6    //64Bytes
#define CacheAssc = 2^3    //8-way set associativity
#define NofIndex = CacheSize / (CacheLine * CacheAssc)

enum TraceOp{
    TRC_L1_DATA_READ     = 0,  // Read request from L1 Data Cache
    TRC_L1_DATA_WRITE    = 1,  // Write request from L1 Data Cache
    TRC_L1_INST_READ     = 2,  // Read request from L1 Instruction Cache
    TRC_SNP_INVALIDATE   = 3,  // Snooped Invalidate command
    TRC_SNP_READ         = 4,  // Snooped Read request
    TRC_SNP_WRITE        = 5,  // Snooped Write request
    TRC_SNP_RWIM         = 6,  // Snooped Read with intent to modify request
    TRC_CLR_CACHE        = 8,  // Clear the cache and reset all state
    TRC_PRNT_CACHE       = 9   // Print cache contents and state of each valid cache line
}

enum BusOp{
    BUS_READ   = 1,  // Read request placed on Bus
    BUS_WRITE  = 2,  // Write request placed on Bus
    BUS_INV    = 3,  // Invalidate command placed on Bus
    BUS_RWIM   = 4   // Read with intent to modify placed on Bus
}

enum SnoopOp{
    SNP_NOHIT  = 0,  // Miss reply to snooped request
    SNP_HIT    = 1,  // Hit reply to snooped request
    SNP_HITM   = 2   // Hit Modified reply to snooped request
}

enum L1L2MSG{
    MSG_GETLINE    = 1,  //L1 should send the latest version of a line to L2
    MSG_SENDLINE   = 2,  //L2 is sending a modified line to L1
    MSG_INV_LINE   = 3,  //L1 should invalidate the line
    MSG_EVICTLINE  = 4   //L1 should evict the line
}





//void printCache(){
//	for (int set = 0; set < NUMSETS; set++){
//		for (int line = 0; line < ASSOCIATIVITY; line++){
//			if (valid[set][line] == 1){
//				cout << "Set: " << set << "Line: " << line << "Tag: " << tag[set][line] << "Dirty bit: " << dirty[set][line] << endl;
//			}
//		}
//	}
//}

//void printStats(){
//	cout <<  "Reads: " << reads << endl;
//	cout <<  "Writes: " << writes << endl;
//	cout <<  "Hits: " << hits << endl;
//	cout <<  "Misses: " << misses << endl;
//}

//void updatePLRU(int set, int way){
//	if (way <= 1)
//	{
//		pLRU[set][0] = false;
//		if (way == 0)
//		{
//			pLRU[set][1] = false;
//		}
//		else
//		{
//			pLRU[set][1] = true;
//		}
//	}
//	else{
//		pLRU[set][0] = true;
//		if (way == 2){
//			pLRU[set][2] = false;
//		}
//		else{
//			pLRU[set][2] = true;
//		}
//	}
//}
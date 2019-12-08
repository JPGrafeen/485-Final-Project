/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE 485 Final Project
*
*   Necessary data structures and definitions for standard program
*/


#include <cstdio>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>

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
};


// ---------------
// EOF
// ---------------



//void printCache(){
//	for (int set = 0; set < NUMSETS; set++){
//		for (int line = 0; line < ASSOCIATIVITY; line++){
//			if (valid[set][line] == 1){
//				cout << "Set: " << set << "Line: " << line << "Tag: " << tag[set][line] << "Dirty bit: " << dirty[set][line] << endl;
//			}
//		}
//	}
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
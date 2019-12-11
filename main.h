/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly Makinster
*
*   ECE 485 Final Project
*
*   Necessary data structures and definitions for standard program
*/

#pragma once
#include <cstdio>
#include <string>
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

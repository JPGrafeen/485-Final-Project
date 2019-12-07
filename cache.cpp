/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE 485 Final Project
*
*   Cache class functions
*/

#include "cache.h"


//--------------------------------------------------------------------------------
// Description:  Read request from L1 Data Cache.
//
//--------------------------------------------------------------------------------
void L1_Data_Read(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Write request from L1 Data Cache.
//
//--------------------------------------------------------------------------------
void L1_Data_Write(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Read request from L1 Instruction Cache.
//
//--------------------------------------------------------------------------------
void L1_Inst_Read(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Snooped Invalidate command
//
//--------------------------------------------------------------------------------
void SNP_Invalidate(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Snooped Read request
//
//--------------------------------------------------------------------------------
void SNP_Read(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Snooped Write request
//
//--------------------------------------------------------------------------------
void SNP_Write(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Snooped Read with intent to modify request
//
//--------------------------------------------------------------------------------
void SNP_RWIM(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Clear the cache and reset all state
//
//--------------------------------------------------------------------------------
void Clear_Cache()
{

}


//--------------------------------------------------------------------------------
// Description: Print contents and state of each valid cache line 
//              (allow subsequent trace activity)
//
//--------------------------------------------------------------------------------
void Print_Cache()
{

}


//--------------------------------------------------------------------------------
// Description: Returns the Tag portion of the given address
//
//--------------------------------------------------------------------------------
unsigned int Get_Tag(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Returns the Index portion of the given address
//
//--------------------------------------------------------------------------------
unsigned int Get_Index(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Returns the Byte Offset portion of the given address
//
//--------------------------------------------------------------------------------
unsigned int Get_Offset(unsigned int Address)
{

}


//--------------------------------------------------------------------------------
// Description: Used to simulate a bus operation and to capture the snoop results
//              of last level caches of other processors
//
//--------------------------------------------------------------------------------
void BusOperation(char BusOp, unsigned int Address, char* SnoopResult)
{
    SnoopResult = GetSnoopResult(Address);

    if (!m_DebugMode)
    {
        printf("BusOp: %d, Address: %h, Snoop Result: %d\n",*SnoopResult);
    }
}


//--------------------------------------------------------------------------------
// Description: Used to simulate the reporting of snoop results by other caches
//
//--------------------------------------------------------------------------------
char GetSnoopResult(unsigned int Address)
{
    // Mask the Address to remove all but the two Least Significant Bits
    int8_t twoLSB = Address && 0x03;
    switch(twoLSB)
    {
        case 0:
            return 0x01;
        case 1:
            return 0x02;
        default:
            return 0x00;
    }
}


//--------------------------------------------------------------------------------
// Description: Used to report the result of our snooping bus operations performed
//              by other caches
//
//--------------------------------------------------------------------------------
void PutSnoopResult(unsigned int Address, char SnoopResult)
{
    if (!m_DebugMode)
    {
        printf("SnoopResult: Address %h, SnoopResult: %d\n", Address,SnoopResult);
    }
}


//--------------------------------------------------------------------------------
// Description: Used to simulate communication to our upper level cache
//
//--------------------------------------------------------------------------------
void MessageToCache(char Message, unsigned int Address)
{
    if (!m_DebugMode)
    {
        printf("L2: %d %h\n", Message, Address);
    }
}


//--------------------------------------------------------------------------------
// Description: Updates the PLRU array after a data usaage has occured. 
//
//--------------------------------------------------------------------------------
void update_PLRU(unsigned int Index, unsigned int Way)
{

}
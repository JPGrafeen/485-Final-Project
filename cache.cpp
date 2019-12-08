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
// Description:  Returns true if tag exists and is valid.
//
//--------------------------------------------------------------------------------
bool Cache::Cache_Hit(unsigned int Address)
{
    unsigned int aIndex = Get_Index(Address);
    unsigned int aTag   = Get_Tag(Address);

    Tag_Array* ptrIndex = &m_TagArray[aIndex];

    bool hit = false;
    for (int i = 0; i < CacheAssc; ++i)
    {
        // If Tag is present and Valid
        if (ptrIndex[i].Tag == aTag && ptrIndex[i].Valid)
        {
            hit = true;
        }
    }

    return hit;
}


//--------------------------------------------------------------------------------
// Description:  Returns true if tag exists and is modified.
//
//--------------------------------------------------------------------------------
bool Cache::Cache_Mod(unsigned int Address)
{
    unsigned int aIndex = Get_Index(Address);
    unsigned int aTag   = Get_Tag(Address);

    Tag_Array* ptrIndex = &m_TagArray[aIndex];

    bool Mod = false;
    for (int i = 0; i < CacheAssc; ++i)
    {
        // If Tag is present and Valid
        if (ptrIndex[i].Tag == aTag && ptrIndex[i].Dirty)
        {
            Mod = true;
        }
    }

    return Mod;
}

//--------------------------------------------------------------------------------
// Description:  Read request from L1 Data Cache.
//
// Operation == 0
//
//--------------------------------------------------------------------------------
void Cache::L1_Data_Read(unsigned int Address)
{
//    if (Cache_Hit(Address))
//    {
//        ++m_CacheHit;
//        ++m_CacheRead;
//        //Stay in MESI state
//    }
//    else
//    {
//        ++m_CacheMiss;
//        ++m_CacheRead;
//
//        char SnoopResult = 0x00;
//        BusOperation(static_cast<char>(BUS_READ), Address, &SnoopResult)
//
//        if ( SnoopResult == SNP_HIT
//          || SnoopResult == SNP_HITM)
//        {
//            unsigned int Tag   = Get_Tag(Address);
//            unsigned int Index = Get_Index(Address);
//        }
//    }
}


//--------------------------------------------------------------------------------
// Description: Write request from L1 Data Cache.
//
// Operation == 1
//
//--------------------------------------------------------------------------------
void Cache::L1_Data_Write(unsigned int Address)
{
    if (Cache_Hit(Address))
    {
        /* code */
    }
    else
    {
        
    }
}


//--------------------------------------------------------------------------------
// Description: Read request from L1 Instruction Cache.
//
// Operation == 2
//
//--------------------------------------------------------------------------------
void Cache::L1_Inst_Read(unsigned int Address)
{
    if (Cache_Hit(Address))
    {
        /* code */
    }
    else
    {
        
    }
}


//--------------------------------------------------------------------------------
// Description: Snooped Invalidate command
//
// Operation == 3
//
//--------------------------------------------------------------------------------
void Cache::SNP_Invalidate(unsigned int Address)
{
    if (Cache_Hit(Address))
    {
        /* code */
    }
    else
    {
        
    }
}


//--------------------------------------------------------------------------------
// Description: Snooped Read request
//
// Operation == 4
//
//--------------------------------------------------------------------------------
void Cache::SNP_Read(unsigned int Address)
{
    if (Cache_Hit(Address))
    {
        /* code */
    }
    else
    {
        
    }
}


//--------------------------------------------------------------------------------
// Description: Snooped Write request
//
// Operation == 5
//
//--------------------------------------------------------------------------------
void Cache::SNP_Write(unsigned int Address)
{
    if (Cache_Hit(Address))
    {
        /* code */
    }
    else
    {
        
    }
}


//--------------------------------------------------------------------------------
// Description: Snooped Read with intent to modify request
//
// Operation == 6
//
//--------------------------------------------------------------------------------
void Cache::SNP_RWIM(unsigned int Address)
{
    if (Cache_Hit(Address))
    {
        /* code */
    }
    else
    {
        
    }
}


//--------------------------------------------------------------------------------
// Description: Clear the cache and reset all state
//
// Operation == 8
//
//--------------------------------------------------------------------------------
void Cache::Clear_Cache()
{

}


//--------------------------------------------------------------------------------
// Description: Print contents and state of each valid cache line 
//              (allow subsequent trace activity)
//
// Operation == 9
//
//--------------------------------------------------------------------------------
void Cache::Print_Cache()
{

}

//--------------------------------------------------------------------------------
// Description: Updates the PLRU array after a data usaage has occured. 
//
//--------------------------------------------------------------------------------
void Cache::update_PLRU(unsigned int Index, unsigned int Way)
{
    // set which element for the given index is now the LRU
}

//--------------------------------------------------------------------------------
// Description: Updates the PLRU array after a data usaage has occured. 
//
//--------------------------------------------------------------------------------
unsigned int Cache::find_PLRU(unsigned int Index)
{
    return 0;
}


//--------------------------------------------------------------------------------
// Description: Used to simulate a bus operation and to capture the snoop results
//              of last level caches of other processors
//
//--------------------------------------------------------------------------------
void Cache::BusOperation(char BusOp, unsigned int Address, char* SnoopResult)
{
    *SnoopResult = GetSnoopResult(Address);

    if (m_DebugMode)
    {
        printf("BusOp: %d, Address: %x, Snoop Result: %d\n", BusOp, Address, *SnoopResult);
    }
}


//--------------------------------------------------------------------------------
// Description: Used to simulate the reporting of snoop results by other caches
//
//--------------------------------------------------------------------------------
char Cache::GetSnoopResult(unsigned int Address)
{
    // Mask the Address to remove all but the two Least Significant Bits
    Address &= 0x03;
    int8_t twoLSB = static_cast<int8_t>(Address);
    char SnoopResult;
    switch(twoLSB)
    {
        case 0:
            SnoopResult = 0x01;
        case 1:
            SnoopResult = 0x02;
        default:
            SnoopResult = 0x00;
    }

    /*
    if the least two significant bits of the address are 00 then the other processors are reporting a HIT
    if the least two significant bits of the address are 01 then the other processors are reporting a HITM
    if the least two significant bits of the address are 1x then the other processors are not reporting a HIT or HITM
    */

    return SnoopResult;
}


//--------------------------------------------------------------------------------
// Description: Used to report the result of our snooping bus operations performed
//              by other caches
//
//--------------------------------------------------------------------------------
void Cache::PutSnoopResult(unsigned int Address, char SnoopResult)
{
    if (m_DebugMode)
    {
        printf("SnoopResult: Address %x, SnoopResult: %d \n", Address, SnoopResult);
    }
}


//--------------------------------------------------------------------------------
// Description: Used to simulate communication to our upper level cache
//
//--------------------------------------------------------------------------------
void Cache::MessageToCache(char Message, unsigned int Address)
{
    if (m_DebugMode)
    {
        printf("L2: %d %x \n", Message, Address);
    }
}




// ---------------
// EOF
// ---------------

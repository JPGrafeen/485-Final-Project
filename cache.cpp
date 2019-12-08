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
    unsigned int Index = Get_Index(Address);
    unsigned int Tag   = Get_Tag(Address);

    Tag_Array* ptrIndex = &m_TagArray[Index];

    bool Hit = false;
    for (int i = 0; i < CacheAssc; ++i)
    {
        // If Tag is present and Valid
        if (ptrIndex[i].Tag == Tag && ptrIndex[i].Valid)
        {
            Hit = true;
        }
    }

    return Hit;
}


//--------------------------------------------------------------------------------
// Description:  Returns true if tag exists and is modified.
//
//--------------------------------------------------------------------------------
bool Cache::Cache_Mod(unsigned int Address)
{
    unsigned int Index = Get_Index(Address);
    unsigned int Tag   = Get_Tag(Address);

    Tag_Array* ptrIndex = &m_TagArray[Index];

    bool Mod = false;
    for (int i = 0; i < CacheAssc; ++i)
    {
        // If Tag is present and Valid
        if (ptrIndex[i].Tag == Tag && ptrIndex[i].Dirty)
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
    if (Cache_Hit(Address))
    {
        ++m_CacheHit;
        //Stay in MESI state
    }
    else
    {
        ++m_CacheMiss;

        char    SnoopResult = 0x00;
        unsigned int  Index = Get_Index(Address);
        unsigned int    Tag = Get_Tag(Address);
        Tag_Array* ptrIndex = &m_TagArray[Index];
        int VictimWay;

        for (VictimWay = 0; VictimWay < CacheAssc && ptrIndex[VictimWay].Valid; )
        {
            // Do nothing, just seeing if every cache line is valid.
            // Consequence, if we find an invalid cache line it will 
            // meet the design assumption by being the lowest numbered
            // cache line to fill.
            ++VictimWay;
        }

        if (VictimWay >= CacheAssc)
        {
            VictimWay = find_PLRU(Index);
            if (ptrIndex[VictimWay].Dirty)
            {
                MessageToCache(static_cast<char>(MSG_GETLINE), Address);
                BusOperation(static_cast<char>(BUS_WRITE), Address, &SnoopResult); //SnoopResult discarded.
            }
        }

        BusOperation(static_cast<char>(BUS_READ), Address, &SnoopResult);

        ptrIndex[VictimWay].Valid = true;
        ptrIndex[VictimWay].Dirty = false;
        ptrIndex[VictimWay].Tag   = Tag;

        update_PLRU(Index, VictimWay);

        if ( SnoopResult == SNP_HIT
          || SnoopResult == SNP_HITM)
        {
            ptrIndex[VictimWay].MESI = 'S';
        }
        else
        {
            ptrIndex[VictimWay].MESI = 'E';
        }
    }

    ++m_CacheRead;
    MessageToCache(static_cast<char>(MSG_SENDLINE), Address);
}


//--------------------------------------------------------------------------------
// Description: Write request from L1 Data Cache.
//
// Operation == 1
//
//--------------------------------------------------------------------------------
void Cache::L1_Data_Write(unsigned int Address)
{
//
//    char    SnoopResult = 0x00;
//    unsigned int  Index = Get_Index(Address);
//    unsigned int    Tag = Get_Tag(Address);
//    Tag_Array* ptrIndex = &m_TagArray[Index];
//    int VictimWay;
//
//
//    if (Cache_Hit(Address))
//    {
//        ++m_CacheHit;
//        for (int i = 0; i < CacheAssc || ptrIndex[i].Tag == Tag; ++i)
//        {
//
//        }
//    }
//    else
//    {
//        ++m_CacheMiss;
//    }


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
// Description: Returns the Way in the Index which was least recently used (pseudo)
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


//--------------------------------------------------------------------------------
// Description: Returns the Cache hit ratio
//
//--------------------------------------------------------------------------------
float Cache::Get_CacheRatio()
{
	// Dividing by zero releases Chuthulu
	if ( m_CacheHit  == 0
	  && m_CacheMiss == 0)
	{
		return 0.0;
	}

	return ( m_CacheHit / (m_CacheHit + m_CacheMiss));
}

// ---------------
// EOF
// ---------------

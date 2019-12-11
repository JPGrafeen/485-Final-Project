/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly Makinster
*
*   ECE 485 Final Project
*
*   Cache class functions
*/

#include "cache.h"


//--------------------------------------------------------------------------------
// Description:  Returns Way if tag exists and is valid. Returns -1 if not found.
//
//--------------------------------------------------------------------------------
int Cache::Find_Way(unsigned int Address)
{
    unsigned int Index = Get_Index(Address);
    unsigned int Tag   = Get_Tag(Address);

    unsigned int ValidWay = 0;
    for (; ValidWay < CacheAssc; ++ValidWay)
    {
        // If Tag is present and Valid
        if ( m_TagArray[Index][ValidWay].Tag  == Tag 
          && m_TagArray[Index][ValidWay].MESI != 'I' )
        {
            return ValidWay; 
        }
    }

    return CACHE_MISS;
}


//--------------------------------------------------------------------------------
// Description:  Common Algorithm for bus interaction with Main Memory. Writes 
//               back if modified and updates the MRU way.
//
//               Returns the first invalid Way or the LRU Way.
//
//--------------------------------------------------------------------------------
int Cache::Process_Line(char BusOp, unsigned int Address, char* SnoopResult)
{
    unsigned int Index       = Get_Index(Address);
    int          VictimWay   = 0;

    // Find first invalid cache line
    for ( ; (VictimWay < CacheAssc) && (m_TagArray[Index][VictimWay].MESI != 'I') ; ++VictimWay)
    {
        // Do nothing
    }

    // If no invalids, find Least Recently Used, sends Evict Line to L1.
    if (VictimWay >= CacheAssc)
    {
        VictimWay = find_LRU(Index);

        // If Modified, write back
        if (m_TagArray[Index][VictimWay].MESI == 'M')
        {
            MessageToCache(MSG_GETLINE, Address);
            BusOperation(BUS_WRITE, Address, SnoopResult); //SnoopResult discarded
        }
        MessageToCache(MSG_EVICTLINE, Address);
    }

    update_MRU(Index, VictimWay);

    // Read from Main Memory
    BusOperation(BusOp, Address, SnoopResult);

    // Put data in selected way
    unsigned int Tag = Get_Tag(Address);
    m_TagArray[Index][VictimWay].Tag = Tag;

    return VictimWay;
}

//--------------------------------------------------------------------------------
// Description:  Read request from L1 Data Cache.
//
// Operation == 0
//
//--------------------------------------------------------------------------------
void Cache::L1_Data_Read(unsigned int Address)
{
    int VictimWay = Find_Way(Address);
    if (CACHE_MISS == VictimWay)
    {
        ++m_CacheMiss;

        char SnoopResult = 0x00;
        unsigned int Index = Get_Index(Address);
        VictimWay = Process_Line(BUS_READ, Address, &SnoopResult);

        if ( SnoopResult == SNP_HIT
          || SnoopResult == SNP_HITM)
        {
            // Another LLC reported having a copy, go to Shared
            m_TagArray[Index][VictimWay].MESI = 'S';
        }
        else
        {
            // No other LLC reported having a copy, go to Exclusive
            m_TagArray[Index][VictimWay].MESI = 'E';
        }
    }
    else
    {
        ++m_CacheHit;
        //Stay in MESI state
    }

    ++m_CacheRead;
    MessageToCache(MSG_SENDLINE, Address);
}


//--------------------------------------------------------------------------------
// Description: Write request from L1 Data Cache.
//
// Operation == 1
//
//--------------------------------------------------------------------------------
void Cache::L1_Data_Write(unsigned int Address)
{
    int VictimWay = Find_Way(Address);

    if (CACHE_MISS == VictimWay)
    {
        ++m_CacheMiss;

        char SnoopResult = 0x00;
        VictimWay = Process_Line(BUS_RWIM, Address, &SnoopResult); //SnoopResult discarded
    }
    else
    {
        ++m_CacheHit;

        unsigned int Index = Get_Index(Address);
        if (m_TagArray[Index][VictimWay].MESI == 'S')
        {
            char SnoopResult = 0x00;
            BusOperation(BUS_INV, Address, &SnoopResult); // SnoopResult discarded
        }
    }

    ++m_CacheWrite;
    unsigned int Index = Get_Index(Address);
    m_TagArray[Index][VictimWay].MESI  = 'M';
    MessageToCache(MSG_SENDLINE, Address);
}


//--------------------------------------------------------------------------------
// Description: Read request from L1 Instruction Cache.
//
// Operation == 2
//
//--------------------------------------------------------------------------------
void Cache::L1_Inst_Read(unsigned int Address)
{
    int VictimWay = Find_Way(Address);

    if (CACHE_MISS == VictimWay)
    {
        ++m_CacheMiss;

        char SnoopResult = 0x00;
        unsigned int Index = Get_Index(Address);
        VictimWay = Process_Line(BUS_READ, Address, &SnoopResult);

        if ( SnoopResult == SNP_HIT
          || SnoopResult == SNP_HITM)
        {
            // Another LLC reported having a copy, go to Shared
            m_TagArray[Index][VictimWay].MESI = 'S';
        }
        else
        {
            // No other LLC reported having a copy, go to Exclusive
            m_TagArray[Index][VictimWay].MESI = 'E';
        }
    }
    else
    {
        ++m_CacheHit;
        // Stay in MESI State
    }

    ++m_CacheRead;
    MessageToCache(MSG_SENDLINE, Address);
}


//--------------------------------------------------------------------------------
// Description: Snooped Invalidate command
//
// Operation == 3
//
//--------------------------------------------------------------------------------
void Cache::SNP_Invalidate(unsigned int Address)
{
    int VictimWay = Find_Way(Address);
    if (CACHE_MISS == VictimWay)
    {
        PutSnoopResult(Address, SNP_NOHIT);
    }
    else
    {
        PutSnoopResult(Address, SNP_HIT);

        // Only the MESI state S is possible. Getting an Invalidate while in Exclusive or Modifed violates MESI coherence.
        unsigned int Index = Get_Index(Address);
        if(m_TagArray[Index][VictimWay].MESI  != 'S' && DEBUG)
        {
            std::cout << "---------- TEST INVALID: Another LLC is sending an Invalid Bus Operation ----------" << std::endl;
            std::cout << "---------- BusOperation: Snooped Invalidate while in Modified or Exclusive --------" << std::endl;
        }

        m_TagArray[Index][VictimWay].MESI  = 'I';
        MessageToCache(MSG_INV_LINE, Address); 
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
    int VictimWay = Find_Way(Address);
    if (CACHE_MISS == VictimWay)
    {
        PutSnoopResult(Address, SNP_NOHIT); 
    }
    else
    {
        unsigned int Index = Get_Index(Address);
        if ( m_TagArray[Index][VictimWay].MESI == 'M' )
        {
            PutSnoopResult(Address,SNP_HITM);

            MessageToCache(MSG_GETLINE, Address);

            char SnoopResult = 0x00;
            BusOperation(BUS_WRITE, Address, &SnoopResult); //SnoopResult Discarded.
        }
        else
        {
            PutSnoopResult(Address, SNP_HIT);
        }

        m_TagArray[Index][VictimWay].MESI = 'S';
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
    int VictimWay = Find_Way(Address);
    if (CACHE_MISS == VictimWay)
    {
        PutSnoopResult(Address, SNP_NOHIT); 
    }
    else
    {
        PutSnoopResult(Address, SNP_HIT);

        // Only the MESI state S is possible. Getting an Write while in Exclusive or Modifed violates MESI coherence.
        unsigned int Index = Get_Index(Address);
        if(m_TagArray[Index][VictimWay].MESI  != 'S' && DEBUG)
        {
            std::cout << "---------- TEST INVALID: Another LLC is sending an Invalid Bus Operation ----------" << std::endl;
            std::cout << "---------- BusOperation: Snooped Invalidate while in Modified or Exclusive --------" << std::endl;
        }

        m_TagArray[Index][VictimWay].MESI  = 'I';
        MessageToCache(MSG_INV_LINE, Address);
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
    int VictimWay = Find_Way(Address);
    if (CACHE_MISS == VictimWay)
    {
        PutSnoopResult(Address, SNP_NOHIT); 
    }
    else
    {
        unsigned int Index = Get_Index(Address);

        if ( m_TagArray[Index][VictimWay].MESI == 'M' )
        {
            PutSnoopResult(Address, SNP_HITM);

            MessageToCache(MSG_GETLINE, Address);

            char SnoopResult = 0x00;
            BusOperation(BUS_WRITE, Address, &SnoopResult); //SnoopResult Discarded.
        }
        else
        {
            PutSnoopResult(Address, SNP_HIT);
        }

        m_TagArray[Index][VictimWay].MESI = 'I';
        MessageToCache(MSG_INV_LINE, Address);
    }
}


//--------------------------------------------------------------------------------
// Description: Clears/Initializes the cache and resets all state
//
//--------------------------------------------------------------------------------
void Cache::Clear_Cache()
{
    for (int Index = 0; Index < NofIndex; ++Index)
    {
        for (int Way = 0; Way < CacheAssc; ++Way)
        {
            m_TagArray[Index][Way].Tag   = 0x00; //Unecessary as a clear, but needed for initialization. Free as simulation, wouldn't be included in optimized design.
            m_TagArray[Index][Way].MESI  = 'I';
            m_pLRU[Index][Way]           = false;
        }
    }

    m_CacheRead  = 0.0;
    m_CacheWrite = 0.0;
    m_CacheHit   = 0.0;
    m_CacheMiss  = 0.0;
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
    for (int Index = 0; Index < NofIndex; ++Index)
    {
        for (int Way = 0; Way < CacheAssc; ++Way)
        {
            if(m_TagArray[Index][Way].MESI != 'I')
            {
                printf("Index: %d, Way: %d, Tag: %d, MESI: %c\n", Index, Way, m_TagArray[Index][Way].Tag, m_TagArray[Index][Way].MESI);
            }
        }
    }
    printf("\n\n");
}


//--------------------------------------------------------------------------------
// Description: Updates the PLRU array after a data usaage has occured. 
//
//--------------------------------------------------------------------------------
void Cache::update_MRU(unsigned int Index, unsigned int Way)
{
    switch(Way)
    {
        case 0: m_pLRU[Index][0] = 0; m_pLRU[Index][1] = 0; m_pLRU[Index][3] = 0; break;
        case 1: m_pLRU[Index][0] = 0; m_pLRU[Index][1] = 0; m_pLRU[Index][3] = 1; break;
        case 2: m_pLRU[Index][0] = 0; m_pLRU[Index][2] = 1; m_pLRU[Index][4] = 0; break;
        case 3: m_pLRU[Index][0] = 0; m_pLRU[Index][2] = 1; m_pLRU[Index][4] = 1; break;
        case 4: m_pLRU[Index][1] = 1; m_pLRU[Index][1] = 0; m_pLRU[Index][5] = 0; break;
        case 5: m_pLRU[Index][1] = 1; m_pLRU[Index][1] = 0; m_pLRU[Index][5] = 1; break;
        case 6: m_pLRU[Index][1] = 1; m_pLRU[Index][2] = 1; m_pLRU[Index][6] = 0; break;
        case 7: m_pLRU[Index][1] = 1; m_pLRU[Index][2] = 1; m_pLRU[Index][6] = 1; break;
    }
}

//--------------------------------------------------------------------------------
// Description: Returns the Way in the Index which was least recently used (pseudo)
//
//--------------------------------------------------------------------------------
int Cache::find_LRU(unsigned int Index)
{
    int VictimWay = 0;

    if ( m_pLRU[Index][0] == true )
    {
        if ( m_pLRU[Index][1] == true )
        {
            if ( m_pLRU[Index][3] == true )
            {
                VictimWay = 0;
            }
            else
            {
                VictimWay = 1;
            }
        }
        else
        {
            if ( m_pLRU[Index][4] == true )
            {
                VictimWay = 2;
            }
            else
            {
                VictimWay = 3;
            }
        }
    }
    else
    {
        if ( m_pLRU[Index][2] == true )
        {
            if ( m_pLRU[Index][5] == true )
            {
                VictimWay = 4;
            }
            else
            {
                VictimWay = 5;
            }
        }
        else
        {
            if ( m_pLRU[Index][6] == true )
            {
                VictimWay = 6;
            }
            else
            {
                VictimWay = 7;
            }
        }
    }

    return VictimWay;
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
            break;
        case 1:
            SnoopResult = 0x02;
            break;
        default:
            SnoopResult = 0x00;
            break;
    }

    /*
    if the least two significant bits of the address are 00 then the other processors are reporting a HIT
    if the least two significant bits of the address are 01 then the other processors are reporting a HITM
    if the least two significant bits of the address are 1x then the other processors are not reporting a HIT or HITM
    */

    return SnoopResult;
}


//--------------------------------------------------------------------------------
// Description: Used to simulate a bus operation and to capture the snoop results
//              of last level caches of other processors
//
//--------------------------------------------------------------------------------
void Cache::BusOperation(char BusOp, unsigned int Address, char* SnoopResult)
{
    *SnoopResult = GetSnoopResult(Address);

    if (!m_SilentMode)
    {
        printf("Bus Operation: BusOp = %d, address = %x, snoop Result = %d\n", BusOp, Address, *SnoopResult);
    }
}


//--------------------------------------------------------------------------------
// Description: Used to report the result of our snooping bus operations performed
//              by other caches
//
//--------------------------------------------------------------------------------
void Cache::PutSnoopResult(unsigned int Address, char SnoopResult)
{
    if (!m_SilentMode)
    {
        printf("SnoopResult: SnoopResult = %d, Address = %x \n", SnoopResult,  Address);
    }
}


//--------------------------------------------------------------------------------
// Description: Used to simulate communication to our upper level cache
//
//--------------------------------------------------------------------------------
void Cache::MessageToCache(char Message, unsigned int Address)
{
    if (!m_SilentMode)
    {
        printf("L2 to L1: message = %d, address = %x \n", Message, Address);
    }
}

// ---------------
// EOF
// ---------------

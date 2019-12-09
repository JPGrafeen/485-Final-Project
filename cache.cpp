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
            //printf("Searching for %d --- Found Tag-%d\n", Tag, ptrIndex[i].Tag);
        }
    }

    return Hit;
}

//--------------------------------------------------------------------------------
// Description:  Common Algorithm for Evicting a line. Returns Way of the Victim.
//
//--------------------------------------------------------------------------------
unsigned int Cache::Evict_Line(unsigned int Address, char* SnoopResult)
{
    unsigned int Index     = Get_Index(Address);
    Tag_Array* ptrIndex    = &m_TagArray[Index];
    unsigned int VictimWay = 0;
    for ( ; VictimWay < CacheAssc && ptrIndex[VictimWay].Valid; ++VictimWay)
    {
        // Do nothing, just seeing if every cache line is valid.
        // Consequence, if we find an invalid cache line it will 
        // meet the design assumption by being the lowest numbered
        // cache line to fill.
    }

    if (VictimWay >= CacheAssc)
    {
        VictimWay = find_PLRU(Index);
        if (ptrIndex[VictimWay].Dirty)
        {
            MessageToCache(MSG_GETLINE, Address);
            BusOperation(BUS_WRITE, Address, SnoopResult);
        }
    }

    MessageToCache(MSG_EVICTLINE, Address);
    update_PLRU(Index, VictimWay);

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
        unsigned int VictimWay = Evict_Line(Address, &SnoopResult);
        BusOperation(BUS_READ, Address, &SnoopResult);

        ptrIndex[VictimWay].Valid = true;
        ptrIndex[VictimWay].Dirty = false;
        ptrIndex[VictimWay].Tag   = Tag;

        if ( SnoopResult == SNP_HIT
          || SnoopResult == SNP_HITM)
        {
            // Another LLC reported having a copy, go to Shared
            ptrIndex[VictimWay].MESI = 'S';
        }
        else
        {
            // No other LLC reported having a copy, go to Exclusive
            ptrIndex[VictimWay].MESI = 'E';
        }
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

    char    SnoopResult = 0x00;
    unsigned int  Index = Get_Index(Address);
    unsigned int    Tag = Get_Tag(Address);
    Tag_Array* ptrIndex = &m_TagArray[Index];
    unsigned int VictimWay;


    if (Cache_Hit(Address))
    {
        ++m_CacheHit;
        for (VictimWay = 0; VictimWay < CacheAssc && ptrIndex[VictimWay].Tag != Tag; ++VictimWay)
        {
            //printf("%d: Searching for %d, Current tag: %d\n", VictimWay, Tag, ptrIndex[VictimWay].Tag);
        }

        // Check Error States
        if ( VictimWay >= CacheAssc)
        {
            std::cout << "Error State: Inside Cache_hit but unable to find matching Tag" << std::endl;
            return;
        }

        if ( ptrIndex[VictimWay].Valid == false )
        {
            std::cout << "Error State: Inside Cache_Hit but Valid bit is false" << std::endl;
            return;
        }

        if (ptrIndex[VictimWay].MESI == 'I')
        {
            std::cout << "Error State: Cache_Hit on Invalid during L1_Data_Write" << std::endl;
            return;
        }

        // Update Tag Array Entry
        ptrIndex[VictimWay].Dirty = true;

        if (ptrIndex[VictimWay].MESI == 'S')
        {
            BusOperation(BUS_INV, Address, &SnoopResult); // SnoopResult discarded
        }

        // All states move to Modified
        ptrIndex[VictimWay].MESI  = 'M';

    }
    else
    {
        ++m_CacheMiss;

        VictimWay = Evict_Line(Address, &SnoopResult);

        BusOperation(BUS_RWIM, Address, &SnoopResult);

        ptrIndex[VictimWay].Valid = true;
        ptrIndex[VictimWay].Dirty = true;
        ptrIndex[VictimWay].Tag   = Tag;
        ptrIndex[VictimWay].MESI  = 'M';

    }

    ++m_CacheWrite;
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
    if (Cache_Hit(Address))
    {
        ++m_CacheHit;
        // Stay in MESI State
    }
    else
    {
        ++m_CacheMiss;

        char    SnoopResult = 0x00;
        unsigned int  Index = Get_Index(Address);
        unsigned int    Tag = Get_Tag(Address);
        Tag_Array* ptrIndex = &m_TagArray[Index];
        unsigned int VictimWay = Evict_Line(Address, &SnoopResult);
        BusOperation(BUS_READ, Address, &SnoopResult);

        ptrIndex[VictimWay].Valid = true;
        ptrIndex[VictimWay].Dirty = false;
        ptrIndex[VictimWay].Tag   = Tag;

        if ( SnoopResult == SNP_HIT
          || SnoopResult == SNP_HITM)
        {
            // Another LLC reported having a copy, go to Shared
            ptrIndex[VictimWay].MESI = 'S';
        }
        else
        {
            // No other LLC reported having a copy, go to Exclusive
            ptrIndex[VictimWay].MESI = 'E';
        }
    }

    ++m_CacheRead;
    MessageToCache(MSG_SENDLINE, Address);
}


//--------------------------------------------------------------------------------
// Description: Common algorithm to find which Way registered a Hit for a Snoop.
//              Returns this Way value.
//
//--------------------------------------------------------------------------------
unsigned int Cache::Snoop_Hit(unsigned int Address)
{
    unsigned int  Index = Get_Index(Address);
    unsigned int    Tag = Get_Tag(Address);
    Tag_Array* ptrIndex = &m_TagArray[Index];
    unsigned int VictimWay = 0;
    for ( ; VictimWay < CacheAssc && ptrIndex[VictimWay].Tag != Tag; ++VictimWay)
    {
        //Walk the index
    }

    // Check Error States
    if ( VictimWay >= CacheAssc
      || ptrIndex[VictimWay].Valid == false)
    {
        std::cout << "Error State: Contents of Index changed" << std::endl;
        VictimWay = find_PLRU(Get_Index(Address));
    }

    if ( ptrIndex[VictimWay].MESI == 'I' )
    {
        std::cout << "Error State: Invalid MESI state" << std::endl;
        VictimWay = find_PLRU(Get_Index(Address));
    }

    PutSnoopResult(Address, SNP_HIT);
    return VictimWay;
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
        unsigned int  Index = Get_Index(Address);
        Tag_Array* ptrIndex = &m_TagArray[Index];
        unsigned int VictimWay = Snoop_Hit(Address);

        if ( ptrIndex[VictimWay].MESI == 'S' )
        {
            ptrIndex[VictimWay].MESI  = 'I';
            ptrIndex[VictimWay].Valid = false;
            MessageToCache(MSG_INV_LINE, Address);
        }
        else
        {
            std::cout << "Error State: Invalid MESI state during SNP_Invalidate" << std::endl;
        }
    }
    else
    {
        PutSnoopResult(Address, SNP_NOHIT); 
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
        unsigned int  Index = Get_Index(Address);
        Tag_Array* ptrIndex = &m_TagArray[Index];
        unsigned int VictimWay = Snoop_Hit(Address);

        if ( ptrIndex[VictimWay].MESI == 'M' )
        {
            char SnoopResult = 0x00;
            MessageToCache(MSG_GETLINE, Address);
            BusOperation(BUS_WRITE, Address, &SnoopResult); //SnoopResult Discarded.
            ptrIndex[VictimWay].Dirty = false;
        }

        ptrIndex[VictimWay].MESI = 'S';
    }
    else
    {
        PutSnoopResult(Address, SNP_NOHIT); 
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
        unsigned int  Index = Get_Index(Address);
        Tag_Array* ptrIndex = &m_TagArray[Index];
        unsigned int VictimWay = Snoop_Hit(Address);

        if ( ptrIndex[VictimWay].MESI == 'S' )
        {
            ptrIndex[VictimWay].MESI  = 'I';
            ptrIndex[VictimWay].Valid = false;
            MessageToCache(MSG_INV_LINE, Address);
        }
        else
        {
            std::cout << "Error State: Invalid MESI state during SNP_Write" << std::endl;
        }
    }
    else
    {
        PutSnoopResult(Address, SNP_NOHIT); 
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
        unsigned int  Index = Get_Index(Address);
        Tag_Array* ptrIndex = &m_TagArray[Index];
        unsigned int VictimWay = Snoop_Hit(Address);

        if ( ptrIndex[VictimWay].MESI == 'M' )
        {
            char SnoopResult = 0x00;
            MessageToCache(MSG_GETLINE, Address);
            BusOperation(BUS_WRITE, Address, &SnoopResult); //SnoopResult Discarded.
            ptrIndex[VictimWay].Valid = false;
        }

        ptrIndex[VictimWay].MESI = 'I';
        MessageToCache(MSG_INV_LINE, Address);
    }
    else
    {
        PutSnoopResult(Address, SNP_NOHIT); 
    }
}


//--------------------------------------------------------------------------------
// Description: Initializes the cache and resets all state
//
//--------------------------------------------------------------------------------
void Cache::Init_Cache()
{
    for (int i = 0; i < NofIndex; ++i)
    {
        Tag_Array* ptrIndex = &m_TagArray[i];
        for (int j = 0; j < CacheAssc; ++j)
        {
            ptrIndex[j].Valid = false;
            ptrIndex[j].Dirty = false;
            ptrIndex[j].Tag   = 0x00;
            ptrIndex[j].MESI  = 'I';

            m_pLRU[i][j]      = false;
        }
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
    for (int i = 0; i < NofIndex; ++i)
    {
        Tag_Array* ptrIndex = &m_TagArray[i];
        for (int j = 0; j < CacheAssc; ++j)
        {
            ptrIndex[j].Valid = false;
            ptrIndex[j].MESI  = 'I';
            m_pLRU[i][j]      = false;
        }
    }
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
    for (int i = 0; i < NofIndex; ++i)
    {
        Tag_Array* ptrIndex = &m_TagArray[i];
        for (int j = 0; j < CacheAssc; ++j)
        {
            if(ptrIndex[j].Valid == true)
            {
                printf("Index: %d, Tag: %d, Valid: %d, Dirty: %d, MESI: %c", i, ptrIndex[j].Tag, ptrIndex[j].Valid, ptrIndex[j].Dirty, ptrIndex[j].MESI);
            }
        }
    }
}

//--------------------------------------------------------------------------------
// Description: Updates the PLRU array after a data usaage has occured. 
//
//--------------------------------------------------------------------------------
void Cache::update_PLRU(unsigned int Index, unsigned int Way)
{
    switch(Way)
    {
        case 0: 
        case 1: 
            m_pLRU[Index][0] != m_pLRU[Index][0]; 
            m_pLRU[Index][1] != m_pLRU[Index][1]; 
            m_pLRU[Index][3] != m_pLRU[Index][3]; 
            break;
        case 2: 
        case 3: 
            m_pLRU[Index][0] != m_pLRU[Index][0]; 
            m_pLRU[Index][1] != m_pLRU[Index][1]; 
            m_pLRU[Index][4] != m_pLRU[Index][4]; 
            break;
        case 4: 
        case 5: 
            m_pLRU[Index][0] != m_pLRU[Index][0]; 
            m_pLRU[Index][2] != m_pLRU[Index][2]; 
            m_pLRU[Index][5] != m_pLRU[Index][5]; 
            break;
        case 6: 
        case 7: 
            m_pLRU[Index][0] != m_pLRU[Index][0]; 
            m_pLRU[Index][2] != m_pLRU[Index][2]; 
            m_pLRU[Index][6] != m_pLRU[Index][6]; 
            break;
    }
}

//--------------------------------------------------------------------------------
// Description: Returns the Way in the Index which was least recently used (pseudo)
//
//--------------------------------------------------------------------------------
unsigned int Cache::find_PLRU(unsigned int Index)
{
    bool* pLRUIndex = m_pLRU[Index];

    unsigned int VictimWay = 0;

    if ( pLRUIndex[0] == false )
    {
        if ( pLRUIndex[1] == false )
        {
            if ( pLRUIndex[3] == false )
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
            if ( pLRUIndex[4] == false )
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
        if ( pLRUIndex[2] == false )
        {
            if ( pLRUIndex[5] == false )
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
            if ( pLRUIndex[6] == false )
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
double Cache::Get_CacheRatio()
{
	// Dividing by zero releases Chuthulu
	if ( m_CacheHit  == 0
	  && m_CacheMiss == 0)
	{
		return 0.0;
	}

	return ( static_cast<double>(m_CacheHit) / static_cast<double>(m_CacheHit + m_CacheMiss) );
}

// ---------------
// EOF
// ---------------

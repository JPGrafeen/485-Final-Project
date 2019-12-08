/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE 485 Final Project
*
*   Cache class declarations
*/


#define CacheSize = 2^24   //16MiB
#define CacheLine = 2^6    //64Bytes
#define CacheAssc = 2^3    //8-way set associativity
#define NofIndex  = CacheSize / (CacheLine * CacheAssc)


class Cache {

public:
    
    // Constructor
    Cache() :
    m_DebugMode(false)
    {
        Clear_Cache();
    }

    // Destructor
    ~Cache(){
        Clear_Cache();
        Tag_Array = nullptr;
    }


    struct Tag_Array_Entry{
        bool         Valid;      // Valid bit, data is able to be read or modified
        bool         Dirty;      // Dirty bit, data has been modified
        char         MESI;       // 1 hot encoding of the MESI states using 8'b vector
        unsigned int Tag;        // Data tag
    };

    enum BusOp{
        BUS_READ   = 1,  // Read request placed on Bus
        BUS_WRITE  = 2,  // Write request placed on Bus
        BUS_INV    = 3,  // Invalidate command placed on Bus
        BUS_RWIM   = 4   // Read with intent to modify placed on Bus
    };

    enum SnoopOp{
        SNP_NOHIT  = 0,  // Miss reply to snooped request
        SNP_HIT    = 1,  // Hit reply to snooped request
        SNP_HITM   = 2   // Hit Modified reply to snooped request
    };

    enum L1L2MSG{
        MSG_GETLINE    = 1,  //L1 should send the latest version of a line to L2
        MSG_SENDLINE   = 2,  //L2 is sending a modified line to L1
        MSG_INV_LINE   = 3,  //L1 should invalidate the line
        MSG_EVICTLINE  = 4   //L1 should evict the line
    };

private:
    void L1_Data_Read(unsigned int Address);
    void L1_Data_Write(unsigned int Address);
    void L1_Inst_Read(unsigned int Address);
    void SNP_Invalidate(unsigned int Address);
    void SNP_Read(unsigned int Address);
    void SNP_Write(unsigned int Address);
    void SNP_RWIM(unsigned int Address);

    void Clear_Cache();
    void Print_Cache();

    void update_PLRU(unsigned int Index, unsigned int Way);
    unsigned int find_PLRU(unsigned int Index);

    unsigned int Get_Tag(unsigned int Address){ return (Address>>22); }
    unsigned int Get_Index(unsigned int Address){ return ((Address && 0x003FFFC0) >> 6); }
    unsigned int Get_Offset(unsigned int Address){ return (Address && 0x0000003F); }

    void BusOperation(char BusOp, unsigned int Address, char* SnoopResult);
    char GetSnoopResult(unsigned int Address);
    void PutSnoopResult(unsigned int Address, char SnoopResult);
    void MessageToCache(char Message, unsigned int Address);

    unsigned int Cache_Ratio() { return (m_CacheHit / (m_CacheHit + m_CacheMiss)); }


    // Member Variables
    typedef Tag_Array_Entry Tag_Array; //renaming makes it easier to conceptualize in an n-way system. 
    Tag_Array* m_TagArray;
    bool m_DebugMode;
    unsigned int m_CacheRead;
    unsigned int m_CacheWrite;
    unsigned int m_CacheHit;
    unsigned int m_CacheMiss;
};


// ---------------
// EOF
// ---------------
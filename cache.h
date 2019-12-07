/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE 485 Final Project
*
*   Cache class declarations
*/



class Cache {

public:
    
    // Constructor
    Cache()
    {
        Clear_Cache();
    }

    // Destructor
    ~Cache(){
        Clear_Cache();
    };


struct Tag_Array_Entry{
    bool         Valid,      // Valid bit, data is able to be read or modified
    bool         Dirty,      // Dirty bit, data has been modified
    char         MESI,       // 1 hot encoding of the MESI states using 8'b vector
    unsigned int Tag         // Data tag
}

private:
    void L1_Data_Read(unsigned int Address);
    void L1_Data_Write(unsigned int Address);
    void L1_Inst_Read(unsigned int Address);
    void Bus_Invalidate(unsigned int Address);
    void Bus_Read(unsigned int Address);
    void Bus_Write(unsigned int Address);
    void Bus_RWIM(unsigned int Address);

    void Clear_Cache();
    void Print_Cache();

    void update_PLRU(unsigned int Index, unsigned int Way);

    unsigned int Get_Tag(unsigned int Address);
    unsigned int Get_Index(unsigned int Address);
    unsigned int Get_Offset(unsigned int Address);

    void BusOperation(char BusOp, unsigned int Address, char* SnoopResult);
    char GetSnoopResult(unsigned int Address);
    void PutSnoopResult(unsigned int Address, char SnoopResult);
    void MessageToCache(char Message, unsigned int Address);

    unsigned int Cache_Ratio() { return (m_CacheHit / (m_CacheHit + m_CacheMiss)); }


    // Member Variables
    Tag_Array_Entry* Tag_Array;
    bool m_DebugMode;
    unsigned int m_CacheRead;
    unsigned int m_CacheWrite;
    unsigned int m_CacheHit;
    unsigned int m_CacheMiss;
}
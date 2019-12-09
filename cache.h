/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly 
*
*   ECE 485 Final Project
*
*   Cache class declarations
*/

#pragma once
#include <cstdio>
#include <iostream>
#include <cstdint>

#define CacheSize 16777216   //16MiB
#define CacheLine 64         //64Bytes
#define CacheAssc 8          //8-way set associativity
#define NofIndex  CacheSize / (CacheLine * CacheAssc)

#define BUS_READ      0x01  // Read request placed on Bus
#define BUS_WRITE     0x02  // Write request placed on Bus
#define BUS_INV       0x03  // Invalidate command placed on Bus
#define BUS_RWIM      0x04  // Read with intent to modify placed on Bus

#define SNP_NOHIT     0x00  // Miss reply to snooped request
#define SNP_HIT       0x01  // Hit reply to snooped request
#define SNP_HITM      0x02  // Hit Modified reply to snooped request

#define MSG_GETLINE   0x01  //L1 should send the latest version of a line to L2
#define MSG_SENDLINE  0x02  //L2 is sending a modified line to L1
#define MSG_INV_LINE  0x03  //L1 should invalidate the line
#define MSG_EVICTLINE 0x04  //L1 should evict the line


class Cache 
{

public:

    struct Tag_Array_Entry
    {
        bool         Valid;      // Valid bit, data is able to be read or modified
        bool         Dirty;      // Dirty bit, data has been modified
        char         MESI;       // encoded MESI state
        unsigned int Tag;        // Data tag
    };
    typedef Tag_Array_Entry Tag_Array; //renaming makes it easier to conceptualize in an n-way system.

    // Constructor
    Cache(Tag_Array* TagArray) :
    m_TagArray(TagArray),
    m_DebugMode(false)
    {
        Init_Cache();
    }

    // Destructor
    ~Cache()
    {
        Clear_Cache();
        m_TagArray = nullptr;
    }


    void L1_Data_Read(unsigned int Address);
    void L1_Data_Write(unsigned int Address);
    void L1_Inst_Read(unsigned int Address);
    void SNP_Invalidate(unsigned int Address);
    void SNP_Read(unsigned int Address);
    void SNP_Write(unsigned int Address);
    void SNP_RWIM(unsigned int Address);

    void Init_Cache();
    void Clear_Cache();
    void Print_Cache();

    double       Get_CacheRatio(); //(m_CacheHit / (m_CacheHit + m_CacheMiss))
    unsigned int Get_CacheRead(){  return m_CacheRead; }
    unsigned int Get_CacheWrite(){ return m_CacheWrite; }
    unsigned int Get_CacheHit(){   return m_CacheHit; }
    unsigned int Get_CacheMiss(){  return m_CacheWrite; }

    void Set_DebugMode(bool DebugMode){ m_DebugMode = DebugMode; }
    bool Get_DebugMode(){ return m_DebugMode; }


private:
    bool Cache_Hit(unsigned int Address);

    unsigned int Evict_Line(unsigned int Address, char* SnoopResult);

    unsigned int Snoop_Hit(unsigned int Address);

    void update_PLRU(unsigned int Index, unsigned int Way);
    unsigned int find_PLRU(unsigned int Index);

    unsigned int Get_Tag(   unsigned int Address){ return ( Address>>22); }
    unsigned int Get_Index( unsigned int Address){ return ((Address &= 0x003FFFC0) >> 6); }
    unsigned int Get_Offset(unsigned int Address){ return ( Address &= 0x0000003F); }

    void BusOperation(char BusOp, unsigned int Address, char* SnoopResult);
    char GetSnoopResult(unsigned int Address);
    void PutSnoopResult(unsigned int Address, char SnoopResult);
    void MessageToCache(char Message, unsigned int Address);


    // Member Variables
    Tag_Array* m_TagArray;
    bool m_DebugMode;
    bool m_pLRU[NofIndex][CacheAssc-1];
    unsigned int m_CacheRead;
    unsigned int m_CacheWrite;
    unsigned int m_CacheHit;
    unsigned int m_CacheMiss;
};


// ---------------
// EOF
// ---------------

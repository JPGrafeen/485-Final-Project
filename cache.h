/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly Makinster
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

#define CACHE_MISS    0x80000000 // Encoded Miss Message. HEX for -0

//#define DEBUG true

#ifndef DEBUG
    #define DEBUG false
#endif

class Cache 
{

public:

    struct Tag_Array_Entry
    {
        char         MESI;       // encoded MESI state
        unsigned int Tag;        // Data tag
    };
    typedef Tag_Array_Entry Tag_Array; //renaming makes it easier to conceptualize in an n-way system.

    // Constructor
    Cache()
    {
        Clear_Cache();
    }

    void L1_Data_Read(unsigned int Address);
    void L1_Data_Write(unsigned int Address);
    void L1_Inst_Read(unsigned int Address);
    void SNP_Invalidate(unsigned int Address);
    void SNP_Read(unsigned int Address);
    void SNP_Write(unsigned int Address);
    void SNP_RWIM(unsigned int Address);

    void Clear_Cache();
    void Print_Cache();

    double       Get_CacheRatio(){ return ( ((m_CacheHit + m_CacheMiss) == 0.0) ? 0.0 : (m_CacheHit / (m_CacheHit + m_CacheMiss)) ); }
    unsigned int Get_CacheRead(){  return m_CacheRead; }
    unsigned int Get_CacheWrite(){ return m_CacheWrite; }
    unsigned int Get_CacheHit(){   return m_CacheHit; }
    unsigned int Get_CacheMiss(){  return m_CacheMiss; }

    void Set_SilentMode(bool SilentMode){ m_SilentMode = SilentMode; }
    bool Get_SilentMode(){ return m_SilentMode; }


private:
    int Find_Way(unsigned int Address);
    int Process_Line(unsigned int Address);

    void update_MRU(unsigned int Index, unsigned int Way);
    int    find_LRU(unsigned int Index);

    unsigned int Get_Tag(   unsigned int Address){ return ( Address>>21); }
    unsigned int Get_Index( unsigned int Address){ return ((Address &= 0x001FFFC0) >> 6); }
    unsigned int Get_Offset(unsigned int Address){ return ( Address &= 0x0000003F); }

    void BusOperation(char BusOp, unsigned int Address, char* SnoopResult);
    char GetSnoopResult(unsigned int Address);
    void PutSnoopResult(unsigned int Address, char SnoopResult);
    void MessageToCache(char Message, unsigned int Address);


    // Member Variables
    Tag_Array m_TagArray[NofIndex][CacheAssc];
    bool m_SilentMode;
    bool m_pLRU[NofIndex][CacheAssc-1];
    double m_CacheRead;
    double m_CacheWrite;
    double m_CacheHit;
    double m_CacheMiss;
};


// ---------------
// EOF
// ---------------

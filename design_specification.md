Assumptions:
	Write-hit action is not defined for the L2 cache. We have decided to use a Write-Back policy for the hit action.

	Write-miss action is not defined for the L1 cache. We have decided to use a Write Allocate policy for the miss action.

	These assumptions will help maintain inclusivity.


Cache:
	Cache is 16MiB, uses 64B lines, and is 8-way set associative using an undefined address size.
		64B * 8 = 256B/index
		16MiB / 256B = 64Ki indexs   (65,536)

		Log2(64) = 6'b
		log2(64K) = 16'b

		Write-Allocate miss action: Write data on to block on miss1

	addressing:
		[00-05] Byte Select
		[06-21] Index
		[22-xx] Tag

Programming Container:
	struct array of [index][way]

	struct Tag_Array_Entry{
	    boolean Valid,
	    boolean Dirty,
	    char MESI,
	    unsigned int Tag
	}

Enums: 

    enum TraceOp{
        TRC_L1_DATA_READ     = 0,
        TRC_L1_DATA_WRITE    = 1,
        TRC_L1_INST_READ     = 2,
        TRC_SNP_INVALIDATE   = 3,
        TRC_SNP_READ         = 4,
        TRC_SNP_WRITE        = 5,
        TRC_SNP_RWIM         = 6,
        TRC_CLR_CACHE        = 8,
        TRC_PRNT_CACHE       = 9
    }

    enum BusOp{
        BUS_READ   = 1,
        BUS_WRITE  = 2,
        BUS_INV    = 3,
        BUS_RWIM   = 4
    }

    enum SnoopOp{
        SNP_NOHIT  = 0,
        SNP_HIT    = 1,
        SNP_HITM   = 2
    }

    enum L1L2MSG{
        MSG_GETLINE    = 1,
        MSG_SENDLINE   = 2,
        MSG_INV_LINE   = 3,
        MSG_EVICTLINE  = 4
    }

    struct Tag_Array_Entry{
        bool         Valid,      // Valid bit, data is able to be read or modified
        bool         Dirty,      // Dirty bit, data has been modified
        char         MESI,       // 1 hot encoding of the MESI states using 8'b vector
        unsigned int Tag         // Data tag
    }


Traces:
	The testbench will read events from a text file of the following format:

		n address

	where n is the 'operation' (Communication, Snoop Result, or Debug Information)

     - 0  read request from L1 data cache
	 - 1  write request from L1 data cache
	 - 2  read request from L1 instruction cache
	 - 3  snooped invalidate command
	 - 4  snooped read request
	 - 5  snooped write request
	 - 6  snooped read with intent to modify request
	 - 8  clear the cache and reset all state
 	 - 9  print contents and state of each valid cache line (allow subsequent trace activity)

 	where address is a hex value

 	 - The trace file needs to be passed to the program as a command line argument (argv)
 	 - main() will error check if the trace file exists, if so, open it and begin reading line by line until eof. 
 	 - For each line in the trace file the operation and address needs to be translated and the appropriate function called
 		- There should be a function for all 9 operations
 			- L1_Data_Read(unsigned int address, Cache* this)
 			- L1_Data_Write(unsigned int address, Cache* this)
 			- L1_Inst_Read(unsigned int address, Cache* this)
 			- SNP_Invalidate(unsigned int address, Cache* this)
 			- SNP_Read(unsigned int address, Cache* this)
 			- SNP_Write(unsigned int address, Cache* this)
 			- SNP_RWIM(unsigned int address, Cache* this)
 			- Clear_Cache()
 			- Print_Cache()


 	 - For each line in the trace file the operation needs to be reported if in the correct "mode"
 	 	- void BusOperation     // Used to simulate a bus operation and to capture the snoop results of other LLC
 	 	- char GetSnoopResult   // Used to simulate the reporting of snoop results by other caches
 	 	- void PutSnoopResult   // Used to report the result of our snooping bus operations performed by other caches
 	 	- void MessageToCache   // Used to simulate communication to our upper level cache. ex: L2 eviction "back invalidation"

 	 		- Each of these functions have an example stub in the project specification pdf.
 	 		- The print statement styling should be preserved in the switch from C to C++.

Pseudo-PRLU:

	The same function used in Homework 5 should be updated for 8-way set associativity and added as
		void update_PLRU(unsigned int Index, unsigned int Way)

Output:

	Maintain and report the following key statistics of cache usage for each cache and print them upon completion of execution of each trace.
		# of Cache Reads
		# of Cache Writes
		# of Cache Hits
		# of Cache Misses
		Cache Hit Ratio

	- This will be achieved with simple member variable integers and printed to screen at the end of main()
		- Increment each member during the appropriate function call for Cache Read and Write.
		- Each Read and Write operation should record both that it was a Read or Write, and if it was a Hit or Miss.

		ex: unsigned int m_CacheRead
			unsigned int m_CacheWrite
			unsigned int m_CacheHit
			unsigned int m_CacheMiss

			unsigned int Cache_Ratio()
			{
				return (m_CacheHit / (m_CacheHit + m_CacheMiss));
			}


Modes:

	Release and Debug modes. 
		Release Mode: Simulation will only print to screen responses to a '9' in the trace file and the usage statistics at the completion of the project. 

		Debug Mode:   Displays everything for Release mode and also bus operations, reported snoop results, and communication messages to higher level cache (L1)

		Switching between Release and Debug will be achieved through the execution arguments list (argv) 


Bus Operations:

	- READ        // Read  data from next level cache
	- WRITE       // Write data to   next level cache
	- INVALIDATE  // Data was modified, inform next level cache
	- RWIM        // Read data from next level cache, with snoopability


Modeling communication between Lower level cache to next higher level cache:

	- For this project we will refer to the Lower Level Cache (LLC) as L2 and the next higher level cache as L1.
	- Messages between L1 and L2 are defined as
		- GETLINE         // Request data for modified line in L1
		- SENDLINE        // Send requested cache line to L1
		- INVALIDATELINE  // Invalidate a line in L1
		- EVICTLINE       // Evict a line from L1
	- Communication from L1 are operations defined as 0s, 1s, and 2s in the trace file. 
	- void MessageToCache   // Used to simulate communication to our upper level cache
	- L1 employs a Write Once Policy. The first write hit to L1 is write through. The subsequent write hits are write back.
		- If the operation is 1
			- on a hit,  set Dirty bit for associated tag in L2 to true.
			- on a miss, 
				- simulate BusOperation for RWIM, 
				- set valid==true, 
				- set dirty==true,
				- set tag, 
				- Simulate a SENDLINE
		- If L2 needs to evict a cache line, and the Dirty bit is true:
			- simulate a GETLINE
		  	- simulate a BusOperation for WRITE
		- If the operation is a 0 or 2
			- on a hit, simulate a SENDLINE
			- on a miss,
				- simulate BusOperation to READ, 
				- set valid==true, 
				- set dirty==false,
				- set tag, 
				- simulate a SENDLINE
        

MESI Protocol:

	Snoop Result Types:
		- NOHIT  // No hit... otherwise officially known as miss
		- HIT    // Hit
		- HITM   // Hit on modified line

	- Operations 3, 4, 5, & 6 are snoop operations.

	- For all operations, simualte a HIT, HITM, or NOHIT appropriately
	- For all operations, update Dirty and Valid bits appropriately

	- For Operation 0 'read request from L1 data cache'
		- on a hit, stay in current MESI state
		- on a miss,
			- simulate BusOperation for READ
				- if GetSnoopResult returns HITM or HIT update state to S
					- else update state to E
	- For Operation 1 'write request from L1 data cache'
		- on a hit,
			- state == M
				- stay in M
			- state == E
				- move to M
			- state == S
				- move to M
				- simulate BusOperation for INVALIDATE
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- simulate BusOperation for RWIM
			- move to M (shoulda been in I)
	- For Operation 2 'read request from L1 instruction cache'
		- on a hit, stay in current MESI state
		- on a miss,
			- simulate BusOperation for READ
				- if GetSnoopResult returns HITM or HIT update state to S
					- else update state to E
	- For Operation 3 'snooped invalidate command'
		- on a hit,
			- state == M
				- Error State - You should have been EXCLUSIVE
			- state == E
				- Error State - You should have been EXCLUSIVE
			- state == S
				- simulate a HIT
				- move to I
				- simulate INVALIDATELINE
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- Stay in I, all others are Error States
	- For Operation 4 'snooped read request'
		- on a hit,
			- state == M
				- simulate a GETLINE
				- simulate a BusOperation for WRITE
				- move to S
			- state == E
				- move to S
			- state == S
				- stay in S
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- stay in I
	- For Operation 5 'snooped write request'
		- on a hit,
			- state == M
				- Error State - You should have been EXCLUSIVE
			- state == E
				- Error State - You should have been EXCLUSIVE
			- state == S
				- move to I
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- stay in I
	- For Operation 6 'snooped read with intent to modify request'
		- on a hit,
			- state == M
				- simulate a GETLINE
				- simulate a BusOperation for WRITE
				- move to I
			- state == E
				- move to I
			- state == S
				- move to I
			- state == I
				- Error State - can't have a hit and Invalid
		- on a miss,
			- stay in I


Combined Cache operations

	- For Operation 0 'read request from L1 data cache'
		- on a hit, 
			- increment m_CacheHit
			- increment m_CacheRead
			- stay in current MESI state
		- on a miss,
			- increment m_CacheMiss
			- increment m_CacheRead
			- simulate BusOperation for READ
				- if GetSnoopResult returns HITM or HIT update state to S
					- else update state to E
	- For Operation 1 'write request from L1 data cache'
		- on a hit,
			- increment m_CacheHit
			- increment m_CacheWrite
			- state == M
				- stay in M
			- state == E
				- move to M
			- state == S
				- move to M
				- simulate BusOperation for INVALIDATE
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- increment m_CacheMiss
			- increment m_CacheWrite
			- simulate BusOperation for RWIM
			- move to M (shoulda been in I)
	- For Operation 2 'read request from L1 instruction cache'
		- on a hit, 
			- increment m_CacheHit
			- increment m_CacheRead
			- stay in current MESI state
		- on a miss,
			- increment m_CacheMiss
			- increment m_CacheRead
			- simulate BusOperation for READ
				- if GetSnoopResult returns HITM or HIT update state to S
					- else update state to E
	- For Operation 3 'snooped invalidate command'
		- on a hit,
			- state == M
				- Error State - You should have been EXCLUSIVE
			- state == E
				- Error State - You should have been EXCLUSIVE
			- state == S
				- simulate a HIT
				- move to I
				- simulate INVALIDATELINE
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- Stay in I, all others are Error States
	- For Operation 4 'snooped read request'
		- on a hit,
			- state == M
				- simulate a GETLINE
				- simulate a BusOperation for WRITE
				- move to S
			- state == E
				- move to S
			- state == S
				- stay in S
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- stay in I
	- For Operation 5 'snooped write request'
		- on a hit,
			- state == M
				- Error State - You should have been EXCLUSIVE
			- state == E
				- Error State - You should have been EXCLUSIVE
			- state == S
				- move to I
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- stay in I
	- For Operation 6 'snooped read with intent to modify request'
		- on a hit,
			- state == M
				- simulate a GETLINE
				- simulate a BusOperation for WRITE
				- move to I
			- state == E
				- move to I
			- state == S
				- move to I
			- state == I
				- Error State - can't have a hit and Invalid
		- on a miss,
			- stay in I



















/*  Fall 2019
*   JP Grafeen
*   Jordan Bergmann
*   Kelly Makinster
*
*   Design Specification used for programming cache operations
*/


Assumptions:
	Write-hit action is not defined for the L2 cache. We have decided to use a Write-Back policy for the hit action.

	Write-miss action is not defined for the L1 cache. We have decided to use a Write Allocate policy for the miss action.

	These assumptions will help maintain inclusivity.

	Professoritorial Assumptions: The usage of an unsigned int in the required debug print statements, which is either 2 or 4 bytes depending on the compiler, indicates the address length is either 16b or 32b. As 16b is too small to fit the required offset, and index, 32b is assumed for the address length.

	Professoritorial Assumptions: The eviction policy should first verify that there are no open lines to copy into. Eviction should only occur if all lines are valid and a L1 Read or Write occurs. 


Cache:
	Cache is 16MiB, uses 64B lines, and is 8-way set associative using an undefined address size.
		64B * 8 = 512B/index
		16MiB / 512B = 32Ki indexs

		Log2(64) = 6'b
		log2(32K) = 15'b

		Write-Allocate miss action: Write data on to block on miss1

	addressing:
		[00-05] Byte Select
		[06-20] Index
		[21-32] Tag

Programming Container:
	struct array of [index][way]

	struct Tag_Array_Entry{
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
 			- L1_Data_Read(unsigned int address)
 			- L1_Data_Write(unsigned int address)
 			- L1_Inst_Read(unsigned int address)
 			- SNP_Invalidate(unsigned int address)
 			- SNP_Read(unsigned int address)
 			- SNP_Write(unsigned int address)
 			- SNP_RWIM(unsigned int address)
 			- Clear_Cache()
 			- Print_Cache()


 	 - For each line in the trace file the operation needs to be reported if in the correct "mode"
 	 	- void BusOperation     // Used to simulate a bus operation and to capture the snoop results of other LLC
 	 	- char GetSnoopResult   // Used to simulate the reporting of snoop results by other caches
 	 	- void PutSnoopResult   // Used to report the result of our snooping bus operations performed by other caches
 	 	- void MessageToCache   // Used to simulate communication to our upper level cache.

 	 		- Each of these functions have an example stub in the project specification pdf.
 	 		- The print statement styling should be preserved in the switch from C to C++.

Pseudo-PRLU:

	The same function used in Homework 5 should be updated for 8-way set associativity and added as
		void update_MRU(unsigned int Index, unsigned int Way)

	The opposite path to set the MRU should be taken to find the LRU.  
		int find_LRU()

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

			double Get_CacheRatio()
			{
				return (m_CacheHit / (m_CacheHit + m_CacheMiss));
			}


Modes:

	Default and Silent modes. 
		Silent: Simulation will only print to screen responses to a '9' from the trace file and the usage statistics at the completion of the run. 

		Default Mode:   Displays bus operations, reported snoop results, and communication messages to higher level cache (L1)

		Switching between Default and Silent will be achieved through the execution arguments list (argv) 


Bus Operations:

	- READ        // Read  data from next level cache
	- WRITE       // Write data to   next level cache
	- INVALIDATE  // Data was modified, inform next level cache
	- RWIM        // Read data from next level cache, with snoopability

Eviction Policy:

	- This cache simulation uses Pseudo Least Recently Used (PLRU) for the eviction policy. 
	- given an index, on a miss, the Victim will be selected and overwritten with the new tag and control bits
	- from the assumptions if there are empty cache lines, the lowest valued way will be filled first. 


Combined Cache operations

	- For Operation 0 'read request from L1 data cache'
		- on a hit, 
			- increment m_CacheHit
			- increment m_CacheRead
			- stay in current MESI state
			- simulate SENDLINE
		- on a miss,
			- increment m_CacheMiss
			- increment m_CacheRead
			- if required, Evict cache line and send EVICTLINE to L1
				- if MESI state is 'Modified' for victim
					- simulate GETLINE
					- simulate a BusOperation for WRITE
			- simulate BusOperation for READ
			- set tag
			- if GetSnoopResult returns HITM or HIT update state to S
				- else update MESI state to 'Exclusive'
			- simulate SENDLINE

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
			- if required, Evict cache line and send EVICTLINE to L1
				- if MESI state is 'Modified' for victim
					- simulate GETLINE
					- simulate a BusOperation for WRITE
			- simulate BusOperation for RWIM
			- set tag
			- simulate SENDLINE
			- move to M (shoulda been in I)

	- For Operation 2 'read request from L1 instruction cache'
		- on a hit, 
			- increment m_CacheHit
			- increment m_CacheRead
			- stay in current MESI state
			- simulate SENDLINE
		- on a miss,
			- increment m_CacheMiss
			- increment m_CacheRead
			- if required, Evict cache line and send EVICTLINE to L1
				- if MESI state is 'Modified' for victim
					- simulate GETLINE
					- simulate a BusOperation for WRITE
			- simulate BusOperation for READ
			- set tag
			- if GetSnoopResult returns HITM or HIT update state to S
				- else update state to E
			- simulate SENDLINE

	- For Operation 3 'snooped invalidate command'
		- on a hit,
			- state == M
				- Error State - You should have been EXCLUSIVE
			- state == E
				- Error State - You should have been EXCLUSIVE
			- state == S
				- simulate PutSnoopResult for HIT
				- move to I
				- simulate INVALIDATELINE
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- simulate PutSnoopResult for NOHIT

	- For Operation 4 'snooped read request'
		- on a hit,
			- simulate PutSnoopResult for HIT
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
			- simulate PutSnoopResult for NOHIT

	- For Operation 5 'snooped write request'
		- on a hit,
			- state == M
				- Error State - You should have been EXCLUSIVE
			- state == E
				- Error State - You should have been EXCLUSIVE
			- state == S
				- simulate PutSnoopResult for HIT
				- move to I
				- simulate INVALIDATELINE
			- state == I
				- Error State - can't have hit and Invalid
		- on a miss,
			- simulate PutSnoopResult for NOHIT

	- For Operation 6 'snooped read with intent to modify request'
		- on a hit,
			- simulate PutSnoopResult for HIT
			- state == M
				- simulate a GETLINE
				- simulate a BusOperation for WRITE
				- move to I
				- simulate INVALIDATELINE
			- state == E
				- move to I
				- simulate INVALIDATELINE
			- state == S
				- move to I
				- simulate INVALIDATELINE
			- state == I
				- Error State - can't have a hit and Invalid
		- on a miss,
			- simulate PutSnoopResult for NOHIT







// ---------------
// EOF
// ---------------











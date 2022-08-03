PAGING WITH SEGMENTATION

#WHAT DOES THIS PROGRAM DO ?

This program is a simulation of paging with segmentation, virtual memory and memory management.Assume each segment consists of fixed size 1KB (1024B) pages.For each segment of this process, half the number of pages of the relevant segment (if odd, round up) physical frames are reserved. Program finds the segment and physical frame infos with 'size' command with executable file. Program takes reference string file as a second argument and first column of the file is segment_number and second column of the file is page_number.

Firstly, first page of each segment is in physical memory and assume the rest is kept in virtual memory.A TLB is reserved with 50 entries.Then simulation starts. It is a simulation, so the latencies must be real:

if tlb hit, program sleeps 1ms

if tlb miss without page fault, program sleeps 10ms

if tlb miss with page fault, program sleeps 100ms

TLB replacement policy is FIFO
Virtual memory replacement policy is LRU

After simulation, the results are printed to the stdout.

#HOW THIS SIMULATION WORKS ?

1- First page of every segment is placed in physical memory
2- reference string file is read and simulation starts.
3- If the value is invalid, then read next row in the file and skip that round. Increase numberOfInvalidReference variable.
4- If the value is is TLB, then TLB hit and 1ms latency.
5- If the value is not in TLB, then look at the page table.
6- If the value is in page table, then it is placed to the TLB and TLB Miss without Page fault and 10ms latency.
7- If the value is not in page table, then the value is bring from virtual memory and it is placed both TLB and Page Table.
8- All of the above steps works again until end of the file.


#HOW TO COMPILE:

gcc -w pagingWithSegmentation.c -o pagingWithSegmentation

#HOW TO EXECUTE:

This program takes two arguments from command-line.

First argument: must be the name of executable file

Second argument: name of the reference string file

./pagingWithSegmentation [executable file name] [name of the reference string file]

example:

./pagingWithSegmentation pagingWithSegmentation referenceStrig.txt









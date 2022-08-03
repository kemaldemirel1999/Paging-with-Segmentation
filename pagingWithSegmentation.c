#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

#define PAGE_SIZE 1024
#define TLB_HIT 0.001
#define TLB_MISS_WITHOUT_PAGE_FAULT 0.01
#define TLB_MISS_WITH_PAGE_FAULT 0.1

/*
    TLB Replacement policy: FIFO
    Virtual Memory Replacement Policy: LRU Replacement Algorithm
    Segment-page numbers information is sufficient for simulation.
    Every segment has fixed size 1kB (1024 byte) pages.

    Example of 'size' command response:
    text        -> 1691
    data        -> 652
    bss         -> 4
    dec         -> 2347
    hex         -> 92b  
    filename    -> memory-layout
*/

int numberOfTLBMiss = 0;
int numberOfAccess = 0;
int numberOfPageFault = 0;
int totalDelay = 0;
int numberOfInvalidReference = 0;

int text;
int bss;
int data;

int physicalFrame[3];
int segments[3];

int TLB[50][3];
int PAGE_TABLE[100][3];

int runSimulaton(char *executableFile, char *filename);
void doOperations(int segment_number, int page_number);
void clearPageTable(int page_table[100][3]);
void increaseUsageCounterInPageTable(int page_table[100][3], int index);
int findPlaceInPageTable(int page_table[100][3]);
int getIndexInPageTable(int page_table[100][3], int segment_number, int page_number);
int IsItInPageTable(int page_table[100][3], int segment_number, int page_number);
int isItInvalid(int segment_number, int page_number);
void clearTLB(int tlb[50][3]);
void increaseTimeStamp(int tlb[50][3]);
int findPlaceInTLB(int tlb[50][3]);
int IsItInTLB(int tlb [50][3], int segment_number, int page_number);
int findTheOldest(int tlb[50][3]);
void findPhysicalFrames();
void findSegmentPages();
int findSegmentInfos( char *executableFileName);
void printPageTable(int page_table[100][3]);
void printTLB(int tlb[50][3]);
void printSimulationResults();

int main(int argc, char *argv[]){
    
    if ( runSimulaton(argv[1], argv[2]) == -1){
        return 1;
    }
    printf("\n");
    printSimulationResults();
    return 0;
}

int runSimulaton(char *executableFile, char *filename){
    if ( findSegmentInfos(executableFile) ){
        printf("Executable file NOT FOUND\n");
        return 1;
    }
    findSegmentPages();
    findPhysicalFrames();
    clearTLB(TLB);

    PAGE_TABLE[0][1] = 0;
    PAGE_TABLE[0][2] = 0;
    PAGE_TABLE[1][1] = 1;
    PAGE_TABLE[1][2] = 0;
    PAGE_TABLE[2][1] = 2;
    PAGE_TABLE[2][2] = 0;

    FILE *file;
    char buffer[100];
    file = fopen(filename, "r");
    while(fgets(buffer, 99, file) != NULL){
        int segment_number;
        int page_number;
        sscanf( buffer, "%d %d", &segment_number, &page_number);
        doOperations(segment_number, page_number);
        increaseTimeStamp(TLB);
    }
    return 0;
}


void doOperations(int segment_number, int page_number){
    
    int invalidReference = isItInvalid(segment_number, page_number);
    if(invalidReference == 0){
        numberOfInvalidReference++;
        printf("INVALID:%d,%d\n",segment_number, page_number);
    }
    else{
        if(IsItInTLB(TLB, segment_number, page_number) == 0){
            sleep(TLB_HIT);
            totalDelay += TLB_HIT*1000;
            numberOfAccess++;
            printf("IN TLB:%d,%d\n",segment_number, page_number);
        }
        else if(IsItInPageTable(PAGE_TABLE, segment_number, page_number) == 0){
            sleep(TLB_MISS_WITHOUT_PAGE_FAULT);
            totalDelay += TLB_MISS_WITHOUT_PAGE_FAULT*1000;
            numberOfAccess++;
            numberOfTLBMiss++;
            int index = getIndexInPageTable(PAGE_TABLE, segment_number, page_number);
            increaseUsageCounterInPageTable(PAGE_TABLE, index);
            printf("PAGE TABLE:%d,%d\n",segment_number, page_number);
            int tlb_index = findPlaceInTLB(TLB);
            TLB[tlb_index][0] = 0;
            TLB[tlb_index][1] = segment_number;
            TLB[tlb_index][2] = page_number;
        }
        else{
            sleep(TLB_MISS_WITH_PAGE_FAULT);
            totalDelay += TLB_MISS_WITH_PAGE_FAULT*1000;
            numberOfAccess++;
            numberOfTLBMiss++;
            numberOfPageFault++;
            int index = findPlaceInPageTable(PAGE_TABLE);
            PAGE_TABLE[index][0] = 0;
            PAGE_TABLE[index][1] = segment_number;
            PAGE_TABLE[index][2] = page_number;
            printf("PAGE FAULT:%d,%d\n",segment_number, page_number);
            int tlb_index = findPlaceInTLB(TLB);
            TLB[tlb_index][0] = 0;
            TLB[tlb_index][1] = segment_number;
            TLB[tlb_index][2] = page_number;
        }
        
    }

}


void clearPageTable(int page_table[100][3]){
    for(int i=0; i<100; i++){
        page_table[i][0] = 0;
        page_table[i][1] = 0;
        page_table[i][2] = 0;
    }
}
// Update Usage counter for LRU Replacement Algorithm in Virtual Memory
void increaseUsageCounterInPageTable(int page_table[100][3], int index){
    page_table[index][0] = page_table[index][0] + 1;
}

// Find place in Virtual Memory using LRU algorithm.
int findPlaceInPageTable(int page_table[100][3]){
    int lru_index = 0;
    for(int i=0; i< 100; i++){
        if(page_table[i][0] < page_table[lru_index][0]){
            lru_index = i;
        }
    }
    return lru_index;
}

int getIndexInPageTable(int page_table[100][3], int segment_number, int page_number){
    for(int i=0; i< 100; i++){
        if(page_table[i][1] == segment_number && page_table[i][2] == page_number){
            return i;
        }
    }
    return -1; // Page Fault
}
// Checks if the value is in virtual memory and returns its index. Otherwise returns -1 (page fault)
int IsItInPageTable(int page_table[100][3], int segment_number, int page_number){
    for(int i=0; i< 100; i++){
        if(page_table[i][1] == segment_number && page_table[i][2] == page_number){
            return 0;
        }
    }
    return -1; // Page Fault
}
// Checks if the segment_number and page_number is invalid
int isItInvalid(int segment_number, int page_number){
    if(segment_number < 0 || segment_number > 2){
        return 0;
    }
    else if( page_number > segments[segment_number]-1 || page_number < 0){
        return 0;
    }
    return 1;
}

// Clears all the values in TLB
void clearTLB(int tlb[50][3]){
    for(int i=0; i<50; i++){
        tlb[i][0] = 0;
        tlb[i][1] = -1;
        tlb[i][2] = -1;
    }
}

// Increases the timestamp in TLB for FIFO
void increaseTimeStamp(int tlb[50][3]){
    for(int i=0; i<50; i++){
        if(tlb[i][1] != -1){
            tlb[i][0] = tlb[i][0] + 1;
        }
    }
}

// Finds an index in TLB to replace
int findPlaceInTLB(int tlb[50][3]){
    for(int i=0; i<50; i++){
        if(tlb[i][1] == -1){
            return i;
        }
    }
    int oldestIndex = findTheOldest(tlb);
    return oldestIndex;
}

// Checks if the value is in TLB
int IsItInTLB(int tlb [50][3], int segment_number, int page_number){
    for(int i=0; i<50; i++){
        if( tlb[i][1] == segment_number && tlb[i][2] == page_number){
            return 0;
        }
    }
    return -1;
}

// Returns an index value with a rule of FIFO
int findTheOldest(int tlb[50][3]){
    int oldestIndex = 0;
    for(int i=0; i<50; i++){
        if(tlb[oldestIndex][0] < tlb[i][0]){
            oldestIndex = i;
        }
    }
    return oldestIndex;
}

// Finds the number of physical frames for each segment.
void findPhysicalFrames(){
    physicalFrame[0] = segments[0] / 2;
    if(segments[0] % 2 == 1){
        physicalFrame[0] = physicalFrame[0] + 1;
    }
    physicalFrame[1] = segments[1] / 2;
    if(segments[1] % 2 == 1){
        physicalFrame[1] = physicalFrame[1] + 1;
    }
    physicalFrame[2] = segments[2] / 2;
    if(segments[2] % 2 == 1){
        physicalFrame[2] = physicalFrame[2] + 1;
    }
    printf("Number of physical frame in segment_0 :%d\n",physicalFrame[0]);
    printf("Number of physical frame in segment_1 :%d\n",physicalFrame[1]);
    printf("Number of physical frame in segment_2 :%d\n",physicalFrame[2]);
}

// Finds the number of pages for each segment
void findSegmentPages(){
    segments[0] = text/PAGE_SIZE;
    if(text % PAGE_SIZE > 0){
        segments[0] = segments[0] + 1;
    }
    segments[1] = data/PAGE_SIZE;
    if(data % PAGE_SIZE > 0){
        segments[1] = segments[1] + 1;
    }
    segments[2] = bss/PAGE_SIZE;
    if(bss % PAGE_SIZE > 0){
        segments[2] = segments[2] + 1;
    }
    printf("Number of pages in segment_0 :%d\n",segments[0]);
    printf("Number of pages in segment_1 :%d\n",segments[1]);
    printf("Number of pages in segment_2 :%d\n",segments[2]);
}

// Finds the values of text, data, and bss
int findSegmentInfos( char *executableFileName){
    FILE *file;
    char buffer[100];
    char command[] = "size ";
    strcat(command, executableFileName);
    file = popen(command, "r");
    if(file == NULL){
        fputs("POPEN: Failed to execute command\n",stderr);
        return 1;
    }
    else{
        int row = 0;
        while(fgets(buffer, 99, file) != NULL){
            if(row == 1){
                char *str = buffer;
                char * pch;
                pch = strtok (str," ,.");
                int i = 0;
                while (pch != NULL)
                {
                    if( i == 0){
                        text = atoi(pch);
                    }
                    else if( i == 1){
                        data = atoi(pch);
                    }
                    else if( i == 2){
                        bss = atoi(pch);
                    }
                    pch = strtok (NULL, " ,.");
                    i++;
                }
            }
            ++row;
        }
    }
    return 0;
}

void printPageTable(int page_table[100][3]){
    for(int i=0; i<100; i++){
        printf("page_table[%d]: %d, %d, %d\n",i,page_table[i][0],page_table[i][1],page_table[i][2]);
    }
}

void printTLB(int tlb[50][3]){
    for(int i=0; i<50; i++){
        printf("tlb[%d]: %d, %d, %d\n",i,tlb[i][0],tlb[i][1],tlb[i][2]);
    }
}
// Prints the simulation results to stdout
void printSimulationResults(){
    double number_of_access = numberOfAccess;
    double tlb_miss_rate = numberOfTLBMiss / number_of_access;
    printf("Number of TLB Miss:%d\n",numberOfTLBMiss);
    printf("TLB Miss Rate:%f\n",tlb_miss_rate);
    double page_fault_rate = numberOfPageFault / number_of_access;
    printf("Number of Page Fault:%d\n",numberOfPageFault);
    printf("Page Fault Miss Rate:%f\n",page_fault_rate);
    printf("Total Delay:%d\n",totalDelay);
    printf("Number of Invalid Reference:%d\n",numberOfInvalidReference);
}

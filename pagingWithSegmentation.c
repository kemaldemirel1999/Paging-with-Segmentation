#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

#define PAGE_SIZE 1024
#define TLB_HIT 1
#define TLB_MISS_WITHOUT_PAGE_FAULT 10
#define TLB_MISS_WITH_PAGE_FAULT 100

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

// Runs with byte values

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
int MEMORY[3][2];

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
    MEMORY[0][0] = 0;
    MEMORY[0][1] = 0;
    MEMORY[1][0] = 1;
    MEMORY[1][1] = 0;
    MEMORY[2][0] = 2;
    MEMORY[2][1] = 0;

    FILE *file;
    char buffer[100];
    file = fopen(filename, "r");
    while(fgets(buffer, 99, file) != NULL){
        int segment_number;
        int page_number;
        sscanf( buffer, "%d %d", &segment_number, &page_number);
        doTLBOperations(segment_number, page_number);
        increaseTimeStamp(TLB);
    }

    return 0;
}

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
void doTLBOperations(int segment_number, int page_number){
    int invalid = 0;
    if(segment_number < 0 || segment_number > 2){
        numberOfInvalidReference++;
        invalid = 1;
    }
    else if( page_number > segments[segment_number]-1 || page_number < 0){
        numberOfInvalidReference++;
        invalid = 1;
    }
    if(IsItInPhysicalMemory(MEMORY, segment_number, page_number)){
        numberOfAccess++;
    }
    else if( IsItInTLB(TLB, segment_number, page_number) == 0){
        sleep(TLB_HIT/1000);
        totalDelay = totalDelay + TLB_HIT;
        numberOfAccess++; 
    }
    else{
        if(invalid){
            sleep(TLB_MISS_WITH_PAGE_FAULT/1000);
            totalDelay = totalDelay + TLB_MISS_WITH_PAGE_FAULT;
            numberOfAccess++;
            numberOfTLBMiss++;
            numberOfPageFault++;
        }
        else{
            sleep(TLB_MISS_WITHOUT_PAGE_FAULT/1000);
            totalDelay = totalDelay + TLB_MISS_WITHOUT_PAGE_FAULT;     
            int index = findPlaceInTLB(TLB);
            TLB[index][0] = 0;
            TLB[index][1] = segment_number;
            TLB[index][2] = page_number;
            numberOfTLBMiss++;
            numberOfAccess++;
        }
    }
}

int IsItInPhysicalMemory(int memory[50][2], int segment_number, int page_number){
    for(int i=0; i<50; i++){
        if( memory[i][0] == segment_number && memory[50][1] == page_number){
            return 0;
        }
    }
    return -1;
}
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
void clearTLB(int tlb[50][3]){
    for(int i=0; i<50; i++){
        tlb[i][0] = 0;
        tlb[i][1] = -1;
        tlb[i][2] = -1;
    }
}
void increaseTimeStamp(int tlb[50][3]){
    for(int i=0; i<50; i++){
        if(tlb[i][1] != -1){
            tlb[i][0] = tlb[i][0] + 1;
        }
    }
}

int findPlaceInTLB(int tlb[50][3]){
    for(int i=0; i<50; i++){
        if(tlb[i][1] == -1){
            return i;
        }
    }
    int oldestIndex = findTheOldest(tlb);
    return oldestIndex;
}

int IsItInTLB(int tlb [50][3], int segment_number, int page_number){
    for(int i=0; i<50; i++){
        if( tlb[i][1] == segment_number && tlb[50][2] == page_number){
            return 0;
        }
    }
    return -1;
}
int findTheOldest(int tlb[50][3]){
    int oldestIndex = 0;
    for(int i=0; i<50; i++){
        if(tlb[oldestIndex][0] < tlb[i][0]){
            oldestIndex = i;
        }
    }
    return oldestIndex;
}
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
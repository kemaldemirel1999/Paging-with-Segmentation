#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

int TLB_HIT = 1;
int TLB_MISS_WITH_PAGE_FAULT = 100;
int TLB_MISS_WITHOUT_PAGE_FAULT = 10;

/*
    TLB Replacement policy: FIFO
    Virtual Memory Replacement Policy: LRU Replacement Algorithm
    Segment-page numbers information is sufficient for simulation.
    Every segment has fixed size (1kB) pages.

    Example of 'size' command response:
    text        -> 1691
    data        -> 652
    bss         -> 4
    dec         -> 2347
    hex         -> 92b  
    filename    -> memory-layout
*/

// Runs with byte values

int text;
int bss;
int data;
int physicalFrame[3];

int main(int argc, char *argv[]){
    
    if ( findSegmentInfos(argv[1]) ){
        printf("Executable file NOT FOUND\n");
        return 1;
    }
    int segments[3];
    segments[0] = text/1024;
    if(text % 1024 > 0){
        segments[0] = segments[0] + 1;
    }
    segments[1] = data/1024;
    if(data % 1024 > 0){
        segments[1] = segments[1] + 1;
    }
    segments[2] = bss/1024;
    if(bss % 1024 > 0){
        segments[2] = segments[2] + 1;
    }
    printf("Number of pages in segment[0]:%d\n",segments[0]);
    printf("Number of pages in segment[1]:%d\n",segments[1]);
    printf("Number of pages in segment[2]:%d\n",segments[2]);

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
    
    runSimulaton(argv[2]);
    return 0; 
}

void runSimulaton(char *filename){
    FILE *file;
    char buffer[100];
    file = fopen(filename, "r");
    while(fgets(buffer, 99, file) != NULL){
        int segment_number;
        int page_number;
        printf("%s",buffer);
    }
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
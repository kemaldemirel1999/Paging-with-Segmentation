#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <sched.h>
#include <pthread_impl.h>

int TLB_HIT = 1;
int TLB_MISS_WITH_PAGE_FAULT = 100;
int TLB_MISS_WITHOUT_PAGE_FAULT = 10;

/*
    TLB Replacement policy: FIFO
    Virtual Memory Replacement Policy: LRU Replacement Algorithm
    Segment-page numbers information is sufficient for simulation.
*/

int main(int argc, char *argv[]){
    char *text = argv[1];
    char *data = argv[2];
    char *bss = argv[3];
    char *dec = argv[4];
    char *hex = argv[5];
    char *fileName = argv[6];
    printf("Text:%s\n", text);
    printf("Data:%s\n", data);
    printf("Bss:%s\n", bss);
    printf("Dec:%s\n", dec);
    printf("Hex:%s\n", hex);
    printf("Filename:%s\n", fileName);
    return 0;


    // Number of TLB miss and its rate

    // Number of Page fault and its rate

    // Total Time

    // Number of invalid references
}
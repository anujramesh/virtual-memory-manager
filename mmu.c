#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define FRAME_SIZE 256
#define NUM_OF_FRAMES 256

#define PAGE_SIZE 256
#define NUM_OF_PAGES 256

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256

#define ADDRESS_SIZE 12

int pageTableIndex[PAGE_TABLE_SIZE];
int lastAccessTimes[PAGE_TABLE_SIZE];
int physicalMemory[NUM_OF_FRAMES][FRAME_SIZE];

int tlbHitCount = 0;
int pageFaultCount = 0;
int firstAvailablePageTableNumber = 0;
int firstAvailableFrame = 0;
int frameNum;
char value;
int tlbHit;
int timer = 0;  // time since program started running
int numOfFrames;

char address[ADDRESS_SIZE];
char backingStoreValue[PAGE_SIZE];

FILE *addressFile, *backingStore, *outputFile;


typedef struct key_value_pair {
    int page;
    int frame;
} Key_value_pair;

typedef struct tlb {
    Key_value_pair pairs[TLB_SIZE];
    int tail;
} TLB;

TLB tlb;

int findLRU() {
    int minAccessTime = lastAccessTimes[0];
    int minAccessTimeIndex = 0;
    
    for (int i = 0; i < numOfFrames; i++) {
        if (lastAccessTimes[i] < minAccessTime) {
            minAccessTime = lastAccessTimes[i];
            minAccessTimeIndex = i;
        }
    }
    
    return minAccessTimeIndex;
}

void handlePageFault(int pageNumber) {
    fseek(backingStore, pageNumber * PAGE_SIZE, SEEK_SET);
    fread(backingStoreValue, sizeof(char), PAGE_SIZE, backingStore);

    if (firstAvailablePageTableNumber >= numOfFrames) {
       firstAvailableFrame = findLRU();
    }

    for (int i = 0; i < PAGE_SIZE; i++) {
         physicalMemory[firstAvailableFrame][i] = backingStoreValue[i];
    }

    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (pageTableIndex[i] == firstAvailableFrame) {
            pageTableIndex[i] = -1;
        }
    }

    pageTableIndex[pageNumber] = firstAvailableFrame;

    if (firstAvailableFrame < numOfFrames) {
        firstAvailableFrame++;
    }

    firstAvailablePageTableNumber++;
}

void insertIntoTLB(int pageNumber, int frameNumber) {

    tlb.pairs[tlb.tail].page = pageNumber;
    tlb.pairs[tlb.tail].frame = frameNumber;

    tlb.tail = (tlb.tail + 1) % TLB_SIZE;
}

int checkTLB(int pageNumber) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb.pairs[i].page == pageNumber) {
            frameNum = tlb.pairs[i].frame;
            tlbHitCount++;
            tlbHit = 1;
            return frameNum;
        }
    }

    return -1;
}

void getPage(int logicalAddress) {

    int pageNum = logicalAddress / PAGE_SIZE;
    int offset = logicalAddress % PAGE_SIZE;

    frameNum = -1;
    tlbHit = 0;

    frameNum = checkTLB(pageNum);

    if (frameNum == -1) {   // if TLB miss, check page table

        frameNum = pageTableIndex[pageNum];

        if (frameNum == -1) {   // page fault
            handlePageFault(pageNum);
            pageFaultCount++;
            frameNum = firstAvailableFrame - 1;
        }
    }  

    if (tlbHit == 0) {
        insertIntoTLB(pageNum, frameNum);
    }

    value = physicalMemory[frameNum][offset];
    timer++;
    lastAccessTimes[frameNum] = timer;

    fprintf(outputFile, "%d,%d,%d\n", logicalAddress, (frameNum << 8) | offset, value);
}

int main(int argc, char *argv[]) {
    
    if (strcmp(argv[1], "256") == 0) {
        numOfFrames = 256;
        outputFile = fopen("output256.csv", "w+");
    } else {
        numOfFrames = 128;
        outputFile = fopen("output128.csv", "w+");
    }

    addressFile = fopen(argv[3], "r");
    backingStore = fopen(argv[2], "rb");

    int numOfTranslatedAddresses = 0;
    tlb.tail = 0;

    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        pageTableIndex[i] = -1;
    }

    for (int i = 0; i < TLB_SIZE; i++) {
            tlb.pairs[i].page = -1;
            tlb.pairs[i].frame = -1;
    }

    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        lastAccessTimes[i] = 0;
    }

    while (fgets(address, ADDRESS_SIZE, addressFile) != NULL) {
        ++numOfTranslatedAddresses; 
        int logicalAddress = atoi(address);

        getPage(logicalAddress);      
    }

    double pageFaultRate = (pageFaultCount / (double)numOfTranslatedAddresses) * 100;
    double tlbHitRate = (tlbHitCount / (double)numOfTranslatedAddresses) * 100;

    fprintf(outputFile, "Page Faults Rate, %.2lf%%,\n", pageFaultRate);
    fprintf(outputFile, "TLB Hits Rate, %.2lf%%,", tlbHitRate);

    fclose(addressFile);
    fclose(backingStore);
    fclose(outputFile);
}
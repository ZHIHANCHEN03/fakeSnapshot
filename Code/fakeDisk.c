#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include "btree.h"
#include "metaData.h"

#define LBA_BLOCK_SIZE 4096;

// For constructing the basic logic of bitmap
typedef uint32_t bitmap_t;
enum { BITS_PER_WORD = sizeof(bitmap_t) * CHAR_BIT };
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

void set_bit(bitmap_t *bitmap, int n) { 
    bitmap[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

void clear_bit(bitmap_t *bitmap, int n) {
    bitmap[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n)); 
}

int get_bit(bitmap_t *bitmap, int n) {
    bitmap_t bit = bitmap[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0; 
}

struct fakeDisk
{
    int maxParallelSize;
    long int availableUsage;
    long int totalUsage;
    long int currentUsage;
    int currentSnapshotVer;
    FILE * fp;
    struct btree * tr;
    int * bitmapForUser;
    int * bitmapForWhole;
};

struct fakeDisk * create_fakeDisk( char * aFakeDiskName, long int aAvailableUsage, long int aTotalUsage ) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    struct fakeDisk * newFakeDisk = (struct fakeDisk * ) malloc( sizeof( struct fakeDisk ) );
    if (newFakeDisk) {
        newFakeDisk->maxParallelSize = 100;
        newFakeDisk->availableUsage = aAvailableUsage;
        newFakeDisk->totalUsage = aTotalUsage;
        newFakeDisk->currentUsage = 0;
        newFakeDisk->currentSnapshotVer = 1;
        newFakeDisk->fp = fopen( aFakeDiskName, "r+" );
        newFakeDisk->tr = btree_new(sizeof(struct metaData), 0, metaData_compare, NULL);
        newFakeDisk->bitmapForUser = malloc(floor((float) newFakeDisk->availableUsage / LBABlockSize));
        newFakeDisk->bitmapForWhole = malloc(floor((float) newFakeDisk->totalUsage / LBABlockSize));
    }
    return newFakeDisk;    
}

// find the single empty offset based on the whole disk range
int findASingalEmptyOffset(struct fakeDisk * fakeDisk) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    int * bitmap = fakeDisk->bitmapForWhole;
    int totalNumOfRealBlocks = fakeDisk->totalUsage / LBABlockSize;
    for (int i = 0; i < totalNumOfRealBlocks; i++) {
        if (get_bit(bitmap, i) == 0) {
            set_bit(bitmap, i);
            return i * LBABlockSize;
        }
    }
    return -1;
}

// find the several empty offsets based on user disk range,
// then call findAsingalEmptyOffset for finding the real disk block for writing
int * findEmptyOffsets(struct fakeDisk * fakeDisk, int requiredNumOfBlock) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    int * bitmap = fakeDisk->bitmapForUser;
    int totalNumOfRealBlocks = fakeDisk->totalUsage / LBABlockSize;
    int offsets[requiredNumOfBlock];
    
    int u = 0;
    for (int i = 0; i < requiredNumOfBlock; i++) {
        if (get_bit(bitmap, i) == 0) {
            set_bit(bitmap, i);
            offsets[u] = i * LBABlockSize;
            if (u == requiredNumOfBlock) {
                return offsets;
            }
        }
    }
    return NULL;
}

// only delete the data in the real offset (in the real disk range)
void lightReinitializeData(FILE * fp, long int realOffset) {

}

long int getUsage(struct fakeDisk * fakeDisk) {
    return fakeDisk->currentUsage;
}

long int getLeftSpace(struct fakeDisk * fakeDisk) {
    return fakeDisk->availableUsage - fakeDisk->currentUsage;
}

int getCurrentSnapshot(struct fakeDisk * fakeDisk) {
    return fakeDisk->currentSnapshotVer;
}

// write the data into the disk
// allcate the area for the whole disk location for real writing based on the designed Offset (in the user disk range)
void writeData(struct fakeDisk * fakeDisk, long int designedOffset, long int writeBytes, char * bufferPointer) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    if (designedOffset%LBABlockSize != 0 || designedOffset >= fakeDisk->totalUsage) {
        printf("Invalid Disk Offset\n");
        return;
    }

    struct btree * tr = fakeDisk->tr;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;
    FILE * fp = fakeDisk->fp;
    // char buffer[4096] = *bufferPointer;

    struct metaData * metaData;
    metaData = btree_get(tr, &(struct metaData){ .fakeOffset=designedOffset, .snapshotVer=currentSnapshotVer });
    if (!(metaData && metaData->snapshotVer == currentSnapshotVer)) {
        int newWriteRealOffset = findASingalEmptyOffset(fakeDisk);
        btree_set(tr, &(struct metaData){ .fakeOffset=designedOffset, .realOffset=newWriteRealOffset, .snapshotVer=currentSnapshotVer });
        fseek(fp, newWriteRealOffset, SEEK_SET);
        fwrite(*bufferPointer, sizeof(*bufferPointer), sizeof(char), fp);
        fakeDisk->currentUsage = fakeDisk->currentUsage + LBA_BLOCK_SIZE;
    } else {
        int realOffset = metaData->realOffset;
        lightReinitializeData(fp, realOffset);
        fseek(fp, realOffset, SEEK_SET);
        fwrite(*bufferPointer, sizeof(*bufferPointer), sizeof(char), fp);
    }

    free(tr);
    free(fp);
}

// write the data into the disk
// first allocate the area for the user disk location (in the user disk range)
// second allcate the area for the whole disk location for real writing
void randomWriteData(struct fakeDisk * fakeDisk, long int writeBytes, char * bufferPointer) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    int requiredLBAs = floor((float) writeBytes / LBABlockSize);
    int * writeBlocks = findEmptyOffsets(fakeDisk, requiredLBAs);
    for (int i = 0; i < requiredLBAs; i++) {
        writeData(fakeDisk, writeBlocks[i], LBABlockSize, bufferPointer+(i*LBABlockSize));
    }
    fakeDisk->currentUsage = fakeDisk->currentUsage + requiredLBAs * LBABlockSize;

    free(writeBlocks);
}

// delete data from the disk
// remove the data from the whole disk location based on the designed Offset (in the user disk range)
void deleteData(struct fakeDisk * fakeDisk, long int designedOffset) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    if (designedOffset%LBABlockSize != 0 || designedOffset >= fakeDisk->totalUsage) {
        printf("Invalid Disk Offset\n");
        return;
    }

    struct btree * tr = fakeDisk->tr;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;
    FILE * fp = fakeDisk->fp;

    struct metaData * metaData;
    metaData = btree_get(tr, &(struct metaData){ .fakeOffset=designedOffset, .snapshotVer=currentSnapshotVer });
    if (metaData && metaData->snapshotVer == currentSnapshotVer) {
        int realOffset = metaData->realOffset;
        lightReinitializeData(fp, realOffset);
        fseek(fp, realOffset, SEEK_SET);
        fakeDisk->currentUsage = fakeDisk->currentUsage - LBA_BLOCK_SIZE;
        btree_delete(tr, &(struct metaData){ .fakeOffset=designedOffset, .snapshotVer=currentSnapshotVer });
    }

    free(tr);
    free(fp);
}

void deleteSnapshotVer(struct fakeDisk * fakeDisk, int snapshotVer) {
    
}

void instantSnapshot(struct fakeDisk * fakeDisk) {
    fakeDisk->currentSnapshotVer = fakeDisk->currentSnapshotVer++;
}

void freeFakeDisk(struct fakeDisk * fakeDisk) {
    fclose(fakeDisk->fp);
    free(fakeDisk->fp);
    btree_free(fakeDisk->tr);
    free(fakeDisk->tr);
}


int main() {
    struct fakeDisk * newFakeDisk = create_fakeDisk("./data/fakeDisk.dat", 123123, 123123123);
    printf("your fakeDisk available usage is %ld\n", getUsage(newFakeDisk));
}

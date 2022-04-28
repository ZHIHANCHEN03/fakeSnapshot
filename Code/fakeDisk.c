#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "btree.h"
#include "metaData.h"

#define LBA_BLOCK_SIZE 4096;

int findASingalEmptyOffset() {
    return 1;
}

int * findEmptyOffsets(int requiredNumOfBlock) {
    int offsets[requiredNumOfBlock];
    return offsets;
}

void reInitializeData(FILE * fp) {

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
};

struct fakeDisk * create_fakeDisk( char * aFakeDiskName, long int aAvailableUsage, long int aTotalUsage ) {
    struct fakeDisk * newFakeDisk = (struct fakeDisk * ) malloc( sizeof( struct fakeDisk ) );
    if (newFakeDisk) {
        newFakeDisk->maxParallelSize = 100;
        newFakeDisk->availableUsage = aAvailableUsage;
        newFakeDisk->totalUsage = aTotalUsage;
        newFakeDisk->currentUsage = 0;
        newFakeDisk->currentSnapshotVer = 1;
        newFakeDisk->fp = fopen( aFakeDiskName, "r+" );
        newFakeDisk->tr = btree_new(sizeof(struct metaData), 0, metaData_compare, NULL);
    }
    return newFakeDisk;    
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

void randomWriteData(struct fakeDisk * fakeDisk, long int writeBytes, char * bufferPointer) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    int requiredLBAs = floor((float) writeBytes / LBABlockSize);
    int * writeBlocks = findEmptyOffsets(requiredLBAs);
    for (int i = 0; i < requiredLBAs; i++) {
        writeData(fakeDisk, writeBlocks[i], LBABlockSize, bufferPointer+(i*LBABlockSize));
    }
    fakeDisk->currentUsage = fakeDisk->currentUsage + requiredLBAs * LBABlockSize;

    free(writeBlocks);
}

void writeData(struct fakeDisk * fakeDisk, long int designedOffset, long int writeBytes, char * bufferPointer) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    if (designedOffset%LBABlockSize != 0 || designedOffset >= fakeDisk->totalUsage) {
        return printf("Invalid Disk Offset\n");
    }

    struct btree * tr = fakeDisk->tr;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;
    FILE * fp = fakeDisk->fp;
    char buffer[] = *bufferPointer;

    struct metaData * metaData;
    metaData = btree_get(tr, &(struct metaData){ .fakeOffset=designedOffset, .snapshotVer=currentSnapshotVer });
    if (!(metaData && metaData->snapshotVer == currentSnapshotVer)) {
        int newWriteRealOffset = findASingalEmptyOffset();
        btree_set(tr, &(struct metaData){ .fakeOffset=designedOffset, .realOffset=newWriteRealOffset, .snapshotVer=currentSnapshotVer });
        fseek(fp, newWriteRealOffset, SEEK_SET);
        fwrite(buffer, sizeof(char), sizeof(buffer), fp);
        fakeDisk->currentUsage = fakeDisk->currentUsage + LBA_BLOCK_SIZE;
    } else {
        int realOffset = metaData->realOffset;
        fseek(fp, realOffset, SEEK_SET);
        reInitializeData(fp);
        fwrite(buffer, sizeof(char), sizeof(buffer), fp);
    }

    free(tr);
    free(fp);
}

void deleteData(struct fakeDisk * fakeDisk, long int designedOffset) {
    int LBABlockSize = LBA_BLOCK_SIZE;
    if (designedOffset%LBABlockSize != 0 || designedOffset >= fakeDisk->totalUsage) {
        return printf("Invalid Disk Offset\n");
    }

    struct btree * tr = fakeDisk->tr;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;
    FILE * fp = fakeDisk->fp;

    struct metaData * metaData;
    metaData = btree_get(tr, &(struct metaData){ .fakeOffset=designedOffset, .snapshotVer=currentSnapshotVer });
    if (metaData && metaData->snapshotVer == currentSnapshotVer) {
        int realOffset = metaData->realOffset;
        fseek(fp, realOffset, SEEK_SET);
        reInitializeData(fp);
        fakeDisk->currentUsage = fakeDisk->currentUsage - LBA_BLOCK_SIZE;
        btree_delete(tr, &(struct metaData){ .fakeOffset=designedOffset, .snapshotVer=currentSnapshotVer });
    }

    free(tr);
    free(fp);
}

void rollbackToSnapshotVer(struct fakeDisk * fakeDisk, int snapshotVer) {

}

void deleteSnapshotVer(struct fakeDisk * fakeDisk, int snapshotVer) {

}

void instantSnapshot(struct fakeDisk * fakeDisk) {
    fakeDisk->currentSnapshotVer = fakeDisk->currentSnapshotVer++;
}

void freeFakeDisk(struct fakeDisk * fakeDisk) {
    close(fakeDisk->fp);
    free(fakeDisk->fp);
    btree_free(fakeDisk->tr);
    free(fakeDisk->tr);
}

int main() {
    struct fakeDisk * newFakeDisk = create_fakeDisk("./data/fakeDisk.dat", 123123, 123123123);
    printf("your fakeDisk available usage is %ld\n", getUsage(newFakeDisk));
}

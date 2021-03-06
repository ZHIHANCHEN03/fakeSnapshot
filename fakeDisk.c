#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include "btree.h"
#include "metaData.h"
#include "bit_array.h"

#define LBA_BLOCK_SIZE 4096;

struct fakeDisk
{
    int maxParallelSize;
    long int availableUsage;
    long int totalUsage;
    long int currentUsage;
    int currentSnapshotVer;
    FILE *fp;
    struct btree *tr;
    BIT_ARRAY *bitmapForUser;
    BIT_ARRAY *bitmapForReal;
};

struct fakeDisk * create_fakeDisk(char *aFakeDiskName, long int aAvailableUsage, long int aTotalUsage)
{
    int LBABlockSize = LBA_BLOCK_SIZE;
    struct fakeDisk *newFakeDisk = (struct fakeDisk *)malloc(sizeof(struct fakeDisk));
    if (newFakeDisk)
    {
        newFakeDisk->maxParallelSize = 100;
        newFakeDisk->availableUsage = aAvailableUsage;
        newFakeDisk->totalUsage = aTotalUsage;
        newFakeDisk->currentUsage = 0;
        newFakeDisk->currentSnapshotVer = 1;
        newFakeDisk->fp = fopen(aFakeDiskName, "r+");
        newFakeDisk->tr = btree_new(sizeof(struct metaData), 0, metaData_compare, NULL);
        newFakeDisk->bitmapForUser = bit_array_create(floor((float)aAvailableUsage / LBABlockSize));
        newFakeDisk->bitmapForReal = bit_array_create(floor((float)aTotalUsage / LBABlockSize));
    }
    return newFakeDisk;
}

// find the single empty offset based on the user disk range
int findSingalEmptyOffsetForUser(struct fakeDisk *fakeDisk)
{
    int LBABlockSize = LBA_BLOCK_SIZE;
    BIT_ARRAY *BIT_MAP = fakeDisk->bitmapForUser;
    bit_index_t result = -1;
    bit_array_find_first_set_bit(BIT_MAP, &result);
    return (int)result * LBABlockSize;
}

// find the single empty offset based on the user disk range
int findSingalEmptyOffsetForDisk(struct fakeDisk *fakeDisk)
{
    int LBABlockSize = LBA_BLOCK_SIZE;
    BIT_ARRAY *BIT_MAP = fakeDisk->bitmapForReal;
    bit_index_t result = -1;
    bit_array_find_first_set_bit(BIT_MAP, &result);
    return (int)result * LBABlockSize;
}

// only delete the data in the real offset (in the real disk range)
void lightReinitializeData(FILE *fp, long int realOffset)
{
}

long int getUsage(struct fakeDisk *fakeDisk)
{
    return fakeDisk->currentUsage;
}

long int getLeftSpace(struct fakeDisk *fakeDisk)
{
    return fakeDisk->availableUsage - fakeDisk->currentUsage;
}

int getCurrentSnapshot(struct fakeDisk *fakeDisk)
{
    return fakeDisk->currentSnapshotVer;
}

// write the data into the disk
// allcate the area in real disk range for real writing based on the designed Offset (in the user disk range)
void writeData(struct fakeDisk *fakeDisk, long int designedOffset, long int writeBytes, char *bufferPointer)
{
    int LBABlockSize = LBA_BLOCK_SIZE;
    if (designedOffset % LBABlockSize != 0 || designedOffset >= fakeDisk->totalUsage)
    {
        printf("Invalid Disk Offset\n");
        return;
    }

    struct btree *tr = fakeDisk->tr;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;
    FILE *fp = fakeDisk->fp;

    struct metaData *metaData;
    metaData = btree_get(tr, &(struct metaData){.fakeOffset = designedOffset, .snapshotVer = currentSnapshotVer});
    if (!(metaData && metaData->snapshotVer == currentSnapshotVer))
    {
        int newWriteRealOffset = findSingalEmptyOffsetForDisk(fakeDisk);
        if (newWriteRealOffset < 0)
        {
            printf("No Enough Space For New Writing: Real");
            free(tr);
            free(fp);
            return;
        }
        btree_set(tr, &(struct metaData){.fakeOffset = designedOffset, .realOffset = newWriteRealOffset, .snapshotVer = currentSnapshotVer});
        fseek(fp, newWriteRealOffset, SEEK_SET);
        fwrite(*bufferPointer, sizeof(*bufferPointer), sizeof(char), fp);
        fakeDisk->currentUsage = fakeDisk->currentUsage + LBA_BLOCK_SIZE;
    }
    else
    {
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
void randomWriteData(struct fakeDisk *fakeDisk, long int writeBytes, char *bufferPointer)
{
    int LBABlockSize = LBA_BLOCK_SIZE;
    int requiredLBAs = floor((float)writeBytes / LBABlockSize);

    for (int i = 0; i < requiredLBAs; i++)
    {
        int newWriteUserOffset = findSingalEmptyOffsetForUser(fakeDisk);
        if (newWriteUserOffset >= 0)
        {
            writeData(fakeDisk, newWriteUserOffset, LBABlockSize, bufferPointer + (i * LBABlockSize));
        }
        else
        {
            printf("No Enough Space For New Writing: User");
            break;
        }
    }
}

// delete data from the disk
// remove the data from the whole disk location based on the designed Offset (in the user disk range)
void deleteData(struct fakeDisk *fakeDisk, long int designedOffset)
{
    int LBABlockSize = LBA_BLOCK_SIZE;
    if (designedOffset % LBABlockSize != 0 || designedOffset >= fakeDisk->totalUsage)
    {
        printf("Invalid Disk Offset\n");
        return;
    }

    struct btree *tr = fakeDisk->tr;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;
    FILE *fp = fakeDisk->fp;

    struct metaData *metaData;
    metaData = btree_get(tr, &(struct metaData){.fakeOffset = designedOffset, .snapshotVer = currentSnapshotVer});
    if (metaData)
    {
        int realOffset = metaData->realOffset;
        lightReinitializeData(fp, realOffset);
        fseek(fp, realOffset, SEEK_SET);
        fakeDisk->currentUsage = fakeDisk->currentUsage - LBA_BLOCK_SIZE;
        btree_delete(tr, &(struct metaData){.fakeOffset = designedOffset, .snapshotVer = currentSnapshotVer});
    }

    free(tr);
    free(fp);
}

// read the data from the designedOffset (in the range of user disk)
char * readData(struct fakeDisk *fakeDisk, long int designedOffset)
{
    int LBABlockSize = LBA_BLOCK_SIZE;
    if (designedOffset % LBABlockSize != 0 || designedOffset >= fakeDisk->totalUsage)
    {
        printf("Invalid Disk Offset\n");
        return;
    }

    char buffer[LBABlockSize];
    struct btree *tr = fakeDisk->tr;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;
    FILE *fp = fakeDisk->fp;

    struct metaData *metaData;
    metaData = btree_get(tr, &(struct metaData){.fakeOffset = designedOffset, .snapshotVer = currentSnapshotVer});
    if (metaData)
    {
        int realOffset = metaData->realOffset;
        fseek(fp, realOffset, SEEK_SET);
        fread(buffer, LBABlockSize, sizeof(char), fp);
    }

    free(tr);
    free(fp);
    return buffer;
}

void rollbackToSnapshotVer(struct fakeDisk *fakeDisk, int designedSnapshotVer)
{
    if (designedSnapshotVer >= fakeDisk->currentSnapshotVer)
    {
        printf("Invalid Snapshot Ver, Max Snapshot Ver Should Be Less Than %d", fakeDisk->currentSnapshotVer);
        return;
    }

    struct btree *tr = fakeDisk->tr;
    BIT_ARRAY *bitarr = fakeDisk->bitmapForUser;
    FILE *fp = fakeDisk->fp;
    int currentSnapshotVer = fakeDisk->currentSnapshotVer;

    int currentFirstOffsetForUserDisk;
    bit_array_find_first_set_bit(bitarr, &currentFirstOffsetForUserDisk);
    int currentLastOffsetForUserDisk;
    bit_array_find_last_set_bit(bitarr, &currentLastOffsetForUserDisk);

    int currentReadingOffsetForUserDisk = currentFirstOffsetForUserDisk;
    while (true)
    {

        for (int i = designedSnapshotVer + 1; i < currentSnapshotVer; i++)
        {
            struct metaData *metaData;
            metaData = btree_get(tr, &(struct metaData){.fakeOffset = currentReadingOffsetForUserDisk, .snapshotVer = i});
            if (metaData)
            {
                int realOffset = metaData->realOffset;
                lightReinitializeData(fp, realOffset);
                fseek(fp, realOffset, SEEK_SET);
                fakeDisk->currentUsage = fakeDisk->currentUsage - LBA_BLOCK_SIZE;
                btree_delete(tr, &(struct metaData){.fakeOffset = currentReadingOffsetForUserDisk, .snapshotVer = i});
            }
        }

        if (currentReadingOffsetForUserDisk == currentLastOffsetForUserDisk)
        {
            break;
        }

        bit_array_find_next_set_bit(bitarr, currentReadingOffsetForUserDisk + 1, &currentReadingOffsetForUserDisk);
    }

    free(tr);
    free(bitarr);
    free(fp);
}

void instantSnapshot(struct fakeDisk *fakeDisk)
{
    fakeDisk->currentSnapshotVer = fakeDisk->currentSnapshotVer++;
}

void freeFakeDisk(struct fakeDisk *fakeDisk)
{
    fclose(fakeDisk->fp);
    free(fakeDisk->fp);
    btree_free(fakeDisk->tr);
    free(fakeDisk->tr);
    bit_array_free(fakeDisk->bitmapForUser);
    free(fakeDisk->bitmapForUser);
    bit_array_free(fakeDisk->bitmapForReal);
    free(fakeDisk->bitmapForReal);
}

int main()
{
    struct fakeDisk *newFakeDisk = create_fakeDisk("./data/fakeDisk.dat", 123123, 123123123);
    printf("your fakeDisk available usage is %ld\n", getUsage(newFakeDisk));
}

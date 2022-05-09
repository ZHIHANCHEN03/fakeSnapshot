#include <stdio.h>
#include "btree.h"
#include "bit_array.h"


#ifndef FAKEDISK_H
#define FAKEDISK_H

struct fakeDisk
{
    int maxParallelSize;
    long int availableUsage;
    long int totalUsage;
    long int currentUsage;
    int currentSnapshotVer;
    FILE * fp;
    struct btree * tr;
    BIT_ARRAY * bitmapForUser;
    BIT_ARRAY * bitmapForReal;
};

struct fakeDisk *fakeDisk(const char *aFakeDiskName, long int aAvailableUsage, long int aTotalUsage);
long int getUseage(struct fakeDisk *fakeDisk);
long int getLeftSpace(struct fakeDisk *fakeDisk);
char * getCurrentSnapshot(struct fakeDisk *fakeDisk);

void randomWriteData(struct fakeDisk *fakeDisk, long int writeBytes, char *buffer);
void writeData(struct fakeDisk *fakeDisk, long int designedOffset, long int writeBytes, char *buffer);
void deleteData(struct fakeDisk *fakeDisk, long int currentOffset, long int deleteBytes);
void rollbackToSnapshotVer(struct fakeDisk *fakeDisk, long int currentOffset, const char *versionName);

void instantSnapshot(struct fakeDisk *fakeDisk);

#endif
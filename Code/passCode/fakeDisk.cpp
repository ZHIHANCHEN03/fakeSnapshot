#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "fakeDisk.h"
#include "btree.h"

struct metaData {
    char * fakeOffset;
    char * realOffset;
    char * snapshotVer;
};

int metaData_compare(const void *a, const void *b, void *udata) {
    const struct metaData *ua = (struct metaData *) a;
    const struct metaData *ub = (struct metaData *) b;
    int cmp = strcmp(ua->fakeOffset, ub->fakeOffset);
    if (cmp == 0) {
        cmp = strcmp(ua->snapshotVer, ub->snapshotVer);
    }
    return cmp;
}

bool metaData_iter(const void *a, void *udata) {
    const struct metaData *metaData = (struct metaData *) a;
    printf("fakeOffset: %s -- realOffset: %s (snapshotVer=%s)\n", metaData->fakeOffset, metaData->realOffset, metaData->snapshotVer);
    return true;
}

FakeDisk::FakeDisk(const char * aFakeDiskName, long int aAvailableUsage, long int aTotalUsage) {
    maxParallelSize = 100;
    availableUsage = aAvailableUsage;
    totalUsage = aTotalUsage;
    strcpy(currentSnapshotVer, "1");
    fp = fopen( aFakeDiskName, "r+");
    struct btree *tr = btree_new(sizeof(struct metaData), 0, metaData_compare, NULL);
}

long int FakeDisk::getUseage() const {
    return currentUsage;
}

long int FakeDisk::getLeftSpace() const {
    return availableUsage - currentUsage;
}

char * FakeDisk::getCurrentSnapshot() const {
    return currentSnapshotVer;
}

vector<char *> FakeDisk::getSnapshotVer() const {
    return snapshotVer;
}

void FakeDisk::randomWriteData(long int writeBytes, char * buffer) {

}

void FakeDisk::writeData(long int designedOffset, long int writeBytes, char * buffer) {

}

void FakeDisk::deleteData(long int designedOffset, long int deleteBytes) {

}

void FakeDisk::rollbackToSnapshotVer(const char * versionName) {

}

void FakeDisk::deleteSnapshotVer(const char * versionName) {

}

void FakeDisk::instantSnapshot() {

}

long int FakeDisk::offsetAddress(char * fakeOffset, char * snapshotVer) {
    // struct metaData * metaData;

    // if (! btree_get(tr, &(struct metaData){ .fakeOffset=fakeOffset, .snapshotVer=snapshotVer }) ) {
    //     metaData = (struct metaData *) btree_get(tr, &(struct metaData){ .fakeOffset=fakeOffset, .snapshotVer=snapshotVer });
        
    // }

    btree_set(tr, &(struct metaData){ .fakeOffset="Dale", .realOffset="Murphy", .snapshotVer="433" });
}

int main() {
    return 0;
}
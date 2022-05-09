#include <stdio.h>
#include <string.h>
#include "btree.h"

struct metaData {
    long int fakeOffset;
    long int realOffset;
    int snapshotVer;
};

int metaData_compare(const void *a, const void *b, void *udata) {
    const struct metaData *ua = (struct metaData *) a;
    const struct metaData *ub = (struct metaData *) b;
    int cmp = ua->fakeOffset - ub->fakeOffset;
    if (cmp == 0) {
        cmp = ua->snapshotVer - ub->snapshotVer;
    }
    return cmp;
}

bool metaData_iter(const void *a, void *udata) {
    const struct metaData *metaData = (struct metaData *) a;
    printf("fakeOffset: %ld -- realOffset: %ld (snapshotVer=%s)\n", metaData->fakeOffset, metaData->realOffset, metaData->snapshotVer);
    return true;
}
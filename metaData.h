#include <stdbool.h>

#ifndef METADATA_H
#define METADATA_H

struct metaData {
    long int fakeOffset;
    long int realOffset;
    int snapshotVer;
};

int metaData_compare(const void *a, const void *b, void *udata);
bool metaData_iter(const void *a, void *udata);

#endif
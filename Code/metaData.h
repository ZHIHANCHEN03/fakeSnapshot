#include <stdbool.h>

#ifndef METADATA_H
#define METADATA_H

struct metaData {
    char * fakeOffset;
    char * realOffset;
    char * snapshotVer;
};

int metaData_compare(const void *a, const void *b, void *udata);
bool metaData_iter(const void *a, void *udata);

#endif
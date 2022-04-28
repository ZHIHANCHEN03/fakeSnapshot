#include <stdio.h>
#include <string.h>
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

int main() {
    // create a new btree where each item is a `struct metaData`. 
    struct btree *tr = btree_new(sizeof(struct metaData), 0, metaData_compare, NULL);

    // load some metaDatas into the btree. Each set operation performas a copy of 
    // the data that is pointed to in the second argument.
    btree_set(tr, &(struct metaData){ .fakeOffset="0dx12", .realOffset="0dx14", .snapshotVer="44" });
    btree_set(tr, &(struct metaData){ .fakeOffset="0dx12", .realOffset="0dx15", .snapshotVer="68" });
    btree_set(tr, &(struct metaData){ .fakeOffset="0dx13", .realOffset="0dx144", .snapshotVer="47" });

    struct metaData *metaData; 
    
    printf("\n-- get some metaDatas --\n");
    metaData = btree_get(tr, &(struct metaData){ .fakeOffset="0dx12", .snapshotVer="44" });
    printf("%s snapshotVer=%s\n", metaData->fakeOffset, metaData->snapshotVer);

    metaData = btree_get(tr, &(struct metaData){ .fakeOffset="0dx12", .snapshotVer="68" });
    printf("%s snapshotVer=%s\n", metaData->fakeOffset, metaData->snapshotVer);

    metaData = btree_get(tr, &(struct metaData){ .fakeOffset="0dx13", .snapshotVer="47" });
    printf("%s snapshotVer=%s\n", metaData->fakeOffset, metaData->snapshotVer);

    printf("\n-- iterate over all metaDatas --\n");
    btree_ascend(tr, NULL, metaData_iter, NULL);

    printf("\n-- iterate beginning with realOffset name `0dx13` --\n");
    btree_ascend(tr, &(struct metaData){.fakeOffset="0dx13",.snapshotVer=""}, metaData_iter, NULL);

    btree_free(tr);
}
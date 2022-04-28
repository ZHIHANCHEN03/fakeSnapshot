#include <vector>
#include "btree.h"

using namespace std;

struct metaData;

int metaData_compare(const void *a, const void *b, void *udata);
bool metaData_iter(const void *a, void *udata);

class FakeDisk {
public:
    FakeDisk(const char * aFakeDiskName, long int aAvailableUsage, long int aTotalUsage);

    // ACCESSORS
    long int getUseage() const;
    long int getLeftSpace() const;
    char * getCurrentSnapshot() const;
    vector<char *> getSnapshotVer() const;

    // MODIFIERS
    void randomWriteData(long int writeBytes, char * buffer);
    void writeData(long int designedOffset, long int writeBytes, char * buffer);
    void deleteData(long int currentOffset, long int deleteBytes);
    void rollbackToSnapshotVer(const char * versionName);
    void deleteSnapshotVer(const char * versionName);

    // OTHERS
    void instantSnapshot();

private:
    FILE * fp;
    long int availableUsage;
    long int totalUsage;
    long int currentUsage;
    int maxParallelSize;
    char * currentSnapshotVer;
    vector<char *> snapshotVer;
    struct btree *tr;
    
    long int offsetAddress(char * fakeOffset, char * snapshotVer );
};
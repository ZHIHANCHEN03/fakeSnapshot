**FakeSnapshot**
==================

https://github.com/ZHIHANCHEN03/fakeSnapshot/  
License: Public Domain, no warranty

About
==================

A fake snapshot system based on B+tree and Bitmap, which imitate the snapshot system in Linux in the pro-visioning mode.

Using fakesnapshot in your code
==================

I am happy to see that your involve fakeSnapshot in your own code. Add to the top of your code:

    #include "fakeDisk.h"

Basic
-----

Constructor - create a new fake disk for CRUD with snapshot

    struct fakeDisk *create_fakeDisk(char *aFakeDiskName, long int aAvailableUsage, long int aTotalUsage)

Destructor - free the memory used for a fake disk

    void instantSnapshot(struct fakeDisk *fakeDisk)

Basic Static Info
-----------------

get the current useage of the disk

    long int getUseage(struct fakeDisk *fakeDisk)

get the value of the free space the disk

    long int getLeftSpace(struct fakeDisk *fakeDisk)

get the value of the current snapshot ver

    char * getCurrentSnapshot(struct fakeDisk *fakeDisk)

CRUD
----

write data to the disk with designed offset (location) - you should already include a data.dat, data.bin or data.txt file with designed size under the folder named data 

    void writeData(struct fakeDisk *fakeDisk, long int designedOffset, long int writeBytes, char *buffer)

write data to the disk without designed offset (location), allow the size of data more than the size of one LBA block (4kb)

    void randomWriteData(struct fakeDisk *fakeDisk, long int writeBytes, char *buffer)

delete data from the disk with designed offset (location)

    void deleteData(struct fakeDisk *fakeDisk, long int currentOffset, long int deleteBytes)

read data from the disk with designed offset (location)

    char * readData(struct fakeDisk *fakeDisk, long int designedOffset)

Snapshot Verisoning
-------------------

roll back to the snapshot ver (snapshot ver should be less than the current version)

    void rollbackToSnapshotVer(struct fakeDisk *fakeDisk, long int currentOffset, const char *versionName)

instantly make a snapshot (update the current snapshot ver)

    void instantSnapshot(struct fakeDisk *fakeDisk)

Reference
=========

BitArray
--------
https://github.com/noporpoise/BitArray

btree.c
-------
https://github.com/tidwall/btree.c

License
=======

This software is in the *Public Domain*. That means you can do whatever you like
with it. That includes being used in proprietary products without attribution or
restrictions. There are no warranties and there may be bugs. 

Formally we are using CC0 - a Creative Commons license to place this work in the
public domain. A copy of CC0 is in the LICENSE file. 

    "CC0 is a public domain dedication from Creative Commons. A work released
    under CC0 is dedicated to the public domain to the fullest extent permitted
    by law. If that is not possible for any reason, CC0 also provides a lax,
    permissive license as a fallback. Both public domain works and the lax
    license provided by CC0 are compatible with the GNU GPL."
      - http://www.gnu.org/licenses/license-list.html#CC0



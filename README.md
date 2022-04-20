# FakeSnapshot.c

A fake snapshot system based on B+tree, which imitate the snapshot system in Linux.

## Features

- Users could customize the size for storing information and snapshot pool
- Based on efficien B+tree

## Steps to realize the project

- A interface for users to customize the disk size - main.c
- Fake writing and delete - main.c
- Provide users with imperceptible disk area access - operation_WRD.c and btree.c
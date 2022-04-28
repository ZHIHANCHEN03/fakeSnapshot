#include <stdio.h>
#include <string.h>
#pragma warning(disable:4996)

typedef struct stu {
    int num;
    char name[20];
    char *sex;
} STU;

int main() {
    struct stu bob = {
        121,
        "boy"
    };
    bob.num = 131;
    strcpy(bob.name, "bob");
    struct stu lucy = {
        101,
        "lucy",
        "male"
    };
    STU edu[3] = {
        {
            101,
            "lucy",
            "male"
        },
        {
            101,
            "lucy",
            "male"
        },
        {
            101,
            "lucy",
            "male"
        }
    };

    struct stu boy = {101, "lucy", "male"};
    const STU *stuPointer = &boy;
    printf("boy's name is %s\n", stuPointer->name);

    return 0;
}
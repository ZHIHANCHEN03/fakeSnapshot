#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

typedef struct stu {
    int num;
    char name[20];
    float score;
} STU;

void fun (STU *p) {
    p->num = 101;
    strcpy(p->name, "lucy");
    p->score = 123.2f;
}

int main() {
    STU girl;
    fun(&girl);
    printf("%d, %s, %f\n", girl.num, girl.name, girl. score);
}
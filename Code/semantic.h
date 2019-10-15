#ifndef __SEMACTIC_H_
#define __SEMACTIC_H_
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#define HASH_SIZE 16384
typedef struct Symbol {
    char* name;
    struct Symbol* next;
} Symbol;

typedef struct Bucket {
    Symbol* head;
} Bucket;

typedef struct HashSet {
    int size;
    Bucket* buckets;
} HashSet;

HashSet* initializeHashSet(int size);
unsigned int pjwHash(char* str);
bool isContain(HashSet* hashSet, char* name);
bool insert(HashSet* hashSet, Symbol* symbol);
Symbol* get(HashSet* hashSet, char* name);
Symbol* createSymbol(char* name);
#endif
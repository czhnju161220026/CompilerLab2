#include "hashset.h"

HashSet* initializeHashSet(int size) {
    HashSet* hashSet = (HashSet*) malloc(sizeof(HashSet));
    if (hashSet == NULL) {
        return NULL;
    }
    hashSet->size = size;
    hashSet->buckets = (Bucket*) malloc(sizeof(Bucket) * size);
    for (int i = 0; i < size; i++) {
        hashSet->buckets[i].head = NULL;
    }

    return hashSet;
}

unsigned int pjwHash(char* str) {
    str = (unsigned char*) str;
    unsigned int val = 0;
    unsigned int i;
    for (; *str; ++str) {
        val = (val << 2) + *str;
        if (i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }

    return val;
}

bool isContain(HashSet* hashSet, char* name) {
    if (hashSet == NULL) {
        printf("the hash set is NULL\n");
        return false;
    }

    unsigned int val = pjwHash(name) % HASH_SIZE;
    Symbol* head = hashSet->buckets[val].head;
    while (head != NULL) {
        if (strcmp(name, head->name) == 0) {
            return true;
        }
        head = head->next;
    }

    return false;
}

Symbol* get(HashSet* hashSet, char* name) {
    if (hashSet == NULL) {
        printf("the hash set is NULL\n");
        return NULL;
    }

    unsigned int val = pjwHash(name) % HASH_SIZE;
    Symbol* head = hashSet->buckets[val].head;
    while (head != NULL) {
        if (strcmp(name, head->name) == 0) {
            return head;
        }
        head = head->next;
    }

    return NULL;
}

bool insert(HashSet* hashSet, Symbol* symbol) {
    if (isContain(hashSet, symbol->name)) {
        return false;
    } else {
        unsigned int val = pjwHash(symbol->name) % HASH_SIZE;
        symbol->next = hashSet->buckets[val].head;
        hashSet->buckets[val].head = symbol;
        return true;
    }
}

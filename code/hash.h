#if !defined(HASH_H)

#define HASH_H

#include <stdint.h>
#include <string.h>
#include "da.h"

typedef uint32_t u32;
typedef int32_t s32;
typedef s32 bool32;
typedef double Value;

#define internal static

struct string
{
    size_t len;
    char *chars;

    u32 hash;
};

struct HashTableEntry
{
    string *key;
    Value value;
};

struct HashTable
{
    HashTableEntry *entries;
    string *interns;
    int count;
    int capacity;
};


#define TABLE_MAX_LOAD 0.75

void init_table(HashTable *table);
u32 hash(char *key, size_t length);
bool32 table_set(HashTable *table, char *key, Value value);
string *find_string(HashTable *table, char *key, size_t length, u32 hash);

#endif

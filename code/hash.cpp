#include <stdio.h>
#include "hash.h"

// NOTE: A "tombstone" entry in the hash table is an entry that has
// its key removed but its value is still set. Its value should NEVER
// be used! For the user of the hash table a tombstone is for all
// intents and purposes an empty entry. A tombstone lets us remove an
// entry from the hash table without ruining the linear probing.

// TODO: Have the hash table be type agnostic and return void *s.


internal HashTableEntry *add_null_entries(HashTableEntry *entries, u32 capacity)
{
    while(buf_count(entries) < (s32)capacity)
    {
        HashTableEntry entry = {};
        buf_push(entries, entry); 
    }

    return entries;
}


void init_table(HashTable *table)
{
    table->count = 0;
    table->capacity = 8;
    table->entries = NULL;
    table->entries = add_null_entries(table->entries, table->capacity);
}

internal void free_table(HashTable *table)
{
    buf_free(table->entries);
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

internal void free_string_table(HashTable *table)
{
    for(int i = 0; i < buf_count(table->entries); i++)
    {
        free(table->entries[i].key);
    }
    free_table(table);
}

u32 hash(char *key, size_t length)
{
    u32 hash = 216613626lu;

    for(int i = 0; i < length; i++)
    {
        hash ^= key[i];
        hash *= 16777619;
    }

    return hash;
}

internal HashTableEntry *find_entry(HashTableEntry *entries, char *key, u32 capacity)
{
    size_t key_len = strlen(key);
    u32 key_hash = hash(key, key_len);
    u32 index = key_hash % capacity;
    HashTableEntry *tombstone = NULL;

    for(;;)
    {
        HashTableEntry *entry = &entries[index];
        if(entry->key == NULL)
        {
            if(entry->value == NULL)
            {
                if(tombstone != NULL)
                {
                    return tombstone;
                }
                else
                {
                    return entry;
                }
            }
            else
            {
                tombstone = entry;
            }
        }
        else
        {
            if(strlen(entry->key) == key_len &&
               hash(entry->key) == key_hash &&
               (memcmp(entry->key, key, key_len) == 0))
            {
                return entry;
            }
        }
        index = (index + 1) % capacity;
    }
    
}

internal void adjust_capacity(HashTable *table, u32 capacity)
{   
    HashTableEntry *new_entries_array = NULL;
    new_entries_array = add_null_entries(new_entries_array, capacity);
    table->count = 0;
    for(int i = 0; i < table->capacity; i++)
    {
        HashTableEntry entry = table->entries[i];
        if(entry.key == NULL)
        {
            continue;
        }

        HashTableEntry *dest = find_entry(new_entries_array, entry.key, capacity);
        dest->key = entry.key;
        dest->value = entry.value;
        table->count++;
    }

    buf_free(table->entries);
    table->entries = new_entries_array;
    table->capacity = capacity;

}



bool32 table_set(HashTable *table, char *key, Value value)
{
    if(table->count + 1 > table->capacity * TABLE_MAX_LOAD)
    {
        adjust_capacity(table, table->capacity * 2);
    }

    HashTableEntry *entry = find_entry(table->entries, key, table->capacity);

    bool32 is_new_key = entry->key == NULL;
    if(is_new_key
       && entry->value == NULL)
    {
        table->count++;
    }

    entry->key->chars = key;
    entry->value = value;
    return is_new_key;

}

internal bool32 table_get(HashTable *table, string *key, Value *value)
{
    if(table->count == 0)
    {
        return false;
    }

    HashTableEntry *entry = find_entry(table->entries, key, table->capacity);
    if(entry->key == NULL)
    {
        return false;
    }

    *value = entry->value;
    return true;
}

internal bool32 table_delete(HashTable *table, string *key)
{
    
    if(table->count == 0)
    {
        return false;
    }

    HashTableEntry *entry = find_entry(table->entries, key, table->capacity);
    if(entry->key == NULL)
    {
        return false;
    }

    entry->key = NULL;
    return true;
}



string *find_string(HashTable *table, char *key, size_t length, u32 hash)
{
    if(table->count == 0)
    {
        return NULL;
    }

    
    u32 index = hash % table->capacity;

    for(;;)
    {
        HashTableEntry *entry = &table->entries[index];
        if(entry->key == NULL)
        {
            if(entry->value == NULL)
            {
                return NULL;
            }
        }
        else if(entry->key->len == length &&
                entry->key->hash == hash &&
                (memcmp(key, entry->key->chars, length) == 0))
                
        {
            return entry->key;
        }
        index = (index + 1) % table->capacity;
    }
    
}

int main(int argc, char** argv)
{
    HashTable table;
    init_table(&table);
    Value *testValue;
    *testValue = 4;
    table_set(&table, "test", testValue);
}


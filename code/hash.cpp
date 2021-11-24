#include <stdio.h>
#include "hash.h"

// NOTE: A "tombstone" entry in the hash table is an entry that has
// its key removed but its value is still set. Its value should NEVER
// be used! For the user of the hash table a tombstone is for all
// intents and purposes an empty entry. A tombstone lets us remove an
// entry from the hash table without ruining the linear probing.

internal string *intern_range(HashTable *table, char *chars, size_t len)
{

    for(int i = 0; i < buf_count(table->interns); i++)
    {
        if((len == table->interns[i].len) &&
           (strncmp(chars, table->interns[i].chars, len) == 0))
        {
            return &table->interns[i];
        }
    }

    string new_intern;
    new_intern.chars = chars;
    new_intern.len = len;
    new_intern.hash = hash(chars, len);
    buf_push(table->interns, new_intern);
    int index = (buf_count(table->interns) - 1);
    return &table->interns[index];

}


internal string *intern(HashTable *table, char *string)
{
    return intern_range(table, string, strlen(string));
}


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
    table->interns = NULL;

}

internal void free_table(HashTable *table)
{
    buf_free(table->entries);
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;

    if(table->interns)
    {
        buf_free(table->interns);
    }
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

internal HashTableEntry *find_entry(HashTableEntry *entries, string *key, u32 capacity)
{
    u32 index = key->hash % capacity;
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
            if(entry->key == key)
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



bool32 table_set(HashTable *table, char *key, void *value)
{

    string *key_string = intern(table, key);
    if(table->count + 1 > table->capacity * TABLE_MAX_LOAD)
    {
        adjust_capacity(table, table->capacity * 2);
    }

    HashTableEntry *entry = find_entry(table->entries, key_string, table->capacity);

    bool32 is_new_key = entry->key == NULL;
    if(is_new_key
       && entry->value == NULL)
    {
        table->count++;
    }

    entry->key = key_string;
    entry->value = value;
    return is_new_key;

}

internal void *table_get(HashTable *table, char *key)
{
    string *key_string = intern(table, key);
    if(table->count == 0)
    {
        return false;
    }

    HashTableEntry *entry = find_entry(table->entries, key_string, table->capacity);
    if(entry->key == NULL)
    {
        return false;
    }

    return entry->value;
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
    HashTable intern_table;
    init_table(&intern_table);
    int value = 4;
    int *test = 0;
    table_set(&table, "test", (void *) (&value));
    test = (int *) table_get(&table, "test");
    printf("%d", *test);
    return 0;
}


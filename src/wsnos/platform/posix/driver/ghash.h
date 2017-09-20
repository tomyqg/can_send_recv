/**
 * @brief       : this is git hash code.
 * @file        : ghash.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-04-25
 * change logs  :
 * Date       Version     Author        Note
 * 2016-04-25  v0.0.1  gang.cheng    first version
 */
#ifndef __GHASH_H__
#define __GHASH_H__

/*
 * These are some simple generic hash table helper functions.
 * Not necessarily suitable for all users, but good for things
 * where you want to just keep track of a list of things, and
 * have a good hash to use on them.
 *
 * It keeps the hash table at roughly 50-75% free, so the memory
 * cost of the hash table itself is roughly
 *
 *  3 * 2*sizeof(void *) * nr_of_objects
 *
 * bytes.
 *
 * FIXME: on 64-bit architectures, we waste memory. It would be
 * good to have just 32-bit pointers, requiring a special allocator
 * for hashed entries or something.
 */

#ifndef NULL
#define NULL    (void *)0
#endif 

typedef uint32_t hash_key_t;

struct hash_table_entry {
    hash_key_t hash;
    void *ptr;
};

struct hash_table {
    unsigned int size, nr;
    struct hash_table_entry *array;
};

#define alloc_nr(x) (((x)+16)*3/2)

extern void *lookup_hash(hash_key_t hash, const struct hash_table *table);
extern void **insert_hash(hash_key_t hash, void *ptr, struct hash_table *table);
extern int for_each_hash(const struct hash_table *table, int (*fn)(void **, void *), void *data);
extern void free_hash(struct hash_table *table);

static inline void init_hash(struct hash_table *table)
{
    table->size = 0;
    table->nr = 0;
    table->array = NULL;
}

static inline void preallocate_hash(struct hash_table *table, unsigned int elts)
{
    assert(table->size == 0 && table->nr == 0 && table->array == NULL);
    table->size = elts * 2;
    table->array = calloc(table->size, sizeof(struct hash_table_entry));
}

#endif

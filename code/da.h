#if !defined(DA_H)

#define DA_H

#include <stdlib.h>

typedef struct 
{
    int count;
    int capacity;
} buffer_header;

#define buf_header(b)       ((buffer_header *) (b) - 1)
#define buf_count_raw(b)    buf_header(b)->count
#define buf_cap_raw(b)      buf_header(b)->capacity
#define buf_count(b)        ((b) == NULL ? 0 : buf_count_raw(b))
#define buf_cap(b)          ((b) == NULL ? 0 : buf_cap_raw(b))


#define buf__add(b, n)      (buf_count_raw(b)+=(n), &(b)[buf_count_raw(b)-(n)])
#define buf_add(b, n)       (need_grow(b, n) ? buf_grow(b, n), buf__add(b, n) : buf__add(b, n))
#define buf__push(b, a)     ((b)[buf_count_raw(b)++] = (a))
#define buf_push(b, a)      need_grow(b, 1) ? buf_grow(b, 1), buf__push(b, a) : buf__push(b, a)
#define need_grow(b, n)     (((b) == NULL) || buf_count(b) + (n) > buf_cap(b) ? 1 : 0)
#define buf_grow(b, n)      grow(buf_cap(b), sizeof(*(b)), (n), (void **) &(b))
#define buf_free(b)         ((b) ? free(buf_header(b)),0 : 0)

void grow(int capacity, size_t elementSize, int sizeToGrow, void **data)
{
    
    int newCapacity = capacity ? (capacity * 2) + sizeToGrow : 8 + sizeToGrow;
    
    void *pointer;
    if(*data)
    {
        pointer = realloc(buf_header(*data), (newCapacity * elementSize) + sizeof(buffer_header));
    }
    else
    {
        pointer = realloc(0, (newCapacity * elementSize) + sizeof(buffer_header));
        ((buffer_header *)pointer)->count = 0;
    }
    *data = (void *) ((buffer_header *)pointer + 1);
    buf_cap_raw(*data) = newCapacity;
}

#endif

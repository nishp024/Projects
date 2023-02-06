/* mem.h 
 * Dynamic Memory Allocation
 * Fall 2022
 */

#define PAGESIZE 4096      // number of bytes in one page
#define FIRST_FIT 0xFF 
#define BEST_FIT  0xBF
#define TRUE 1
#define FALSE 0

// must be FIRST_FIT or BEST_FIT
int SearchPolicy;

// TRUE if memory returned to free list is coalesced 
int Coalescing;

/* deallocates the space pointed to by return_ptr; it does nothing if
 * return_ptr is NULL.  
 */
void Mem_free(void *return_ptr);

/* returns a pointer to space for an object of size nbytes, or NULL if the
 * request cannot be satisfied.  The space is uninitialized.
 */
void *Mem_alloc(const int nbytes);

/* prints stats about the current free list
 *
 * number of items in the linked list
 * min, max, and average size of each item (bytes)
 * total memory in list (bytes)
 * number of calls to sbrk and number of pages requested
 */
void Mem_stats(void);

/* print table of memory in free list.
 * A unit is the size of one mchunk_t structure
 * example format
 *     mchunk_t *p;
 *     printf("p=%p, size=%d (units), end=%p, next=%p, prev=%p\n", 
 *              p, p->size, p + p->size, p->next, p->prev);
 */
void Mem_print(void);

/* The mchunk_t definition.  
 *
 * We don't really need the definition of mchunk_t in mem.h.  However,
 * for debugging it is nice to be able to print the size of mchunk_t
 * in the drivers.  Note the size of a mchunk_t is one unit.
 * 
 */
typedef struct memory_chunk_tag {
    struct memory_chunk_tag *prev;   // prev block in free list
    struct memory_chunk_tag *next;   // next block in free list
    int size;                        // one unit equals sizeof(mchunk_t)
    char padding[12];                // unused
} mchunk_t;

/* vi:set ts=8 sts=4 sw=4 et: */

/* mem.c
 * Nish Patel
 * Dynamic Memory Allocation
 * Fall 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"

static mchunk_t DummyChunk = {&DummyChunk,&DummyChunk, 0};
static mchunk_t * Rover = &DummyChunk;   // one time initialization
static int NumSbrkCalls = 0;
static int NumPages = 0;

// private function prototypes
void mem_validate(void);

/* function to request 1 or more pages from the operating system.
 *
 * new_bytes must be the number of bytes that are being requested from
 *           the OS with the sbrk command.  It must be an integer 
 *           multiple of the PAGESIZE
 *
 * returns a pointer to the new memory location.  If the request for
 * new memory fails this function simply returns NULL, and assumes some
 * calling function will handle the error condition.  Since the error
 * condition is catastrophic, nothing can be done but to terminate 
 * the program.
 *
 * You can update this function to match your design.  But the method
 * to test sbrk much not be changed.  
 */
mchunk_t *morecore(int new_bytes) 
{
    char *cp;
    mchunk_t *new_p;
    // preconditions that must be true for all designs
    assert(new_bytes > 0);
    assert(new_bytes % PAGESIZE == 0);
    assert(PAGESIZE % sizeof(mchunk_t) == 0);
    cp = sbrk(new_bytes);
    if (cp == (char *) -1)  /* no space available */
        return NULL;
    new_p = (mchunk_t *) cp;
    // You should add some code to count the number of calls
    // to sbrk, and the number of pages that have been requested
    NumSbrkCalls++; NumPages += new_bytes/PAGESIZE;
    return new_p;
}

/* deallocates the space pointed to by return_ptr; it does nothing if
 * return_ptr is NULL.  
 *
 * This function assumes that the Rover pointer has already been 
 * initialized and points to some memory block in the free list.
 */
void Mem_free(void *return_ptr)
{
    // precondition
    assert(Rover != NULL && Rover->next != NULL && Rover->prev != NULL);

    mchunk_t *p = ((mchunk_t *)return_ptr); //new pointer that points to the start of the block
    p = p - 1;
    if(Coalescing != TRUE){
        p->next = Rover->next;
        Rover->next = p;
        p->next->prev = p;
        p->prev = Rover;

    }
    else if(Coalescing == TRUE){
        mchunk_t *Left = NULL;
        mchunk_t *Right = NULL;
        while(1){ //goes until one of the if statements go off.
            if((Rover < p && Rover->next->size == 0)){ //confirms rover spot
                break;
            }
            if((Rover < p && p < Rover->next)){ //checks rover spot
                break;
            }
            Rover = Rover->next; //moves through rover
        }
        Left = Rover; //used to sift through list
        Right = Left->next; 

        p->next = Rover->next; //swaps values
        Rover->next = p;
        p->next->prev = p;
        p->prev = Rover;

        if((Left + Left->size == p) && (p+p->size == Right)){ //checks both
            Left->size = p->size + Right->size + Left->size;
            p->size = 0;
            Right->size = 0;
            Left->next = Right->next;
            Left->next->prev = Left;
            p->next = NULL; //removing links
            p->prev = NULL;
            Right->next = NULL; //removing links
            Right->prev = NULL;
        }
        else if(p+p->size == Right){ //checks second
            p->size += Right->size;
            Right->size = 0;
            p->next = Right->next;
            p->next->prev = p; //linking data
            Right->next = NULL; //removing links
            Right->prev = NULL; 
        }
        else if(Left + Left->size == p){ //checks first
            Left->size += p->size;
            p->size = 0;
            Left->next = Right;
            Right->prev = Left; //linking data
            p->next = NULL; //removing links
            p->prev = NULL; 
        }
        else{
            return;
        }
    }
    // assume p points to the start of the block to return to the free list
    // assert(p->size > 1);   // verify that the size field is valid
    // assert(p->next == NULL && p->prev == NULL);    // this is not required but would be a good idea

    // obviously the next line is WRONG!!!!  You must fix it.
    //free(return_ptr);

}

/* returns a pointer to space for an object of size nbytes, or NULL if the
 * request cannot be satisfied.  The memory is uninitialized.
 *
 * This function assumes that there is a Rover pointer that points to
 * some item in the free list.  
 */
void *Mem_alloc(const int nbytes)
{
    // precondition
    assert(nbytes > 0);
    assert(Rover != NULL && Rover->next != NULL && Rover->prev != NULL);

    mchunk_t *start = Rover; //start
    mchunk_t *roverPrev;
    mchunk_t *temp = Rover; //temp variable

    mchunk_t *p = NULL;
    mchunk_t *q = NULL;

    mchunk_t *Brover = NULL;
    mchunk_t *BroverPrev = NULL;

    mchunk_t *MoreChunk;
    Rover = Rover->next;
    int ChunksNum;
    int Units;
    Units = nbytes / sizeof(mchunk_t) + 1;
    if(nbytes % sizeof(mchunk_t)){
        Units++;
    }

    if(SearchPolicy == BEST_FIT) { //best fit policy
        roverPrev = Rover; //sets roverPrev
        Rover = Rover->next;
        start = Rover;

        do{
            if(Rover->size == Units){ //confirms size of the rover
                Brover = Rover;
                BroverPrev = roverPrev; 
                break;
            }
            if((Rover->size >= Units) && (Rover->size < Brover->size || Brover == NULL)){
                Brover = Rover;
                BroverPrev = roverPrev;
            }
            roverPrev = Rover;
            Rover = Rover->next; //moves through list
        }
        while(Rover != start);
            if(Brover != NULL){ //searches for value in list
                Rover = Brover; 
                roverPrev = BroverPrev;
                p = Brover; //sets p and q
                q = p + 1; //sets q
                Brover = NULL;
                BroverPrev = NULL; //used segment in MP4 file as help
            }
            else{
                q = NULL;
            }
    }
    else{ //first fit policy
        roverPrev = Rover; //sets roverPrev
        Rover = Rover->next;
        start = Rover;
        do{
            p = Rover; //sets p and q
            if(Rover->size >= Units){ //find first spot that has enough space
                q = p + 1; //sets q
                break;
            }
            roverPrev = Rover;
            Rover = Rover->next; //moves through list
        }
        while(Rover != start);
    }

    if(q == NULL){
        p = NULL;
    }

    if(p == NULL){ //incase there is no fit
        ChunksNum = (nbytes + sizeof(mchunk_t));
        if(ChunksNum % PAGESIZE != 0){ //checks for valid size
            ChunksNum = PAGESIZE * (ChunksNum / PAGESIZE) + PAGESIZE;
        }
        MoreChunk = morecore(ChunksNum); //more memory
        if(MoreChunk == NULL){ //incase morecore does not allocate more memory
            return NULL; 
        }
        MoreChunk->size = ChunksNum/sizeof(mchunk_t);
        Mem_free(MoreChunk + 1); //frees excess memory

        return Mem_alloc(nbytes); //returns correctly allocated memory
    }

    if(p->size > Units + 1){ //the memory block is bigger than needed
        p->size = p->size - Units;
        p = p + p->size; //corrects the size
        p->size = Units;
        p->next = NULL;
        p->prev = NULL;
        q = p + 1; //sets q
    }
    else if(p->size == Units + 1 || p->size == Units){ //memory block is perfect fit
        assert(p == Rover);
        Rover->prev->next = Rover->next;
        Rover->next->prev = Rover->prev;
        temp = Rover->next;
        Rover->next = NULL;
        Rover->prev = NULL;
        Rover = temp; //sets rover to value after it
        q = p + 1; //sets q
    }
 
    assert((p->size - 1)*sizeof(mchunk_t) >= nbytes);
    assert((p->size - 1)*sizeof(mchunk_t) < nbytes + 2*sizeof(mchunk_t));
    return q; 


    // Insert your code here to find memory block

    // here are possible post-conditions, depending on your design. 
    // Include these tests before returning from this function.
    //
    // one unit is equal to sizeof(mchunk_t) bytes
    //
    // assume p is a pointer to the starting address of the memory block 
    // to be given to the user. The address returned to the user is q.
    //mchunk_t *p;
    //mchunk_t *q;
    //
    // assert(p + 1 == q);   // +1 means one unit or sizeof(mchunk_t)
    //
    // Next, test that the memory block is just big enough.
    // The minus one in the next two tests accounts for the header
    //
   
    //assert((p->size-1)*sizeof(mchunk_t) >= nbytes);
    //assert((p->size-1)*sizeof(mchunk_t) < nbytes + 2*sizeof(mchunk_t));
    // can add one extra chunk to avoid leaving a block of size 1
    //assert(p->next == NULL && p->prev == NULL);  // saftey first!
    //return q;


    // obviously the next line is WRONG!!!!  You must fix it.
    //return malloc(nbytes);
}

/* prints stats about the current free list
 *
 * -- number of items in the linked list including dummy item
 * -- min, max, and average size of each item (in bytes) except dummy
 * -- total memory in list (in bytes) except dummy
 * -- number of calls to sbrk and number of pages requested
 *
 * A message is printed if all the memory is in the free list
 */
void Mem_stats(void)
{
    int NumItems = -1; //keeps track of num of items and accounts for dummy
    int average; //keeps track of the average memory used in the list
    int min; //max memory walue
    int max; //min memory value
    int M = 0; //keeps track of the total memory in the list

    mchunk_t *starter = Rover; //used to run through the list
    do{
        if (Rover->size > max){
            max = Rover->size;
        }
        if (Rover != &DummyChunk && Rover->size < min){
            min = Rover->size;
        } 
        M += Rover->size*sizeof(mchunk_t);
        NumItems++;
        Rover = Rover->next;
    }
    while (Rover != starter);{
        average = M/NumItems; //updates average everytime it loops
    }

    printf("Number of items: %d\n", NumItems); //print statements
    printf("Min size: %ld\n", (min * sizeof(mchunk_t)));
    printf("Max size: %ld\n", (max * sizeof(mchunk_t)));
    printf("Average size: %d\n", average);
    printf("Total memory: %d\n", M);
    printf("Number of calls to sbrk(): %d\n", NumSbrkCalls);
    printf("Total number of pages requested: %d\n", NumPages);
    if (M == NumPages * PAGESIZE){
        printf("all memory is in the heap -- no leaks are possible\n");   
    }
    // One of the stats you must collect is the total number
    // of pages that have been requested using sbrk.
    // Say, you call this NumPages.  You also must count M,
    // the total number of bytes found in the free list 
    // (including all bytes used for headers).  If it is the case
    // that M == NumPages * PAGESiZE then print
}

/* print table of memory in free list 
 *
 * The print should include the dummy item in the list 
 *
 * A unit is the size of one mchunk_t structure
 */
void Mem_print(void)
{
    char *comments[] = {"", "<-- dummy", "<-- jetsam"};
    // note position of Rover is not changed by this function
    assert(Rover != NULL && Rover->next != NULL && Rover->prev != NULL);
    mchunk_t *p = Rover;
    mchunk_t *start = p;
    int message;
    do {
        // example format.  Modify for your design
        if (p->size == 0) message = 1;
        else if (p->size == 1) message = 2;
        else message = 0;
        printf("p=%p, size=%d (units), end=%p, next=%p, prev=%p %s\n", 
                p, p->size, p + p->size, p->next, p->prev,
                comments[message]);
        p = p->next;
    } while (p != start);
    mem_validate();
}

/* This is an experimental function to attempt to validate the free
 * list when coalescing is used.  It is not clear that these tests
 * will be appropriate for all designs.  If your design utilizes a different
 * approach, that is fine.  You do not need to use this function and you
 * are not required to write your own validate function.
 */
void mem_validate(void)
{
    // note position of Rover is not changed by this function
    assert(Rover != NULL && Rover->next != NULL && Rover->prev != NULL);
    assert(Rover->size >= 0);
    int wrapped = FALSE;
    int found_dummy = FALSE;
    int found_rover = FALSE;
    int size_warning = FALSE;
    mchunk_t *p, *largest, *smallest;

    // for validate begin at DummyChunk
    p = &DummyChunk;
    do {
        assert(p->next->prev == p);
        if (p->size == 0) {
            assert(found_dummy == FALSE);
            found_dummy = TRUE;
        } else {
            assert(p->size > 0);
            if (p->size == 1) {
                size_warning = TRUE;
            }
        }
        if (p == Rover) {
            assert(found_rover == FALSE);
            found_rover = TRUE;
        }
        p = p->next;
    } while (p != &DummyChunk);
    assert(found_dummy == TRUE);
    assert(found_rover == TRUE);
    if (size_warning == TRUE) {
        printf("Found block with size one.  Preferred design uses 2 as min size\n");
    }

    if (Coalescing) {
        do {
            if (p >= p->next) {
                // this is not good unless at the one wrap around
                if (wrapped == TRUE) {
                    printf("validate: List is out of order, already found wrap\n");
                    printf("first largest %p, smallest %p\n", largest, smallest);
                    printf("second largest %p, smallest %p\n", p, p->next);
                    assert(0);   // stop and use gdb
                } else {
                    wrapped = TRUE;
                    largest = p;
                    smallest = p->next;
                }
            } else {
                assert(p + p->size < p->next);
            }
            p = p->next;
        } while (p != &DummyChunk);
        assert(wrapped == TRUE);
    }
}
/* vi:set ts=8 sts=4 sw=4 et: */


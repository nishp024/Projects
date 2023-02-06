/*
 * Hash Table Implementation
 * Fall 2022
 *
 * This file contains drivers to test the Hash Table ADT package.
 *
 *   -m to set the table size
 *   -a to set the load factor
 *   -h to set the type of probe sequence {linear|double|quad}
 *
 * The -r driver builds a table using table_insert and then accesses
 * keys in the table using table_retrieve.  Use
 *   -r run the retrieve driver and specifiy the type of initial table keys 
 *      (rand|seq|fold|worst)
 *   -t to set the number of access trials 
 *
 * To test using the rehash driver.  The table size must be at least 6
 *   -b
 *
 * To test the Two Sum Problem use -p X for X=1, 2, 3, or 4
 *    For -p 4 only:
 *        -m to set the size of the array (not the hash table size)
 *        -t to set number of trials, each with a new array
 *
 * Another test driver tests random inserts and deletes.  This driver builds
 * an initial table with random keys, and then performs insertions and deletions
 * with equal probability.
 *   -e run the equilibrium driver
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

#include "table.h"

/* constants used with Global variables */

enum TableType_t {RAND, SEQ, FOLD, WORST};

#define LOWID 0
#define MAXID 999999999
#define MINID -999999999
#define PEEK_NOKEY INT_MAX

#define TRUE 1
#define FALSE 0

/* Global variables for command line parameters.  */
int Verbose = FALSE;
static int TableSize = 11;
static int ProbeDec = LINEAR;
static double LoadFactor = 0.9;
static int TableType = RAND;
static int RetrieveTest = FALSE;
static int EquilibriumTest = FALSE;
static int UnitInt = FALSE;
static int RehashTest = FALSE;
static int TwoSumTest = 0;
static int Trials = 50000;
static int Seed = 11222022;

/* prototypes for functions in this file only */
void getCommandLine(int argc, char **argv);
void equilibriumDriver(void);
void RetrieveDriver(void);
void UnitDriver(int);
void RehashDriver(int);
void TwoSumDriver(int);
int build_random(table_t *T, int ,int);
int build_seq(table_t *T, int, int);
int build_fold(table_t *T, int, int);
int build_worst(table_t *T, int, int);
void performanceFormulas(double);
int find_first_prime(int number);
void twoSum(const int* nums, const int numsSize, const int target, int *ans1, int *ans2);

int main(int argc, char **argv)
{
    getCommandLine(argc, argv);
    printf("Table size (%d), load factor (%g)\n", TableSize, LoadFactor);
    if (ProbeDec == LINEAR)
        printf("Open addressing with linear probe sequence\n");
    else if (ProbeDec == DOUBLE)
        printf("Open addressing with double hashing\n");
    else if (ProbeDec == QUAD)
        printf("Open addressing with quadratic probe sequence\n");
    printf("Seed: %d\n", Seed);
    srand48(Seed);

    /* ----- small table tests  ----- */

    if (RehashTest)                         /* enable with -b */
        RehashDriver(TableSize);

    /* ----- large table tests  ----- */
    if (RetrieveTest)                        /* enable with -r flag */
        RetrieveDriver();

    /* ----- Two Sum Problem    ----- */
    if (TwoSumTest)                        /* enable with -p flag */
        TwoSumDriver(TwoSumTest);

    /* test for performance in equilibrium */
    if (EquilibriumTest)                   /* enable with -e flag */
        equilibriumDriver();
    /* test for performance made by Nish */
    if (UnitInt != 0)                   /* enable with -u flag */
        UnitDriver(UnitInt);

    return 0;
}

void build_table(table_t *test_table, int num_keys)
{
    int probes = -1;
    printf("  Build table with");
    if (TableType == RAND) {
        printf(" %d random keys\n", num_keys);
        probes = build_random(test_table, TableSize, num_keys);
    } else if (TableType == SEQ) {
        printf(" %d sequential keys\n", num_keys);
        probes = build_seq(test_table, TableSize, num_keys);
    } else if (TableType == FOLD) {
        printf(" %d folded keys\n", num_keys);
        probes = build_fold(test_table, TableSize, num_keys);
    } else if (TableType == WORST) {
        printf(" %d worst keys\n", num_keys);
        probes = build_worst(test_table, TableSize, num_keys);
    } else {
        printf("invalid option for table type\n");
        exit(7);
    }
    printf("    The average number of probes for a successful search = %g\n", 
            (double) probes/num_keys);

    if (Verbose)
        table_debug_print(test_table);

    int size = table_entries(test_table);
    assert(size == num_keys);
}

/* driver to test small tables.  This is a series of 
 * simple tests and is not exhaustive.
 *
 * input: test_M is the table size for this test run
 */
void RehashDriver(int test_M)
{
    int i, *ip, code;
    table_t *H;

    printf("\n----- Rehash driver -----\n");
    hashkey_t startkey = LOWID + (test_M - LOWID%test_M);
    assert(startkey%test_M == 0);
    assert(test_M > 5);  // tests designed for size at least 6

    H = table_construct(test_M, ProbeDec);
    // fill table sequentially 
    for (i = 0; i < test_M-1; i++) {
        ip = (int *) malloc(sizeof(int));
        *ip = 10*i;
        assert(table_full(H) == 0);
        code = table_insert(H, startkey+i, ip);
        ip = NULL;
        assert(code == 0);
        assert(table_entries(H) == i+1);
        assert(table_stats(H) == 1);
        assert(table_peek(H,i) == startkey+i);
    }
    if (Verbose) {
        printf("\nfull table, last entry empty\n");
        table_debug_print(H);
    }
    // tests on empty position
    assert(table_peek(H,i) == PEEK_NOKEY);
    assert(NULL == table_retrieve(H, startkey+i));
    assert(table_stats(H) == 1);
    assert(table_full(H) == 1);
    assert(-1 == table_insert(H, MAXID, NULL));
    // retrieve and replace each entry
    for (i = 0; i < test_M-1; i++) {
        ip = table_retrieve(H, startkey+i);
        assert(*(int *)ip == 10*i);
        ip = NULL;
        assert(table_stats(H) == 1);
        ip = table_retrieve(H, startkey+i+test_M);
        assert(ip == NULL);
        assert(2 <= table_stats(H) && table_stats(H) <= test_M);
        if (ProbeDec == LINEAR)
            assert(table_stats(H) == i+2);
        ip = (int *) malloc(sizeof(int));
        *ip = 99*i;
        assert(1 == table_insert(H, startkey+i, ip));
        ip = NULL;
        ip = table_retrieve(H, startkey+i);
        assert(*(int *)ip == 99*i);
        ip = NULL;
    }
    assert(table_entries(H) == test_M-1);
    assert(table_full(H) == 1);
    // delete tests
    assert(table_deletekeys(H) == 0);
    ip = table_delete(H, startkey+1);
    if (Verbose) {
        printf("\nfull table, delete first key %d\n", startkey+1);
        table_debug_print(H);
    }
    assert(*(int *)ip == 99);
    free(ip); ip = NULL;
    if (Verbose) {
        printf("\nsecond entry deleted, last entry empty\n");
        table_debug_print(H);
    }
    assert(table_entries(H) == test_M-2);
    assert(table_full(H) == 0);
    assert(table_peek(H,1) == PEEK_NOKEY);
    assert(table_deletekeys(H) == 1);
    ip = table_retrieve(H, startkey+1);  // check key is not there
    assert(ip == NULL);
    assert(table_stats(H) >= 2);
    // attempt to delete keys not in table 
    assert(NULL == table_delete(H, startkey+1));
    assert(NULL == table_delete(H, startkey+test_M-1));
    // insert key in its place
    ip = (int *) malloc(sizeof(int));
    *ip = 123;
    assert(0 == table_insert(H, startkey+1+test_M, ip));
    ip = NULL;

    assert(table_peek(H,1) == startkey+1+test_M);
    ip = table_retrieve(H, startkey+1+test_M);
    assert(*(int *)ip == 123);
    ip = NULL;
    assert(table_entries(H) == test_M-1);
    assert(table_full(H) == 1);
    if (Verbose) {
        printf("\nsecond entry deleted, checking delete keys\n");
        table_debug_print(H);
    }
    assert(table_deletekeys(H) == 0);
    for (i = 2; i < test_M-1; i++) {     // clear out all but two keys
        ip = table_delete(H, startkey+i);
        assert(*(int *)ip == 99*i);
        free(ip); ip = NULL;
    }
    if (Verbose) {
        printf("\ntwo entries in table only one empty location\n");
        table_debug_print(H);
    }
    assert(table_entries(H) == 2);
    ip = (int *) malloc(sizeof(int));    // fill last empty
    *ip = 456;
    assert(0 == table_insert(H, startkey+test_M-1, ip));
    ip = NULL;
    if (Verbose) {
        printf("\nthree entries in table and no empty locations\n");
        table_debug_print(H);
    }
    assert(table_entries(H) == 3);
    // unsuccessful search when no empty keys
    assert(NULL == table_retrieve(H, startkey+test_M));

    // two keys the collide in position 0
    ip = (int *) malloc(sizeof(int));
    *ip = 77;
    assert(0 == table_insert(H, startkey+test_M, ip));
    ip = (int *) malloc(sizeof(int));
    *ip = 88;
    assert(0 == table_insert(H, startkey+10*test_M, ip));
    ip = NULL;
    assert(table_entries(H) == 5);
    ip = table_delete(H, startkey);  // delete position 0
    assert(*(int *)ip == 0);
    free(ip); ip = NULL;
    assert(table_entries(H) == 4);
    ip = (int *) malloc(sizeof(int));  // replace 
    *ip = 87;
    assert(1 == table_insert(H, startkey+10*test_M, ip));
    ip = NULL;
    assert(table_entries(H) == 4);
    ip = (int *) malloc(sizeof(int));   // put back position 0
    *ip = 76;
    assert(0 == table_insert(H, startkey+20*test_M, ip));
    ip = NULL;
    assert(table_entries(H) == 5);
    assert(table_peek(H,0) == startkey+20*test_M);
    assert(table_deletekeys(H) == test_M-5);
    // verify 5 items in table
    ip = table_retrieve(H, startkey+1+test_M);
    assert(*(int *)ip == 123);
    ip = table_retrieve(H, startkey+test_M);
    assert(*(int *)ip == 77);
    ip = table_retrieve(H, startkey+10*test_M);
    assert(*(int *)ip == 87);
    ip = table_retrieve(H, startkey+20*test_M);
    assert(*(int *)ip == 76);
    ip = table_retrieve(H, startkey+test_M-1);
    assert(*(int *)ip == 456);
    ip = NULL;
    // rehash
     if (Verbose) {
        printf("\nRIGHT BEFORE REHASH\n");
        table_debug_print(H);
    }
    H = table_rehash(H, test_M);
    if (Verbose) {
        printf("\nAfter rehash\n");
        table_debug_print(H);
    }
    assert(table_entries(H) == 5);

    assert(table_deletekeys(H) == 0);
    if (Verbose) {
        printf("\ntable after rehash with 5 items\n");
        table_debug_print(H);
    }
    // verify 5 items in table
    ip = table_retrieve(H, startkey+1+test_M);
    assert(*(int *)ip == 123);
    ip = table_retrieve(H, startkey+test_M);
    assert(*(int *)ip == 77);
    ip = table_retrieve(H, startkey+10*test_M);
    assert(*(int *)ip == 87);
    ip = table_retrieve(H, startkey+20*test_M);
    assert(*(int *)ip == 76);
    ip = table_retrieve(H, startkey+test_M-1);
    assert(*(int *)ip == 456);
    ip = NULL;

    // rehash and increase table size
    // If linear or quad, double the size
    // If double, need new prime
    int new_M = 2*test_M;
    if (ProbeDec == DOUBLE)
        new_M = find_first_prime(new_M);

    H = table_rehash(H, new_M);
    assert(table_entries(H) == 5);
    assert(table_deletekeys(H) == 0);
    if (Verbose) {
        printf("\nafter increase table to %d with 5 items\n", new_M);
        table_debug_print(H);
    }
    // verify 5 keys and information not lost during rehash
    ip = table_retrieve(H, startkey+1+test_M);
    assert(*(int *)ip == 123);
    ip = table_retrieve(H, startkey+test_M);
     if (Verbose) {
        printf("\ntesting retrieve function\n");
        table_debug_print(H);
    }
    assert(*(int *)ip == 77);
    ip = table_retrieve(H, startkey+10*test_M);
    assert(*(int *)ip == 87);
    ip = table_retrieve(H, startkey+20*test_M);
    assert(*(int *)ip == 76);
    ip = table_retrieve(H, startkey+test_M-1);
    assert(*(int *)ip == 456);
    ip = NULL;

    // fill the new larger table
    assert(table_full(H) == 0);
    int new_items = new_M - 1 - 5;
    int base_addr = 2*startkey + 20*test_M*test_M;
    if (base_addr+new_items*test_M > MAXID) {
        printf("re-run -b driver with smaller table size\n");
        exit(1);
    }
    for (i = 0; i < new_items; i++) {
        ip = (int *) malloc(sizeof(int));
        *ip = 10*i;
        code = table_insert(H, base_addr+i*test_M, ip);
        ip = NULL;
        assert(code == 0);
        assert(table_entries(H) == i+1+5);
    }
    if (Verbose) {
        printf("\nafter larger table filled\n");
        table_debug_print(H);
    }
    assert(table_full(H) == 1);
    assert(table_entries(H) == new_M-1);
    // verify new items are found 
    for (i = 0; i < new_items; i++) {
        ip = table_retrieve(H, base_addr+i*test_M);
        assert(*(int *)ip == 10*i);
        ip = NULL;
    }

    // clean up table
    table_destruct(H);
    printf("----- Passed rehash driver -----\n\n");
}

//User made function

void UnitDriver(int UnitInt)
{
    /* print parameters for this test run */
    printf("\n----- Unit driver -----\n");
    printf("Table:\n");
    table_t *test_table;
    if(UnitInt == 1){
        test_table = table_construct(20, DOUBLE);
        build_table(test_table, 10);

        printf("Table after inserts\n");
        table_debug_print(test_table);

        table_delete(test_table, 967132642);
        table_delete(test_table, -124980478);
        printf("Table after deletions\n");
        table_debug_print(test_table);

        table_delete(test_table, 2); //nothing happens since 2 is not in the table
        table_delete(test_table, -98084562);
        printf("Table after deleting nonexistant values\n");
        table_debug_print(test_table);
        
        table_delete(test_table, 90497939);
        printf("Table after deleting last value which should have trouble due to table size\n");
        table_debug_print(test_table);

        int *data_ptr = malloc(sizeof(int));
        *data_ptr = 1;
        table_insert(test_table, -278894312, data_ptr);
        printf("Table after inserting a key value that is already in the hashtable\n");
        table_debug_print(test_table);
    }
    else if (UnitInt == 2){
        printf("PART A\n");
        test_table = table_construct(7, LINEAR);

        int *data_ptr1 = malloc(sizeof(int));
        *data_ptr1 = 1;
        int *data_ptr2 = malloc(sizeof(int));
        *data_ptr2 = 1;
        int *data_ptr3 = malloc(sizeof(int));
        *data_ptr3 = 1;
        int *data_ptr4 = malloc(sizeof(int));
        *data_ptr4 = 1;
        table_insert(test_table, 5, data_ptr1);
        table_insert(test_table, 12, data_ptr2);
        table_insert(test_table, 11, data_ptr3);
        table_insert(test_table, 19, data_ptr4);
        printf("Table after inserting 5,12,11,19\n");
        table_debug_print(test_table);

        table_delete(test_table, 5);
        table_delete(test_table,8);
        table_delete(test_table, 12);
        printf("Table after removing 5,8,12\n");
        table_debug_print(test_table);

        int *data_ptr5= malloc(sizeof(int));
        *data_ptr5 = 1;
        int *data_ptr6 = malloc(sizeof(int));
        *data_ptr6 = 1;
        table_insert(test_table, 19, data_ptr5);
        table_insert(test_table, 26, data_ptr6);
        printf("Table after inserting 19,26\n");
        table_debug_print(test_table);

        printf("PART B\n");

        int *data_ptr7 = malloc(sizeof(int));
        *data_ptr7 = 1;
        int *data_ptr8 = malloc(sizeof(int));
        *data_ptr8 = 1;
        int *data_ptr9 = malloc(sizeof(int));
        *data_ptr9 = 1;
        int *data_ptr10 = malloc(sizeof(int));
        *data_ptr10 = 1;
        int *data_ptr11 = malloc(sizeof(int));
        *data_ptr11 = 1;
        int *data_ptr12 = malloc(sizeof(int));
        *data_ptr12 = 1;
        table_insert(test_table, 7, data_ptr7);
        table_insert(test_table, 8, data_ptr8);
        table_insert(test_table, 9, data_ptr9);
        table_insert(test_table, 10, data_ptr10);
        table_insert(test_table, 11, data_ptr11);
        table_insert(test_table, 12, data_ptr12);
        printf("Table after inserting 7,8,9,10,11,12\n");
        table_debug_print(test_table);

        table_delete(test_table, 7);
        table_delete(test_table,8);
        table_delete(test_table, 9);
        table_delete(test_table,10);
        table_delete(test_table, 11);
        printf("Table after removing 7,8,9,10,11\n");
        table_debug_print(test_table);

        int *data_ptr13 = malloc(sizeof(int));
        *data_ptr13 = 1;
        int *data_ptr14 = malloc(sizeof(int));
        *data_ptr14 = 1;
        table_insert(test_table, 13, data_ptr13);
        table_insert(test_table, 14, data_ptr14);
        printf("Table after inserting 13,14\n");
        table_debug_print(test_table);
        if(table_retrieve(test_table, 16) == NULL){
            printf("\ncould not find 16\n\n");
        }
    }
    /* remove and free all items from table */
    table_destruct(test_table);
    printf("----- End of unit driver -----\n\n");
}

/* driver to build and test tables. Note this driver  
 * does not delete keys from the table.
 */
void RetrieveDriver()
{
    int i;
    int key_range, num_keys;
    int suc_search, suc_trials, unsuc_search, unsuc_trials;
    table_t *test_table;
    hashkey_t key;
    data_t dp;

    /* print parameters for this test run */
    printf("\n----- Retrieve driver -----\n");
    printf("  Trials: %d\n", Trials);

    num_keys = (int) (TableSize * LoadFactor);
    test_table = table_construct(TableSize, ProbeDec);

    build_table(test_table, num_keys);

    key_range = MAXID - MINID + 1;

    if (Trials > 0) {
        /* access table to measure probes for an unsuccessful search */
        suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
        for (i = 0; i < Trials; i++) {
            /* random key with uniform distribution */
            key = (hashkey_t) (drand48() * key_range) + MINID;
            if (Verbose)
                printf("%d: looking for %d\n", i, key);
            dp = table_retrieve(test_table, key);
            if (dp == NULL) {
                unsuc_search += table_stats(test_table);
                unsuc_trials++;
                if (Verbose)
                    printf("\t not found with %d probes\n", 
                            table_stats(test_table));
            } else {
                // this should be very rare
                suc_search += table_stats(test_table);
                suc_trials++;
                if (Verbose)
                    printf("\t\t FOUND with %d probes (this is rare!)\n", 
                            table_stats(test_table));
                assert(*(int *)dp == key);
            }
        }
        assert(num_keys == table_entries(test_table));
        if (suc_trials > 0)
            printf("    Avg probes for successful search = %g measured with %d trials\n", 
                    (double) suc_search/suc_trials, suc_trials);
        if (unsuc_trials > 0)
            printf("    Avg probes for unsuccessful search = %g measured with %d trials\n", 
                    (double) unsuc_search/unsuc_trials, unsuc_trials);
    }

    /* print expected values from analysis with compare to experimental
     * measurements */
    performanceFormulas(LoadFactor);

    /* remove and free all items from table */
    table_destruct(test_table);
    printf("----- End of access driver -----\n\n");
}

/* Given a target, find indices of the two numbers that add up to the target.
 * The index positions must be unique.  The same array entry cannot be used
 * twice.
 *
 * inputs
 *    nums: an array of integers
 *    numsSize: number of items in array
 *    target: there is exactly one solution that sums to target
 * 
 * outputs
 *    ans1 and ans2: index positions such that
 *        nums[ans1] + nums[ans2] == target
 *        ans1 != ans2
 *
 * You cannot change the array nums.
 *
 * See mp5.pdf for notes on the options for -p 1, 2, 3, or 4.  Or run with -v
 *
 * -t: number of trials for -p 4 only
 * -m: size of array for -p 4 only
 * -v: turn on extra prints
 *
 * Note -m does not set the size of the hash table.
 * Instead you determine an appropriate hash table size.
 */
void twoSum(const int* nums, const int numsSize, const int target, int *ans1, int *ans2)
{
    table_t *table = table_construct(find_first_prime(numsSize * 2), DOUBLE);
    for(int i = 0; i < numsSize; i++){
        if(table_retrieve(table,target-nums[i]) != NULL){
            *ans1 = i;
            *ans2 = *(int*) table_retrieve(table,target-nums[i]);
            break;
        }
        int *data_ptr = malloc(sizeof(int));
        *data_ptr = i;
        table_insert(table, nums[i], data_ptr);
    }
}

/* support function to generate arrays for Two Sums Problem
 *
 * You should not change this function
 *
 * inputs
 *    twosumcase: one of 1, 2, 3, or 4
 *    numsSize: size of array to malloc
 *
 * returns:
 *    nums: malloced array of numbers
 *    target: the sum to find
 *    place1 and 2: locations of nums that sum to target
 *
 * Notes
 *     This design creates many sequences, which is a significant problem for
 *     hash tables, especially with linear probing
 */
int *generatetest(int twosumcase, int numsSize, int *target, int *place1, int *place2)
{
    int i, temp;
    int *nums = (int *) malloc(numsSize * sizeof(int));
    if (twosumcase == 4) {
        if (numsSize < 6) {
            printf("   This case designed for list size of 6 or larger\n");
            exit(1);
        }
        int spot1 = 0, spot2 = numsSize-1;
        *target = numsSize * drand48();
        float mix = drand48();
        if (mix < 0.2) *target += 6*numsSize;
        else if (mix < 0.4) *target -= 4*numsSize;
        nums[spot1] = numsSize * drand48();
        nums[spot2] = *target - nums[spot1];
        for (i = 0; i < numsSize - 2; i++) {
            if (i < (numsSize - 2)/2) {
                nums[i+1] = 2.5 * numsSize + i;
            } else {
                nums[i+1] = -numsSize - (i - (numsSize-2)/2);
            }
        }
        if (*target%2==0 && *target/2 != nums[0])
            nums[2] = *target/2;
        // shuffle
        for (i = 0; i<numsSize; i++) {
            int j = (int) (drand48() * (numsSize - i)) + i;
            assert(i <= j && j < numsSize);
            temp = nums[i]; nums[i] = nums[j]; nums[j] = temp;
            if (i == spot1)
                spot1 = j;
            else if (j == spot1)
                spot1 = i;
            if (i == spot2)
                spot2 = j;
            else if (j == spot2)
                spot2 = i;
        }
        if (spot1 > spot2) {
            temp = spot1; spot1 = spot2; spot2 = temp;
        }
        assert(spot1 != spot2);
        assert(0 <= spot1 && spot1 < numsSize && 0 <= spot2 && spot2 < numsSize);
        assert(nums[spot1]+nums[spot2] == *target);
        *place1 = spot1;
        *place2 = spot2;
    } else if (twosumcase == 1) {
        *target = 9;
        nums[0] = 2; nums[1] = 7; nums[2] = 11; nums[3] = 15;
        *place1 = 0; *place2 = 1;
    } else if (twosumcase == 2) {
        *target = 6;
        nums[0] = 3; nums[1] = 2; nums[2] = 4;
        *place1 = 1; *place2 = 2;
    } else if (twosumcase == 3) {
        *target = 6;
        nums[0] = 3; nums[1] = 3;
        *place1 = 0; *place2 = 1;
    } else {
        printf("Invalid case number for Two Sum Driver -p %d\n", twosumcase);
        exit(1);
    }
    return nums;
}
/* Driver to test Two Sum Problem
 *
 * You should not change this function
 */
void TwoSumDriver(int twosumcase)
{
    int *nums;
    int numsSize;
    int target;
    int ans1 = -1, ans2 = -2;
    int place1, place2;
    int trials = 1;
    int i, j;

    if (twosumcase == 1) {
        numsSize = 4;
    } else if (twosumcase == 2) {
        numsSize = 3;
    } else if (twosumcase == 3) {
        numsSize = 2;
    } else if (twosumcase == 4) {
        trials = Trials;
        numsSize = TableSize;    // redefines input table size as array size instead!
    } else {
        printf("Invalid case number for Two Sum Driver -p %d\n", twosumcase);
        exit(1);
    }
    printf("Two Sum Driver -p %d for -t %d trials array size -m %d\n", TwoSumTest, trials, numsSize);
    assert(0 < trials && 1 < numsSize && numsSize <= pow(2,20));
    int all_pass = TRUE;
    for (i = 0; i < trials; i++) {
        ans1 = -1; ans2 = -2;
        nums = generatetest(twosumcase, numsSize, &target, &place1, &place2);
        if (Verbose) {
            printf("Trial %d: expect [%d, %d], target=%d\n", i+1, place1, place2, target);
            if (numsSize <= 20) {
                printf("[ ");
                for (j = 0; j < numsSize - 1; j++) {
                    printf("%d, ", nums[j]);
                }
                printf("%d]\n", nums[j]);
            } else {
                printf("numbers are %d and %d\n", nums[place1], nums[place2]);
            }
        }
        twoSum(nums, numsSize, target, &ans1, &ans2);
        if ((ans1 == place1 && ans2 == place2) || (ans1 == place2 && ans2 == place1)) {
            if (Verbose) {
                printf("Good job! You found [%d, %d]\n", ans1, ans2);
            }
            free(nums);
        } else {
            printf("Failed Two Sum problem on trial %d of %d\n", i+1, trials);
            printf("    You found [%d, %d]\n", ans1, ans2);
            if (ans1 == ans2)
                printf("    Cannot use same index twice\n");
            if (0 <= ans1 && ans1 < numsSize) {
                printf("      index %d has %d\n", ans1, nums[ans1]);
                if (0 <= ans2 && ans2 < numsSize) {
                    printf("      index %d has %d\n", ans2, nums[ans2]);
                    printf("      your sum is %d\n", nums[ans1]+nums[ans2]);
                }
            }
            printf("    Expected [%d, %d], target=%d\n", place1, place2, target);
            printf("      index %d has %d\n", place1, nums[place1]);
            printf("      index %d has %d\n", place2, nums[place2]);
            all_pass = FALSE;
            free(nums);
            break;
        }
    }
    if (all_pass == TRUE) {
        printf("    All trials passed!\n");
    }

}

/* driver to test sequence of inserts and deletes.
*/
void equilibriumDriver(void)
{
    int i, code;
    int key_range, num_keys;
    int size;
    int ran_index;
    int suc_search, suc_trials, unsuc_search, unsuc_trials;
    int keys_added, keys_removed;
    int *ip;
    table_t *test_table;
    hashkey_t key;
    data_t dp;
    clock_t start, end;

    /* print parameters for this test run */
    printf("\n----- Equilibrium test driver -----\n");
    printf("  Trials: %d\n", Trials);

    test_table = table_construct(TableSize, ProbeDec);
    num_keys = (int) (TableSize * LoadFactor);

    /* build a table as starting point */
    build_table(test_table, num_keys);
    size = num_keys;

    key_range = MAXID - MINID + 1;
    /* in equilibrium make inserts and removes with equal probability */
    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    keys_added = keys_removed = 0;
    start = clock();
    for (i = 0; i < Trials; i++) {
        if (drand48() < 0.5 && table_full(test_table) == FALSE) {
            // insert only if table not full
            key = (hashkey_t) (drand48() * key_range) + MINID;
            ip = (int *) malloc(sizeof(int));
            *ip = key;
            /* insert returns 0 if key not found, 1 if older key found */
            if (Verbose) printf("Trial %d, Insert Key %d", i, key);
            code = table_insert(test_table, key, ip);
            if (code == 0) {
                /* key was not in table so added */
                unsuc_search += table_stats(test_table);
                unsuc_trials++;
                keys_added++;
                if (Verbose) printf(" added\n");
            } else if (code == 1) {
                suc_search += table_stats(test_table);
                suc_trials++;
                if (Verbose) printf(" replaced (rare!)\n");
            } else {
                printf("!!!Trial %d failed to insert key (%d) with code (%d)\n", i, key, code);
                exit(10);
            }
        } else if (table_entries(test_table) > TableSize/4) {
            // delete only if table is at least 25% full
            // why 25%?  Would 10% be better?  Lower than 10% will
            // be computationally expensive
            do {
                ran_index = (int) (drand48() * TableSize);
                key = table_peek(test_table, ran_index);
            } while (key == PEEK_NOKEY);
            if (Verbose) printf("Trial %d, Delete Key %d", i, key);
            if (key < MINID || MAXID < key)
            {
                printf("\n\n  table peek failed: invalid key (%d) during trial (%d)\n", key, i);
                exit(12);
            }
            dp = table_delete(test_table, key);
            if (dp != NULL) {
                if (Verbose) printf(" removed\n");
                suc_search += table_stats(test_table);
                suc_trials++;
                keys_removed++;
                assert(*(int *)dp == key);
                free(dp);
            } else {
                printf("!!! failed to find key (%d) in table, trial (%d)!\n", key, i);
                printf("this is a catastrophic error!!!\n");
                exit(11);
            }
        }
    }
    end = clock();

    if (Verbose) {
        printf("Table after equilibrium trials\n");
        table_debug_print(test_table);
    }

    size += keys_added - keys_removed;
    printf("  Keys added (%d), removed (%d) new size should be (%d) and is (%d)\n",
            keys_added, keys_removed, size, table_entries(test_table));
    assert(size == table_entries(test_table));
    printf("  After exercise, time=%g \n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    printf("  successful searches during exercise=%g, trials=%d\n", 
            (double) suc_search/suc_trials, suc_trials);
    printf("  unsuccessful searches during exercise=%g, trials=%d\n", 
            (double) unsuc_search/unsuc_trials, unsuc_trials);


    /* test access times for new table */

    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    start = clock();
    /* check each position in table for key */
    for (i = 0; i < TableSize; i++) {
        key = table_peek(test_table, i);
        if (key != PEEK_NOKEY) {
            assert(MINID <= key && key <= MAXID);
            dp = table_retrieve(test_table, key);
            if (dp == NULL) {
                printf("Failed to find key (%d) but it is in location (%d)\n", 
                        key, i);
                exit(16);
            } else {
                suc_search += table_stats(test_table);
                suc_trials++;
                assert(*(int *)dp == key);
            }
        }
    }
    for (i = 0; i < Trials; i++) {
        /* random key with uniform distribution */
        key = (hashkey_t) (drand48() * key_range) + MINID;
        dp = table_retrieve(test_table, key);
        if (dp == NULL) {
            unsuc_search += table_stats(test_table);
            unsuc_trials++;
        } else {
            // this should be very rare
            assert(*(int *)dp == key);
        }
    }
    end = clock();
    size = table_entries(test_table);
    printf("  After retrieve experiment, time=%g\n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    printf("  New load factor = %g\n", (double) size/TableSize);
    printf("  Percent empty locations marked deleted = %g\n",
            (double) 100.0 * table_deletekeys(test_table)
            / (TableSize - table_entries(test_table)));

    printf("   Measured avg probes for successful search=%g, trials=%d\n", 
            (double) suc_search/suc_trials, suc_trials);

    printf("   Measured avg probes for unsuccessful search=%g, trials=%d\n", 
            (double) unsuc_search/unsuc_trials, unsuc_trials);
    printf("    Do deletions increase avg number of probes?\n");
    performanceFormulas((double) size/TableSize);

    /* rehash and retest table */
    printf("  Rehash table\n");
    test_table = table_rehash(test_table, TableSize);
    /* number entries in table should not change */
    assert(size == table_entries(test_table));
    /* rehashing must clear all entries marked for deletion */
    assert(0 == table_deletekeys(test_table));

    /* test access times for rehashed table */

    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    start = clock();
    /* check each position in table for key */
    for (i = 0; i < TableSize; i++) {
        key = table_peek(test_table, i);
        if (key != PEEK_NOKEY) {
            assert(MINID <= key && key <= MAXID);
            dp = table_retrieve(test_table, key);
            if (dp == NULL) {
                printf("Failed to find key (%d) after rehash but it is in location (%d)\n", 
                        key, i);
                exit(26);
            } else {
                suc_search += table_stats(test_table);
                suc_trials++;
                assert(*(int *)dp == key);
            }
        }
    }
    for (i = 0; i < Trials; i++) {
        /* random key with uniform distribution */
        key = (hashkey_t) (drand48() * key_range) + MINID;
        dp = table_retrieve(test_table, key);
        if (dp == NULL) {
            unsuc_search += table_stats(test_table);
            unsuc_trials++;
        } else {
            // this should be very rare
            assert(*(int *)dp == key);
        }
    }
    end = clock();
    size = table_entries(test_table);
    printf("  After rehash, time=%g\n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    printf("   Measured avg probes for successful search=%g, trials=%d\n", 
            (double) suc_search/suc_trials, suc_trials);

    printf("   Measured avg probes for unsuccessful search=%g, trials=%d\n", 
            (double) unsuc_search/unsuc_trials, unsuc_trials);

    /* remove and free all items from table */
    table_destruct(test_table);

    printf("----- End of equilibrium test -----\n\n");
}

/* build a table with random keys.  The keys are generated with a uniform
 * distribution.  
 */
int build_random(table_t *T, int table_size, int num_addr)
{
    hashkey_t key;
    int i, range, code;
    int probes = 0;
    int *ip;
    range = MAXID - MINID + 1;
    for (i = 0; i < num_addr; i++) {
        key = (hashkey_t) (drand48() * range) + MINID;
        assert(MINID <= key && key <= MAXID);
        ip = (int *) malloc(sizeof(int));
        *ip = key;
        code = table_insert(T, key, ip);
        if (code == 1) {
            i--;   // since does not increase size of table
            // replaced.  The chances should be very small
            printf("during random build generated duplicate key (%d) on trial (%d)\n", key, i);
            printf("this should be unlikely: if see more than a few you have a problem\n");
        }
        else if (code != 0) {
            printf("build of random table failed code (%d) index (%d) key (%d)\n",
                    code, i, key);
            exit(2);
        }
        probes += table_stats(T);
    }
    return probes;
}

/* build a table with sequential keys.  The starting address is random.  The
 * keys are are in adjacent table locations.
 */
int build_seq(table_t *T, int table_size, int num_addr)
{
    hashkey_t key;
    int i, range, starting, code;
    int *ip;
    int probes = 0;
    range = MAXID - MINID + 1;
    starting = (int) (drand48() * range) + MINID;
    if (starting >= MAXID - table_size)
        starting -= table_size;
    for (i = starting; i < starting + num_addr; i++) {
        assert(MINID <= i && i <= MAXID);
        key = i;
        ip = (int *) malloc(sizeof(int));
        *ip = i;
        code = table_insert(T, key, ip);
        if (code != 0) {
            printf("build of sequential table failed code (%d) index (%d) key (%d)\n",
                    code, i - starting, key);
            exit(3);
        }
        probes += table_stats(T);
    }
    return probes;
}

/* build a table with folded keys.  The starting address is random.  The first
 * set of keys are sequential, and the second set hashes to the same table
 * locations as the first set.
 */
int build_fold(table_t *T, int table_size, int num_addr)
{
    int i, range, starting, code;
    int probes = 0;
    int *ip;
    range = MAXID - MINID + 1;
    starting = (int) (drand48() * range) + MINID;
    if (starting <= MINID + table_size)
        starting += table_size;
    if (starting >= MAXID - table_size)
        starting -= table_size;
    for (i = starting; i > starting - num_addr/2; i--) {
        assert(MINID <= i && i <= MAXID);
        ip = (int *) malloc(sizeof(int));
        *ip = i;
        code = table_insert(T, i, ip);
        if (code != 0) {
            printf("build of first phase of folded table failed code (%d) index (%d) key (%d)\n",
                    code, i - starting, i);
            exit(4);
        }
        probes += table_stats(T);
    }
    for (i = starting + table_size; i > starting + table_size - (num_addr+1)/2; i--) {
        assert(MINID <= i && i <= MAXID);
        ip = (int *) malloc(sizeof(int));
        *ip = i;
        code = table_insert(T, i, ip);
        if (code != 0) {
            printf("build of second phase of folded table failed code (%d) index (%d) key (%d)\n",
                    code, i - starting, i);
            exit(5);
        }
        probes += table_stats(T);
    }
    return probes;
}

/* build a table with worst keys.  Insert keys that hash to the same table
 * location.  Protects against invalid keys by wrapping around if the total
 * number of addresses times the table size is large.
 */
int build_worst(table_t *T, int table_size, int num_addr)
{
    hashkey_t key = MAXID;
    int i, batches = 0, code;
    int probes = 0;
    int *ip;
    for (i = 0; i < num_addr; i++) {
        assert(MINID <= key && key <= MAXID);
        ip = (int *) malloc(sizeof(int));
        *ip = key;
        code = table_insert(T, key, ip);
        if (code != 0) {
            printf("build of worst table failed: code (%d) index (%d) key (%d) batch (%d)\n",
                    code, i, key, batches);
            exit(6);
        }
        if (key < MINID + table_size) {
            batches++;
            printf("batch %d\n", batches);
            key = MAXID - batches;
        }
        else
            key -= table_size;
        probes += table_stats(T);
    }
    return probes;
}

/* return first prime number at number or greater
 *
 * There is at least one prime p such that n < p < 2n
 * for n>=25, n < p < 1.2n
 */
int find_first_prime(int number)
{
    int i, foundfactor;
    double upper;
    assert(number > 1);
    // if even move to next odd
    if (number % 2 == 0)
        number++;
    do {
        foundfactor = 0;      // assume number is prime
        upper = sqrt(number);
        for (i = 3; i < upper + 1; i += 2)
            if (number % i == 0) {
                foundfactor = 1;
                number += 2;  // only test odds
                break;
            }
    } while (foundfactor);
    return number;
}


/* print performance evaulation formulas from Standish pg. 479 and pg 484
 *
 * Added additional formulas for linear probing and sequential, folded, and
 * worst addressing.  Also, for quadratic with worst addressing since it
 * behaves the same as linear in this case.  Formulas for the other cases
 * are unknown.
 */
void performanceFormulas(double load_factor)
{
    int n = TableSize * load_factor;
    if (TableType == RAND) {
        if (ProbeDec == LINEAR) {
            printf("--- Linear probe sequence performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    0.5 * (1.0 + 1.0/(1.0 - load_factor)));
            printf("    Expected probes for unsuccessful search %g\n",
                    0.5 * (1.0 + pow(1.0/(1.0 - load_factor),2)));
        }
        else if (ProbeDec == DOUBLE) {
            printf("--- Double hashing performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    (1.0/load_factor) * log(1.0/(1.0 - load_factor)));
            printf("    Expected probes for unsuccessful search %g\n",
                    1.0/(1.0 - load_factor));
        }
        else if (ProbeDec == QUAD) {
            printf("--- Quadratic probe sequence performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    1.0 - log(1.0 - load_factor) - load_factor/2.0);
            printf("    Expected probes for unsuccessful search %g\n",
                    1.0/(1.0 - load_factor) - load_factor - log(1.0 - load_factor));
        }
    }
    else if (TableType == SEQ) {
        if (ProbeDec == LINEAR) {
            printf("--- Linear probe sequence performance formulas ---\n");
            printf("    Expected probes for successful search 1\n");
            printf("    Expected probes for unsuccessful search %g\n",
                    n * load_factor / 2.0 + load_factor/2.0 + 1);
        }
    }
    else if (TableType == FOLD) {
        if (ProbeDec == LINEAR) {
            printf("--- Linear probe sequence performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    n / 4.0 + 1);
            printf("    Expected probes for unsuccessful search %g\n",
                    n * load_factor / 2.0 + load_factor/2.0 + 1);
        }
    }
    else if (TableType == WORST) {
        if (ProbeDec == LINEAR) {
            printf("--- Linear probe sequence performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    n / 2.0 + 0.5);
            printf("    Expected probes for unsuccessful search %g\n",
                    n * load_factor / 2.0 + load_factor/2.0 + 1);
        }
        else if (ProbeDec == QUAD) {
            printf("--- Quadratic probe sequence performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    n / 2.0 + 0.5);
        }
    }
}

/* read in command line arguments and store in global variables for easy
 * access by other functions.
 */
void getCommandLine(int argc, char **argv)
{
    /* optopt--if an unknown option character is found
     * optind--index of next element in argv 
     * optarg--argument for option that requires argument 
     * "x:" colon after x means argument required
     */
    int c;
    int index;

    while ((c = getopt(argc, argv, "m:a:h:i:t:s:p:erbvu:")) != -1)
        switch(c) {
            case 'm': TableSize = atoi(optarg);      break;
            case 'a': LoadFactor = atof(optarg);     break;
            case 's': Seed = atoi(optarg);           break;
            case 't': Trials = atoi(optarg);         break;
            case 'v': Verbose = TRUE;                break;
            case 'e': EquilibriumTest = TRUE;        break;
            case 'r': RetrieveTest = TRUE;           break;
            case 'b': RehashTest = TRUE;             break;
            case 'p': TwoSumTest = atoi(optarg);     break;
            case 'u': UnitInt = atoi(optarg);     break;
            case 'h':
                      if (strcmp(optarg, "linear") == 0)
                          ProbeDec = LINEAR;
                      else if (strcmp(optarg, "double") == 0)
                          ProbeDec = DOUBLE;
                      else if (strcmp(optarg, "quad") == 0)
                          ProbeDec = QUAD;
                      else {
                          fprintf(stderr, "invalid type of probing decrement: %s\n", optarg);
                          fprintf(stderr, "must be {linear | double | quad}\n");
                          exit(1);
                      }
                      break;
            case 'i':
                      if (strcmp(optarg, "rand") == 0)
                          TableType = RAND;
                      else if (strcmp(optarg, "seq") == 0)
                          TableType = SEQ;
                      else if (strcmp(optarg, "fold") == 0)
                          TableType = FOLD;
                      else if (strcmp(optarg, "worst") == 0)
                          TableType = WORST;
                      else {
                          fprintf(stderr, "invalid type of address generation: %s\n", optarg);
                          fprintf(stderr, "must be {rand | seq | fold | worst}\n");
                          exit(1);
                      }
                      break;
            case '?':
                      if (isprint(optopt))
                          fprintf(stderr, "Unknown option %c.\n", optopt);
                      else
                          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                      printf("Lab6 command line options\n");
                      printf("General options ---------\n");
                      printf("  -m 11     table size\n");
                      printf("  -a 0.9    load factor\n");
                      printf("  -h linear|double|quad\n");
                      printf("            Type of probing decrement\n");
                      printf("  -r        run retrieve test driver \n");
                      printf("  -b        run basic test driver \n");
                      printf("  -p x      run Two Sums Problem driver for x=1,2,3, or 4\n");
                      printf("                 use -m array size -t test trials\n");
                      printf("  -e        run equilibrium test driver\n");
                      printf("  -i rand|seq|fold|worst\n");
                      printf("            type of keys for retrieve test driver \n");
                      printf("\nOptions for drivers ---------\n");
                      printf("  -t 50000  number of trials in drivers\n");
                      printf("  -v        turn on verbose prints (default off)\n");
                      printf("  -s 26214  seed for random number generator\n");
                      exit(1);
        }
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
}

/* vi:set ts=8 sts=4 sw=4 et: */

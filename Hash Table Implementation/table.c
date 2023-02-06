/* table.c
 * Nish Patel
 * Fall 2022
 *
 * Propose: Contains the functions needed to create a hash table. Also has some functions which help with 
 * displaying the hash table as well as displaying some stats about it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "table.h"
#define EMPTYKEY 1000000001
#define DELETEKEY 1000000002

int H(hashkey_t K, table_t *T){ //CREATED HASH FUNCTION
    int hashval = abs(K) % T->table_size_M; //given hash function
    return hashval;
}

int P(hashkey_t K, table_t *T){ //CREATED PROBE DECREMENT FUNCTION
    if(T->type_of_probing_used_for_this_table == LINEAR){ //linear probing
        return 1;
    }
    else if(T->type_of_probing_used_for_this_table == DOUBLE){ //double probing
        int len = sizeof(K); //uses given equation
        unsigned char *p = (unsigned char *) &K;
        unsigned h = 0;
        for (int i = 0; i < len; i++)
        {
            h = 33 * h ^ p[i];
        }
        int temp = h%T->table_size_M;
        if(temp == 0){ //ensures decrement is atleast 1
            temp = 1;
        }
        return temp;
    }
    else if(T->type_of_probing_used_for_this_table == QUAD){
        return -1; //so probing can start at first index needed.
    }
    return 1;
}

table_t *table_construct(int table_size, int probe_type){
    table_t *table = malloc(sizeof(table_t)); //mallocs table
    table->oa = malloc(sizeof(table_entry_t) * table_size); //mallocs oa

    for(int i = 0; i < table_size; i++){
        table->oa[i].key = EMPTYKEY; //gives values to all keys in oa
    } 

    table->table_size_M = table_size; //sets values for table info
    table->type_of_probing_used_for_this_table = probe_type;
    table->num_keys_stored_in_table = 0;
    table->num_probes_for_most_recent_call = 0;
    table->deletedkeys = 0;

    return table;
}

table_t *table_rehash(table_t *T, int new_table_size){
    table_t *newtable = table_construct(new_table_size, T->type_of_probing_used_for_this_table);
    for(int i = 0; i < T->table_size_M; i++){ //goes and reinserts nonempty or not deleted keys into the new table
        if(T->oa[i].key != EMPTYKEY && T->oa[i].key != DELETEKEY){
            table_insert(newtable, T->oa[i].key, T->oa[i].data_ptr);
            T->oa[i].data_ptr = NULL; //nullifies the data pointers from the old table
        }
    }
    //table_destruct(T);
    return newtable;
}  

int table_entries(table_t *T){
    return T->num_keys_stored_in_table; //returns number of keys in table
}

int table_full(table_t *T){

    if(T->num_keys_stored_in_table == T->table_size_M-1){ //checks and returns if the table is full
        return 1;
    }
    return 0;
}

int table_deletekeys(table_t *T){
    return T->deletedkeys; //returns number of deleted keys
}

int table_insert(table_t *T, hashkey_t K, data_t I){
    T->num_probes_for_most_recent_call = 0;
    // if(table_full(T) == 1){
    //     return -1;
    // }

    int index = H(K,T);
    int probedec = P(K,T);
    int deleteslot = -1;
    while((T->oa[index].key != EMPTYKEY)){//loops until valid empty key is found
        T->num_probes_for_most_recent_call++;
        if(T->oa[index].key == K){
            T->num_probes_for_most_recent_call++;
            free(T->oa[index].data_ptr);
            T->oa[index].data_ptr = I;
            return 1; //sent when a key is replaced with a new data_ptr
        }
        if(T->oa[index].key == DELETEKEY && deleteslot == -1){
            deleteslot = index; //goes back to last seen deleted key if there were no empty keys
        }
        if(T->type_of_probing_used_for_this_table == QUAD){
            probedec++; //used for quadratic probing
        }
        index -= probedec; //decrements
        if(index < 0){
            index += T->table_size_M; //loops around the table
        }
        if(T->num_probes_for_most_recent_call > T->table_size_M){
            break;
        }
    }
    if(table_full(T) == 1){
        return -1; //returns -1 if the list is full
    }
    if(deleteslot != -1){
        T->deletedkeys--;
        assert(0 <= deleteslot && deleteslot < T->table_size_M);
        index = deleteslot;
    }
    // else if(T->oa[index].key == DELETEKEY){
    //     T->deletedkeys--;
    // }
    T->num_probes_for_most_recent_call++;
    T->oa[index].key = K;
    T->oa[index].data_ptr = I;
    T->num_keys_stored_in_table++;
    return 0; //returns 0 when a new key replaces a previously empty key or deleted key
}

data_t table_delete(table_t *T, hashkey_t K){
    T->num_probes_for_most_recent_call = 0;
    int index = H(K,T);
    int probedec = P(K,T);

    table_entry_t probe;
    //probe = malloc(sizeof(probe));
    //probe->key = malloc(sizeof(sizeof(hashkey_t)));
    probe.key = T->oa[index].key; //gets initial index value
    probe.data_ptr = T->oa[index].data_ptr;
    while((probe.key != K) && (probe.key != EMPTYKEY)){ //loops until the valid key or empty key is found
        T->num_probes_for_most_recent_call++;
        if(T->type_of_probing_used_for_this_table == QUAD){
            probedec++; //used for quad probing to decrement
        }
        index -= probedec;
        if(index < 0){
            index += T->table_size_M;
        }
        probe.key = T->oa[index].key;
        probe.data_ptr = T->oa[index].data_ptr;
        if(T->num_probes_for_most_recent_call > T->table_size_M){
            return NULL;
        }
    }
    if(probe.key == EMPTYKEY){ //returns NULL since no key was found
        T->num_probes_for_most_recent_call++;
        return NULL;
    }
    else{
        T->num_probes_for_most_recent_call++;
        T->oa[index].key = DELETEKEY;
        T->oa[index].data_ptr = NULL;
        T->num_keys_stored_in_table--;
        T->deletedkeys++;
        return probe.data_ptr; //return data_ptr of the key that was meant to be deleted
    }
    return NULL;
}

data_t table_retrieve(table_t *T, hashkey_t K){
    T->num_probes_for_most_recent_call = 0;
    int index = H(K,T);
    int probedec = P(K,T);

    table_entry_t probe;
    //probe = malloc(sizeof(probe));
    //probe->key = malloc(sizeof(sizeof(hashkey_t)));
    probe.key = T->oa[index].key;
    probe.data_ptr = T->oa[index].data_ptr;
    while((probe.key != K) && (probe.key != EMPTYKEY)){ //loops until empty key or wanted key is found
        T->num_probes_for_most_recent_call++;
        if(T->type_of_probing_used_for_this_table == QUAD){ //used for quad probing
            probedec++;
        }
        index -= probedec;
        if(index < 0){ //loops through the table
            index += T->table_size_M;
        }
        probe.key = T->oa[index].key;
        probe.data_ptr = T->oa[index].data_ptr;
        if(T->num_probes_for_most_recent_call > T->table_size_M){
            probe.key = EMPTYKEY;
            break; //breaks once all index have been searched
        }
    }
    if(probe.key == EMPTYKEY || probe.key == DELETEKEY){
        T->num_probes_for_most_recent_call++;
        return NULL; //returns null since the key was not found
    }
    else{
        T->num_probes_for_most_recent_call++;
        return probe.data_ptr; //returns data_ptr value of key that was searched for
    }
    return NULL;
}

void table_destruct(table_t *T){
    for(int i = 0; i < T->table_size_M; i++){ //loops and frees each data_ptr in oa
            if(T->oa[i].data_ptr != NULL){
                free(T->oa[i].data_ptr);
            }
    }
    free(T->oa); //frees oa
    free(T); //frees the table last
}

int table_stats(table_t *T){
    return T->num_probes_for_most_recent_call; //returns the number of comparisons that were done in the last used function
}

hashkey_t table_peek(table_t *T, int index){
    assert(0 <= index && index < T->table_size_M);

    if(T->oa[index].data_ptr != NULL){
        return T->oa[index].key; //returns key if the given index has a nonempty or not deleted data_ptr
    }
    else{
        return (hashkey_t) INT_MAX; //otherwise returns INT_MAX
    }
}

void table_debug_print(table_t *T){
    for(int i = 0; i < T->table_size_M; i++){ //loops through each index and prints the key value
        table_entry_t entry = T->oa[i];
        if(entry.key == EMPTYKEY){
            printf("%d: EMPTY KEY\n", i);
        }
        else if(entry.key == DELETEKEY){
            printf("%d: DELETED KEY\n", i);
        }
        else{
            printf("%d: %d\n", i, entry.key);
        }
    }
}


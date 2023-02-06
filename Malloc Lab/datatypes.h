/* datatypes.h 
 * Dynamic Memory Allocation
 * Fall 2022
 *
 * data_t: The type of data that we want to store in the linked list
 */

/* include the definitions of the data we want to store in the list */

/* a dummy definition as MP4 creates arrays with a dynamic size */
typedef struct char_tab {
    char c;
} char_t;

/* the list ADT works on data of this type */
typedef char_t data_t;

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

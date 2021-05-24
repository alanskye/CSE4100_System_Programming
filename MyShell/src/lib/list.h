#ifndef __MYSH_LIST_H
#define __MYSH_LIST_H

// C++ STL Style Linked List from pintos project, modified by 20181677

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

struct list_elem {
    struct list_elem *prev;
    struct list_elem *next;
};

struct list {
    struct list_elem head;
    struct list_elem tail;
};

/* It returns the pointer of the given user-defined struct by
 * subtracting the offset of list_elem from the head of the struct.
 */

#define list_entry(LIST_ELEM, STRUCT, MEMBER)       \
        ((STRUCT*) ((uint8_t*) &(LIST_ELEM)->next   \
        - offsetof (STRUCT, MEMBER.next)))

// list initializer
void list_init(struct list*);

struct list_elem* list_begin(struct list*);
struct list_elem* list_end(struct list*);
struct list_elem* list_rbegin(struct list*);

// list iteration
struct list_elem* list_next(struct list_elem*);

// 안전한 insert를 위한 함수들
struct list_elem* list_head(struct list*);
struct list_elem* list_tail(struct list*);

// list에 원소 삽입
void list_insert(struct list_elem*, struct list_elem*);
void list_push_front(struct list*, struct list_elem*);
void list_push_back(struct list*, struct list_elem*);

struct list_elem* list_front(struct list*);

// remove list elements
struct list_elem* list_remove(struct list_elem*);
struct list_elem* list_pop_front(struct list*);

// utility functions
size_t list_size(struct list*);
bool list_empty(struct list*);

#endif

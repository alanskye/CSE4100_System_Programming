#include "list.h"

/* utility functions to prevent the list handling function from 
 * falling into undefined behaviors
 */

static inline bool is_head(struct list_elem *elem) {
    return elem != NULL && elem->prev == NULL && elem->next != NULL;
}

static inline bool is_tail(struct list_elem *elem) {
    return elem != NULL && elem->prev != NULL && elem->next == NULL;
}

static inline bool is_interior(struct list_elem *elem) {
    return elem != NULL && elem->prev != NULL && elem->next != NULL;
}

// list initializer
void list_init(struct list* list) {
    assert(list != NULL);
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

struct list_elem* list_begin(struct list* list) {
    assert(list != NULL);
    return list->head.next;
}

struct list_elem* list_end(struct list* list) {
    assert(list != NULL);
    return &list->tail;
}

struct list_elem* list_rbegin(struct list* list) {
    assert(list != NULL);
    return list->tail.prev;
}

struct list_elem* list_next(struct list_elem* elem) {
    assert(is_head(elem) || is_interior(elem));
    return elem->next;
}

struct list_elem* list_head(struct list* list) {
    assert(list != NULL);
    return &list->head;
}

struct list_elem* list_tail(struct list* list) {
    assert(list != NULL);
    return &list->tail;
}

/* inserts [old_elem] just before [old_elem], which may be either an interior
 * element or tail. The latter case is same as list_push_back()
 */

void list_insert(struct list_elem* old_elem, struct list_elem* new_elem) {
    assert(is_interior(old_elem) || is_tail(old_elem));
    assert(new_elem != NULL);
    new_elem->prev = old_elem->prev;
    new_elem->next = old_elem;
    old_elem->prev->next = new_elem;
    old_elem->prev = new_elem;
}

void list_push_front(struct list* list, struct list_elem* elem) {
    list_insert(list_begin(list), elem);
}

void list_push_back(struct list* list, struct list_elem* elem) {
    list_insert(list_end(list), elem);
}

struct list_elem* list_front(struct list* list) {
    assert(!list_empty(list));
    return list->head.next;
}

struct list_elem* list_remove(struct list_elem *elem) {
    assert(is_interior(elem));
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    return elem->next;
}

struct list_elem* list_pop_front(struct list* list) {
    struct list_elem* front = list_front(list);
    list_remove(front);
    return front;
}

size_t list_size(struct list* list) {
    size_t ret = 0;
    struct list_elem *it;
    for (   it = list_begin(list);
            it != list_end(list);
            it = list_next(it)
        )
    {
        ret++;
    }
    return ret;
}

bool list_empty(struct list* list) {
    return list_begin(list) == list_end(list);
}


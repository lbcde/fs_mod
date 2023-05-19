#ifndef LIST_H
#define LIST_H
#include <stddef.h>

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head* n,struct list_head *prev,struct list_head *next) {
	next->prev = n;
	n->next = next;
	n->prev = prev;
	prev->next = n;
}

static inline void timer_list_addxxx(struct list_head *n, struct list_head *head) {
	__list_add(n, head, head->next);
}

static inline void list_add_tail(struct list_head *n, struct list_head *head) {
	__list_add(n, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry) {
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

static inline int list_empty(const struct list_head *head) {
	return head->next == head;
}

#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_for_each_entry_safe(pos, n, head, member)              \
    for (pos = list_entry((head)->next, __typeof__(*pos), member),  \
         n = list_entry(pos->member.next, __typeof__(*pos), member); \
         &pos->member != (head);                                    \
         pos = n, n = list_entry(n->member.next, __typeof__(*n), member))

#endif

#include "ch.h"
#include "hal.h"
#include "emlist.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// TODO would a static array backed implementation be useful, too?

LinkedList* emlist_create(void) {
    LinkedList* list = (LinkedList*) chHeapAlloc(NULL,sizeof(LinkedList));

    chDbgCheck(list!=NULL);
    emlist_initialize(list);
    return list;
}

void emlist_destroy(LinkedList* list, bool freeContainedValue) {
    if(list != NULL) {
        emlist_deinitialize(list,freeContainedValue);
        chHeapFree(list);
    }
}

void emlist_deinitialize(LinkedList* list, bool freeContainedValue) {
    while(!emlist_is_empty(list)) {
    	emlist_freeHeadItemAndValue(list,freeContainedValue);
    }
}

void emlist_initialize(LinkedList* list) {
    list->head = NULL;
    list->last = NULL;
}

bool emlist_contains(LinkedList* list, void* value) {
    LinkedListIterator iterator = emlist_iterator(list);
    LinkedListElement* candidate = NULL;

    while((candidate = emlist_iterator_next(&iterator)) != NULL) {
        if(candidate->value == value) {
            return true;
        }
    }
    return false;
}
//This is a simple list insert. it inserts at the end of the list; not in between two elements
bool emlist_insert(LinkedList* list, void* value) {
    LinkedListElement* element = (LinkedListElement*) chHeapAlloc(NULL,sizeof(LinkedListElement));

    if(element != NULL) {
        element->value = value;
        element->next = NULL;
        element->prev = NULL;
        list->last = element;
        if(emlist_is_empty(list)) {
            list->head = element;
        } else {
            LinkedListIterator iterator = emlist_iterator(list);
            LinkedListElement* current = NULL;
            //loops until the end is found
            while((current = emlist_iterator_next(&iterator)) != NULL && current->next != NULL) {
                continue;
            }
            if(current != NULL) {
                current->next = element;
                element->prev = current;
            }
        }
        return true;
    }
    return false;
}

bool emlist_remove(LinkedList* list, void* value) {
    LinkedListElement* prev = NULL;
    LinkedListElement* next = NULL;
    LinkedListIterator iterator = emlist_iterator(list);

    while((next = emlist_iterator_next(&iterator)) != NULL) {
        if(next->value == value) {
            if(prev == NULL) {
                list->head = next->next;
            } else {
                prev->next = next->next;
            }
            if ( next->next == NULL )
            	list->last = prev;
            chHeapFree(next);
            return true;
        }
        prev = next;
    }
    return false;
}

void* emlist_pop(LinkedList* list) {
	if ( list == NULL || emlist_is_empty(list))
		return NULL;
    LinkedListElement* element = list->head;
    if(list->head != NULL) {
        list->head = list->head->next;
    }
    void* value = element->value;
    chHeapFree(element);
    return value;
}

void emlist_freeHeadItemAndValue(LinkedList* list, bool freeContainedValue) {
	if ( list == NULL || emlist_is_empty(list))
		return;
    LinkedListElement* element = list->head;
    if( list->head != NULL) {
        list->head = list->head->next;
    }

    if ( freeContainedValue && element->value != NULL ){
    	chHeapFree(element->value);
    	element->value = NULL;
    }
    chHeapFree(element);
    return;
}



int emlist_size(LinkedList* list) {
    int size = 0;
    LinkedListIterator iterator = emlist_iterator(list);
    LinkedListElement* element = NULL;
    while((element = emlist_iterator_next(&iterator)) != NULL) {
        ++size;
    }
    return size;
}

bool emlist_is_empty(LinkedList* list) {
    return list != NULL && list->head == NULL;
}

void *emlist_iterate_over(LinkedList* list, emlist_process_item_cb cb, void *arg){
	LinkedListElement *currentItem = emlist_iterate_over2(list,cb,arg);
	return currentItem!=NULL?currentItem->value:NULL;;
}
LinkedListElement *emlist_iterate_over2(LinkedList* list, emlist_process_item_cb cb, void *arg){
	LinkedListIterator iterator     = emlist_iterator(list);
	LinkedListElement  *currentItem = NULL;

	while( (currentItem=emlist_iterator_next(&iterator)) != NULL){
		if ( cb(currentItem, arg) != NULL)
			break;
	}
	return currentItem!=NULL?currentItem:NULL;;
}

LinkedListIterator emlist_iterator(LinkedList* list) {
    LinkedListIterator iterator;
    iterator.next = list != NULL ? list->head : NULL;
    return iterator;
}

LinkedListElement* emlist_iterator_next(LinkedListIterator* iterator) {
    LinkedListElement* next = NULL;
    if(iterator != NULL) {
        next = iterator->next;
        iterator->next = next != NULL ? next->next : NULL;
    }
    return next;
}

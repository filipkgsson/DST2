#include "kernel.h"

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

listobj *alloc_obj(uint nData);
list *alloc_list();
void print_list(list *prTemp); 
void add_Sort(list **prRefList, listobj *prObj);
void remove_first(list **prRefList);
listobj *remove_obj(list **prRefList, listobj *pBlock);





#endif
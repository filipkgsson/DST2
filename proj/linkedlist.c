#include "linkedlist.h"
#include "kernel.h"

/*void main(void)
{
  struct list * list = alloc_list();

  add_Sort(&list, alloc_obj(8));
  add_Sort(&list, alloc_obj(6));
  add_Sort(&list, alloc_obj(2));
  add_Sort(&list, alloc_obj(5));  
  add_Sort(&list, alloc_obj(3));

  //remove_first(list);
  print_list(list);
}*/

listobj *alloc_obj(uint nData)
{
  /* Pointer to a list object */
  listobj *prListObj;
  
  /* Allocate memory for a list object */
  //prListObj = (struct l_obj *) calloc(1, sizeof(struct l_obj));
  prListObj = (listobj *) calloc(1, sizeof(listobj));
  
  /* In case memory cant be allocated exit, else assign data and return pointer */
  if (prListObj == NULL) {
    printf("Error: out of memory\n");
    exit(-1);
    return NULL;
  }
  else{
    prListObj->pTask->DeadLine = nData;
    prListObj->pNext = NULL;
    prListObj->pPrevious = NULL;
    return prListObj;
  }
}

list *alloc_list()
{
  /* Pointer to a list object */
  list *prListObj;
  
  /* Allocate memory for a list object */
  prListObj = (list *) calloc(1, sizeof(list));
  
  /* In case memory cant be allocated exit, else assign data and return pointer */
  if (prListObj == NULL) {
    //printf("Error: out of memory\n");
    //exit(-1);
    return NULL;
  }
  else{
    prListObj->pHead = NULL;
    prListObj->pTail = NULL;
    return prListObj;
  }
}


void print_list(list *prTemp)
{
  /*listobj *prTemp2;
  //Operate on the temporary pointer not on the list
  prTemp2 = prTemp->pHead;
  while (prTemp2 != NULL) {
    printf("%d\n", prTemp2->nInfo);
    prTemp2 = prTemp2->next;
  }*/
}

void add_Sort(list **prRefList, listobj *prObj)
{
  listobj *tmp = (*prRefList)->pHead;
  
  if (tmp != NULL) {
    while (tmp->pNext != NULL && prObj->pTask->DeadLine > tmp->pNext->pTask->DeadLine) {
      tmp = tmp->pNext;
    }
    if (tmp->pNext != NULL && prObj->pTask->DeadLine > tmp->pTask->DeadLine) { // mitten av listan
      prObj->pNext = tmp->pNext;
      (tmp->pNext)->pPrevious = prObj;
      tmp->pNext = prObj;
      prObj->pPrevious = tmp;
    }
    else if (tmp->pNext == NULL && (*prRefList)->pHead != (*prRefList)->pTail) { //Sist i listan
      tmp->pNext = prObj;
      prObj->pPrevious = tmp;
      (*prRefList)->pTail = prObj;
    }
    else { //Först i listan
      tmp->pPrevious = prObj;
      prObj->pNext = tmp;
      (*prRefList)->pHead = prObj;
    }
  }
  else {
    /* List was empty, so the new list _is_ the node provided */
    (*prRefList)->pHead = prObj;
    (*prRefList)->pTail = prObj;
  }
}

void remove_first(list **prRefList)
{
  if ((*prRefList)->pHead->pNext == NULL) {
    (*prRefList)->pHead = (*prRefList)->pTail = NULL;
  }
  else {
    (*prRefList)->pHead = (*prRefList)->pHead->pNext;
  }
}

listobj *remove_obj(list **prRefList, listobj *pBlock){
  listobj *tmp = (*prRefList)->pHead;
  while(tmp->pNext != NULL){
    if(tmp == pBlock){
      pBlock->pNext->pPrevious = pBlock->pPrevious;
      pBlock->pPrevious->pNext = pBlock->pNext;
      return pBlock;
    }
    tmp = tmp->pNext;
  }
  return NULL;
}
  
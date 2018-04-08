// dlist.c

#include "dlist.h"

list * create_list()
{
	list * mylist = (list *)calloc(1, sizeof(list));
	if (mylist == NULL) {
		return NULL;
	}

	mylist->pHead = (listobj *)calloc(1, sizeof(listobj));
	if (mylist->pHead == NULL) {
		free(mylist);
		return NULL;
	}

	mylist->pTail = (listobj *)calloc(1, sizeof(listobj));
	if (mylist->pTail == NULL) {
		free(mylist->pHead);
		free(mylist);
		return NULL;
	}
	mylist->pHead->pPrevious = mylist->pHead;
	mylist->pHead->pNext = mylist->pTail;
	mylist->pTail->pPrevious = mylist->pHead;
	mylist->pTail->pNext = mylist->pTail;
	return mylist;
}


listobj * create_listobj(int num)
{
	listobj * myobj = (listobj *)calloc(1, sizeof(listobj));
	if (myobj == NULL)
	{
		return NULL;
	}
	myobj->nTCnt = num;
	return (myobj);
}

void insert_first(list * mylist, listobj * pObj)
{
	// insert first in list
	listobj *pMarker;
	pMarker = mylist->pHead;

	/* Position found, insert element */
	pObj->pNext = pMarker->pNext;
	pObj->pPrevious = pMarker;
	pMarker->pNext = pObj;
	pObj->pNext->pPrevious = pObj;
}

void insert(list *prRefList, listobj *prObj)
{
  listobj *tmp = prRefList->pHead;
  
  if (tmp->pNext != prRefList->pTail) {
    while (tmp->pNext != prRefList->pTail && prObj->pTask->DeadLine > tmp->pNext->pTask->DeadLine) {
      tmp = tmp->pNext;
    }
    if (tmp->pNext != prRefList->pTail && prObj->pTask->DeadLine > tmp->pTask->DeadLine) { // mitten av listan
      prObj->pNext = tmp->pNext;
      (tmp->pNext)->pPrevious = prObj;
      tmp->pNext = prObj;
      prObj->pPrevious = tmp;
    }
    else if (tmp->pNext == prRefList->pTail) { //Sist i listan
      tmp->pNext = prObj;
      prObj->pPrevious = tmp;
      prObj->pNext = prRefList->pTail;
      prRefList->pTail->pPrevious = prObj;
    }
    else { //Först i listan
      prObj->pPrevious = tmp;
      prObj->pNext = tmp->pNext;
      tmp->pNext->pPrevious = prObj; //Här
      tmp->pNext = prObj;
    }
  }
  else {
    /* List was empty, so the new list _is_ the node provided */
    prRefList->pHead->pNext = prObj;
    prRefList->pTail->pPrevious = prObj;
    prObj->pNext = prRefList->pTail;
    prObj->pPrevious = prRefList->pHead;
  }
}

listobj *
extract(listobj *pObj)
{
	pObj->pPrevious->pNext = pObj->pNext;
	pObj->pNext->pPrevious = pObj->pPrevious;
	pObj->pNext = pObj->pPrevious = NULL;
	return (pObj);
}

#include "kernel.h"
#include "stdio.h"
#include "string.h"

list *readylist;
list *waitinglist;
list *timerlist;
uint kernelRun = !RUNNING;
TCB *Running;
uint tickcounter;


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

	// Position found, insert element 
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
    // List was empty, so the new list _is_ the node provided
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

void IdleTask(void){
  while(TRUE){
    SaveContext();
    TimerInt();
    LoadContext();
  }
}

int init_kernel(){
  void (*pIdleTask)(void) = &IdleTask;
  tickcounter = 0;
  readylist = create_list();
  waitinglist = create_list();
  timerlist = create_list();
  if(readylist == NULL || waitinglist == NULL || timerlist == NULL){
    return FAIL;
  }
  if(create_task(pIdleTask, 0xFFFFFFFF) == FAIL){
    return FAIL;
  }
  return OK;
}

exception create_task(void (* body)(), uint d){
  volatile int first;
  TCB *block;
  listobj *node;
  first = TRUE;
  block = calloc(1, sizeof(TCB));
  node = create_listobj(0);
  if(node == NULL || block == NULL){
    free(block);
    free(node);
    return FAIL;
  }
  node->pTask = block;
  block->DeadLine = d;
  block->SP = &(block->StackSeg[STACK_SIZE - 1]);
  block->SPSR = 0;
  block->PC = body;
  if(kernelRun == !RUNNING){
    insert(readylist, node);
    Running = readylist->pHead->pNext->pTask;
  }
  else{  
    isr_off();
    SaveContext();
    if(first){
      first = FALSE;
      insert(readylist, node);
      Running = readylist->pHead->pNext->pTask;
      LoadContext();
    }
  }
  return OK;
}

void run(){
  //timer0_start();
  kernelRun = RUNNING;
  isr_on();
  LoadContext();
}

void terminate(){
  listobj *obj;
  obj = extract(readylist->pHead->pNext);
  free(obj->pTask);
  free(obj);
  Running = readylist->pHead->pNext->pTask;
  LoadContext();
}

mailbox* create_mailbox(uint nMessages, uint nDataSize){
  mailbox *prMailbox;
  prMailbox = (mailbox*)calloc(1, sizeof(mailbox));
  prMailbox->pHead = (msg*)calloc(1, sizeof(msg));
  prMailbox->pTail = (msg*)calloc(1, sizeof(msg));
  if(prMailbox == NULL || prMailbox->pHead == NULL || prMailbox->pTail == NULL){
    free(prMailbox->pHead);
    free(prMailbox->pTail);
    free(prMailbox);
    return NULL;
  }
  prMailbox->pHead->pNext = prMailbox->pTail;
  prMailbox->pTail->pPrevious = prMailbox->pHead;
  prMailbox->nDataSize = nDataSize;
  prMailbox->nMaxMessages = nMessages;
  prMailbox->nMessages = 0;
  prMailbox->nBlockedMsg = 0;
  return prMailbox;
}

exception remove_mailbox(mailbox *mBox){
  if(mBox->pHead->pNext == mBox->pTail){
    free(mBox->pHead);
    free(mBox->pTail);
    free(mBox);
    return OK;
  }
  else{
    return NOT_EMPTY;
  }
}

exception send_wait(mailbox *mBox, void *Data){
  volatile int first;
  msg *message;
  msg *temp;
  first = TRUE;
  isr_off();
  SaveContext();
  if(first){
    first = FALSE;
    if(mBox->nMessages <= RECEIVER){
      memcpy(mBox->pHead->pNext->pData, Data, sizeof(char)); //dest src size
      message = mBox->pHead->pNext;
      mBox->pHead->pNext = message->pNext;
      mBox->pHead->pNext->pPrevious = message->pPrevious;
      mBox->nMessages ++;
      if(message->pBlock != NULL && waitinglist->pHead->pNext != waitinglist->pTail){
        insert(readylist, extract(waitinglist->pHead->pNext));
        Running=readylist->pHead->pNext->pTask;
        free(message);
      }
    }
    else{
      message = (msg*)calloc(1, sizeof(msg));
      if(message == NULL){
        free(message);
        return FAIL;
      }
      message->pData = Data;
      if(mBox->nMessages >= mBox->nMaxMessages){
        temp = mBox->pHead->pNext;
        temp->pNext->pPrevious = mBox->pHead;
        mBox->pHead->pNext = temp->pNext;
        mBox->nMessages --;
        free(temp); 
      }
      mBox->pTail->pPrevious->pNext = message;
      message->pPrevious = mBox->pTail->pPrevious;
      message->pNext = mBox->pTail;
      mBox->pTail->pPrevious = message;
      readylist->pHead->pNext->pMessage = message;
      mBox->nMessages ++;
      message->pBlock = readylist->pHead->pNext;
      insert(waitinglist, extract(readylist->pHead->pNext));
      Running=readylist->pHead->pNext->pTask;
    }
    LoadContext();
    return FAIL;
  }
  else{
    if(readylist->pHead->pNext->pTask->DeadLine <= ticks()){
      isr_off();
      message = readylist->pHead->pNext->pMessage;
      message->pNext->pPrevious = message->pPrevious;
      message->pPrevious->pNext = message->pNext;
      mBox->nMessages --;
      free(message);
      isr_on();
      return DEADLINE_REACHED;
    }
    else{
      return OK;
    }
  }
}

exception receive_wait(mailbox *mBox, void *Data){
  volatile int first;
  msg *message;
  msg *temp;
  first = TRUE;
  isr_off();
  SaveContext();
  if(first){
    first = FALSE;
    if(mBox->nMessages >= SENDER){
      memcpy(Data, mBox->pHead->pNext->pData, sizeof(char)); //dest src size
      message = mBox->pHead->pNext;
      mBox->pHead->pNext = message->pNext;
      mBox->pHead->pNext->pPrevious = message->pPrevious;
      mBox->nMessages --;
      if(message->pBlock != NULL && waitinglist->pHead->pNext != waitinglist->pTail){
        insert(readylist, extract(waitinglist->pHead->pNext));
        Running=readylist->pHead->pNext->pTask;
        free(message);
      }
      else{
        free(message->pData);
      }
    }
    else{
      message = (msg*)calloc(1, sizeof(msg));
      if(message == NULL){
        free(message);
        return FAIL;
      }
      message->pData = Data;
      if(mBox->nMessages <= -mBox->nMaxMessages){
        temp = mBox->pHead->pNext;
        temp->pNext->pPrevious = mBox->pHead;
        mBox->pHead->pNext = temp->pNext;
        mBox->nMessages ++;
        free(temp);
      }
      mBox->pTail->pPrevious->pNext = message;
      message->pPrevious = mBox->pTail->pPrevious;
      message->pNext = mBox->pTail;
      mBox->pTail->pPrevious = message;
      mBox->nMessages--;
      message->pBlock = readylist->pHead->pNext;
      readylist->pHead->pNext->pMessage = message;
      insert(waitinglist, extract(readylist->pHead->pNext));
      Running=readylist->pHead->pNext->pTask;
    }
    LoadContext();
    return FAIL;
  }
  else{
    if(readylist->pHead->pNext->pTask->DeadLine <= ticks()){
      isr_off();
      message = readylist->pHead->pNext->pMessage;
      message->pNext->pPrevious = message->pPrevious;
      message->pPrevious->pNext = message->pNext;
      mBox->nMessages ++;
      free(message);
      isr_on();
      return DEADLINE_REACHED;
    }
    else{
      return OK;
    }
  }
}

exception send_no_wait(mailbox *mBox, void *Data){
  volatile int first;
  msg *message;
  msg *temp;
  first = TRUE;
  isr_off();
  SaveContext();
  if(first){
    first = FALSE;
    if(mBox->nMessages <= RECEIVER){
      memcpy(mBox->pHead->pNext->pData, Data, sizeof(char)); //dest src size
      message = mBox->pHead->pNext;
      mBox->pHead->pNext = message->pNext;
      mBox->pHead->pNext->pPrevious = message->pPrevious;
      mBox->nMessages ++;
      if(message->pBlock != NULL && waitinglist->pHead->pNext != waitinglist->pTail){
        insert(readylist, message->pBlock);
        Running=readylist->pHead->pNext->pTask;
        free(message);
      }
      LoadContext();
    }
    else{
      message = (msg*)calloc(1, sizeof(msg));
      if(message == NULL){
        free(message);
        return FAIL;
      }
      message->pData = Data;
      if(mBox->nMessages >= mBox->nMaxMessages){
        temp = mBox->pHead->pNext;
        temp->pNext->pPrevious = mBox->pHead;
        mBox->pHead->pNext = temp->pNext;
        mBox->nMessages --;
        free(temp);
      }
      mBox->pTail->pPrevious->pNext = message;
      message->pPrevious = mBox->pTail->pPrevious;
      message->pNext = mBox->pTail;
      mBox->pTail->pPrevious = message;
      readylist->pHead->pNext->pMessage = message;
      message->pBlock = readylist->pHead->pNext;
      mBox->nMessages ++;
    }
  }
  return OK;
}

exception receive_no_wait(mailbox *mBox, void *Data){
  volatile int first;
  msg *message;
  volatile int status;
  first = TRUE;
  isr_off();
  SaveContext();
  if(first){
    first = FALSE;
    if(mBox->nMessages >= SENDER){
      memcpy(Data, mBox->pHead->pNext->pData, sizeof(char)); //dest src size
      message = mBox->pHead->pNext;
      mBox->pHead->pNext = message->pNext;
      mBox->pHead->pNext->pPrevious = message->pPrevious;
      mBox->nMessages --;
      status = OK;
      if(message->pBlock != NULL && waitinglist->pHead->pNext != waitinglist->pTail){
        insert(readylist, extract(waitinglist->pHead->pNext));
        Running = readylist->pHead->pNext->pTask;
        free(message);
      }
      else{
        free(message->pData);
      }
    }
    else{
      status = FAIL;
    }
    LoadContext();
  }
  return status;
}

exception wait(uint nTicks){
  volatile int first;
  first = TRUE;
  isr_off();
  SaveContext();
  if(first){
    first = FALSE;
    readylist->pHead->pNext->nTCnt = nTicks + tickcounter;
    insert(timerlist, extract(readylist->pHead->pNext));
    Running = readylist->pHead->pNext->pTask;
    LoadContext();
  }
  else{
    if(readylist->pHead->pNext->pTask->DeadLine <= ticks()){
      return DEADLINE_REACHED;
    }
    else{
      return OK;
    }
  }
  Running = readylist->pHead->pNext->pTask;
  return OK;
}

void set_ticks(uint Ticks){
  tickcounter = Ticks;
}

uint ticks(){
  return tickcounter;
}

uint deadline(){
  return readylist->pHead->pNext->pTask->DeadLine;
}

void set_deadline(uint deadline){
  volatile int first;
  first = TRUE;
  isr_off();
  SaveContext();
  if(first){
    first = FALSE;
    readylist->pHead->pNext->pTask->DeadLine = deadline;
    insert(readylist, extract(readylist->pHead->pNext));
    LoadContext();
  }
}

void TimerInt(void){
  tickcounter++;
  if(timerlist->pHead->pNext->nTCnt == tickcounter){
    insert(readylist, extract(timerlist->pHead->pNext));
    Running = readylist->pHead->pNext->pTask;
  }
  if(waitinglist->pHead->pNext->pTask->DeadLine < tickcounter){
    insert(readylist, extract(waitinglist->pHead->pNext));
  }
  if(timerlist->pHead->pNext->pTask->DeadLine < tickcounter){
    insert(readylist, extract(timerlist->pHead->pNext));
  }
  Running = readylist->pHead->pNext->pTask;
}

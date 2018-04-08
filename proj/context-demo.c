// main.c
#include "kernel.h"
TCB taskA;

void task1(void);
void task2(void);
void task3(void);
void task4(void);
#define TEST_PATTERN_1 0xAA
#define TEST_PATTERN_2 0x55
mailbox *mb;
int nTest1=0, nTest2=0, nTest3=0;
volatile int nData_t1;
volatile int nData_t2;
volatile int nData_t3;

void main(void){
  
  if (init_kernel() != OK ) {
    while(1);
  }
  
  if (create_task( task1, 2000 ) != OK ) {
    while(1);
  }
  
  if (create_task( task2, 4000 ) != OK ) {
    // Memory allocation problems 
    while(1);
  }
  
  if (create_task( task3, 1000 ) != OK ){
    while(1);
  }
 
  if ( (mb=create_mailbox(2,sizeof(char)) ) == NULL) {
           //Memory allocation problems
          while(1);
  }

  run();
}
  
void task1(void){
  nData_t2 = TEST_PATTERN_2;
  if(send_wait(mb, (void*)&nData_t2) == FAIL){
    while(1);
  }
  if(wait(10) == DEADLINE_REACHED){
    while(1);
  }
  if(create_task(task4, 4000) == FAIL){
    while(1);
  }
  if(receive_wait(mb, (void*)&nData_t2) == FAIL){
    while(1);
  }
  nData_t1 = TEST_PATTERN_1;
  if(send_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  nData_t1 = TEST_PATTERN_1;
  if(send_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  terminate(); 
}

void task2(void){
  if(receive_wait(mb, (void*)&nData_t3) == FAIL || nData_t3 != TEST_PATTERN_1){
    while(1);
  }
  if((mb = create_mailbox(3, sizeof(char))) == NULL){
    while(1);
  }
  if(receive_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  nData_t1 = TEST_PATTERN_1;
  if(send_wait(mb, (void*)&nData_t1) == FAIL || nData_t2 != TEST_PATTERN_1){
    while(1);
  }
  if(receive_no_wait(mb, (void*)&nData_t2) == FAIL || nData_t2 != TEST_PATTERN_1){
    while(1);
  }
  if(wait(200) == DEADLINE_REACHED){
    while(1);
  }
  if(receive_wait(mb, (void*)&nData_t3) == FAIL || nData_t3 != TEST_PATTERN_1){
    while(1);
  }
  terminate();
}

void task3(void){
  nData_t1 = TEST_PATTERN_1;
  if(send_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  if(remove_mailbox(mb) != NOT_EMPTY){
    while(1);
  }
  if(receive_no_wait(mb, (void*)&nData_t1) == FAIL || nData_t1 != TEST_PATTERN_2){
    while(1);
  }
  if(remove_mailbox(mb) == NOT_EMPTY){
    while(1);
  }
  set_deadline(2);
  if(wait(10) != DEADLINE_REACHED){
    while(1);
  }
  set_deadline(1000);
  if(wait(100) == DEADLINE_REACHED){
    while(1);
  }
  nData_t2 = TEST_PATTERN_2;
  if(send_wait(mb, (void*)&nData_t2) == FAIL){
    while(1);
  }
  terminate();
}

void task4(void){
  nData_t2 = TEST_PATTERN_2;
 if(send_wait(mb, (void*)&nData_t2) == FAIL){
   while(1);
 }
 terminate();
}
     
void isr_on(void){
}
void isr_off(void){
}

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
  
  if (create_task( task1, 1000 ) != OK ) {
    while(1);
  }
  
  if (create_task( task2, 2000 ) != OK ) {
    // Memory allocation problems 
    while(1);
  }
  
  if (create_task( task3, 3000 ) != OK ){
    while(1);
  }
 
  if ( (mb=create_mailbox(2,sizeof(char)) ) == NULL) {
           //Memory allocation problems
          while(1);
  }

  run(); //Task 1 kör
}
  
void task1(void){
  //Test 1:
  nData_t1 = TEST_PATTERN_1;
  if(send_no_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  wait(10);
  //Rensar upp test 1:
  receive_wait(mb, (void*)&nData_t2);
  nData_t1 = nData_t2 = nData_t3 = 0x00;
  //Set up test 2:
  wait(1);
  //Test 2:
  nData_t2 = TEST_PATTERN_2;
  if(send_no_wait(mb, (void*)&nData_t2) == FAIL){
    while(1);
  }
  wait(10);
  //Rensar upp test 2:
  receive_wait(mb, (void*)&nData_t2);
  nData_t1 = nData_t2 = nData_t3 = 0x00;
  //Test 1 med receive_no_wait:
  nData_t1 = TEST_PATTERN_1;
  if(send_no_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  wait(10);
  //Rensar upp test 1:
  receive_wait(mb, (void*)&nData_t2);
  nData_t1 = nData_t2 = nData_t3 = 0x00;
  //Set up test 2 med receive_no_wait:
  wait(1);
  //Test 2 med receive_no_wait:
  nData_t2 = TEST_PATTERN_2;
  if(send_no_wait(mb, (void*)&nData_t2) == FAIL){
    while(1);
  }
  wait(10);
  //Rensar upp test 2:
  receive_wait(mb, (void*)&nData_t2);
  nData_t1 = nData_t2 = nData_t3 = 0x00;
  terminate(); //Task 2 kör
}

void task2(void){
  //Test 1:
  nData_t2 = TEST_PATTERN_2;
  if(send_no_wait(mb, (void*)&nData_t2) == FAIL){
    while(1);
  }
  wait(10);
  //Test 2:
  nData_t1 = TEST_PATTERN_1;
  if(send_no_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  wait(11);
  //Test 1 med receive_no_wait:
  nData_t2 = TEST_PATTERN_2;
  if(send_no_wait(mb, (void*)&nData_t2) == FAIL){
    while(1);
  }
  wait(10);
  //Test 2 med receive_no_wait:
  nData_t1 = TEST_PATTERN_1;
  if(send_no_wait(mb, (void*)&nData_t1) == FAIL){
    while(1);
  }
  wait(11);
  terminate(); //Idle task kör
}

void task3(void){
  //Test 1:
  if(receive_wait(mb, (void*)&nData_t3) == FAIL || nData_t3 != TEST_PATTERN_1){
    while(1);
  }
  wait(10);
  //Set up test 2:
  wait(1);
  //Test 2:
  if(receive_wait(mb, (void*)&nData_t3) == FAIL || nData_t3 != TEST_PATTERN_1){
    while(1);
  }
  wait(10);
  //Test 1 med receive_no_wait:
  if(receive_no_wait(mb, (void*)&nData_t3) == FAIL || nData_t3 != TEST_PATTERN_1){
    while(1);
  }
  wait(10);
  //Set up test 2:
  wait(1);
  //Test 2 med receive_no_wait:
  if(receive_no_wait(mb, (void*)&nData_t3) == FAIL || nData_t3 != TEST_PATTERN_1){
    while(1);
  }
  terminate(); //Idle task sen Task 2
}

void task4(void){
  terminate(); //Idle task sen Task 3
}

void isr_on(void){
}
void isr_off(void){
}
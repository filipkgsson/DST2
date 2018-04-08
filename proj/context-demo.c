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
  nData_t2 = TEST_PATTERN_2;
  if(send_wait(mb, (void*)&nData_t2) == FAIL){ //Task 2 startar
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
  if(wait(10) == DEADLINE_REACHED){ //Task 2 kör
    while(1);
  }
  if(create_task(task4, 4000) == FAIL){
    while(1);
  }
  if(receive_wait(mb, (void*)&nData_t2) == FAIL){ //Task 4 startar
    while(1);
  }
  terminate(); //Task 2 kör
}

void task2(void){
  nData_t2 = TEST_PATTERN_2;
  if(send_wait(mb, (void*)&nData_t3) == FAIL){ //Task 3 kör
    while(1);
  }
  if((mb = create_mailbox(3, sizeof(char))) == NULL){
    while(1);
  }
  if(receive_wait(mb, (void*)&nData_t1) == FAIL){ //Task 3 kör
    while(1);
  }
  nData_t1 = TEST_PATTERN_1;
  if(send_wait(mb, (void*)&nData_t1) == FAIL || nData_t2 != TEST_PATTERN_1){ //Task 1 kör
    while(1);
  }
  if(receive_no_wait(mb, (void*)&nData_t2) == FAIL || nData_t2 != TEST_PATTERN_1){ //Task 1 kör
    while(1);
  }
  if(wait(200) == DEADLINE_REACHED){ //Task 4 kör
    while(1);
  }
  terminate(); //Idle task kör
}

void task3(void){
  if(receive_wait(mb, (void*)&nData_t3) == FAIL || nData_t3 != TEST_PATTERN_1){ //Task 1 startar
    while(1);
  }
  set_deadline(2);
  if(wait(10) != DEADLINE_REACHED){ //Idle task sen Task 3
    while(1);
  }
  set_deadline(3000);
  if(wait(100) == DEADLINE_REACHED){ //Idle task sen Task 1
    while(1);
  }
  terminate(); //Idle task sen Task 2
}

void task4(void){
  nData_t2 = TEST_PATTERN_2;
  if(send_wait(mb, (void*)&nData_t2) == FAIL){ //Task 2 kör
    while(1);
  }
  terminate(); //Idle task sen Task 3
}
     
void isr_on(void){
}
void isr_off(void){
}

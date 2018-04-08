// main.c
#include "kernel.h"
TCB taskA;

void task1(void);
void task2(void);
void task3(void);
#define TEST_PATTERN_1 0xAA
#define TEST_PATTERN_2 0x55
mailbox *mb;
int nTest1=0, nTest2=0, nTest3=0;
volatile int nData_t1;
volatile int nData_t2;
volatile int nData_t3;

void main(void){

  /*if (init_kernel() != OK ) {
    // Memory allocation problems
    while(1);
  }
  
  //mb=create_mailbox(2,sizeof(int));
  
 if (create_task( task1, 2000 ) != OK ) {
    // Memory allocation problems 
    while(1);
  }
  if (create_task( task2, 4000 ) != OK ) {
    // Memory allocation problems 
    while(1);
  }
 
  if ( (mb=create_mailbox(1,sizeof(int)) ) == NULL) {
           //Memory allocation problems
          while(1);
  }*/
  init_kernel();
  mb = create_mailbox(1000, sizeof(int));
  create_task(&task1, 8 );
  //create_task( task_apa5, 25 );
  run(); //First in readylist is task1
}
  
void task1(void){
  create_task(&task2, 45);
  send_wait(mb, (void*)&nData_t1);
  wait(10);
  if(send_wait(mb, (void*)&nData_t1) == DEADLINE_REACHED){
    set_deadline(20);
  }
  //receive_no_wait(mb,(void*)&nData_t1);
  send_wait(mb,(void*)&nData_t1);
  //nData_t1 = TEST_PATTERN_2;
  //send_wait(mb,(void*)&nData_t1);
  /*receive_wait(mb,(void*)&nData_t1);
  receive_wait(mb,(void*)&nData_t1);*/
  //if(receive_wait(mb,(void*)&nData_t1) == DEADLINE_REACHED)
    //terminate();  //ERROR 
  wait(10);  //task2 börjar köra 
   //start test 3 
  //if(send_wait(mb,(void*)&nData_t1)==DEADLINE_REACHED) {
    //if( no_messages(mb) != 0 )
      //terminate();  //ERROR
  //}
  remove_mailbox(mb);
  terminate(); 
}

void task2(void){
  nData_t2 = TEST_PATTERN_2;
  //wait(20);
  create_task(&task3, 50);
  send_wait(mb, (void*)&nData_t2);
  //if(send_wait(mb,(void*)&nData_t2) ==
     //DEADLINE_REACHED)  //t1 kör nu 
    //terminate();  //ERROR 
  //if( no_messages(mb) != 0 )
    //terminate();  //ERROR 
  if (nData_t2 == TEST_PATTERN_1) nTest1 = 1;
  wait(25);  //t1 kör nu 
   //start test 2 
  //if( no_messages(mb) != 1 )
    //terminate();  //ERROR 
  //if(receive_wait(mb,(void*)&nData_t2) ==
    // DEADLINE_REACHED)  //t1 kör nu 
    //terminate();  //ERROR 
  //if( no_messages(mb) != 0 )
    //terminate();  //ERROR 
  if (nData_t2 == TEST_PATTERN_2) nTest2 = 1;
   //Start test 3 
  terminate();
}

void task3(void){
  nData_t3 = TEST_PATTERN_1;
  receive_no_wait(mb, (void*)&nData_t3);
  terminate();
}

void isr_on(void){
}
void isr_off(void){
}

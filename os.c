#include <avr/io.h>
#include <avr/interrupt.h>
#include "globals.h"
#include "os.h"
#include <stdlib.h>



system_t system;
uint8_t volatile threadNum = 0;

uint8_t volatile oldThreadVal;
uint8_t volatile newThreadVal;
//This interrupt routine is automatically run every 10 milliseconds
__attribute__((naked)) void context_switch(uint16_t *new_tp, uint16_t *old_tp);

uint8_t get_next_thread();
void sleep_refresh();
void yield();
void print_int32(uint32_t i);

ISR(TIMER0_COMPA_vect) {
   //At the beginning of this ISR, the registers r0, r1, and r18-31 have 
   //already been pushed to the stack

   //The following statement tells GCC that it can use registers r18-r31 
   //for this interrupt routine.  These registers (along with r0 and r1) 
   //will automatically be pushed and popped by this interrupt routine.
   asm volatile ("" : : : "r18", "r19", "r20", "r21", "r22", "r23", "r24", \
                 "r25", "r26", "r27", "r30", "r31");                        

   //Insert your code here
   //Call get_next_thread to get the thread id of the next thread to run
   //Call context switch here to switch to that next thread
 
   //print_string("HELLO INTERRUPT WORLD");
   set_color(WHITE);
   (system.intCount)++;

   if(threadNum == MAXTHREADS) //Think about this make sure it isn't always running.
      threadNum = 0;

   oldThreadVal = threadNum;

   sleep_refresh();
   newThreadVal = get_next_thread();
  
   /*
   print_string("OT: ");
   print_int32(oldThreadVal);
   print_string(" ");

   print_string("NT: ");
   print_int32(newThreadVal);
   print_string(" ");
   */
   system.threads[newThreadVal].threadState = THREAD_RUNNING;
   system.threads[oldThreadVal].threadState = THREAD_READY;
   context_switch(&(system.threads[newThreadVal].sp), &(system.threads[oldThreadVal].sp));
   
   //At the end of this ISR, GCC generated code will pop r18-r31, r1, 
   //and r0 before exiting the ISR
}

ISR(TIMER1_COMPA_vect) {
   //This interrupt routine is run once a second
   //The 2 interrupt routines will not interrupt each other
}

void start_system_timer() {
   //start timer 0 for OS system interrupt
   TIMSK0 |= _BV(OCIE0A);             //interrupt on compare match
   TCCR0A |= _BV(WGM01);              //clear timer on compare match

   //Generate timer interrupt every ~10 milliseconds
   TCCR0B |= _BV(CS02) | _BV(CS00);   //prescalar /1024
   OCR0A = 156;                       //generate interrupt every 9.98 milliseconds
                                      //start timer 1 to generate interrupt every 1 second
   OCR1A = 15625;
   TIMSK1 |= _BV(OCIE1A);             //interrupt on compare
   TCCR1B |= _BV(WGM12) | _BV(CS12) | _BV(CS10); //slowest prescalar /1024

}

/* Legacy Code
//Call this to start the system timer interrupt
void start_system_timer() {
   TIMSK0 |= _BV(OCIE0A);  //interrupt on compare match
   TCCR0A |= _BV(WGM01);   //clear timer on compare match

   //Generate timer interrupt every ~10 milliseconds
   TCCR0B |= _BV(CS02) | _BV(CS00);    //prescalar /1024
   OCR0A = 156;             //generate interrupt every 9.98 milliseconds
}
*/

__attribute__((naked)) void context_switch(uint16_t *new_tp, uint16_t *old_tp) {
   asm volatile ("push r2");
   asm volatile ("push r3");
   asm volatile ("push r4");
   asm volatile ("push r5");
   asm volatile ("push r6");
   asm volatile ("push r7");
   asm volatile ("push r8");
   asm volatile ("push r9");
   asm volatile ("push r10");
   asm volatile ("push r11");
   asm volatile ("push r12");
   asm volatile ("push r13");
   asm volatile ("push r14");
   asm volatile ("push r15");
   asm volatile ("push r16");
   asm volatile ("push r17");
   asm volatile ("push r28");
   asm volatile ("push r29");

   // Take SP from CPU, put into SP for T1 (CPU SP located at 0x5E (high byte) and Ox5D(low byte))
   // Take SP from T2, put into SP from CPU

   // Saving temp regs
 

   asm volatile("LDI r31,0x00");
   asm volatile("LDI r30,0x5D");
   asm volatile("LD r10, Z"); 

   asm volatile("LDI r31,0x00");
   asm volatile("LDI r30,0x5E");
   asm volatile("LD r11, Z"); 
   
   asm volatile("MOVW r30, r22"); 

   asm volatile("ST Z+, r10");
   asm volatile("ST Z, r11");


   asm volatile("MOVW r30, r24");
   asm volatile("LD r12, Z+"); 

   
   asm volatile("LD r13, Z");

   asm volatile("LDI r31,0x00");
   asm volatile("LDI r30,0x5D");

   asm volatile("ST Z+, r12");
   asm volatile("ST Z, r13");

   
   // Pop registers r2 through r17
   asm volatile("pop r29");
   asm volatile("pop r28");
   asm volatile("pop r17");
   asm volatile("pop r16");
   asm volatile("pop r15");
   asm volatile("pop r14");
   asm volatile("pop r13");
   asm volatile("pop r12");
   asm volatile("pop r11");
   asm volatile("pop r10");
   asm volatile("pop r9");
   asm volatile("pop r8");
   asm volatile("pop r7");
   asm volatile("pop r6");
   asm volatile("pop r5");
   asm volatile("pop r4");
   asm volatile("pop r3");
   asm volatile("pop r2");
   asm volatile("ret");
}

__attribute__((naked)) void thread_start(void) {
    
   sei();
   asm volatile("MOVW r24, r4");
   asm volatile("MOVW r30, r2");
   asm volatile("ijmp");

}


// any OS specific init code
void os_init() {
   system.totalThreads = 0;
   system.intCount = 0;
   serial_init();
}

// Call once for each thread you want to create
void create_thread(char *name, uint16_t address, void *args, uint16_t stack_size) {
   uint16_t defaultStackSize = 50;
   uint16_t size = defaultStackSize + stack_size;

   uint16_t threadStart = (uint16_t)thread_start;
   uint8_t tSLow = threadStart & 0xFF;
   uint8_t tSHigh = threadStart >> 8;
   
   uint8_t addressHigh = address >> 8;
   uint8_t addressLow = address & 0xFF;
   
   uint16_t argAddress = (uint16_t) args;
   uint8_t argsLow = argAddress & 0xFF;
   uint8_t argsHigh = argAddress >> 8;

   struct regs_context_switch *x; 

   (system.threads[threadNum]).tName = name;
   
   (system.threads[threadNum]).base = (uint16_t)malloc(size);

   (system.threads[threadNum]).sp = (system.threads[threadNum]).base + (size - 1);

   (system.threads[threadNum]).sp = (system.threads[threadNum]).sp - sizeof(struct regs_context_switch);

   x = (struct regs_context_switch*) (system.threads[threadNum]).sp;

   x->r3 = addressHigh;
   x->r2 = addressLow;
   x->r5 = argsHigh;
   x->r4 = argsLow;
   x->pcl = tSLow;
   x->pch = tSHigh;
   x->eind = 0;
   
   (system.threads[threadNum]).id = threadNum;
   (system.threads[threadNum]).sSize = size;
   (system.threads[threadNum]).threadState = THREAD_READY;
   (system.totalThreads)++;
   threadNum++;
   // print_string("Thread created!");
   // print_hex(threadNum);


}

// start running the OS
void os_start() {
   start_system_timer();
   sei();
   clear_screen();
   context_switch(&(system.threads[0].sp), &(system.threads[2].sp));
}

// return id of next thread to run
uint8_t get_next_thread() {
   int i = threadNum;
   int ogThreadNum = threadNum;
   while(1){
      i++;
      if( i == MAXTHREADS)
      {
        i = 0;
      }
      
      if(system.threads[i].threadState == THREAD_READY)
      {
         threadNum = i;
         return threadNum;
      }

      if(i == ogThreadNum)
         return threadNum;

   }
}

void sleep_refresh() {
   int i = 0;

   for (i = 0; i < MAXTHREADS; i++) {
      if (system.threads[i].threadState == THREAD_SLEEPING)
         if (system.threads[i].sleepCycles == 0)
            system.threads[i].threadState = THREAD_READY;
         else
            system.threads[i].sleepCycles--;
   }
}
   

void thread_sleep(uint16_t ticks) {
   /*set_cursor(40, 50);
   print_string("We put thread ");
   print_int(threadNum);
   print_string(" to sleep");*/

   if (ticks == 1) {
      oldThreadVal = threadNum;
      newThreadVal = get_next_thread();

      context_switch(&(system.threads[newThreadVal].sp), &(system.threads[oldThreadVal].sp));
   }
   else {
      system.threads[threadNum].threadState = THREAD_SLEEPING;
      system.threads[threadNum].sleepCycles = ticks;
   }
}

void mutex_init(mutex_t *m) {
   m->available = TRUE;
   m->tail = 0;
   m->head = 0;
   m->curSize = 0;
   m->waitlistMax = MAXTHREADS;
}

void mutex_lock(mutex_t *m) {

   uint8_t next;

   if (m->available){

     m->available = FALSE;

   }
   else
   {
      next = m->head + 1;
      if (next >= m->waitlistMax)
         next = 0;

      /* 
      if (next == m->tail){
         set_cursor(60, 50); // check if circular buffer is full
         print_string("Buffer is full");
      } */
      
      (m->curSize)++;
      m->waitList[m->head] = threadNum;

      system.threads[threadNum].threadState = THREAD_WAITING;
     
      m->head = next;
   }
}

void mutex_unlock(mutex_t *m) {
   uint8_t readyThread = m->waitList[(m->tail)];
   uint8_t next;

   m->available = TRUE;

   if(m->curSize > 0){
      system.threads[readyThread].threadState = THREAD_READY;

      //Waitlist update
      /*
      if (m->head == m->tail){ // check if buffer is empty
          set_cursor(55, 50);
         print_string("empty");
      }*/ 

      next = m->tail + 1;

      if(next >= m->waitlistMax){
        next = 0;
      }
      (m->curSize)--;
      m->tail = next;
      m->available = FALSE;
   }
   

   
}

void sem_init(semaphore_t *s, int8_t value) {
   int i = 0;
   s->tail = 0;
   s->head = 0;
   s->value = value;
   s->waitlistMax = MAXTHREADS;
   for(i = 0; i < MAXTHREADS; i++)
      s->waitList[i] = 200;
}

void sem_wait(semaphore_t *s) {
   cli();
   uint8_t next;
   int i;
   int row = 20;
   (s->value)--;
 
   
   if(s->value < 0)
   {
      // next is where head will point after.
      next = s->head + 1;
      if (next >= s->waitlistMax)
         next = 0;

      /*
      if (next == s->tail){
         set_cursor(60, 50); // check if circular buffer is full
         print_string("Buffer is full");
      }
      */

      s->waitList[s->head] = threadNum;

      system.threads[threadNum].threadState = THREAD_WAITING;
     
      s->head = next;            // head to next offset.

      // blocking area goes to new thread because of wait
    
      //yield();
   }
   
}

void sem_signal(semaphore_t *s) {
   cli();
   int i;
   uint8_t readyThread = s->waitList[(s->tail)];
   uint8_t next;
   (s->value)++;

   if (s->value <= 0) { //should be 0 or -1???
      
      // Take the top one off and prepare it to run

      system.threads[readyThread].threadState = THREAD_READY;

      //Waitlist update
      if (s->head == s->tail){ 
           set_cursor(65, 50);// check if buffer is empty
      } 

      next = s->tail + 1;

      if(next >= s->waitlistMax){
        next = 0;
      }

      s->tail = next;      // tail to next.
   }
   sei();
}
   
   


void sem_signal_swap(semaphore_t *s) {
   cli();
   int i;
   uint8_t runThread = s->waitList[(s->tail)];
   uint8_t next;
   (s->value)++;

   if (s->value <= 0) {
      // Take the top one off and run it
      
      system.threads[runThread].threadState = THREAD_RUNNING;
      system.threads[threadNum].threadState = THREAD_READY;

      oldThreadVal = threadNum;
      newThreadVal = runThread;
      threadNum = runThread;

      if (s->head == s->tail){
          // check if buffer is empty
          set_cursor(70, 0);
         print_string("something weird is happening");
      }  //shouldn't happen

      next = s->tail + 1;

      if(next >= s->waitlistMax){

        next = 0;
      }

      s->tail = next;             // tail to next.

      context_switch(&(system.threads[newThreadVal].sp), &(system.threads[oldThreadVal].sp));

   }
   sei();
  
}

void yield() {
   uint8_t oldVal, newVal;

   oldVal = threadNum;

   newVal = get_next_thread();

   context_switch(&(system.threads[newVal].sp), &(system.threads[oldVal].sp));
   
}

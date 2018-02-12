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

void print_int32(uint32_t i);

ISR(TIMER0_COMPA_vect) {
   //At the beginning of this ISR, the registers r0, r1, and r18-31 have 
   //already been pushed to the stack

   //The following statement tells GCC that it can use registers r18-r31 
   //for this interrupt routine.  These registers (along with r0 and r1) 
   //will automatically be pushed and popped by this interrupt routine.
   asm volatile ("" : : : "r18", "r19", "r20", "r21", "r22", "r23", "r24", \
                 "r25", "r26", "r27", "r28", "r29", "r30", "r31");                        

   //Insert your code here
   //Call get_next_thread to get the thread id of the next thread to run
   //Call context switch here to switch to that next thread
 
   //print_string("HELLO INTERRUPT WORLD");


   if(threadNum == 6) 
      threadNum = 0;


   oldThreadVal = threadNum;

   newThreadVal = get_next_thread();
  
   /*
   print_string("OT: ");
   print_int32(oldThreadVal);
   print_string(" ");

   print_string("NT: ");
   print_int32(newThreadVal);
   print_string(" ");
   */

   context_switch(&(system.threads[newThreadVal].sp), &(system.threads[oldThreadVal].sp));
   
   int i;
   for (i = 0; i < 6; i++) {
      if (system.threads[i].threadState == THREAD_SLEEPING)
         if (system.threads[i].sleepCycles == 0)
            system.threads[i].threadState = THREAD_READY;
         else
            system.threads[newThreadVal].sleepCycles--;
   }

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

   threadNum++;
   print_string("Thread created!");
   print_hex(threadNum);
}

// start running the OS
void os_start() {
   start_system_timer();
   sei();
   context_switch(&(system.threads[0].sp), &(system.threads[2].sp));
}

// return id of next thread to run
uint8_t get_next_thread() {
   switch(threadNum) {
      case 0:
         if (system.threads[5].threadState != THREAD_SLEEPING) {
            threadNum = 5;
            return threadNum;
         }
         if (system.threads[4].threadState != THREAD_SLEEPING) {
            threadNum = 4;
            return threadNum;
         }
         if (system.threads[3].threadState != THREAD_SLEEPING) {
            threadNum = 3;
            return threadNum;
         }
         if (system.threads[2].threadState != THREAD_SLEEPING) {
            threadNum = 2;
            return threadNum;
         }
         if (system.threads[1].threadState != THREAD_SLEEPING) {
            threadNum = 1;
            return threadNum;
         }
         return threadNum;

         case 5:
         if (system.threads[4].threadState != THREAD_SLEEPING) {
            threadNum = 4;
            return threadNum;
         }
         if (system.threads[3].threadState != THREAD_SLEEPING) {
            threadNum = 3;
            return threadNum;
         }
         if (system.threads[2].threadState != THREAD_SLEEPING) {
            threadNum = 2;
            return threadNum;
         }
         if (system.threads[1].threadState != THREAD_SLEEPING) {
            threadNum = 1;
            return threadNum;
         }
         if (system.threads[0].threadState != THREAD_SLEEPING) {
            threadNum = 0;
            return threadNum;
         }
         return threadNum;

         case 4:
         if (system.threads[3].threadState != THREAD_SLEEPING) {
            threadNum = 3;
            return threadNum;
         }
         if (system.threads[2].threadState != THREAD_SLEEPING) {
            threadNum = 2;
            return threadNum;
         }
         if (system.threads[1].threadState != THREAD_SLEEPING) {
            threadNum = 1;
            return threadNum;
         }
         if (system.threads[0].threadState != THREAD_SLEEPING) {
            threadNum = 0;
            return threadNum;
         }
         if (system.threads[5].threadState != THREAD_SLEEPING) {
            threadNum = 5;
            return threadNum;
         }
         return threadNum;

         case 3:
         if (system.threads[2].threadState != THREAD_SLEEPING) {
            threadNum = 2;
            return threadNum;
         }
         if (system.threads[1].threadState != THREAD_SLEEPING) {
            threadNum = 1;
            return threadNum;
         }
         if (system.threads[0].threadState != THREAD_SLEEPING) {
            threadNum = 0;
            return threadNum;
         }
         if (system.threads[5].threadState != THREAD_SLEEPING) {
            threadNum = 5;
            return threadNum;
         }
         if (system.threads[4].threadState != THREAD_SLEEPING) {
            threadNum = 4;
            return threadNum;
         }
         return threadNum;

         case 2:
         if (system.threads[1].threadState != THREAD_SLEEPING) {
            threadNum = 1;
            return threadNum;
         }
         if (system.threads[0].threadState != THREAD_SLEEPING) {
            threadNum = 0;
            return threadNum;
         }
         if (system.threads[5].threadState != THREAD_SLEEPING) {
            threadNum = 5;
            return threadNum;
         }
         if (system.threads[4].threadState != THREAD_SLEEPING) {
            threadNum = 4;
            return threadNum;
         }
         if (system.threads[3].threadState != THREAD_SLEEPING) {
            threadNum = 3;
            return threadNum;
         }
         return threadNum;

         case 1:
         if (system.threads[0].threadState != THREAD_SLEEPING) {
            threadNum = 0;
            return threadNum;
         }
         if (system.threads[5].threadState != THREAD_SLEEPING) {
            threadNum = 5;
            return threadNum;
         }
         if (system.threads[4].threadState != THREAD_SLEEPING) {
            threadNum = 4;
            return threadNum;
         }
         if (system.threads[3].threadState != THREAD_SLEEPING) {
            threadNum = 3;
            return threadNum;
         }
         if (system.threads[2].threadState != THREAD_SLEEPING) {
            threadNum = 2;
            return threadNum;
         }
         return threadNum;
   }
}

void thread_sleep(uint16_t ticks) {
   if (ticks ==1) {
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
}

void mutex_lock(mutex_t *m) {
   while (m->available)
      ;
   m->available = FALSE;
}

void mutex_unlock(mutex_t *m) {
   m->available = TRUE;
}

void sem_init(semaphore_t *s, int8_t value) {
   s->tail = 0;
   s->value = value;
}

void sem_wait(semaphore_t *s) {
   (s->value)--;

   if(s->value <= 0)
      s->waitList[s->tail] = threadNum;
   // block the process
   system.threads[threadNum].threadState = THREAD_WAITING;
}

void sem_signal(semaphore_t *s) {
   int i;
   s->value++;
   if (s-> value <= 0) {
      // Take the top one off and run it
      system.threads[s->waitList[0]].threadState = THREAD_RUNNING;
      for (i = 0; i < s->tail - 1; i++) {
         s->waitList[i] = s->waitList[i + 1];
      }
      (s->tail)--;
      (s->value)--;
   }
}

void sem_signal_swap(semaphore_t *s) {
   int i;
   s->value++;
   if (s-> value <= 0) {
      // Take the top one off and run it
      system.threads[s->waitList[0]].threadState = THREAD_RUNNING;

      // TODO inspect this section
      oldThreadVal = threadNum;
      newThreadVal = get_next_thread();

      context_switch(&(system.threads[newThreadVal].sp), &(system.threads[oldThreadVal].sp));

      for (i = 0; i < s->tail - 1; i++) {
         s->waitList[i] = s->waitList[i + 1];
      }
      (s->tail)--;
      (s->value)--;
   }
  
}

void yield() {
}

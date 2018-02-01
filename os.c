#include <avr/io.h>
#include <avr/interrupt.h>
#include "globals.h"
#include "os.h"
#include <stdlib.h>

//This interrupt routine is automatically run every 10 milliseconds
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
   // context_switch();
   
   //At the end of this ISR, GCC generated code will pop r18-r31, r1, 
   //and r0 before exiting the ISR
}

//Call this to start the system timer interrupt
void start_system_timer() {
   TIMSK0 |= _BV(OCIE0A);  //interrupt on compare match
   TCCR0A |= _BV(WGM01);   //clear timer on compare match

   //Generate timer interrupt every ~10 milliseconds
   TCCR0B |= _BV(CS02) | _BV(CS00);    //prescalar /1024
   OCR0A = 156;             //generate interrupt every 9.98 milliseconds
}

system_t system;
int threadNum = 0;
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
 

   asm volatile("LDI r31,0x00");// Z points to CPU
   asm volatile("LDI r30,0x5D");
   asm volatile("LD r10, Z"); //r18 holds original CPU Lower

   asm volatile("LDI r31,0x00");// Z points to CPU
   asm volatile("LDI r30,0x5E");
   asm volatile("LD r11, Z"); //r19 holds original CPU Higher
   
   asm volatile("MOVW r30, r22"); // END of 1

   asm volatile("ST Z+, r10");
   asm volatile("ST Z, r11");


   asm volatile("MOVW r30, r24");
   asm volatile("LD r12, Z+"); //r20 holds NT lower

   
   asm volatile("LD r13, Z");

   asm volatile("LDI r31,0x00");// Z points to CPU Lower
   asm volatile("LDI r30,0x5D");

   asm volatile("ST Z+, r12");
   asm volatile("ST Z, r13");

   
   //asm volatile("MOVW r30, r22"); //Z points to OT r23-r22
   //asm volatile("LD r16, Z"); //r16 holds value pointed to by OT r23-r22


  

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
   sei(); //enable interrupts - leave as the first statement in thread_start()
}

// any OS specific init code
void os_init() {
}

// Call once for each thread you want to create
void create_thread(char *name, uint16_t address, void *args, uint16_t stack_size) {
   uint16_t defaultStackSize = 50;
   uint16_t size = defaultStackSize + stack_size;
   uint8_t addressHigh = address >> 8;
   struct regs_context_switch *x; 

   (system.threads[threadNum]).tName = name;
   
   (system.threads[threadNum]).base = (uint16_t)malloc(size);

   (system.threads[threadNum]).sp = (system.threads[threadNum]).base + (size  - 1);

   (system.threads[threadNum]).sp = (system.threads[threadNum]).sp - sizeof(struct regs_context_switch);

   x = (struct regs_context_switch*) &((system.threads[threadNum]).sp);

   x->r2 = addressHigh;
   //Do address low
   //put args high and low in regs
   //put thread_start into pc

   
   (system.threads[threadNum]).sSize = stack_size;

    threadNum++;
}

// start running the OS
void os_start() {
}

// return id of next thread to run
uint8_t get_next_thread() {
}

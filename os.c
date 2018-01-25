#include <avr/io.h>
#include <avr/interrupt.h>
#include "globals.h"
#include "os.h"

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
   thread *oldThread = (thread *)old_tp;
   uint8_t highestOT = (intptr_t)oldThread >> 8;
   uint8_t lowestOT = (intptr_t)oldThread & 0x00FF;

   thread *newThread = (thread *)new_tp;
   uint8_t highestNT = (intptr_t)newThread >> 8;
   uint8_t lowestNT = (intptr_t)newThread & 0x00FF;

   // asm volatile("LDS ")
   // Take SP from T2, put into SP from CPU
   // Pop registers r2 through r17
}

__attribute__((naked)) void thread_start(void) {
   sei(); //enable interrupts - leave as the first statement in thread_start()
}


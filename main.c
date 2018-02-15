/**
 * @author Christian Coelho & Justin Tomas
 */

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "os.h"
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t pRate = 100;
uint16_t cRate = 100;  //default rates
uint8_t buffer = 0;
void function_blink();
void function_stats();
void display_bounded_buffer();
void producer();
void consumer();
extern system_t system;
mutex_t prMutex;
mutex_t crMutex;
mutex_t proMutex;
mutex_t conMutex;
semaphore_t empty;
semaphore_t full;
semaphore_t m;

int main(int argc, char *argv[]) {
   int i = 1;
   uint32_t test = 20;

   os_init();
   print_string("We are in main");
   create_thread("stats", &function_stats, NULL, 50); //0
   create_thread("blink", &function_blink, NULL, 50); // 1
   create_thread("main", &main, NULL, 50); // 2
   create_thread("display_bounded_buffer", &display_bounded_buffer, NULL, 50); // 3
   create_thread("producer", &producer, NULL, 50); // 4
   create_thread("consumer", &consumer, NULL, 50); // 5

   mutex_init(prMutex);
   mutex_init(crMutex);
   mutex_init(proMutex);
   mutex_init(conMutex);

   sem_init(empty, 10);
   sem_init(full, 0);
   sem_init(m, 1);
   os_start();

   sei();
   while(1) {
      print_string("I'm main. ");
   }
   

   // context_switch(&(system.threads[0].sp), &(system.threads[0].sp));
   
   return 0;
}

void function_blink() {
    int i, test = 0;
    while(1) {
      test = test + 1;
    for(i = 0; i < 10; i++) {
        _delay_ms(10);
    }   
    led_on();

    for(i = 0; i < 10; i++) {
        _delay_ms(10);
    }

    led_off();
    }
}
void function_stats() {
   while(1)
   {
      print_string("Stack base 0: ");
      print_int32(system.threads[0].base);

      print_string("SP 0: ");
      print_int32(system.threads[0].sp);

      print_string("Stack base 1: ");
      print_int32(system.threads[1].base);

      print_string("SP 1: "); 
      print_int32(system.threads[1].sp);
      print_string(" ");
   }
}

void display_bounded_buffer() {
   int i  = 0;
   
   print_string("I'm bounded buffer. ");
   for( i = 0; i < buffer; i++)
   {
      print_string("Item: ");
      print_int32(i);
      print_string("\n");
   }
      
   print_string("Production: ");
   print_int32(pRate);
   print_string("\n");

   print_string("Consumer: ");
   print_int32(cRate);
   print_string("\n");

}

void producer() {
   
   uint8_t producedItem;
   print_string("I'm producer. ");

   while(1) {
      thread_sleep(pRate);
      producedItem = 1;

      sem_wait(empty);
      sem_wait(m);

      //Add item to buffer
      buffer += producedItem;
      if(buffer > 10)
         buffer = 10;

      sem_signal(m);
      sem_signal(full);
   }
}

void consumer() {
   print_string("I'm consumer. ");

   while(1) {
      sem_wait(full);
      sem_wait(m);

      thread_sleep(cRate);
   
      // Remove item from buffer
      buffer--;
      if(buffer < 0)
         buffer = 0;
      
      sem_signal(m);
      sem_signal(empty);

      // Consume item
   }
}

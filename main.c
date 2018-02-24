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

#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 38

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
mutex_t buffMutex;
semaphore_t empty;
semaphore_t full;
semaphore_t m;

int main(int argc, char *argv[]) {
   int i = 1;
   uint32_t test = 20;

   os_init();
   // print_string("We are in main");
   create_thread("stats", &function_stats, NULL, 50); //0
   create_thread("blink", &function_blink, NULL, 50); // 1
   create_thread("main", &main, NULL, 50); // 2
   create_thread("producer", &producer, NULL, 50); // 3
   create_thread("display_bounded_buffer", &display_bounded_buffer, NULL, 50); // 4
   create_thread("consumer", &consumer, NULL, 50); // 5

   /*mutex_init(prMutex);
   mutex_init(crMutex);
   mutex_init(proMutex);*/
   mutex_init(buffMutex);

   sem_init(&empty, 10);
   sem_init(&full, 0);
   sem_init(&m, 1);
   

   os_start();

   sei();
   while(1) {
     /* set_cursor(24, 0);
      print_string("I'm main.");*/
   }
   

 
   
   return 0;
}

void function_blink() {
    int i, test = 0;
    while(1) {
       //mutex_lock(&buffMutex);
       test = test + 1;
    /*for(i = 0; i < 10; i++) {
         _delay_ms(10);
       
    }*/ 
       thread_sleep(5);
       led_on();

    /*for(i = 0; i < 10; i++) {
       // _delay_ms(10);
       
    }*/
       thread_sleep(5);
       led_off();
       //mutex_unlock(&buffMutex);
    }
}
void function_stats(){
   uint16_t sysTime = 0;
   // clear_screen();
   while(1)
   {
      set_color(BLACK);
      if((system.intCount % 100) == 0)
        sysTime++;

      // Thread 0
      set_cursor_home();
      print_string("System Time: ");
      print_int(sysTime);
      
      set_cursor(3, 0);
      print_string("Number of threads: ");
      print_int(system.totalThreads);
      

      set_cursor(4, 0);
      print_string("Thread ID:");
      print_int(system.threads[0].id);
      
      set_cursor(5, 0);
      print_string("Thread Name:");
      print_string(system.threads[0].tName);
      

      set_cursor(6, 0);
      print_string("Thread PC:");
      print_int(&function_stats);


      set_cursor(7, 0);
      print_string("Stack Usage:");
      print_int(system.threads[0].sp - system.threads[0].base + system.threads[0].sSize);
     

      set_cursor(8, 0);
      print_string("Total Stack Size:");
      print_int(system.threads[0].sSize);
      

      set_cursor(9, 0);
      print_string("Current top of Stack:");
      print_int(system.threads[0].sp);

      set_cursor(10, 0);
      print_string("Stack base:");
      print_int(system.threads[0].base);

      set_cursor(11, 0);
      print_string("Stack end:");
      print_int(system.threads[0].base + system.threads[0].sSize);

      set_cursor(12, 0);
      print_string("SP: ");
      print_int(system.threads[0].sp);

      // Thread 1
      
      set_cursor(14, 0);
      print_string("Thread ID:");
      print_int(system.threads[1].id);
      
      set_cursor(15, 0);
      print_string("Thread Name:");
      print_string(system.threads[1].tName);

      set_cursor(16, 0);
      print_string("Thread PC:");
      print_int(&function_blink);

      set_cursor(17, 0);
      print_string("Stack Usage:");
      print_int(system.threads[1].sp - system.threads[1].base + system.threads[1].sSize);

      set_cursor(18, 0);
      print_string("Total Stack Size:");
      print_int(system.threads[1].sSize);

      set_cursor(19, 0);
      print_string("Current top of Stack:");
      print_int(system.threads[1].sp);

      set_cursor(20, 0);
      print_string("Stack base:");
      print_int(system.threads[1].base);

      set_cursor(21, 0);
      print_string("Stack end:");
      print_int(system.threads[1].base + system.threads[1].sSize);

      set_cursor(22, 0);
      print_string("SP: ");
      print_int(system.threads[1].sp);

   }
}

void display_bounded_buffer() {
   mutex_lock(&buffMutex);
   int i  = 0;
   int row = 30;
   // print_string("I'm bounded buffer. ");
   set_color(RED);
   set_cursor(25, 80);

   print_string("Production: ");
   print_int(pRate);
   
   set_cursor(27, 80);
   print_string("Consumer: ");
   print_int(cRate);

/*
   // TODO THIS IS WHAT WE CHANGED
   if (buffer > 10) {
      buffer = 10;
   }
*/

   for( i = 0; i < buffer; i++)
   {
      set_cursor((row+i),80);
      print_string("Item: ");
      print_int(i);
      
   }
   
   set_color(BLACK);
   mutex_unlock(&buffMutex);

}

void producer() {
   
   uint8_t producedItem;
   // print_string("I'm producer. ");

   while(1) {
      mutex_lock(&buffMutex);
      thread_sleep(pRate);
      producedItem = 1;

      sem_wait(&empty);
      sem_wait(&m);
    
      //Add item to buffer
      buffer += producedItem;
      if(buffer > 10) {
         buffer = 10;
      }

      sem_signal(&m);
      sem_signal(&full);
      mutex_unlock(&buffMutex);
  
   }
}

void consumer() {
   // print_string("I'm consumer. ");
   
   while(1) {
      mutex_lock(&buffMutex);
      sem_wait(&full);
      sem_wait(&m);

      thread_sleep(cRate);
   
      // Remove item from buffer
      if(buffer <= 0)
         buffer = 0;
      else
         buffer--;
      sem_signal(&m);
      sem_signal(&empty);
       

      // Consume item
      mutex_unlock(&buffMutex);
   }
}

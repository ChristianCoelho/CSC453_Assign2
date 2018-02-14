/**
 * @author Christian Coelho & Justin Tomas
 */

// #include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "os.h"
#include <string.h>
void function_blink();
void function_stats();
extern system_t system;
int main(int argc, char *argv[]) {
   int i = 1;
   uint32_t test = 20;

   os_init();
   print_string("We are in main");
   create_thread("stats", &function_stats, NULL, 50); //0
   create_thread("blink", &function_blink, NULL, 50); // 1
   // 1
   create_thread("main", &main, NULL, 50); // 2

   os_start();
   

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
void function_stats(){
   uint16_t sysTime = 0;
   clear_screen();
   while(1)
   {
      if((system.intCount % 100) == 0)
        sysTime++;

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
      //print_string(system.threads[0].tName);
      

      set_cursor(7, 0);
      print_string("Stack Usage:");
      //print_string(system.threads[0].tName);
     

      set_cursor(8, 0);
      print_string("Total Stack Size:");
      print_string(system.threads[0].sSize);
      

      set_cursor(9, 0);
      print_string("Current top of Stack:");
      //print_string(system.threads[0].tName);

      set_cursor(10, 0);
      print_string("Stack base:");
      print_int(system.threads[0].base);

      set_cursor(11, 0);
      print_string("Stack end:");
      //print_int(system.threads[0].base);

      set_cursor(12, 0);
      print_string("SP: ");
      print_int(system.threads[0].sp);

      //Theard 1
      
      set_cursor(14, 0);
      print_string("Thread ID:");
      print_int(system.threads[1].id);
      
      set_cursor(15, 0);
      print_string("Thread Name:");
      print_string(system.threads[1].tName);

      set_cursor(16, 0);
      print_string("Thread PC:");
      //print_string(system.threads[1].tName);

      set_cursor(17, 0);
      print_string("Stack Usage:");
      //print_string(system.threads[1].tName);

      set_cursor(18, 0);
      print_string("Total Stack Size:");
      print_string(system.threads[1].sSize);

      set_cursor(19, 0);
      print_string("Current top of Stack:");
      //print_string(system.threads[1].tName);

      set_cursor(20, 0);
      print_string("Stack base:");
      print_int(system.threads[1].base);

      set_cursor(21, 0);
      print_string("Stack end:");
      //print_int(system.threads[1].base);

      set_cursor(22, 0);
      print_string("SP: ");
      print_int(system.threads[1].sp);
      
   }
}


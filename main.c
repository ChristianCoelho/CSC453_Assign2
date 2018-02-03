/**
 * @author Christian Coelho & Justin Tomas
 */

// #include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "os.h"
#include <string.h>

int main(int argc, char *argv[]) {
   system_t system;
   int i = 0;
   uint32_t test = 20;

   create_thread("blink", &function_blink, NULL, 50); // 0
   create_thread("stats", &function_stats, NULL, 50); // 1
   create_thread("main", &main, NULL, 50); // 2
   os_init();
   os_start();

   // context_switch(&(system.threads[0].sp), &(system.threads[0].sp));
   context_switch(&(system.threads[0].sp), &main);
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
    print_hex32(test);
    }
}
void function_stats(){
   print_string("Yes");
}


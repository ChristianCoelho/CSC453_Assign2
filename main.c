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
   uint16_t test = 1;
   serial_init();
while(1)
{
	test = test * 2;
   for(i = 0; i < 10; i++)
	   {
	       _delay_ms(10);
	   }
	   
	   led_on();

	   for(i = 0; i < 10; i++)
	   {
	       _delay_ms(10);
	   }

       led_off();

   context_switch(&(system.threads[0].sp), &(system.threads[0].sp));
   print_hex(test);

 }
   return 0;
}

/**
 * @author Christian Coelho & Justin Tomas
 */

// #include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "os.h"

led_on(void)
{
 asm volatile("LDI r30, 0x24");
 asm volatile("LDI r31, 0x00");
 asm volatile("LD r18, Z");
 asm volatile("ori r18, 128");
 asm volatile("st Z, r18");

 asm volatile("LDI r30, 0x25");
 asm volatile("LDI r31, 0x00");
 asm volatile("LD r18, Z");
 asm volatile("ori r18, 128");
 asm volatile("st Z, r18");
}
led_off(void)
{
  asm volatile("LDI r30, 0x24");
 asm volatile("LDI r31, 0x00");
 asm volatile("LD r18, Z");
 asm volatile("ori r18, 128");
 asm volatile("st Z, r18");

 asm volatile("LDI r30, 0x25");
 asm volatile("LDI r31, 0x00");
 asm volatile("LD r18, Z");
 asm volatile("andi r18, 127");
 asm volatile("st Z, r18");
}


int main(int argc, char *argv[]) {
   system_t system;
   int i = 0;
while(1)
{
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
 }
   return 0;
}

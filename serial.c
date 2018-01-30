#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define ESC 27
/*
 * Initialize the serial port.
 */
void serial_init() {
   uint16_t baud_setting;

   UCSR0A = _BV(U2X0);
   baud_setting = 16; //115200 baud

   // assign the baud_setting
   UBRR0H = baud_setting >> 8;
   UBRR0L = baud_setting;

   // enable transmit and receive
   UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
}

/*
 * Return 1 if a character is available else return 0.
 */
uint8_t byte_available() {
   return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

/*
 * Unbuffered read
 * Return 255 if no character is available otherwise return available character.
 */
uint8_t read_byte() {
   if (UCSR0A & (1 << RXC0)) return UDR0;
   return 255;
}

/*
 * Unbuffered write
 *
 * b byte to write.
 */
uint8_t write_byte(uint8_t b) {
   //loop until the send buffer is empty
   while (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) {}

   //write out the byte
   UDR0 = b;
   return 1;
}

print_string(char* s)
{
   int i = 0;

   for(i = 0; s[i] != '\0'; i++)
   {
      write_byte(s[i]);
   }
}

print_int(uint16_t i)
{
   char s[5];
   utoa(i,s,10);
   print_string(s);
}

print_int32(uint32_t i)
{
   int j = 0;
   char array[10];

   if(i == 0)
   {
     write_byte('0');
     return;
   }
   
   while(i > 0)
   {
      array[j]= i % 10 + 48;
      i = i / 10;
      j++;
   }
   while(j >= 0)
   {
      write_byte(array[j]);
      j--;
   }
}

print_hex(uint16_t i)
{
   char s[10];
   sprintf(s, "%X", i);
   print_string(s);
}

print_hex32(uint32_t i)
{
   char s[10];
   int j = 0, remainder = 0;

   if(i == 0)
   {
     write_byte('0');
     return;
   }

   while(i > 0)
   {
      remainder = i % 16;

      if(remainder < 10)
         s[j] = remainder + 48;
      else
         s[j] = remainder + 55;

      i = i / 16;
      j++;
   }

   while(j >= 0)
   {
      write_byte(s[j]);
      j--;
   }
   
}

set_cursor(uint8_t row, uint8_t col)
{
   write_byte(ESC);
   write_byte('[');
   print_int(row);
   write_byte(';');
   print_int(col);
   write_byte('H');

   /*<ESC>[{ROW};{COLUMN}H*/
}

set_cursor_home()
{
   write_byte(ESC);
   write_byte('[');
   write_byte('H');

   /*<ESC>[{ROW};{COLUMN}H*/
}

set_color(uint8_t color)
{
   write_byte(ESC);
   write_byte('[');
   print_int(color);
   write_byte('m');
}

clear_screen(void)
{
   write_byte(ESC);
   write_byte('[');
   print_int(2);
   write_byte('J');

}

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

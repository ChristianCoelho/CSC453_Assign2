/**
 * @author Christian Coelho & Justin Tomas
 */

// #include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "os.h"

int main(int argc, char *argv[]) {
   system_t system;

   context_switch(main, main);

   return 0;
}

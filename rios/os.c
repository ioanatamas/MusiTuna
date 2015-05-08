/* FortunaOS     
   _____             _                          ___   ____  
  |  ___|___   _ __ | |_  _   _  _ __    __ _  / _ \ / ___| 
  | |_  / _ \ | '__|| __|| | | || '_ \  / _` || | | |\___ \ 
  |  _|| (_) || |   | |_ | |_| || | | || (_| || |_| | ___) |
  |_|   \___/ |_|    \__| \__,_||_| |_| \__,_| \___/ |____/ 

  Minimalist Operating System for LaFortuna board, build on:
    * Peter Dannegger's Code for the Rotary Encoder
    * Peter Dannegger's Code for Switch debouncing
    * Steve Gunn's display driver
    * The RIOS scheduler

  Occupies Timer T0 for scheduling and LED brightness.
 
*/

#include "os.h"
#include "rios.h"
#include "ruota.h"

void os_init(void) {
	/* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    DDRB  |=  _BV(PB7);  	 /* LED as output */
    
    init_lcd();
    os_init_scheduler();
    os_init_ruota();
}


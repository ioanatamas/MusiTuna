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

#ifndef OS_H
#define OS_H

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "buttons.h"
#include "rios.h"
#include "ruota.h"


#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7) 
#define LED_TOGGLE  PINB  |=  _BV(PINB7)

void os_init(void);

#endif /* OS_H */


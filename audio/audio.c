/*
 * Plays a song given the note pitches and durations using PWM
 * Adapted from:
 * https://github.com/fatcookies
 * and
 * http://avrpcm.blogspot.co.uk/2010/11/playing-8-bit-pcm-using-any-avr.html 
 */


#include "audio.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>

/* Wave details */
const double freq[] =
    {392.00, 415.30, 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99,
    783.99, 830.61, 880.00, 932.33, 987.77, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98};
const uint8_t len[] = {102, 212, 200, 103, 162, 107, 101, 109, 180, 182, 126, 173, 102, 183, 100, 103, 162, 237, 101, 143, 135, 182, 126, 200};           
const uint16_t DUNIT = 500;
unsigned char wave[24][250];

/* Auxiliary variables */
int i,j,wv;

/* Current notes */
uint8_t *current, csz;

/* Current sample playing from the buffer */
volatile uint16_t sample;

/* How often to take samples (32000/sample_count) = sample rate*/
volatile int sample_count;

/* Playing audio flag */
volatile uint8_t playing = 0;

/* initialise the PWM */
void pwm_init(uint8_t sz, uint8_t x[20]) {
	 
   /* use OC1A (RCH) and OC3A (LCH) pin as output */
    DDRB |= _BV(PB5);
    DDRC |= _BV(PC6);

    /* 
    * clear OC1A/OC3A on compare match 
    * set OC1A/OC3A at BOTTOM, non-inverting mode
    * Fast PWM, 8bit
    */
    TCCR1A |= _BV(COM1A1) | _BV(WGM10);
    TCCR3A |= _BV(COM3A1) | _BV(WGM30);
    
    /* 
    * Fast PWM, 8bit
    * Prescaler: clk/1 = 8MHz
    * PWM frequency = 8MHz / (255 + 1) = 31.25kHz
    */
    TCCR1B |= _BV(WGM12) | _BV(CS10);
    TCCR3B |= _BV(WGM32) | _BV(CS30);
    
    /* set initial duty cycle to zero */
    OCR1A = 0;
    OCR3A = 0;
    
    /* Setup Timer0 (RCH) */
    TCCR1A |= (1<<CS00);
    TCNT1 = 0;
    TIMSK1 |= (1<<TOIE1);
    
    /* Setup Timer3 (LCH) */
    TCCR3A |= (1<<CS00);
    TCNT3 = 0;
    TIMSK3 |= (1<<TOIE3);
    
    current = x;
    csz = sz;
    sample = 0;
    sample_count = SAMPLE;
    playing = 1;
    sei();
}

/* Replicate same behavior on LCH as RCH */
ISR(TIMER3_OVF_vect) {
	OCR3A = OCR1A;
} 

/* Play a wave using the PWM register*/
ISR(TIMER1_OVF_vect) {
    sample_count--;
    if (sample_count == 0) {
        sample_count = SAMPLE;
        wv = 128;
        for(j=0;j<csz;++j) {
            wv += wave[current[j]][sample%len[current[j]]];
        }
        OCR1A = wv;
        ++sample;
    }
}

/* Return playing flag */
uint8_t audio_isplaying() {
	return playing;
}

/* Disable Timer0 and Timer3 and set playing flag to FALSE */
void audio_close() {
	if(playing) {
        cli();
		TCCR1B &= ~_BV(CS10);
		TCCR3B &= ~_BV(CS30);
		TCCR1A &= ~_BV(CS00);
		TIMSK1 &= (0<<TOIE1); 
		TCCR3A &= (0<<CS00);
		TIMSK3 &= (0<<TOIE3);
		playing = 0;
		sei();
	}
}

/* Compute the waves for the 24 declared frequencies */
void init_notes() {
    for(i=0;i<24;++i) {
        for(j=0;j<len[i];++j) {
            wave[i][j] = VOLUME * sin(j * MULTIPLIER * freq[i]);
        }
    }
}

/* Play the wave for the current set of notes for the given duration */
void play_note(uint8_t sz, uint8_t x[3], uint8_t duration) {
	pwm_init(sz, x);
    while (playing) {
        if(sample >= DUNIT*duration) {
            audio_close();
        }
    }
}

/* Play multiple sets of notes */
void play_song(uint8_t sz, uint8_t notes[20][3], uint8_t szs[20], uint8_t durations[20]) {
    for(i=0; i<sz;++i) {
        play_note(szs[i], notes[i], durations[i]);
    }
}

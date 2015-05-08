/*
 * Plays a song given the note pitches and durations using PWM
 * Adapted from:
 * https://github.com/fatcookies
 * and
 * http://avrpcm.blogspot.co.uk/2010/11/playing-8-bit-pcm-using-any-avr.html 
 */

#include <stdint.h>
#define SAMPLE_RATE 8000
#define SAMPLE 32000/SAMPLE_RATE
#define VOLUME 20
#define MULTIPLIER 2*M_PI/SAMPLE_RATE

uint8_t audio_isplaying();
void audio_close();
void play_note(uint8_t sz, uint8_t x[3], uint8_t duration);
void init_notes();
void play_song(uint8_t sz, uint8_t notes[20][3], uint8_t szs[20], uint8_t durations[20]);

/* https://github.com/ioanatamas/MusiTuna
* Displays and enables editing and playing of short song
*/


#include "os.h"
#include "audio.h"

/* Wheel reading */
int collect_delta(int);
int check_switches(int);
int position, last_position; /* rotary encoder */

/* Height, sharpness and name of each note */
const uint8_t he[] = {124,124,118,118,111,105,105,98,98,92,85,85,79,79,72,72,66,59,59,53,53,46,40,40};
const uint8_t sharp[] = {0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1};
const char* names[] = {"G","G#","A","A#","B","C","C#","D","D#","E","F","F#","g","g#","a","a#","b","c","c#","d","d#","e","f","f#"};

/* Application state */
uint8_t button_mode, add_mode; 

/* Helper variables */
uint8_t h, i, j;

/* Current note */
uint8_t note_height;

/* Current song */
uint8_t song_len, notes[20][3], szs[20], durations[20];

/* Indicator */
rectangle indicator = {10,13,3,13};
uint16_t indicator_pos, indicator_col;


void draw_note(uint8_t x, uint8_t y, uint8_t d, uint16_t col) {
    cli();
    h = he[y];
    
    /* Additional lines */
    if(y <= 6) {
        fill_rec(x*20,x*20+19,104,105,GREY);
    }
    if(y <= 3) {
        fill_rec(x*20,x*20+19,117,118,GREY);
    }

    /* Sharp notes */
    if(sharp[y]) {
        fill_rec(x*20+16,x*20+16,h-2,h+3,col);
        fill_rec(x*20+18,x*20+18,h-2,h+3,col);
        fill_rec(x*20+15,x*20+19,h-1,h-1,col);
        fill_rec(x*20+15,x*20+19,h+1,h+1,col);
    }
    /* No tail */
    if(d==16) {
        fill_rec(x*20+2,x*20+12,h-5,h+4,col);
        fill_rec(x*20,x*20+1,h-3,h+2,col);
        fill_rec(x*20+13,x*20+14,h-3,h+2,col);
        fill_rec(x*20+3,x*20+11,h-4,h+3,WHITE);
        fill_rec(x*20+1,x*20+2,h-2,h+1,WHITE);
        fill_rec(x*20+12,x*20+13,h-2,h+1,WHITE);
        return;
    }
   
    fill_rec(x*20+2,x*20+12,h-5,h+4,col);
    fill_rec(x*20,x*20+1,h-3,h+2,col);
    fill_rec(x*20+13,x*20+14,h-25,h+2,col); 

    /* empty inside */
    if(d==8) {
        fill_rec(x*20+3,x*20+11,h-4,h+3,WHITE);
        fill_rec(x*20+1,x*20+2,h-2,h+1,WHITE);
        fill_rec(x*20+12,x*20+13,h-4,h+1,WHITE);
    }

    /* flag 1 */
    if(d<=2) {
        fill_rec(x*20+15,x*20+16,h-23,h-21,col);
        fill_rec(x*20+17,x*20+18,h-21,h-19,col);
        fill_rec(x*20+19,x*20+19,h-19,h-17,col); 
    }

    /* flag 2 */
    if(d==1) {
        fill_rec(x*20+15,x*20+16,h-17,h-15,col);
        fill_rec(x*20+17,x*20+18,h-15,h-13,col);
        fill_rec(x*20+19,x*20+19,h-13,h-11,col); 
    }
    sei();
}

void preview_current_note() {
    if(add_mode) {
        draw_note(indicator_pos, note_height, durations[indicator_pos], PLUM);
        fill_rec(10,30,153,177,YELLOW);
        display_color(RED,YELLOW);
        display_string_xy(names[note_height],15,160);
        display_color(WHITE,BLACK);
    }
}

void draw_section(uint8_t x) {
    fill_rec(x*20,x*20+19,15,130,WHITE);
    for(j=40;j<=92;j+=13) {
        fill_rec(x*20,x*20+19,j,j+1,GREY);
    }
    for(j=0;j<szs[x];++j) {
        draw_note(x, notes[x][j], durations[x], BLACK);
    }
}

void init_graphics() {
    button_mode = 0;
    add_mode = 1;

    position = 0;
    last_position = 0;
    
    clear_screen();
    indicator_col = LIME_GREEN;
    indicator_pos = 0;
    indicator.left = 10;
    indicator.right = 13;
    fill_rectangle(indicator,indicator_col);
    indicator_pos = 0;

    note_height = 5;
    song_len = 0;
    for(i=0;i<20;++i) {
        szs[i] = 0;
        durations[i] = 4;
    }

    for(i=0;i<16;++i) {
        draw_section(i);
    }
    preview_current_note();

    init_buttons();
    make_button(50,150, "  RESET  ");
    make_button(140,150,"  PLAY!  ");
    make_button(230,150,"ADD/ERASE");

    display_string_xy("Use the wheel to change the duration",10,190);
    display_string_xy("Long press C to go to and back from the buttons",10,210);
    display_string_xy("Press C to add the purple note to your song",10,200);
    display_string_xy("Push ADD/ERASE to go to or back from erase mode",10,220);
}

/* Actions for the 3 buttons */
void do_action(uint8_t x) {
    /*Reset*/
    if(x==0) {
        init_graphics();
    }
    /*Play*/
    if(x==1) {
        play_song(song_len,notes,szs,durations);
    }
    /*Toggle between add and erase*/
    if(x==2) {
        add_mode = 1 - add_mode;
        if(add_mode) {
            indicator_col = LIME_GREEN;
        } else {
            indicator_col = RED;
        }
        fill_rectangle(indicator, indicator_col);
    }
}

/* Add note to current section if there is room */
void add_current_note() { 
    if(szs[indicator_pos] < 3) {
        notes[indicator_pos][szs[indicator_pos]] = note_height;
        ++szs[indicator_pos];
        draw_section(indicator_pos);
    }
    if(indicator_pos >= song_len) {
        song_len = indicator_pos+1;
    }
}

/* Clear a section. If it is the last one, shorten song */
void clear_section() {
    szs[indicator_pos] = 0;
    while(song_len > 0 && szs[song_len-1] == 0) {
        --song_len;
    } 
    draw_section(indicator_pos);
}

void welcome_screen() {
    display_string_xy("Welcome to MusiTuna!",90,150);
    draw_note(2, 7, 1, RED);
    draw_note(5, 10, 2, ORANGE);    
    draw_note(8, 7, 4, YELLOW);    
    draw_note(11, 12, 2, GREEN);    
    draw_note(14, 14, 1, BLUE);    
}

void main(void) {
    os_init();
    welcome_screen();
    init_notes();
    os_add_task( collect_delta,   100, 1);
    os_add_task( check_switches,  100, 1);
    init_graphics();
    sei();
    for(;;) {}   
}

/* Rotary encoder */
int collect_delta(int state) {
	position += os_enc_delta();
	return state;
}

/* Wheel switches */
int check_switches(int state) {	
    if(position < last_position) {
        if(!button_mode && durations[indicator_pos]>=2) {
            durations[indicator_pos] /= 2;
            draw_section(indicator_pos);
            preview_current_note();
        }
        last_position = position;
    }

    if(position > last_position) {
        if(!button_mode && durations[indicator_pos] <= 8) {
            durations[indicator_pos] *= 2;
            draw_section(indicator_pos);
            preview_current_note();
        }
        last_position = position;
    }

	if (get_switch_press(_BV(SWN))) {
	    if(!button_mode && szs[indicator_pos] < 3 && note_height < 23) {
            ++note_height;
            draw_section(indicator_pos);
            preview_current_note();
        }
    }
		
	if (get_switch_press(_BV(SWE))) {
        if(!button_mode) {
            if(indicator.right + 20 <= display.width) {
                draw_section(indicator_pos);
                ++indicator_pos;
                fill_rectangle(indicator, BLACK);
                indicator.left += 20;
                indicator.right += 20;
                fill_rectangle(indicator, indicator_col);
                preview_current_note();
            }
        } else {
            move_button_right(); 
        }
    }
		
	if (get_switch_press(_BV(SWS))) { 
        if(!button_mode && szs[indicator_pos] < 3 && note_height > 0) {
            --note_height;
           draw_section(indicator_pos);
           preview_current_note();
        }
    }
		
	if (get_switch_press(_BV(SWW))) {
        if(!button_mode) {
            if(indicator.left > 20) {
                draw_section(indicator_pos);
                --indicator_pos;
                fill_rectangle(indicator, BLACK);
                indicator.left -= 20;
                indicator.right -= 20;
                fill_rectangle(indicator, indicator_col);
                preview_current_note();
            }
        } else {
            move_button_left(); 
        }
    }
		
	if (get_switch_long(_BV(SWC))) {
        if(button_mode) {
            reset_selections();
            preview_current_note();
        } else {
            draw_section(indicator_pos);
            select_button(0);
        }
        button_mode = 1-button_mode;
    }

	if (get_switch_short(_BV(SWC))) {
        if(button_mode) {
            do_action(get_selected_button());
        } else {
            if(add_mode) {
                add_current_note();
            } else {
                clear_section();
            }
        }
    }
	return state;	
}

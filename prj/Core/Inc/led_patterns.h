#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#define METEOR_LEN 5

extern uint32_t col_arr[9];
extern char * col_name [9];
extern uint32_t col_arr_idx;

void display_pattern (pattern_state pattern_in, uint32_t cur_color, uint8_t brightness);
void static_color(unsigned int grb_value);
void rainbow();
void meteor(unsigned int grb_value);
void breathe(unsigned int grb_value);

#endif


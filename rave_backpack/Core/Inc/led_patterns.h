/*
 *	===============================================================================================
 *  File Name   :   led_patterns.h
 *
 *  Description :   header file for patterns to display on WS2812B
 *
 *  Contributors:   Nick Rizzo
 *	===============================================================================================
 */
#ifndef LED_PATTERNS_H_
#define LED_PATTERNS_H_

#define METEOR_LEN 5

extern uint32_t col_arr[9];
extern char * col_name [9];
extern uint32_t col_arr_idx;

void display_pattern (pattern_state pattern_in, uint32_t cur_color, uint8_t brightness, uint8_t speed);
void static_color(unsigned int grb_value);
void rainbow(uint8_t delay_modifier);
void meteor(unsigned int grb_value, uint8_t delay_modifier);
void breathe(unsigned int grb_value, uint8_t delay_modifier);

#endif


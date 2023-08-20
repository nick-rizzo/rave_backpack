#ifndef LED_PATTERNS
#define LED_PATTERNS
#include "globals.h"
#include "led_driver.h"

typedef enum{
		RED,
		RED_2_GRN,
		GRN,
		GRN_2_BLU,
		BLU,
		BLU_2_RED
	}color_state;

typedef enum {
	INITIAL,
	BR_IN,
	BR_OUT
} breathe_states;

//GRB
const uint32_t red  	= 0x00FF00U;
const uint32_t orange 	= 0x88FF00U;
const uint32_t yellow 	= 0xFFFF00U;
const uint32_t green 	= 0xFF0000U;
const uint32_t blue 	= 0x0000FFU;
const uint32_t purple 	= 0x005CBFU;
const uint32_t pink		= 0x00BFAFU;
const uint32_t cyan 	= 0xBF00BFU;
const uint32_t white 	= 0xFFFFFFU;
const uint32_t black 	= 0x000000U;

const uint32_t col_arr [9] = {red, orange, yellow, green, blue, purple, pink, cyan, white};
const char * col_name [9] = {
		"    Red     ",
		"   Orange   ",
		"   Yellow   ",
		"   Green    ",
		"    Blue    ",
		"   Purple   ",
		"    Pink    ",
		"    Cyan    ",
		"   White    "
};
uint32_t col_arr_idx = 0;
int head_pos = 0;

unsigned int brightness_modifier = 230; //int to subtract brightness from
unsigned int r_val=25;
unsigned int g_val=0;
unsigned int b_val=0;

uint32_t cur_col = red;

color_state cur_state = RED;
breathe_states br_state = INITIAL;

unsigned int breathe_color;

void static_color(unsigned int grb_value){
	for(int i=0;i<NUM_LEDS;i++){
		drive_led(grb_value);
	}
}

void rainbow(){
	unsigned int color;
	color = (g_val<<16) + (r_val<<8) + (b_val<<0);

	switch(cur_state){
		case RED:{
			if(g_val == 255 - brightness_modifier){
				cur_state = RED_2_GRN;
			}else{
				g_val++;

			}
			break;
		}
		case RED_2_GRN:{
			if (r_val == 0){
				cur_state = GRN;
			}else{
				r_val--;
			}
				break;
		}
		case GRN:{
			if (b_val == 255 - brightness_modifier){
				cur_state = GRN_2_BLU;
			}else{
				b_val++;
			}
				break;

		}
		case GRN_2_BLU:{
			if (g_val == 0){
				cur_state = BLU;
			}else{
				g_val--;
			}
				break;
		}
		case BLU:{
			if(r_val == 255 - brightness_modifier){
				cur_state = BLU_2_RED;
			}else{
				r_val++;
			}
				break;
		}
		case BLU_2_RED:{
			if(b_val == 0){
				cur_state = RED;
			} else{
				b_val--;
			}
				break;

		}
	}

	for(int i=0;i<NUM_LEDS;i++){
		drive_led(color);
	}
	HAL_Delay(5 * (255/(255-brightness_modifier)));
}

void meteor(unsigned int grb_value){
    #define METEOR_TAIL_LEN 5
	uint32_t meteor_array [NUM_LEDS + METEOR_TAIL_LEN + NUM_LEDS] = {0};
	meteor_array[NUM_LEDS+0] = grb_value;
	meteor_array[NUM_LEDS+1] = grb_value&0b001111110011111100111111U;
	meteor_array[NUM_LEDS+2] = grb_value&0b000011110000111100001111U;
	meteor_array[NUM_LEDS+3] = grb_value&0b000000110000001100000011U;
	meteor_array[NUM_LEDS+4] = grb_value&0b000000010000000100000001U;

	for(int i=head_pos;i<head_pos + NUM_LEDS;i++){
		drive_led(meteor_array[i]);
	}

	if (head_pos == METEOR_TAIL_LEN + NUM_LEDS){
		head_pos = 0;
		HAL_Delay(2000);
	} else{
		head_pos++;
	}
	HAL_Delay(60);
}

void breathe(unsigned int grb_value){

	unsigned int color;
	uint8_t target_g;
	uint8_t target_r;
	uint8_t target_b;

	switch (br_state){
		case INITIAL:{
			breathe_color = grb_value;
			g_val = (grb_value>>16)&0xFF;
			r_val = (grb_value>>8)&0xFF;
			b_val = (grb_value>>0)&0xFF;
			br_state = BR_IN;
			break;
		}
		case BR_IN:{
			if ((g_val==0) && (r_val==0) && (b_val==0)){
				br_state = BR_OUT;
				HAL_Delay(1000);
			}else{
				if (g_val == 0){ g_val = 0; }else{ g_val--; }
				if (r_val == 0){ r_val = 0; }else{ r_val--; }
				if (b_val == 0){ b_val = 0; }else{ b_val--; }
			}
			break;
		}
		case BR_OUT:{
			target_g = (breathe_color>>16)&0xFF;
			target_r = (breathe_color>>8)&0xFF;
			target_b = (breathe_color>>0)&0xFF;
			if ((g_val==target_g) && (r_val==target_r) && (b_val==target_b)){
				br_state = BR_IN;
			}else{
				if (g_val == target_g) { g_val = g_val; }else{ g_val++; }
				if (r_val == target_r) { r_val = r_val; }else{ r_val++; }
				if (b_val == target_b) { b_val = b_val; }else{ b_val++; }
			}
			break;
		}
	}


	color = (g_val<<16) + (r_val<<8) + (b_val<<0);
	for(int i=0;i<NUM_LEDS;i++){
		drive_led(color);
	}

	HAL_Delay(10);
}

void display_pattern (pattern_state pattern_in){
	switch (pattern_in){
		case IDLE:{
			break;
		}
		case STATIC:{
			static_color(cur_col);
			break;
		}
		case BREATHE:{
			breathe(cur_col);
			break;
		}
		case RAINBOW:{
			rainbow();
			break;
		}
		case METEOR:{
			meteor(cur_col);
			break;
		}
	}
}

#endif

#ifndef LED_DRIVER
#define LED_DRIVER

#include <stdint.h>
#include<string.h>
#include "globals.h"
#include "led_driver.h"
#include "main.h"


SPI_HandleTypeDef hspi1;
int led_idx = 0;
int num_iter = 0;
uint8_t dma_buffer [DMA_BUFFER_SIZE] = {0};

unsigned int gamma_correct(unsigned int grb_value){
	const uint8_t gamma_lut[] = {
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
	unsigned int corrected_color;
	unsigned int cor_g = gamma_lut[(grb_value>>16)&CMASK];
	unsigned int cor_r = gamma_lut[(grb_value>>8)&CMASK];
	unsigned int cor_b = gamma_lut[(grb_value>>0)&CMASK];
	corrected_color = ((cor_g<<16)&0x00FF0000) | ((cor_r<<8)&0x0000FF00) | ((cor_b<<0)&0x000000FF);
	return corrected_color;
}

void dma_buffer_init(){
	led_idx = 0;
	memset(dma_buffer,0x00,DMA_BUFFER_SIZE);
	HAL_SPI_Transmit_DMA(&hspi1,dma_buffer,DMA_BUFFER_SIZE);
}

void dma_buffer_write(unsigned int grb_value){
// take 24 bit GRB color value and write to SPI compatible serial
// bit low - 100, bit high = 110
	uint32_t red_buffer = 0;
	uint32_t grn_buffer = 0;
	uint32_t blu_buffer = 0;
//	unsigned int corrected_color = gamma_correct(grb_value);
	uint8_t gval = (grb_value>>16)&CMASK;
	uint8_t rval = (grb_value>>8)&CMASK;
	uint8_t bval = (grb_value>>0)&CMASK;

	//green
	for(int g=0;g<8;g++){
		grn_buffer = grn_buffer<<3;
		if (gval & 0x80){
			grn_buffer |= 0b110;
		} else{
			grn_buffer |= 0b100;
		}
		gval = gval << 1;
	}

	//red
	for(int r=0;r<8;r++){
		red_buffer = red_buffer<<3;
		if (rval & 0x80){
			red_buffer |= 0b110;
		} else{
			red_buffer |= 0b100;
		}
		rval = rval << 1;
	}

	//blue
	for(int b=0;b<8;b++){
		blu_buffer = blu_buffer<<3;
		if (bval & 0x80){
			blu_buffer |= 0b110;
		} else{
			blu_buffer |= 0b100;
		}
		bval = bval << 1;
	}

	dma_buffer[(9*led_idx)+0] = (grn_buffer>>16)&CMASK;
	dma_buffer[(9*led_idx)+1] = (grn_buffer>>8)&CMASK;
	dma_buffer[(9*led_idx)+2] = (grn_buffer>>0)&CMASK;
	dma_buffer[(9*led_idx)+3] = (red_buffer>>16)&CMASK;
	dma_buffer[(9*led_idx)+4] = (red_buffer>>8)&CMASK;
	dma_buffer[(9*led_idx)+5] = (red_buffer>>0)&CMASK;
	dma_buffer[(9*led_idx)+6] = (blu_buffer>>16)&CMASK;
	dma_buffer[(9*led_idx)+7] = (blu_buffer>>8)&CMASK;
	dma_buffer[(9*led_idx)+8] = (blu_buffer>>0)&CMASK;

	if (led_idx == NUM_LEDS-1){
		led_idx = 0;
	}else{
		led_idx++;
	}
};

void drive_led(unsigned int grb_value){
	dma_buffer_write(grb_value);
	HAL_SPI_Transmit_DMA(&hspi1,dma_buffer,DMA_BUFFER_SIZE);
}

#endif

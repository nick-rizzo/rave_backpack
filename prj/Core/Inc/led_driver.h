#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "main.h"

#define NUM_LEDS 19
#define END_TX_DELAY 199 //3MHz * 150 = ~50us
#define DMA_BUFFER_SIZE (NUM_LEDS*9) + END_TX_DELAY
#define CMASK 0xFF

extern int led_idx;

unsigned int gamma_correct(unsigned int grb_value);
void dma_buffer_init();
void dma_buffer_write(unsigned int grb_value);
void drive_led(unsigned int grb_value);
void change_brightness(uint8_t brightness_modifier);

#endif

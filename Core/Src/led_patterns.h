#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#define METEOR_LEN 5

extern int test_white;
extern int test_green;
extern int test_red;
extern int test_blue;
extern int test_black;

void static_color(unsigned int grb_value);
void rainbow();
void meteor(unsigned int grb_value);
void breathe(unsigned int grb_value);

#endif


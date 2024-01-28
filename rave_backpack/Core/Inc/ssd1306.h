/*
 *	===============================================================================================
 *  File Name   :   ssd1306.h
 *
 *  Description :   header file for WS2812B driver
 *
 *  Contributors:   Nick Rizzo
 *	===============================================================================================
 */
#ifndef SSD1306_H_
#define SSD1306_H_

#define SSD1306_ADDR (0x3C<<1)
#define SSD1306_BUF_SIZE (64/8) * 128//8 pages of 8 bits, 128 columns

const typedef enum {
	LEFT_ARROW,
	RIGHT_ARROW
} shapes;

void write_command(uint8_t data);
void write_data(uint8_t *data);
void ssd1306_update_display();
void ssd1306_fill_buffer(uint8_t val);
void clear_display();
void place_pixel(int x, int y);
void clear_pixel(int x, int y);
void invert_pixel(int x, int y);
void place_char(char c, int x, int y);
void write_page(int page_num, char* line);
void draw_box(int width, int height, int x, int y);
void clear_box(int width, int height, int x, int y);
void clear_page(int page_num);
void invert_box(int width, int height, int x, int y);
void insert_shape (int x, int y, shapes shape);
void ssd1306_init();
void display_init();
void insert_selector(int cur_select);
void move_selector(int cur_select, int new_select);

#endif

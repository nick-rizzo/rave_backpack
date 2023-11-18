/*
 *	===============================================================================================
 *  File Name   :   ssd1306.c
 *
 *  Description :   SSD1306 OLED driver
 *
 *  Contributors:   Nick Rizzo
 *	===============================================================================================
 */
#ifndef SSD1306_C_
#define SSD1306_C_

#include <string.h>
#include <stdint.h>
#include "main.h"
#include "ssd1306.h"
#include "fonts.h"

//I2C_HandleTypeDef hi2c1;
uint8_t ssd1306_buf[SSD1306_BUF_SIZE];

void write_command(uint8_t data){
	uint8_t cmd_data[2];
	cmd_data[0] = 0x00;
	cmd_data[1] = data;
	HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, cmd_data, 2, HAL_MAX_DELAY);
}

void write_data(uint8_t *data){
	uint8_t data_buf[SSD1306_BUF_SIZE+1];
	data_buf[0] = 0x40;
	for(int i=0;i<SSD1306_BUF_SIZE;i++){
		data_buf[i+1] = data[i];
	}
	HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, data_buf, SSD1306_BUF_SIZE+1, HAL_MAX_DELAY);
}

void ssd1306_update_display(){
	write_command(0x21); // column start/end
	write_command(0x00);
	write_command(0x7F);
	write_command(0x22); // PAGE start/end
	write_command(0x00);
	write_command(0x07);

	write_data(ssd1306_buf);
}

void ssd1306_fill_buffer(uint8_t val){
	memset(ssd1306_buf, val, SSD1306_BUF_SIZE);
}

void clear_display(){
	ssd1306_fill_buffer(0x00);
	ssd1306_update_display();
}

void ssd1306_init(){
	write_command(0xAE); //display off

	write_command(0x20); //Set Memory Addressing Mode
	write_command(0x00); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid

	write_command(0x81); //--set contrast control register
	write_command(0xFF); //     contrast 255

	write_command(0x21); //set start column
	write_command(0x00); //    start col 0
	write_command(0x7F); //    end col 127

	write_command(0x22); // PAGE start/end
	write_command(0x00); //    start page 0
	write_command(0x07); //    end page 7

	write_command(0x40); //--set start line address

	write_command(0xA1); //--set segment re-map 0 to 127

	write_command(0xA8); //--set multiplex ratio(1 to 64)
	write_command(0x3F); //

	write_command(0xC8); //Set COM Output Scan Direction

	write_command(0xD3); //-set display offset
	write_command(0x00); //-not offset

	write_command(0xDA); //--set com pins hardware configuration
	write_command(0x12);

	write_command(0xA6); //--set normal display

	write_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content


	write_command(0xD5); //--set display clock divide ratio/oscillator frequency
	write_command(0xF0); //--set divide ratio

	write_command(0xD9); //--set pre-charge period
	write_command(0x22); //

	write_command(0xDB); //--set vcomh
	write_command(0x20); //0x20,0.77xVcc

	write_command(0x8D); //--set DC-DC enable
	write_command(0x14); //

	write_command(0xAF); //--turn on SSD1306 panel

	clear_display();
}

void place_pixel(int x, int y){
	//0,0 is top left
	// x is col num -- run
	// y is row num -- rise
	ssd1306_buf[((y/8)*128) + x] |= (1<<(y%8));
}

void clear_pixel(int x, int y){
	ssd1306_buf[((y/8)*128) + x] &= (0<<(y%8));
}

void invert_pixel(int x, int y){
	//0,0 is top left
	// x is col num -- run
	// y is row num -- rise
	ssd1306_buf[((y/8)*128) + x] ^= (1<<(y%8));
}

void place_char(char c, int x, int y){
	//places a character with top left being x,y
	int pixel = 0;
	int font_idx=0;
	while (c!=charmap[font_idx]){
		font_idx++;
	}
	for(int h=0;h<8;h++){
		for (int w=0;w<10;w++){
			pixel = (Font8x10[(8*font_idx)+h]>>(9-w)&0x001);
			if (pixel == 1){
				place_pixel((x+w),(y+h));
			}
		}
	}
}

void write_page(int page_num, char* line){
	// writes a page of display with 12 character string
	int x = 0;
	int y = page_num*8;
	for(int l=0;l<12;l++){
		x = 10*l;
		place_char(line[l], x, y);
	}
}

void draw_box(int width, int height, int x, int y){
	//draws a box of width, height with top left corner being x,y
	for(int w=0;w<width;w++){
		for(int h=0;h<height;h++){
			place_pixel((x+w),(y+h));
		}
	}
}

void clear_box(int width, int height, int x, int y){
	//draws a box of width, height with top left corner being x,y
	for(int w=0;w<width;w++){
		for(int h=0;h<height;h++){
			clear_pixel((x+w),(y+h));
		}
	}
}

void clear_page(int page_num){
	int x = 0;
	int y = page_num*8;
	clear_box(128, 8, x, y);
}

void invert_box(int width, int height, int x, int y){
	//inverts pixels of box selection width, height, with top left corner
	//being coords x,y
	for(int w=0;w<width;w++){
		for(int h=0;h<height;h++){
			invert_pixel((x+w),(y+h));
		}
	}
}


void insert_shape (int x, int y, shapes shape){
	//inserts custom 8x10 shape defined in fonts.h
	int pixel = 0;
	for(int h=0;h<8;h++){
		for (int w=0;w<10;w++){
			pixel = (shape_arr[(8*shape)+h]>>(9-w)&0x001);
			if (pixel == 1){
				place_pixel((x+w),(y+h));
			}
		}
	}
}

void display_init(){
	// initial display screen
	  write_page(0, "MODE:       ");
	  write_page(1, "   Static   ");
	  write_page(2, "COLOR:      ");
	  write_page(3, "     Red    ");
	  write_page(4, "SPEED:      ");
	  draw_box(72-18, 5, 9, (5*8)+1);
	  write_page(6, "BRIGHTNESS: ");
	  draw_box(128-18, 5, 9, (7*8)+1);
	  ssd1306_update_display();
}

void insert_selector(int cur_select){
	insert_shape(0,(8*cur_select), LEFT_ARROW);
	insert_shape((127-10),(8*cur_select), RIGHT_ARROW);
	invert_box(128, 7, 0, (cur_select*8));
}


void move_selector(int cur_select, int new_select){
	insert_selector(cur_select);
	insert_selector(new_select);
    ssd1306_update_display();
}

#endif




#ifndef SSD_1306_H
#define SSD_1306_H
#define SSD1306_ADDR (0x3C<<1)
#define SSD1306_BUF_SIZE (64/8) * 128//8 pages of 8 bits, 128 columns

extern I2C_HandleTypeDef hi2c1;

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
void invert_pixel(int x, int y);
void place_char(char c, int x, int y);
void write_page(int page_num, char* line);
void draw_box(int width, int height, int x, int y);
void invert_box(int width, int height, int x, int y);
void insert_shape (int x, int y, shapes shape);
void ssd1306_init();
void display_init();
void choice_selection(int page_num);

#endif /* SSD_1306_H */

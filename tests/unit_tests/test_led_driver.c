#define TEST
#include "../Unity/src/unity.h"
#include "../../prj/Core/Inc/led_driver.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_led_grb_to_ws2812b_stream(void) {
    uint8_t exp_dma_buffer [DMA_BUFFER_SIZE] = {0};
    // uint32_t col = 0x00ff00;
    //red
    for(uint32_t col = 0; col < 0xffffff; col++){
    // for (int rep=0;rep<NUM_LEDS;rep++){
    uint8_t exp_g = (col>>16) & 0xFF;
    uint8_t exp_r = (col>>8) & 0xFF;
    uint8_t exp_b = (col>>0) & 0xFF;

    uint32_t g_bits = 0;
    uint32_t r_bits = 0;
    uint32_t b_bits = 0;

    printf("Color: %x\n", col);
    printf("G:     %x\n", exp_g);
    printf("R:     %x\n", exp_r);
    printf("B:     %x\n", exp_b);

	//green
	for(int g=0;g<8;g++){
		g_bits = g_bits<<3;
		if (exp_g & 0x80){
			g_bits |= 0b110;
		} else{
			g_bits |= 0b100;
		}
		exp_g = exp_g << 1;
	}

	//red
	for(int r=0;r<8;r++){
		r_bits = r_bits<<3;
		if (exp_r & 0x80){
			r_bits |= 0b110;
		} else{
			r_bits |= 0b100;
		}
		exp_r = exp_r << 1;
	}

	//blue
	for(int b=0;b<8;b++){
		b_bits = b_bits<<3;
		if (exp_b & 0x80){
			b_bits |= 0b110;
		} else{
			b_bits |= 0b100;
		}
		exp_b = exp_b << 1;
	}

    printf("G Buf: %x\n", g_bits);
    printf("R Buf: %x\n", r_bits);
    printf("B Buf: %x\n", b_bits);


	exp_dma_buffer[(9*led_idx)+0] = (g_bits>>16)&CMASK;
	exp_dma_buffer[(9*led_idx)+1] = (g_bits>>8)&CMASK;
	exp_dma_buffer[(9*led_idx)+2] = (g_bits>>0)&CMASK;
	exp_dma_buffer[(9*led_idx)+3] = (r_bits>>16)&CMASK;
	exp_dma_buffer[(9*led_idx)+4] = (r_bits>>8)&CMASK;
	exp_dma_buffer[(9*led_idx)+5] = (r_bits>>0)&CMASK;
	exp_dma_buffer[(9*led_idx)+6] = (b_bits>>16)&CMASK;
	exp_dma_buffer[(9*led_idx)+7] = (b_bits>>8)&CMASK;
	exp_dma_buffer[(9*led_idx)+8] = (b_bits>>0)&CMASK;

    for(int j=0;j<DMA_BUFFER_SIZE;j++){
        printf("%x", exp_dma_buffer[j]);
    }
    printf("\n");
    dma_buffer_write(col);
    for(int j=0;j<DMA_BUFFER_SIZE;j++){
        printf("%x", dma_buffer[j]);
    }
    printf("\n");

    // for(int j = 0;j < DMA_BUFFER_SIZE;j++){
    TEST_ASSERT_EQUAL_UINT8_ARRAY(exp_dma_buffer, dma_buffer, DMA_BUFFER_SIZE);
    // }
    // }
}

}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_led_grb_to_ws2812b_stream);
    return UNITY_END();
}
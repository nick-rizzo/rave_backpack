# WS2812B Driver
## To be Embedded into Wearables

### Description

This project was created as a driver for WS2812B individually addressable RGB LED strips. The driver is combined with a user interface that uses directional buttons (up, down, left, right) in order to control characteristics of the LED strip. The current modifiers are: changing to a set number of patterns, color, brightness, and speed. The status of these modifiers are displayed to a display

This repo is made to work with STM32F446RE MCU using C.

### Hardware Used:
- x1 WS2812B LED Strip with xN LEDs (N>=1)
- x1 SSD1306 128x64 OLED display for information displays
- x4 pushbuttons for inputs

### Software Used
- STM32CubeIDE
- STM32F4 HAL as a submodule in this repo

### Planned Features
- Remove depedency on STM HAL
    - Create I2C Driver
    - Create WS2812b driver that does not utilize SPI STM32 peripheral OR create a SPI driver
- Potentially convert to RTOS for ease of adding peripherals







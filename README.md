# WS2812B Controller
## Addressable RGB LED driver combined with status display and button inputs

### Description

This project was created as a driver for WS2812B individually addressable RGB LED strips. The driver is combined with a user interface that uses directional buttons (up, down, left, right) in order to control characteristics and modifiers of the LEDs. The current modifiers are: changing to a set number of patterns, color, brightness, and speed. The current status of the device is displayed on an OLED panel

This repo is made to work with STM32F446RE MCU using C.

### Main peripherals Used:
- x1 WS2812B LED Strip with xN LEDs (N>=1)
- x1 SSD1306 128x64 OLED display
- x4 pushbuttons for directional inputs (up/down/left/right)

### Software Used
- STM32CubeIDE
- STM32F4 HAL as a submodule in this repo

### Planned Features
- Remove depedency on STM HAL
    - Create I2C Driver
    - Create WS2812b driver that does not utilize SPI STM32 peripheral OR create a SPI driver
- Potentially convert to RTOS for ease of adding peripherals







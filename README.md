# C Library for SSD1306 0.96" OLED display

## SSD1306 Description
Detailed information are described in [Datasheet SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf).

## Library
C library is aimed for driving [0.96" OLED display with SSD1306 driver](#demonstration) 128x64 or 128x32 version through TWI's (I2C). Settings for particular versions:
  - 128x64 version
    - command argument **SSD1306_SET_MUX_RATIO** set to *0x3F* (ssd1306.c)
    - command argument **SSD1306_COM_PIN_CONF** set to *0x12*  (ssd1306.c)
    - **END_PAGE_ADDR** set to 7 (ssd1306.h)
  - 128x32 version
    - command argument **SSD1306_SET_MUX_RATIO** set to *0x1F* (ssd1306.c)
    - command argument **SSD1306_COM_PIN_CONF** set to *0x02* (ssd1306.c)
    - **END_PAGE_ADDR** set to 3 (ssd1306.h)

### Versions
- 1.0 - basic functions. The first publication.
- [2.0](https://github.com/Matiasus/SSD1306/tree/v2.0.0) - more changes: 
  - rebuild to 'cacheMemLcd' array approach. It means that every request is stored in 'cacheMemLcd' array (RAM) and by the [SSD1306_UpdateScreen (uint8_t)](#ssd1306_updatescreen) function is printed on the display.
  - added new function -> [SSD1306_DrawLine (uint8_t, uint8_t, uint8_t, uint8_t)](#ssd1306_drawline). Possible depicted any line (horizontal, vertical, with slope).
  - possible to use for more than 1 display (not tested). 
  - **!!!** ~1kB RAM memory consumption.
- [3.0](https://github.com/Matiasus/SSD1306/tree/v3.0.0) - simplified alphanumeric version
  - displaying alphanumeric characters
  - for **one display** applicable
  - **only few RAM bytes** consumption
  - **!!!** no graphic functions like drawLine

## Dependencies
- [font.h](https://github.com/Matiasus/SSD1306/blob/master/lib/font.h)
- [twi.c](https://github.com/Matiasus/SSD1306/blob/master/lib/twi.c)
- [twi.h](https://github.com/Matiasus/SSD1306/blob/master/lib/twi.h)

Font.c can be modified according to application requirements with form defined in font.c. Maximal permissible horizontal dimension is 8 bits.

### Usage
Prior defined for MCU Atmega328p / Atmega8 / Atmega16. Need to be carefull with TWI ports definition.

| PORT  | [Atmega16](http://ww1.microchip.com/downloads/en/devicedoc/doc2466.pdf) | [Atmega8](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2486-8-bit-AVR-microcontroller-ATmega8_L_datasheet.pdf) / [Atmega328](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf) |
| :---: | :---: | :---: |
| SCL | PC0 | PC5 |
| SDA | PC1 | PC4 |

### Tested
Library was tested and proved with **_0.96″ 128x64 and 128x32 OLED Display (SSD1306 driver)_** and **_Atmega328p_**.

## Init OLED Sequence
Init sequence OLED display was defined according to page 64 (next to last page) of [Datasheet SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf).

```
// +---------------------------+
// |      Set MUX Ratio        |
// +---------------------------+
// |        0xA8, 0x3F         |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Display Offset     |
// +---------------------------+
// |        0xD3, 0x00         |
// +---------------------------+
//              |
// +---------------------------+
// |  Set Display Start Line   |
// +---------------------------+
// |          0x40             |
// +---------------------------+
//              |
// +---------------------------+
// |     Set Segment Remap     |
// +---------------------------+
// |       0xA0 / 0xA1         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set COM Output Scan     |
// |        Direction          |
// +---------------------------+
// |       0xC0 / 0xC8         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set COM Pins hardware   |
// |       configuration       |
// +---------------------------+
// |        0xDA, 0x02         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set Contrast Control    |
// +---------------------------+
// |        0x81, 0x7F         |
// +---------------------------+
//              |
// +---------------------------+
// | Disable Entire Display On |
// +---------------------------+
// |          0xA4             |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Normal Display     |
// +---------------------------+
// |          0xA6             |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Osc Frequency      |
// +---------------------------+
// |       0xD5, 0x80          |
// +---------------------------+
//              |
// +---------------------------+
// |    Enable charge pump     |
// |        regulator          |
// +---------------------------+
// |       0x8D, 0x14          |
// +---------------------------+
//              |
// +---------------------------+
// |        Display On         |
// +---------------------------+
// |          0xAF             |
// +---------------------------+
```
## Functions
- [SSD1306_Init (uint8_t)](#ssd1306_init) - Init display
- [SSD1306_ClearScreen (void)](#ssd1306_clearscreen) - Clear screen
- [SSD1306_NormalScreen (uint8_t)](#ssd1306_normalscreen) - Normal screen
- [SSD1306_InverseScreen (uint8_t)](#ssd1306_inversescreen) - Inverse screen
- [SSD1306_SetPosition (uint8_t, uint8_t)](#ssd1306_setposition) - Set position
- [SSD1306_DrawChar (char)](#ssd1306_drawchar) - Draw specific character
- [SSD1306_DrawString (char*)](#ssd1306_drawstring) - Draw specific string
- [SSD1306_UpdateScreen (uint8_t)](#ssd1306_updatescreen) - Update content on display
- [SSD1306_DrawLine (uint8_t, uint8_t, uint8_t, uint8_t)](#ssd1306_drawline) - Draw line

## Demonstration version v1.0.0
<img src="img/ssd1306_v100.jpg" />

## Demonstration version v2.0.0
<img src="img/ssd1306_v200.png" />

## Demonstration version v3.0.0
<img src="img/ssd1306_v300.jpg" />

## Acknowledgement
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)

## Links
- [Datasheet SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [Atmega328](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf)


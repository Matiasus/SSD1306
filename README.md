# SSD1306 0.96" OLED display

## SSD1306 Description
Detailed information are described in [Datasheet SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf).

## Library
C library is aimed for driving [SSD1306 0.96" OLED display](#demonstration) 128x64 through TWI's Atmega16.

## Dependencies
- [font.c](https://github.com/Matiasus/SSD1306/blob/readme-edits/lib/font.c)
- [font.h](https://github.com/Matiasus/SSD1306/blob/readme-edits/lib/font.h)
- [twi.c](https://github.com/Matiasus/SSD1306/blob/readme-edits/lib/twi.c)
- [twi.h](https://github.com/Matiasus/SSD1306/blob/readme-edits/lib/twi.h)

### Usage
Prior defined for MCU Atmega16 / Atmega8. Need to be carefull with TWI ports definition.

| PORT  | [Atmega16](http://ww1.microchip.com/downloads/en/devicedoc/doc2466.pdf) | [Atmega8](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2486-8-bit-AVR-microcontroller-ATmega8_L_datasheet.pdf) |
| :---: | :---: | :---: |
| SCL | PC0 | PC5 |
| SDA | PC1 | PC4 |

### Tested
Library was tested and proved on a **_SSD1306 0.96″ OLED Dispay_** with **_Atmega16_**.

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
- [char SSD1306_Init(void)](#ssd1306_init) - Init display
- [char SSD1306_ClearScreen(char)](#ssd1306_clearscreen) - Clear screen
## Links
- [Datasheet SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

#ifndef LCD_D22_5003_H
#define LCD_D22_5003_H

#include <SPI.h>

/* Define the commands
 * These come from the datasheet
 */
#define CMD_DISPLAY_ON              0xAF
#define CMD_DISPLAY_OFF             0xAE
#define CMD_INITIAL_DISPLAY_LINE    0x40  // Lowest 6 bits define MODE
#define CMD_SET_PAGE_ADDR           0xB0  // Lowest 4 bits set Page
#define CMD_SET_COL_MSB             0x10  // Lowest 4 bits set MSB
#define CMD_SET_COL_LSB             0x00  // Lowest 4 bits set LSB
#define CMD_ADC_SELECT              0xA0  // Lowest 1 bit sets direction
#define CMD_REVERSE_DISPLAY         0xA6  // Lowest 1 bit sets reverse
#define CMD_ENTIRE_DISPLAY          0xA4  // Lowest 1 bit sets entire display on
#define CMD_LCD_BIAS                0xA2  // Lowest 1 bit sets bias
#define CMD_SET_MODIFY_READ         0xE0
#define CMD_RESET_MODIFY_READ       0xEE
#define CMD_RESET                   0xE2
#define CMD_SHL                     0xC0  // 4th bit sets COM output direction
#define CMD_PWR_CTRL                0x28  // Lowest 3 bits control internal power management
#define CMD_REG_RESISTOR            0x20  // Lowest 3 bits select the regulator resistor
#define CMD_STATIC_IND_MODE         0xAA  // Lowest 1 bit sets static mode
#define CMD_STATIC_IND_REG          0x00  // Lowest 2 bits set register which controls blink rate

// Display related pins
#define PIN_LDR             34
#define PIN_LCD_BACKLIGHT   15
#define PIN_NEOPIXELS       12
#define NUM_LEDS            8

// Backlight PWM
#define BACKLIGHT_CHANNEL   0
#define FREQUENCY           5000
#define RESOLUTION          8



class Lcd_d22_5003 {
    private:
        SPIClass spi;
        int LCD_RS_Pin;
        int LCD_Reset_Pin;
        uint8_t spiData[132];

    public:
        Lcd_d22_5003(int RS_Pin, int Reset_Pin, int CS_Pin, int SPI_Data_Pin, int SPI_Clk_Pin);
        void reset();
        void write_cmd(uint8_t c);
        void set_position(uint8_t row, uint8_t col);
        void write_char(char Char);
        void write_str(char *Str);
        void clearln(uint8_t row);
        void clear();
        void message(char *Str, uint8_t row, uint8_t col=0, bool centered=false);
};

/* FreeRTOS Task */
void display(void * parameters);

#endif
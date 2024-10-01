#include <lcd_d22_5003.h>
#include <Arduino.h>
#include <SPI.h>
#include <font5x7int.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(NUM_LEDS, PIN_NEOPIXELS, NEO_GRB + NEO_KHZ800);

Lcd_d22_5003::Lcd_d22_5003(int RS_Pin = 4,
                           int Reset_Pin = 2,
                           int CS_Pin = 5,
                           int SPI_Data_Pin = 23,
                           int SPI_Clk_Pin = 18)
{
    /* Keep RS_Pin */
    LCD_RS_Pin = RS_Pin;
    LCD_Reset_Pin = Reset_Pin;

    // Set pins as outputs
    pinMode(RS_Pin, OUTPUT);
    pinMode(Reset_Pin, OUTPUT);
    pinMode(CS_Pin, OUTPUT);
    pinMode(SPI_Data_Pin, OUTPUT);
    pinMode(SPI_Clk_Pin, OUTPUT);

    /* Setup SPI */
    spi = SPIClass(VSPI);
    /* Display is write-only when using serial connection
     * Hence -1 in MISO pin below
     */
    spi.begin(SPI_Clk_Pin, -1, SPI_Data_Pin, CS_Pin);

    char msg[22];

    reset();
    clear();
    set_position(2, 5);
    snprintf(msg, 22, "rLab Energy Monitor");
    // message(msg, 1);
    // message(msg, 3, 0, true);
    write_str(msg);
}

void Lcd_d22_5003::reset()
{
    /* Configure display */
    /* Hard Reset */
    digitalWrite(LCD_Reset_Pin, 0);
    delayMicroseconds(1000);
    digitalWrite(LCD_Reset_Pin, 1);

    /* Configure registers */
    write_cmd(CMD_DISPLAY_ON);           // Turn on the display
    write_cmd(CMD_RESET);                // Soft reset
    write_cmd(CMD_PWR_CTRL | 0x07);      // Voltage follower, regulator and converter all on
    write_cmd(CMD_SET_PAGE_ADDR | 0x03); // 180 - Row = Row 1
    write_cmd(CMD_LCD_BIAS | 0x01);      // 1/9 Bias
    delayMicroseconds(1000);
}

void Lcd_d22_5003::write_cmd(uint8_t c)
{
    digitalWrite(LCD_RS_Pin, 0); // Send command
    spi.write(c);
}

void Lcd_d22_5003::set_position(uint8_t row, uint8_t col)
{
    /* Set the row 0-3 */
    uint8_t r = 3 - (row % 4);
    write_cmd(CMD_SET_PAGE_ADDR | r);
    /* Set the col, 0-131 */
    uint8_t c = col % 132;
    uint8_t cl = c & 0x0F;
    uint8_t cm = c >> 4;
    write_cmd(CMD_SET_COL_LSB | cl);
    write_cmd(CMD_SET_COL_MSB | cm);
}

void Lcd_d22_5003::write_char(char Char)
{
    digitalWrite(LCD_RS_Pin, 1); // Send data
    for (int c = 0; c < 5; c++)
    {
        spiData[c] = (font5x7int[Char * 5 + c]);
    }
    spiData[5] = 0; // The space after the character
    spi.writeBytes(spiData, 6);
}

void Lcd_d22_5003::write_str(char *Str)
{
    int c = 0;
    for (c = 0; c < strlen(Str); c++)
    {
        write_char(Str[c]);
    }
    while (c < 23)
    {
        write_char(' ');
        c++;
    }
}

void Lcd_d22_5003::clearln(uint8_t row)
{
    row = 3 - (row % 4);
    write_cmd(CMD_SET_PAGE_ADDR | row);
    write_cmd(CMD_SET_COL_LSB);
    write_cmd(CMD_SET_COL_MSB);
    write_str(" ");
}

void Lcd_d22_5003::clear()
{
    clearln(0);
    clearln(1);
    clearln(2);
    clearln(3);
}

void Lcd_d22_5003::message(char *Str, uint8_t row, uint8_t col, bool centered)
{
    int16_t c = col;
    if (centered)
    {
        c = (132 - 6 * strlen(Str)) / 2;
        c = c < 0 ? 0 : c;
    }
    memset(spiData, 0, 132); // Clear the array
    set_position(row, (uint8_t)c);
    for (int i = 0; i < strlen(Str) && i < (132 - c) / 6; i++)
    {
        write_char(Str[i]);
    }
}

void display(void *parameters)
{
    /* Setup LCD Display */
    Lcd_d22_5003 display = Lcd_d22_5003();

    // Set up backlight PWM
    ledcSetup(BACKLIGHT_CHANNEL, FREQUENCY, RESOLUTION);
    ledcAttachPin(PIN_LCD_BACKLIGHT, BACKLIGHT_CHANNEL);
    pinMode(PIN_LDR, INPUT);

    // Setup the Neopixels
    pixels.begin(); // INITIALIZE NeoPixel strip object

    // Variables for backlight and Neopixel brightness
    int backlight = 0;
    int light = 0;
    int min_light = 0;
    int max_light = 65536;
    int backlight_on = 0;

    uint64_t loopcounter = 0;

    char msg[22];
    int count = 0;

    while (true)
    {
        delay(100);
        // Set the brightness
        light = analogRead(PIN_LDR);
        min_light = light > min_light ? light : min_light + 1;
        max_light = light < max_light ? light : max_light - 1;
        backlight = map(light, min_light, max_light, 0, 255);
        Serial.printf("Min: %d   Light: %d   Max: %d   Backlight: %d\n", min_light, light, max_light, backlight);

        ledcWrite(BACKLIGHT_CHANNEL, backlight);
        pixels.setBrightness(backlight / 8);

        display.clear();
        snprintf(msg, 22, " Vance:  %d", count);
        display.set_position(0, 0);
        display.write_str(msg);

        // Neopixels
        for (int i = 0; i < NUM_LEDS; i++)
        {
            pixels.setPixelColor(i, pixels.Color(150, 255, 0));
        }
        pixels.show();

        count++;
    }
}
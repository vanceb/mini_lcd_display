#include <Arduino.h>

#include <pins.h>
#include <lcd_d22_5003.h>

uint64_t loopcounter;

TaskHandle_t display_task;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);


// Create a task to display time and other data
  xTaskCreate(
    display,
    "Display_Task",
    5000,
    NULL,
    0,
    &display_task
  );
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  if((loopcounter % 10) == 0) {
    backlight_on = backlight_on == 0 ? 255 : 0;
    ledcWrite(BACKLIGHT_CHANNEL, backlight_on);
    Serial.println(backlight_on);
  }
  */
 loopcounter++;
  delay(100);
}
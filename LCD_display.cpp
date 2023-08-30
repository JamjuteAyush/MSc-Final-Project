#include "LCD_display.h"
#include "ImageProcessing.h"
#include "Webpage.h"
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x3F,16,2);

void lcd_start_init(void)
{  
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight
  lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
  lcd.print("Hello!");     // The print content is displayed on the LCD
  lcd.setCursor(0,1);
  lcd.print("Ready for test");
}

void lcd_displayProcessing(void)
{
  lcd.clear();             // Clear display and set cursor to (0,0)
  lcd.print("Image Taken");     // The print content is displayed on the LCD
  lcd.setCursor(0,1);
  lcd.print("Processing");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
  lcd.setCursor(0,1);
  lcd.print("Processing");
  delay(1000);
  lcd.print(".");
  delay(1000);
  lcd.print(".");
}

void lcd_displayResult(void)
{
  lcd.clear();             // Clear display and set cursor to (0,0)
  lcd.print("Result");     // The print content is displayed on the LCD
  lcd.setCursor(0,1);
  if (testResult == 1){
    lcd.print("Tested Positive");
    Serial.print("Positive ");
  }
  else if(testResult ==0)
  {
    lcd.print("Tested Negative");
    Serial.print("Negative ");
  } 
  else{
    lcd.print("Invalid Test");
    Serial.println("Invalid Test");
  } 
}

void lcd_displayFailed(void)
{
  lcd.clear();             // Clear display and set cursor to (0,0)
  lcd.print("FAILED ATTEMPT");     // The print content is displayed on the LCD
  lcd.setCursor(0,1);
  lcd.print("Retake Image"); 
}

void lcd_displayIPAddress(void)
{
  lcd.clear();
  lcd.print("CONNECT: http://");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
}

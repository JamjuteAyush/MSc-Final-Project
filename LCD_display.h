
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

void lcd_start_init(void);
void lcd_displayProcessing(void);
void lcd_displayResult(void);
void lcd_displayFailed(void);
void lcd_displayIPAddress(void);


extern LiquidCrystal_I2C lcd;

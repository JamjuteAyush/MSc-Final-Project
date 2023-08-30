/*
#include <WiFi.h>
#include "esp_log.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
*/
#include "camera_ov2640.h"
#include "LCD_display.h"
#include "Webpage.h"
#include "imageProcessing.h"



#define PIN_LED     0
#define CLICK_BTN   15
#define REFRESH_BTN 33
#define SDA         13                    //Define SDA pins
#define SCL         14                    //Define SCL pins


static const char *TAG = "camera_";
int img_flag =0;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Wire.begin(SDA, SCL);           // attach the IIC pin
  
  connectWiFi();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
	Server_OK = true;
	request->send_P(200, "text/html", index_html);
  } );
  
  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
	  if (SPIFFS_OK)
	  {
	  	request->send(SPIFFS, LFT_PHOTO, "image/jpg", false);
	  }
	  else
	  {
	  	request->send(404, "text/plain", "File not found");
		}
	});

   
  
  camera_pins_config_init();
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  camera_sensor_config_init();
  Serial.println("CAMERA CONFIG COMPLETE"); 
  pinMode(PIN_LED, OUTPUT);
  pinMode(CLICK_BTN,INPUT);
  pinMode(REFRESH_BTN,INPUT);

  //lcd_start_init();
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight
  lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
  lcd.print("Hello!");     // The print content is displayed on the LCD
  lcd.setCursor(0,1);
  lcd.print("Ready for test");
  delay(3000);
  lcd_displayIPAddress();
  delay(5000);
  Serial.println("LCD CONFIG COMPLETE");
  lcd.clear();
  lcd.print("Hello!");     // The print content is displayed on the LCD
  lcd.setCursor(0,1);
  lcd.print("Ready for test");
  
  // Start server
  server.begin();

}

void loop() {
  if(digitalRead(CLICK_BTN) == LOW){
	delay(20);
	if ( (digitalRead(CLICK_BTN) == LOW) && (img_flag == 0))
	{
		digitalWrite(PIN_LED,HIGH);
		camera_capture_handler();
		img_flag = 1;

		lcd_displayProcessing();
		digitalWrite(PIN_LED,LOW);
		lcd_displayResult();
		//SerialLog_camera_capture();
		img_flag = 0;
	}
	while (digitalRead(CLICK_BTN) == LOW);
	delay(20);
	while (digitalRead(CLICK_BTN) == LOW);
  }
  
  if ( (digitalRead(REFRESH_BTN) == LOW))
  {
    lcd_start_init();
    delay(3000);
    lcd_displayIPAddress();
    delay(5000);
    SPIFFS_OK=0;
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
    {
      request->send_P(200, "text/html", index_html);
    } 
    );
    lcd.clear();
    lcd.print("Ready for test");
  }
}

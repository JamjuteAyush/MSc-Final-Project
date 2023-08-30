#include <Arduino.h>
#include "camera_ov2640.h"
#include "imageProcessing.h"
#include "Webpage.h"

#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22


camera_config_t config;
camera_fb_t *fb = NULL;

void camera_pins_config_init() {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_CIF;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;
}

void camera_sensor_config_init()
{
  sensor_t * s = esp_camera_sensor_get();  // returns the pointer to camera sensor setting controls
  s->set_vflip(s, 0);        //1-Upside down, 0-No operation
  s->set_hmirror(s, 0);      //1-Reverse left and right, 0-No operation
  s->set_brightness(s, 1);   //up the blightness just a bit
  s->set_saturation(s, -1);  //lower the saturation
}

void camera_capture_handler()
{
  bool x;
  
  esp_camera_fb_return(fb);
  esp_camera_fb_return(fb);
  delay(50);
  
  fb = esp_camera_fb_get();
  fb = esp_camera_fb_get();
  esp_camera_fb_return(fb);
  delay(10);
  
  if (!fb)
  {
      ESP_LOGE(TAG, "Camera capture failed");
      Serial.println("camera capture failed");
  }
  else {
      ESP_LOGE(TAG, "Camera capture success");
      Serial.println("camera capture success");
  }
  imgprcs_generateImage(fb->buf, fb->len, fb->width, fb->height);
  x= saveImgBuf_SPIFFS(ROI_finalImage,LFT_LENBUF,LFT_WIDTH,LFT_HEIGHT);
  esp_camera_fb_return(fb);
}

void SerialLog_camera_capture()
{
  for (int i =0 ; i< (fb->height); i++)
  {
    for (int j =0 ; j< (fb->width); j++)
    {
      /*
	  unsigned int pixVal_MSB, pixVal_LSB, pixVal;
      pixVal_MSB= *fb->buf;
      fb->buf ++;
      pixVal_LSB = *fb->buf;
      fb->buf ++;
      pixVal = (unsigned int) ( (pixVal_MSB << 8) | (pixVal_LSB) ) ; 
      */
	  Serial.print(*fb->buf);
     fb->buf ++;
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.print("image width: ");
  Serial.println(fb->width);
  Serial.print("image height: ");
  Serial.println(fb->height);
  Serial.print("buffer length: ");
  Serial.println(fb->len);
}

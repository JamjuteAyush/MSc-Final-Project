/*
the library esp_camera.h and img_converters.h are IP of espressif systems available on:
"https://github.com/espressif/esp32-camera"
*/
#include "esp_camera.h"
#include "img_converters.h"


#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
#define CONFIG_ESP_FACE_DETECT_ENABLED 0
#define CONFIG_ESP_FACE_RECOGNITION_ENABLED 0



void camera_pins_config_init();
void camera_sensor_config_init();
void camera_capture_handler();
void SerialLog_camera_capture();

extern camera_config_t config;
extern camera_fb_t *fb; 

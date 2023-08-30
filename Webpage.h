#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>
#include "WiFi.h"
#include "esp_timer.h"

// Photo File Name to save in SPIFFS
#define LFT_PHOTO "/photo.jpg"

extern const char* ssid;
extern const char* password;
extern const char index_html[] PROGMEM;
extern AsyncWebServer server;
extern bool SPIFFS_OK;
extern bool Server_OK;


char saveImgBuf_SPIFFS(unsigned char*, unsigned int, unsigned int, unsigned int);
bool checkPhoto(fs::FS &fs);
void connectWiFi(void);

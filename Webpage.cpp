#include "Arduino.h"
#include "Webpage.h"
#include "camera_ov2640.h"

const char* ssid = "Laptop_ideaPad5";
const char* password = "the_password";
AsyncWebServer server(80);
bool SPIFFS_OK=0;
bool Server_OK=0;


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align:center; }
    .vert { margin-bottom: 10%; }
    .hori{ margin-bottom: 0%; }
    .button5 {border-radius: 30%;}
    </style>
</head>
<body>
  <div id="container">
    <h2>LFT E-Reader</h2>
    <p> <button onclick="location.reload()">View Image </button> </p>
    <p> Image: Processed Photo</p>
    </div>
	<div><img src="saved-photo" id="photo" width="70%" alt="Not Available" style="border: 4px solid black;"> </div>
</body>
</html>)rawliteral";
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void connectWiFi(void){
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.isConnected() != true) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
    if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
}

char saveImgBuf_SPIFFS(unsigned char* imgbuf, unsigned int len, unsigned int width, unsigned int height)
{
    File file;
    unsigned int jpg_size = 0;
    unsigned char * jpg_buf = (unsigned char*)malloc(25000);
    bool x;
	
	if (SPIFFS.exists(LFT_PHOTO)) {
        SPIFFS.remove(LFT_PHOTO);
        Serial.println("File existed. Is deleted.");
    } 
   else {
        Serial.println("File does not exist. Continue to next.");
   }
   
   if(jpg_buf == NULL)
   {
		Serial.printf("Malloc failed to allocate buffer for JPG.\n");
	}

  x= fmt2jpg(imgbuf, len, width, height, PIXFORMAT_GRAYSCALE, 31, &jpg_buf, &jpg_size);
  if (x==1){
    Serial.println("Photo converted to JPEG");
    Serial.print("JPG buffer size: ");
    Serial.println(jpg_size);
  }
  else{ 
    Serial.println("Conversion failed");
    return (1);
  }
 
    do {
    // Photo file name
    Serial.printf("Picture file name: %s\n", LFT_PHOTO);
    file = SPIFFS.open(LFT_PHOTO, FILE_WRITE);
    
    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(jpg_buf, jpg_size); // payload (image), payload length
      file.flush();
	  Serial.print("The picture has been saved in ");
      Serial.print(LFT_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    Serial.println("Checking saved photo...");
    SPIFFS_OK = checkPhoto(SPIFFS);
  } while ( !SPIFFS_OK );
  
  free(jpg_buf);
  Serial.println("saved photo in SPIFFS");
  return (0);
}

bool checkPhoto( fs::FS &fs) {
  File f_pic = fs.open( LFT_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

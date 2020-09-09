
// ESP32 Cam Motion Alert | Send Image to Telegram

// Enter your WiFi ssid and password
const char* ssid = "Techarea - Software Developer";//"techarea.co.id";
const char* password = "cintapertama";//"12345678";
String token = "1290796428:AAEhHO6OjUic-JLbjAIK-VQflCb0PDg1ltA";
String chat_id = "31892588";

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>

//UniversalTelegramBot bot(BOTtoken, client);

//CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


int gpioPIR = 13;   //PIR Motion Sensor
//buzzer initializing
int buzzer_pin = 15;
int channel = 4;
int frequence = 8000;
int resolution_buzzer = 8;
//unsigned long timer = millis ();

int done = 0, buzz = 0;
WiFiClientSecure clientTCP;

TaskHandle_t dobitaobyte, dobitaobyte2;
eTaskState statusOf;
//end buzzer 
void setup()
{
  //buzzer setup
  ledcSetup (channel, frequence, resolution_buzzer);
  ledcAttachPin (buzzer_pin, channel);
  //end buzzer
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);
  delay(10);
  WiFi.mode(WIFI_STA);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  long int StartTime=millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    if ((StartTime+10000) < millis()) break;
  } 

  Serial.println("");
  Serial.println("STAIP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reset");
    
    ledcAttachPin(4, 3);
    ledcSetup(3, 5000, 8);
    ledcWrite(3,10);
    delay(200);
    ledcWrite(3,0);
    delay(200);    
    ledcDetachPin(3);
    delay(1000);
    ESP.restart();
  }
  else {
    ledcAttachPin(4, 3);
    ledcSetup(3, 5000, 8);
    for (int i=0;i<5;i++) {
      ledcWrite(3,10);
      delay(200);
      ledcWrite(3,0);
      delay(200);    
    }
    ledcDetachPin(3);      
  }

  camera_config_t config;
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
  config.xclk_freq_hz = 10000000;//20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_XGA); 
  s->set_vflip(s, 1); 
}


void loop(){
  pinMode(gpioPIR, INPUT_PULLUP);
  int v = digitalRead(gpioPIR);
  Serial.print(done);
  Serial.println(v);
  
  if (v==1 && done == 0){
    done = 1;
    buzz = 1;
//    xTaskCreatePinnedToCore(alerttelegram, "telegram", 10000, NULL, 1, &dobitaobyte2, 0);
   xTaskCreatePinnedToCore(Task1code, "siren", 10000, NULL, 1, &dobitaobyte, 1);
    delay(100);
    alerts2Telegram(token, chat_id, "Gambar 1");
//    delay(100);
//    alerts2Telegram(token, chat_id, "Gambar 2");
    delay(10000); 
   
    if(done == 1) {
      done = 0;
//      statusOf = eTaskGetState (dobitaobyte);
//      if (statusOf == eReady) {
//         vTaskDelete(dobitaobyte);
//      }
//      statusOf = eTaskGetState (dobitaobyte2);
//      Serial.println(statusOf);
//      if (statusOf == eReady) {
//         vTaskDelete(dobitaobyte2);
//      }
    }

    
   if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reset");
      }else{
        Serial.println("Ok");
//        testClient("api.telegram.org",443);
      }
  }

  delay(1000);
}

//test client
void testClient(const char * host, uint16_t port) {
  Serial.print("\nconnecting to ");
  Serial.println(host);
  WiFiClient client;
  client.connect(host, port);
  //server_status = client.connected();
  Serial.println(client.connected());
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }
  Serial.println("closing connection\n");
  client.stop();
}
//
//Task1c
void Task1code( void * pvParameters ){
    Serial.print("Task2 running on core ");
    Serial.println(xPortGetCoreID());
    float sinVal;
    int toneVal;
    
  for (byte t = 0; t <60; t ++) {
      ledcWriteTone (channel, 1000);
      delay(100);
      ledcWriteTone (channel, 2000);
      delay(100);
      ledcWriteTone (channel, 0);
  }
  ledcWriteTone (channel, 0);
  vTaskDelete(NULL);
}

void alerttelegram(void *pvParameters){
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
//    sendPhotoTelegram();
//    alerts2Telegram(token, chat_id);
    done = 0;
    vTaskDelete(NULL);
}

String alerts2Telegram(String token, String chat_id, String caption) {
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";

  camera_fb_t *fb = NULL;
  
    Serial.println("captured2");
    
  fb = esp_camera_fb_get();  
    Serial.println("captured1");
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }else{
    Serial.println("captured");
  }
  
  WiFiClientSecure client_tcp;

//  WiFiClient client_tcp;
  
  client_tcp.connect(myDomain, 443);
   //server_status = client.connected();
  int countAttemp = 0;
  while(!client_tcp.connected() && countAttemp<10){
     client_tcp.stop();
     client_tcp.connect(myDomain, 443);
     countAttemp++;
     Serial.print(countAttemp);
     Serial.println("Try Connecting...");
     delay(500);
  }
  if (client_tcp.connected()) {
    Serial.println("Connected to " + String(myDomain));
    
    String head = "--Indonesia\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chat_id + "\r\n";
    head = head + "--Indonesia\r\nContent-Disposition: form-data; name=\"caption\"; \r\n\r\n" + caption + "\r\n";
    head = head + "--Indonesia\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--Indonesia--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;
  
    client_tcp.println("POST /bot"+token+"/sendPhoto HTTP/1.1");
    client_tcp.println("Host: " + String(myDomain));
    client_tcp.println("Content-Length: " + String(totalLen));
    client_tcp.println("Content-Type: multipart/form-data; boundary=Indonesia");
    client_tcp.println();
    client_tcp.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    
    Serial.println("Processing");

    for (size_t n=0;n<fbLen;n=n+1024){
      if (n+1024<fbLen) {
        client_tcp.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client_tcp.write(fbBuf, remainder);
      }
    }  
    
    client_tcp.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTime = millis();
    boolean state = false;
    
    while ((startTime + waitTime) > millis()){
      Serial.print(".");
      delay(100);      
      while (client_tcp.available()) {
          char c = client_tcp.read();
          if (c == '\n') {
            if (getAll.length()==0) state=true; 
            getAll = "";
          } 
          else if (c != '\r') getAll += String(c);
          if (state==true) getBody += String(c);
          startTime = millis();
       }
       if (getBody.length()>0) break;
    }
    client_tcp.stop();
    Serial.println(getBody);
  }
  else {
    esp_camera_fb_return(fb);
    client_tcp.stop();
    getBody = "Connection to telegram failed.";
    Serial.println("Connection to telegram failed.");
  }
  
  return getBody;
}

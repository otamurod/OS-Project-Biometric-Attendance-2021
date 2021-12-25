#include "ArduinoJson.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GyverButton.h> 
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define SS_PIN  5  // ESP32 pin GIOP5 
#define RST_PIN 27 // ESP32 pin GIOP27 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

MFRC522 rfid(SS_PIN, RST_PIN);

GButton up(13, HIGH_PULL);     // Кнопки
GButton down(12, HIGH_PULL);
GButton ok(14, HIGH_PULL);
GButton left(25, HIGH_PULL);
GButton right(26, HIGH_PULL);

#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

const char* ssid = "TAKO";
const char* password = "salarbuyi";

String serverUrl  = "http://192.168.0.112:3000/api/client";

int selected_item = 0; 
uint8_t new_finger_id;
bool is_functions_opened = false;
bool inspector_access = false;
String inspector_card_id;

void setup() {
  Serial.begin(57600);
  Serial2.begin(115200);  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  while (!Serial); 
  delay(100);
  
  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    show_wifi_connection();
    Serial.print("#");
  }
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  while(!inspector_access){
    check_inspector();
  }
 
    
  show_menu();

  finger.begin(57600);
  finger.getParameters();
  Serial.println(finger.capacity);
  Serial.println(finger.packet_len);
  
}

void loop() {
  up.tick();            
  down.tick();
  ok.tick();
  right.tick();
  left.tick();

  if (down.isClick()) {
    if (selected_item < 2 && selected_item > -1) {
      selected_item++;
      show_menu();
    }
  }

  if (up.isClick()) {
    if (selected_item < 3 && selected_item > 0) {
      selected_item--;
      show_menu();
    }
  }

   if (left.isClick() or left.isHold()) {
      is_functions_opened = false;
      show_menu();
   }

  if (ok.isClick()) {
    is_functions_opened = true; 
  }

  if (is_functions_opened) {
    if (selected_item == 0) { 
      finger_scan();
    } 
    else if (selected_item == 1) {
      new_finger();
    }
    else if (selected_item == 2) {
      show_rf_id();
    }
    else {
      is_functions_opened = false; 
    }
  }
}



void finger_scan() {
  uint8_t p = finger.getImage();
   if (p != FINGERPRINT_OK)  
   {
     display.clearDisplay();
     display.setTextSize(2);             // Normal 1:1 pixel scale
     display.setTextColor(SSD1306_WHITE);        // Draw white text
     display.setCursor(15, 20);            // Start at top-left corner
     display.println(("Scanning"));
     display.display();
     return ;
   }
 
 p = finger.image2Tz();
 if (p != FINGERPRINT_OK)  
 {
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(20, 0);            // Start at top-left corner
   display.println(("Messy  Image"));
   display.setCursor(20, 20);
   display.println("Try Again");
   display.display();
    
   delay(3000);
   display.clearDisplay();
   return;
 }
 
 p = finger.fingerFastSearch();
 if (p != FINGERPRINT_OK)  {
   
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(20, 0);            // Start at top-left corner
   display.println(("Invalid"));
   display.setCursor(25, 20);
   display.println("Finger");
   display.setCursor(10, 40);
   display.println("Try Again");
   display.display();
   delay(1500);
   display.clearDisplay();
   return;
 }
  
   int fingerID = finger.fingerID;
   get_id(fingerID);
   return;
}

void new_finger() {
  display_string("Serial 57600");
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  new_finger_id = readnumber();
  if (new_finger_id == 0) {// ID #0 not allowed, try again!
  return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(new_finger_id);
  
  while (! getFingerprintEnroll() );
}

void show_rf_id() {
  
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(10, 5);            // Start at top-left corner
  display.println(("Scanning"));
  display.setCursor(25, 35);            // Start at top-left corner
  display.println(("RF_ID"));
  display.display();
  
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      String card_id = "";
      for (int i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) {
          card_id += "0";
        } 
        else {
          card_id += String(rfid.uid.uidByte[i], HEX);
        }
      }

      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0, 5);            // Start at top-left corner
      display.println(("Found:"));
      display.setCursor(0, 25);  
      display.println(card_id);
      display.display();
      delay(5000);
      
      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}

void show_menu() {
  display.clearDisplay();
  display.setTextSize(2,2);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(20, 0);
  display.println(("SCAN"));
  display.setCursor(20, 20);
  display.println("NEW");
  display.setCursor(20,40);
  display.println("RF_ID");
  display.setCursor(0,selected_item * 20);
  display.println(">");
  display.display();
}

void check_inspector() {
  show_put_your_id();
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      String card_id = "";
      for (int i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) {
          card_id += "0";
        } 
        else {
          card_id += String(rfid.uid.uidByte[i], HEX);
        }
      }
      if(WiFi.status()== WL_CONNECTED){
        httpGETRequest("/check_inspector?card_id=" + card_id, card_id);
      }
    } 
  }
}

void show_wifi_connection() {
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(5, 5);            // Start at top-left corner
   display.println(("Connecting"));
   display.setCursor(25, 35);            // Start at top-left corner
   display.println(("to WiFi"));
   display.display();
}

void show_put_your_id() {
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(35, 5);            // Start at top-left corner
   display.println(("PUT"));
   display.setCursor(15, 35);            // Start at top-left corner
   display.println(("Your ID"));
   display.display();
}

void show_message(String message) {
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(15, 5);            // Start at top-left corner
   display.println(message);
   display.display();
}

void sending_request() {
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(15, 5);            // Start at top-left corner
   display.println(("Sending"));
   display.setCursor(15, 35);            // Start at top-left corner
   display.println(("Request"));
   display.display();
}
void welcome_user(String name) {
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(15, 5);            // Start at top-left corner
   display.println(("Welcome!"));
   display.setCursor(15, 35);            // Start at top-left corner
   display.println((name));
   display.display();
}

void display_string(String name) {
   display.clearDisplay();
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);        // Draw white text
   display.setCursor(0, 20);            // Start at top-left corner
   display.println(name);
   display.display();
}

void get_id(int finger_id) {
      WiFiClient client;
      HTTPClient http;
      sending_request();
      String requestUrl = serverUrl + "/check_finger?card_id=" + inspector_card_id + "&finger_id=" + String(finger_id);
      Serial.println(requestUrl);
      http.begin(client, requestUrl.c_str());
      
      int httpResponseCode = http.GET();
     
      if (httpResponseCode == 200) {
        DynamicJsonDocument doc(1024);
        // Parse JSON object
        deserializeJson(doc, http.getString());
        JsonObject obj = doc.as<JsonObject>();
        String message = obj["message"];
        display_string(message);
        delay(5000);
      } else {
        delay(2000);
        display_string("ERROR !!!");
      }
      http.end();
}

void httpGETRequest(String url, String card_id) {
    WiFiClient client;
    HTTPClient http;
      sending_request();
      String requestUrl = serverUrl + url;
      Serial.println(requestUrl);
      http.begin(client, requestUrl.c_str());

      String httpRequestData = "{\"card_id\":\"" + card_id + "\"}";      
      
      int httpResponseCode = http.GET();
     
      if (httpResponseCode == 200) {
        inspector_access = true;
        inspector_card_id = card_id;  
        DynamicJsonDocument doc(1024);
        // Parse JSON object
        deserializeJson(doc, http.getString());
        JsonObject obj = doc.as<JsonObject>();
        String name = obj["inspector_name"];
        welcome_user(name);
        delay(2000);
      } else {
        show_message("Wrong id");
        delay(1000);
      }
      http.end();
}

bool getFingerprintEnroll() {
 
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(new_finger_id);
  while (p != FINGERPRINT_OK) {
  p = finger.getImage();
  switch (p) {
  case FINGERPRINT_OK:
  Serial.println("Image taken");
  break;
  case FINGERPRINT_NOFINGER:
  Serial.println(".");
  break;
  case FINGERPRINT_PACKETRECIEVEERR:
  Serial.println("Communication error");
  break;
  case FINGERPRINT_IMAGEFAIL:
  Serial.println("Imaging error");
  break;
  default:
  Serial.println("Unknown error");
  break;
  }
  }
   
  // OK success!
   
  p = finger.image2Tz(1);
  switch (p) {
  case FINGERPRINT_OK:
  Serial.println("Image converted");
  break;
  case FINGERPRINT_IMAGEMESS:
  Serial.println("Image too messy");
  return false;
  case FINGERPRINT_PACKETRECIEVEERR:
  Serial.println("Communication error");
  return false;
  case FINGERPRINT_FEATUREFAIL:
  Serial.println("Could not find fingerprint features");
  return false;
  case FINGERPRINT_INVALIDIMAGE:
  Serial.println("Could not find fingerprint features");
  return false;
  default:
  Serial.println("Unknown error");
  return false;
  }
   
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
  p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(new_finger_id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
  p = finger.getImage();
  switch (p) {
  case FINGERPRINT_OK:
  Serial.println("Image taken");
  break;
  case FINGERPRINT_NOFINGER:
  Serial.print(".");
  break;
  case FINGERPRINT_PACKETRECIEVEERR:
  Serial.println("Communication error");
  break;
  case FINGERPRINT_IMAGEFAIL:
  Serial.println("Imaging error");
  break;
  default:
  Serial.println("Unknown error");
  break;
  }
  }
   
  // OK success!
   
  p = finger.image2Tz(2);
  switch (p) {
  case FINGERPRINT_OK:
  Serial.println("Image converted");
  break;
  case FINGERPRINT_IMAGEMESS:
  Serial.println("Image too messy");
  return false;
  case FINGERPRINT_PACKETRECIEVEERR:
  Serial.println("Communication error");
  return false;
  case FINGERPRINT_FEATUREFAIL:
  Serial.println("Could not find fingerprint features");
  return false;
  case FINGERPRINT_INVALIDIMAGE:
  Serial.println("Could not find fingerprint features");
  return false;
  default:
  Serial.println("Unknown error");
  return false;
  }
   
  // OK converted!
  Serial.print("Creating model for #"); Serial.println(new_finger_id);
   
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
  Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
  Serial.println("Communication error");
  return false;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
  Serial.println("Fingerprints did not match");
  return false;
  } else {
  Serial.println("Unknown error");
  return false;
  }
   
  Serial.print("ID "); Serial.println(new_finger_id);
  p = finger.storeModel(new_finger_id);
  if (p == FINGERPRINT_OK) {
  Serial.println("Stored!");
  return true;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
  Serial.println("Communication error");
  return false;
  } else if (p == FINGERPRINT_BADLOCATION) {
  Serial.println("Could not store in that location");
  return false;
  } else if (p == FINGERPRINT_FLASHERR) {
  Serial.println("Error writing to flash");
  return false;
  } else {
  Serial.println("Unknown error");
  return false;
  }
}
uint8_t readnumber(void) {
  uint8_t num = 0;
   
  while (num == 0) {
  while (! Serial.available());
  num = Serial.parseInt();
  }
  return num;
}

  

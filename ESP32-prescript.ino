/*
  `ESP32-prescript` is a sketch to use an ESP32 board as a bluetooth-enabled index prescript device.
  Copyright (C) 2026 Violet Steers

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

const int MAX_CHARS_ON_SCREEN_WITH_NULL_TERMINATOR = ((SCREEN_WIDTH * SCREEN_HEIGHT) / (8 * 8)) + 1;

// Buffer with the ability to hold characters for an 8*8 font filling the entire screen with null terminator. To be used for full prescripts.
char prescriptBuf[MAX_CHARS_ON_SCREEN_WITH_NULL_TERMINATOR]    = {0};

// Expected to be used for the current message to display when 'writing' a prescript
char curPrescriptBuf[MAX_CHARS_ON_SCREEN_WITH_NULL_TERMINATOR] = {0};

const int MAX_PRESCRIPT_SIZE = sizeof(prescriptBuf) - sizeof(prescriptBuf[0]); // Exclude null terminator at end of array from being written to

// Buffer holding passkey characters plus null termination
char passkeyBuf[6 + 1] = {0};


const int MORSE_TIME_UNIT_MS            = 25;
const int MORSE_DOT_TIME                = MORSE_TIME_UNIT_MS;
const int MORSE_DASH_TIME               = MORSE_TIME_UNIT_MS * 3;
const int MORSE_WORD_GAP_TIME           = MORSE_TIME_UNIT_MS * 7;
const int MORSE_WORD_GAP_MINUS_DOT_TIME = MORSE_WORD_GAP_TIME - MORSE_DOT_TIME;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool buzzerState = false;

// TODO: Change to char[]
String message = "_CLEAR._";

const int ledPin = 8; 
const int buzzerPin = 20;


/*
  Gets a random ASCII character that is displayable using a standard font.
*/
char getRandomDisplayableChar() {
  // ! - ~
  return (char) random(33, 127);
}

/*
  Draws a character array to the centre of the screen, with text aligned to the centre around the anchor.
*/
void drawCentredChars(const char *buf, int textSize)
{
    oled.setTextSize(textSize);

    int half_screen_width = SCREEN_WIDTH / 2;
    int half_screen_height = SCREEN_HEIGHT / 2;
    int charcount = strlen(buf);

    int16_t x1, y1;
    uint16_t w, h;
    oled.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); //calc width of new string

    oled.setCursor(half_screen_width - w / 2, half_screen_height - h/2);
    oled.print(buf);
}

// TODO: Maybe use char array and look for null terminator?
void animateWordDisplay(String word, int randomCharFlipCount, int charChangeDelayMs) {
  unsigned int bufLen = word.length();

  // Allocate length of string plus constant null termination for use by OLED library to see end of string
  char displayBuf[bufLen + 1] = {0};

  for (unsigned int i = 0; i < bufLen; i++) {
    displayBuf[i] = getRandomDisplayableChar();
  }

  for (unsigned int i = 0; i < bufLen; i++) {
    for (int j = 0; j < randomCharFlipCount; j++) {
      delay(charChangeDelayMs);
      displayBuf[i] = getRandomDisplayableChar();
      oled.clearDisplay();
      // oled.setCursor(0, 0);
      // oled.setCursor(20, 20);
      // oled.print(displayBuf);
      drawCentredChars(displayBuf, 2);
      
      oled.display();
    }
    delay(charChangeDelayMs);
    displayBuf[i] = word[i];
    oled.clearDisplay();
    // oled.setCursor(20, 20);
    // oled.print(displayBuf);
    drawCentredChars(displayBuf, 2);
    oled.display();
  } 
}

/*
  Formats and displays a `prescript` addressed to `name`, using `prescriptBuf` and `curPrescriptBuf` as storage for the state of the function.
  A maximum of `MAX_PRESCRIPT_SIZE` can be attempted to be displayed by this function. 
  This prevents memory overflow, but *does not* prevent overflow of the display area due to formatting.

  @param name The name of the entity to address the prescript to.
  @param prescript The task to be carried out by the entity.
  @param buzzMorse Whether the morse code for the message should be sent to the digital output pin for the buzzer/LED.
*/
void displayPrescriptFromCharBufs(const char* name, const char* prescript, bool buzzMorse) {
  oled.setTextSize(1);

  snprintf(
    prescriptBuf, 
    MAX_PRESCRIPT_SIZE, // Exclude null terminator at end of array from being written to
    "[To %s]\n\n[%s]", name, prescript
  );
  // char curPrescriptMessage[formattedPrescript.length() + 1] = {0};

  int prescriptLength = strlen(prescriptBuf);
  curPrescriptBuf[0] = '_';

  for (int i = 0; i < prescriptLength; i++) {
    curPrescriptBuf[i] = prescriptBuf[i];
    // Keep the null terminator for the last character and 'remove' the 'cursor'
    if (i != prescriptLength - 1) {
      curPrescriptBuf[i + 1] = '_';
    }

    // Draw currently displayed section of prescript to screen
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.print(curPrescriptBuf);
    oled.display();

    if (buzzMorse) {
      switch (curPrescriptBuf[i]) {
        // Spaces assumed to be word boundary
        case ' ':
          delay(MORSE_WORD_GAP_TIME);
          break;
        // Allows time for the cursor to move in a visible animation for newlines
        case '\n':
          delay(MORSE_DOT_TIME);
          break;
        // Beep morse code representing the current character, if found.
        default:
          digitalBeepAsciiChar(curPrescriptBuf[i], buzzerPin);
          // Add uniform break between letters
          delay(MORSE_DASH_TIME);
      }
    }
    else {
      // Keep typing timing consistent with units used when morse code is buzzed
      delay(MORSE_DASH_TIME);
    }
    
  }

  memset(prescriptBuf, 0, sizeof(prescriptBuf));
  memset(curPrescriptBuf, 0, sizeof(curPrescriptBuf));

}


/*
  A helper function for `displayPrescriptFromCharBufs` to simplify using Strings. 
  Intended for callbacks from the BLE stack, where text is stored in such a manner.
*/
void displayPrescript(String name, String prescript, bool buzzMorse) {
  return displayPrescriptFromCharBufs(name.c_str(), prescript.c_str(), buzzMorse);
}

// void setup() {
//   pinMode(buzzerPin, OUTPUT);
//   Wire.begin(6, 8);
//   oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   // oled.setFont();
//   oled.setTextSize(2);
//   oled.setTextColor(WHITE);
//   oled.clearDisplay();
//   pinMode(buzzerPin, OUTPUT);
//   digitalWrite(buzzerPin, LOW);  

//   animateWordDisplay(message, 3, 33);
//   delay(3000);
//   oled.clearDisplay();

// }

// void loop() { 
//   animateWordDisplay(getMorseForCharacter('S') + getMorseForCharacter('o') + getMorseForCharacter('1'), 3, 33);
//   delay(3000);
//   oled.clearDisplay();
// }

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "60acc2f2-601b-4c01-aebf-59fe16a578d5"
#define CHARACTERISTIC_UUID "9c3cd6b8-5e8f-442d-aff0-87cb8177c43f"

/* BLEServer *pServer = BLEDevice::createServer();
BLEService *pService = pServer->createService(SERVICE_UUID);
BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       ); */

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

BLEService *deviceVersionService;


class PrescriptMessageCallbacks: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    String value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      animateWordDisplay(value, 3, 33);
    }
  }
};

// void display_pairing_code(uint32_t passkey) {
//   snsprintf(passkeyBuf, sizeof(passkeyBuf), "%u", passkey);
//   drawCentredChars(passkeyBuf, 2);
// }

// void gap_event_handler(esp_gap_ble_cb_event_t event, 
//                        esp_ble_gap_cb_param_t *param) {
//     switch(event) {
//         case ESP_GAP_BLE_NC_REQ_EVT:
//             // 6-digit PIN appears on both screens
//             uint32_t passkey = param->ble_security.key_notif.passkey;

//             // Display on device screen
//             display_pairing_code(passkey);

//             // User must confirm match on both devices
//             esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
//             break;
//     }
// }


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE Server!");
  Wire.begin(6, 8);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // oled.setFont();
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); 

  // esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, 
  //                               &ESP_LE_AUTH_REQ_SC_MITM_BOND, sizeof(uint8_t)); 

  BLEDevice::init("Prescript of The Index");
  pServer = BLEDevice::createServer();
  
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  // BLEDescriptor desc = bmeHumidityDescriptor(BLEUUID((uint16_t)0x2903));
  // TODO
  // pCharacteristic->addDescriptor("Description for this service");

  
  /* BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );*/

  pCharacteristic->setCallbacks(new PrescriptMessageCallbacks());
  pCharacteristic->setValue("Hello, Hermes.");
  pService->start();



  //BLEAdvertising *pAdvertising = pServer->getAdvertising();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in the Client!");


  animateWordDisplay(message, 3, 33);
  delay(3000);
  
  displayPrescript("Proselyte", "Drink only orange juice for the next 28 days. Completion will result in promotion to proxy.", true);
  delay(5000);
  displayPrescript("Proxy", "Stay at home for the next 8 hours. Congratulations on your promotion.", true);
}

void loop()
{
  // String value = pCharacteristic->getValue();
  // Serial.print("The new characteristic value is: ");
  // Serial.println(value.c_str());
  delay(2000);
}
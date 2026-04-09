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
  char displayBuf[bufLen + 1];
  displayBuf[bufLen] = 0;

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

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Wire.begin(6, 8);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // oled.setFont();
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);  

  animateWordDisplay(message, 3, 33);
  delay(3000);
  oled.clearDisplay();

}

void loop() { 
  animateWordDisplay(getMorseForCharacter('S') + getMorseForCharacter('o') + getMorseForCharacter('1'), 3, 33);
  delay(3000);
  oled.clearDisplay();
}
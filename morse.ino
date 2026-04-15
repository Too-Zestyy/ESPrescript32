/*
  An array containing the values for morse code of each letter.
*/ 
static const String morseLetters[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
  ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." //, "E"
};

/*
  An array containing the values for morse code of each number.
*/ 
static const String morseNumbers[] = {
  "-----", ".----", "..---", "...--", "....-", "....." , "-....", "--...", "---..", "----."
};

// There is no contigious parts of the ASCII table for symbols with morse translations, so a switch case is an ugly but applicable solution

/*
  Gets the morse translation for a symbol in the ASCII table, if available.
  @returns The morse translation if available, otherwise an empty string.
*/
String getSymbolMorse(char symbol) {
  switch (symbol) {
    case '!':
      return "-.-.--";
    case '"':
      return ".-..-.";
    case '$':
      return "...-..-";
    case '&':
      return ".-...";
    case '\'':
      return ".----.";
    case '(':
      return "-.--.";
    case ')':
      return "-.--.-";
    case '+':
      return ".-.-.";
    case ',':
      return "--..--";
    case '-':
      return "-....-";
    case '.':
      return ".----.";
    case '/':
      return "-..-.";
    case ':':
      return "---...";
    case ';':
      return "-.-.-.";
    case '=':
      return "-...-";
    case '?':
      return "..--..";
    case '@':
      return ".--.-.";
  }

  return "";
}

/*
  Attempts to get the morse code for an ASCII character. Includes letters of both cases, numbers and some symbols.
  @returns The morse translation if available, otherwise an empty string.
*/
String getMorseForCharacter(char character) {
  // Check letters first
  if (character >= 65 && character <= 90) {
    return morseLetters[character - 65];
  }
  else if (character >= 97 && character <= 122) {
    return morseLetters[character - 97];
  }
  // Then numbers
  else if (character >= 48 && character <= 57) {
    return morseNumbers[character - 48];
  }
  // Then any other registered symbols
  return getSymbolMorse(character);
}

/*
  Sounds an active buzzer connected via `buzzerPin`
*/
void digitalBeepMorseChar(char morseCharacter, int buzzerPin) {
  digitalWrite(buzzerPin, HIGH);

  switch (morseCharacter) {
    case '.':
      delay(MORSE_DOT_TIME);
    case '-':
      delay(MORSE_DASH_TIME);
  }

  digitalWrite(buzzerPin, LOW);
  // delay(MORSE_SHORT);
}

void digitalBeepAsciiChar(char character, int buzzerPin) {
  String morse = getMorseForCharacter(character);

  if (morse != "") {
    for (int i = 0; i < morse.length(); i++) {
      digitalBeepMorseChar(morse[i], buzzerPin);
      // Prevent a delay after the morse code for the letter has completely sounded
      if (i < morse.length() - 1) {
        delay(MORSE_DOT_TIME);
      }
    }
  }

}

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
  @returns The morse translation if available, otherwise `NULL`.
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

  return NULL;
}
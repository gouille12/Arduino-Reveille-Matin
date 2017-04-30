
/*

1, 2, 3, A
4, 5, 6, B
7, 8, 9, C
*, 0, #, D

4 fils pour lignes, 4 pour colonnes
*/

#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte colPins[COLS] = {5, 4, 3, 2}; /* Fils du côté de ABCD */
byte rowPins[ROWS] = {9, 8, 7, 6}; /* Fils du côté de 147* */
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
}

void loop() {
  char keyPressed = keypad.getKey();
  if (keyPressed) {
    Serial.println(keyPressed);
  }

}
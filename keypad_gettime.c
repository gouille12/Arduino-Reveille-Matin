
/*

1, 2, 3, A
4, 5, 6, B
7, 8, 9, C
*, 0, #, D

4 fils pour lignes, 4 pour colonnes
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte colPins[COLS] = {5, 4, 3, 2};
byte rowPins[ROWS] = {9, 8, 7, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String timeToSet;



void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initialisation");
  delay(1000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Entrez l'heure actuelle :");
  lcd.setCursor(0,1);

  char keyPressed = keypad.getKey();

  while(keyPressed != 'A') {
    lcd.print(keyPressed);
    timeToSet = timeToSet + keyPressed

  }


}

void loop() {
  char keyPressed = keypad.getKey();

  if (keyPressed == 'A'){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(timeToSet);
  }

  else if (keyPressed) {
    lcd.print(keyPressed);
    timeToSet = timeToSet + keyPressed;
    Serial.println(timeToSet);
  }
}
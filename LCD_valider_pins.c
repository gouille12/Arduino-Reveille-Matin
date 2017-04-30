#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x3F  // Define I2C Address for controller
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void setup()
{
  Serial.begin(9600); //this opens the serial connection that we are going to connect to later.
  //  Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.begin(16,2); // initialize the lcd 
}

void loop()
{
  if(Serial.available())  //this if is checking to see if someone connected to our open serial connection.
  {
    for(int i = 0; i < 16;i++) //this for loop is going through our list of ports and changing toggling each
    {
      Serial.print("Setting pin: ");
      Serial.print(i);
      Serial.println(" low");
      lcd.setBacklightPin(i, NEGATIVE);
      delay(2000);
      Serial.print("Setting pin: ");
      Serial.print(i);
      Serial.println(" high");
      lcd.setBacklightPin(i, POSITIVE);
      delay(2000);
    }
  }
}
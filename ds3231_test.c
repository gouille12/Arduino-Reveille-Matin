#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68

#define PINLED 13;
#define PININTERRUPT 2;
volatile bool clockInterrupt = false; // pour alarme

struct 

// Convertir nombre decimal en Binary Coded Decimal. ex. 12 = 0001 0010
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}

// Convertir BCD en nombre decimal ex. 0001 0010 = 12
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}




void setup() {
  Wire.begin();
  Serial.begin(9600);
  setTime(0, 9, 21, 7, 22, 10, 16);
  setAlarm1(15, 9, 21, 7, 22, 10, 16);
  pinMode(PINLED, OUTPUT);
  digitalWrite(PINLED, HIGH);
  pinMode(PININTERRUPT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PININTERRUPT), clockTrigger, RISING);
  /*Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);
  Wire.write(0b00011100);*/

}

void setTime(byte second, byte minute, byte hour, byte dayOfWeek,
byte dayOfMonth, byte month, byte year) {

  byte timeToWrite[] = {decToBcd(second), decToBcd(minute), decToBcd(hour),
    decToBcd(dayOfWeek), decToBcd(dayOfMonth), decToBcd(month), decToBcd(year)};
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // Lire la premiere adresse pour commencer 00h
  Wire.write(timeToWrite, 7);
  Wire.endTransmission();
}


void readTime(byte *second, byte *minute, byte *hour, byte *dayOfWeek,
byte *dayOfMonth, byte *month, byte *year){

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);// request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime(int arg){
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  if (arg == 1) {
    readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  }
  else {
    readAlarm1(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  }
  Serial.print(hour, DEC);
  Serial.print(":");
  if (minute<10){
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10){
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch(dayOfWeek){
    case 1:
      Serial.println("Sunday");
      break;
    case 2:
      Serial.println("Monday");
      break;
    case 3:
      Serial.println("Tuesday");
      break;
    case 4:
      Serial.println("Wednesday");
      break;
    case 5:
      Serial.println("Thursday");
      break;
    case 6:
      Serial.println("Friday");
      break;
    case 7:
      Serial.println("Saturday");
      break ;
  }
}




void setAlarm1(byte second, byte minute, byte hour, byte dayOfWeek,
  byte dayOfMonth, byte month, byte year) {

  byte timeAlarm[] = {decToBcd(second), decToBcd(minute), decToBcd(hour),
    decToBcd(dayOfWeek), decToBcd(dayOfMonth), decToBcd(month), decToBcd(year)};
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x07);
  Wire.write(timeAlarm, 7);
  Wire.endTransmission(); 
}

void readAlarm1(byte *second, byte *minute, byte *hour, byte *dayOfWeek,
byte *dayOfMonth, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x07);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read()); 
}

void clockTrigger() {
  clockInterrupt = !clockInterrupt;
  digitalWrite(PINLED, (state)? LOW:HIGH);
}

/*void disableAlarmFlag() {
  Wire.beginTransmission(0x68);
  Wire.write(0x0E);
  Wire.write(0b00011100);
  Wire.endTransmission();
}*/

void loop() {
  displayTime(1);
  displayTime(2);
  Serial.println("_____");
  delay(1000);
}
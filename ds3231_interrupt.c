#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
#define PINLED 13
#define PININTERRUPT 2

/* Permet de setter une alarme, de setter le temps, de lever une interruption
si le temps et l'alarme s'égalent. Ca allume une LED pour le montrer */

struct TimeAlarm {
  byte second;
  byte minute;
  byte hour;
  byte date;
};


volatile bool clockInterrupt = false; // pour alarme

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
  
  TimeAlarm time;
  time.second = 10;
  time.minute = 9;
  time.hour = 21;
  time.date = 7;
  setAlarm1(time);

  pinMode(PINLED, OUTPUT);
  pinMode(PININTERRUPT, INPUT);
  digitalWrite(PINLED, HIGH);
  attachInterrupt(digitalPinToInterrupt(PININTERRUPT), clockTrigger, FALLING);
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);
  Wire.write(0b00011101);
  Wire.write(0b10001000);
  Wire.endTransmission();
}

void setTime(byte second, byte minute, byte hour, byte dayOfWeek,
byte dayOfMonth, byte month, byte year) {

  byte timeToWrite[] = {decToBcd(second), decToBcd(minute), decToBcd(hour) & 0x3F,
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
  TimeAlarm time;

  if (arg == 1) {
    readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  }
  else {
    readAlarm1(time);
    second = time.second;
    minute = time.minute;
    hour = time.hour;
    dayOfWeek = time.date;
    dayOfMonth = 0;
    month = 0;
    year = 0;
  }

  Serial.print(hour, DEC);
  Serial.print(":");
  
  if (minute < 10){
    Serial.print("0");
  }
  
  Serial.print(minute, DEC);
  Serial.print(":");
  
  if (second < 10){
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



  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0F);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 1);
  byte register0Fh = Wire.read();
  Serial.println(register0Fh, BIN);
}

void setAlarm1(const TimeAlarm& time) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x07);

  byte timeAlarm[] = {
    decToBcd(time.second) & 0b01111111u,
    decToBcd(time.minute) & 0b01111111u,
    decToBcd(time.hour) & 0b00111111u,
    decToBcd(time.date) | 0b11000000u
  };

  Wire.write(timeAlarm, 4);
  Wire.endTransmission();
}

void readAlarm1(TimeAlarm& time) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x07);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 4);

  time.second = bcdToDec(Wire.read() & 0x7F);
  time.minute = bcdToDec(Wire.read() & 0x7F);
  time.hour = bcdToDec(Wire.read() & 0x3F);
  time.date = bcdToDec(Wire.read() & 0x3F);
}

void clockTrigger() {
  clockInterrupt = true;
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
  if (clockInterrupt) {
    digitalWrite(PINLED, LOW);
    Serial.print("interruption!!");

  }
  Serial.println("_____");
  delay(1000);
}

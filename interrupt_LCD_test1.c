#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DS3231_I2C_ADDRESS 0x68
#define LCD_I2C_ADDRESS 0x3F
#define PININTERRUPT 2

struct TimeAlarm {
  byte second;
  byte minute;
  byte hour;
  byte date;
};

volatile bool clockInterrupt = false; // pour alarme
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
int targetedTime = 0;


// Convertir nombre decimal en Binary Coded Decimal. ex. 12 = 0001 0010
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}

// Convertir BCD en nombre decimal ex. 0001 0010 = 12
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
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


void displayTime(int arg, String* results){
  

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  TimeAlarm time;
  String LCD_time = "";
  String LCD_DayOfWeek; 

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

  LCD_time = LCD_time + hour + ":";
  if (minute < 10){
    LCD_time += "0";
  }
  LCD_time = LCD_time + minute + ":";
  if (second < 10){
    LCD_time += "0";
  }
  LCD_time = LCD_time + second + " " + dayOfMonth + "/" + month + "/" + year;

   
  LCD_DayOfWeek = "Jour : ";
  switch(dayOfWeek){
    case 1:
      LCD_DayOfWeek += "Dimanche";
      break;
    case 2:
      LCD_DayOfWeek += "Lundi";
      break;
    case 3:
      LCD_DayOfWeek += "Mardi";
      break;
    case 4:
      LCD_DayOfWeek += "Mercredi";
      break;
    case 5:
      LCD_DayOfWeek += "Jeudi";
      break;
    case 6:
      LCD_DayOfWeek += "Vendredi";
      break;
    case 7:
      LCD_DayOfWeek += "Samedi";
      break ;
  }

  results[0] = LCD_time;
  results[1] = LCD_DayOfWeek;
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

void setup() {
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(20, 4);
  
  setTime(0, 9, 21, 7, 22, 10, 16);
  
  TimeAlarm time; // Alarme doit sonner 10 secondes plus tard
  time.second = 10;
  time.minute = 9;
  time.hour = 21;
  time.date = 7;
  setAlarm1(time);

  pinMode(PININTERRUPT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PININTERRUPT), clockTrigger, FALLING);
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);
  Wire.write(0b00011101); // LSB : Alarm 1 interrupt enable
  Wire.write(0b10001000); // Bit 2 : not-busy, Bit 1 : Clear alarm 2 flag, LSB : Clear Alarm 1 flag
  Wire.endTransmission();

  lcd.setCursor(0, 2);
  lcd.print("allo");
  delay(5000);
  lcd.clear();
}


void loop() {
  if (millis() > targetedTime) {
    lcd.clear();
    lcd.setCursor(0, 0);
    //lcd.print(displayTime(1)[0]);
    lcd.setCursor(0, 1);
    //lcd.print(displayTime(1)[1]);
    lcd.setCursor(0, 3);
    //lcd.print(displayTime(2)[0]);


    Serial.println("______________");
    String results[2];
      displayTime(1, results);
    //Serial.println(displayTime(1)[0]);
    //Serial.println(displayTime(1)[1]);
    //Serial.println(displayTime(2)[0]);
    //Serial.println("Jour");
    Serial.println("______________");
    
    if (clockInterrupt) {
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("interruption!!");
    }
    targetedTime = millis() + 1000; // refresh aux secondes
  }
}




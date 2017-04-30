/* Ce script fonctionne 
Il permet d'afficher l'heure d'un RTC DS3231 sur un écran LCD
Parfois, il saute une seconde et on voit bien qu'il raffraichit,
peut être ajusté. Envisager de mettre des pull-up sur les SDA/SCL*/


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DS3231_I2C_ADDRESS 0x68
#define LCD_I2C_ADDRESS 0x3F

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

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


void displayTime(){
  

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  String LCD_time;


  LCD_time = LCD_time + hour + ":";
  if (hour < 10){
    LCD_time += "0";
  }
  LCD_time = LCD_time + minute + ":";
  if (minute < 10){
    LCD_time += "0";
  }
  LCD_time = LCD_time + second + " " + dayOfMonth + "/" + month + "/" + year;

  String LCD_DayOfWeek;  
  LCD_DayOfWeek +="Jour : ";
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

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(LCD_time);
  lcd.setCursor(0, 1);
  lcd.print(LCD_DayOfWeek);
}


void setup() {
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(20, 4);
  //setTime(0, 40, 21, 3, 7, 3, 17);

}

void loop() {
  displayTime();
  delay(1000);
}
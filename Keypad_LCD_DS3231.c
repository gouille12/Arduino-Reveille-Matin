/* 
5. Avoir un effet lumineux lors d'un interrupt (6 LED qui éclaire l'une après l'autre en cercle?)
6. Avoir un effet sonore lors d'un interrupt (https://diyhacking.com/arduino-audio-player/, un genre de petite mélodie type cellulaire genre?)
9. Alimentation à base de pile (attn 9V -> 5V direct = pas bon) + Pouvoir afficher l'état de la pile?
12. Affichage sur le LCD propre
16. Assurer la précision des alarmes : doit sonner exactement 10 sec après avoir mis le dernier chiffre -> CHANGE? (voir sur qq heures si c'est le meme écart)
20. Afficher l'heure avec des leading 0 si nécessaire
21. Msg d'erreur si date/heure invalide sur le lcd
24. Bouton back?
25. S'assurer que ca sonne pas à 0:0:0
26. Penser à une façon pour que je désactive pas facilement une alarme (D) (ex. bouton rigide?)
27. Bouton power
28. Erreur si countdown 24h et + (match_hours seulement)
29. Éviter le plus possible les refresh visibles

7. Clairer le code pour qu'il soit le plus clair possible + doc string
8. Faire un KiCad complet du circuit qui est clair
10. Prévoir implantation permanente (eagle + etching? ou plaque avec trous?)
11. Esthétisme du produit fini
13. Test de toutes les fonctions
14. sauver le code sur github
15. Penser à ce que je recherche dans un réveille-matin
18. Charger les piles au complet
30. Mettre un poids dans le case pour qu'il soit pas facilement renversable
*/


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DS3232RTC.h>
#include <Time.h>

#define DS3231_I2C_ADDRESS 0x68
#define PININTERRUPT 2

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte colPins[COLS] = {6, 5, 4, 3};
byte rowPins[ROWS] = {10, 9, 8, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
time_t actualTime;
time_t countdownTargetTimeT;
time_t countdownLCD;
char keyPressed;
bool backlightFlag;
volatile bool clockInterrupt = false;
String alarmlcd1 = "";
String alarmlcd2 = "";


String timeTToString(time_t t) {

  String strTimeT = "";
  strTimeT = strTimeT + String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t));
  return strTimeT;
}

String dateTToString(time_t t) {
  String strDateT = "";
  String dayOfWeekLong[] = {"Samedi", "Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi"};
  strDateT = strDateT + dayOfWeekLong[weekday(t)] + " " + String(day(t)) + "/" + String(month(t)) + "/" + String(year());
  return strDateT;
}

void clearLine(int line) {
  lcd.setCursor(0, line);
  lcd.print("                    ");
}



void setup() {
  Wire.begin();
  Serial.begin(9600);

  lcd.begin(20, 4);
  lcd.clear();
  lcd.noBacklight();
  backlightFlag = false;

  setSyncProvider(RTC.get); // sync time.h et rtc
  Serial.println("RTCsync");
  if (timeStatus() != timeSet) {
    Serial.println("Erreur");
  }


  RTC.squareWave(SQWAVE_NONE);
  pinMode(PININTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PININTERRUPT), clockTrigger, FALLING);
}

void setTimeMenu() {
  String timeToSet;
  String dateToSet;
  String dayOfWeekToSet;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Entrez l'heure");
  lcd.setCursor(0, 1);
  lcd.print("HH:MM:SS");
  lcd.setCursor(0, 1);

  for (int i = 0; i <= 5; i++) {
    keyPressed = keypad.waitForKey();
    if (keyPressed) {
      if (i == 2 || i == 4) {
        lcd.print(":");
      }
      lcd.print(keyPressed);
      timeToSet = timeToSet + keyPressed; // HHMMSS
    }
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Entrez la date");
  lcd.setCursor(0, 1);
  lcd.print("JJ/MM/AAAA");
  lcd.setCursor(0, 1);

  for (int i = 0; i <= 7; i++) {
    keyPressed = keypad.waitForKey();  
    if (keyPressed) {
      if (i == 2 || i == 4) {
        lcd.print("/");
      }
      lcd.print(keyPressed);
      dateToSet = dateToSet + keyPressed; // JJMMAAAA
    }
  }

  Serial.println(timeToSet);
  Serial.println(dateToSet);
  if (isValidTime(timeToSet) == true || isValidDate(dateToSet) == true) {
    tmElements_t tm;
    time_t t;
    tm.Year = dateToSet.substring(6).toInt();
    tm.Year = y2kYearToTm(tm.Year);
    tm.Month = dateToSet.substring(2, 4).toInt();
    tm.Day = dateToSet.substring(0, 2).toInt();
    tm.Hour = timeToSet.substring(0, 2).toInt();
    tm.Minute = timeToSet.substring(2, 4).toInt();
    tm.Second = timeToSet.substring(4).toInt();
    t = makeTime(tm);
    RTC.set(t);
    setTime(t);    
  }
  else {
    Serial.println("Heure/date invalide");
  }
  lcd.clear();
}


void setAlarm2() {
  String alarmToSet;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Entrez l'alarme");
  lcd.setCursor(0, 1);
  lcd.print("HH:MM");
  lcd.setCursor(0, 1);

  for (int i = 0; i <= 3; i++) {
    keyPressed = keypad.waitForKey();
    if (keyPressed) {
      if (i == 2) {
        lcd.print(":");
      }
      lcd.print(keyPressed);
      alarmToSet = alarmToSet + keyPressed; // HHMM
    }
  }

  Serial.println(alarmToSet.substring(2).toInt());
  Serial.println(alarmToSet.substring(0, 2).toInt());


  if (isValidTime(alarmToSet) == true) {
    RTC.setAlarm(ALM2_MATCH_HOURS, 0, alarmToSet.substring(2).toInt(), alarmToSet.substring(0, 2).toInt(), 1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_2, true); 
    alarmlcd2 = "Alarme : " + String(alarmToSet.substring(0, 2).toInt()) + ":" + String(alarmToSet.substring(2).toInt());  
  }

  else {
    Serial.println("Heure/date invalide");
  }  

  lcd.clear();
  displayAlarms();
}

void clockTrigger() {
  clockInterrupt = true;
}

time_t setCountdown() {
  String countdownToSet;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Entrez la durée :");
  lcd.setCursor(0, 1);
  lcd.print("HH:MM:SS");
  lcd.setCursor(0, 1);

  for (int i = 0; i <= 5; i++) {
    keyPressed = keypad.waitForKey();
    if (keyPressed) {
      if (i == 2 || i == 4) {
        lcd.print(":");
      }
      lcd.print(keyPressed);
      countdownToSet = countdownToSet + keyPressed; // HHMMSS
    }
  }

  int countdownHours = countdownToSet.substring(0, 2).toInt();
  int countdownMinutes = countdownToSet.substring(2, 4).toInt();
  int countdownSeconds = countdownToSet.substring(4).toInt();
  time_t nowTime = now(); // pas que ca change pendant l'assignation
  tmElements_t tmcountdown;
  tmcountdown.Year = String(year(nowTime)).substring(2).toInt();
  tmcountdown.Year = y2kYearToTm(tmcountdown.Year);
  tmcountdown.Month = month(nowTime);
  tmcountdown.Day = day(nowTime);
  tmcountdown.Hour = countdownHours + hour(nowTime);
  tmcountdown.Minute = countdownMinutes + minute(nowTime);
  tmcountdown.Second = countdownSeconds + second(nowTime);
  countdownTargetTimeT = makeTime(tmcountdown);

  RTC.setAlarm(ALM1_MATCH_HOURS, second(countdownTargetTimeT), minute(countdownTargetTimeT), hour(countdownTargetTimeT), 1);
  RTC.alarm(ALARM_1);
  RTC.alarmInterrupt(ALARM_1, true);
  alarmlcd1 = "Alarme : " + timeTToString(countdownTargetTimeT);
  lcd.clear();
  Serial.println("_________INFO_____________");
  Serial.println(timeTToString(countdownTargetTimeT));
  Serial.println("__________END INFO_____________");
}


void displayTime() {
  if (actualTime != now()) {
    actualTime = now();
    clearLine(1);
    lcd.setCursor(0, 1);
    lcd.print(timeTToString(actualTime));
    Serial.println(timeTToString(actualTime));
    clearLine(0);
    lcd.setCursor(0, 0);
    lcd.print(dateTToString(actualTime));
    Serial.println(dateTToString(actualTime));
    
    if (alarmlcd1 != "") {
      clearLine(3);
      countdownLCD = countdownTargetTimeT - now();
      lcd.setCursor(0,3);
      lcd.print(timeTToString(countdownLCD));
    }
  }


}

void displayAlarms() {

  if (alarmlcd1 == "") {
    clearLine(3);
  }

  else {
    lcd.setCursor(0, 3);
    lcd.print(alarmlcd1);
  }

  if (alarmlcd2 == "") {
    clearLine(2);
  }

  else {
    lcd.setCursor(0, 2);
    lcd.print(alarmlcd2);
  }
}

bool isValidTime(String time) {
  // time = HHMMSS
  int hours = time.substring(0, 2).toInt();
  int minutes = time.substring(2, 4).toInt();
  int seconds = time.substring(4).toInt();

  if (hours <= 24 && hours >= 0 &&
    minutes <= 59 && minutes >= 0 &&
    seconds <= 59 && seconds >= 0 &&
    time.toInt() != 0) {
    return true;
  }

  else {
    return false;
  }
}

bool isValidDate(String date) {
  // date = JJMMAAAA
  int days = date.substring(0, 2).toInt();
  int months = date.substring(2, 4).toInt();
  int years = date.substring(4).toInt();

  if (days <= 31 && days >= 1 &&
    months <= 12 && months >= 1 &&
    years >= 1970 && date.toInt() != 0) {
    return true;
  }

  else {
    return false;
  }
}



void loop() {

  keyPressed = keypad.getKey();
  if (keyPressed == 'A') {
    // Setter l'alarme
    setAlarm2();
  }

  else if (keyPressed == 'B') {
    // Compte à rebours/chrono
    setCountdown();
  }

  else if (keyPressed == 'C') {
    //Modifier heure ou voir l'heure
    setTimeMenu();
  }

  else if (keyPressed == 'D') {
    // désactiver toutes les alarmes -> penser à mettre un bouton rigide moins facilement accrochable
    RTC.alarmInterrupt(ALARM_2, false); 
    alarmlcd2 = "";
    RTC.alarmInterrupt(ALARM_1, false);
    alarmlcd1 = "";
    displayAlarms();
  }

  else if (keyPressed == '*') {
    if (backlightFlag == true) {
      lcd.noBacklight();
      backlightFlag = false;
    }
    else if (backlightFlag == false) {
      lcd.backlight();
      backlightFlag = true;
    }
  }

  else if (keyPressed == '#') {
    // clairer alarmes déclenchées
    displayAlarms();
  }

  if (clockInterrupt) {
    Serial.print("interrupt");
    if (RTC.alarm(ALARM_2)) {
      clearLine(2);
      lcd.setCursor(0, 2);
      lcd.print("ALARME 2!!");
      RTC.alarmInterrupt(ALARM_2, false); 
      alarmlcd2 = "";
    }
    if (RTC.alarm(ALARM_1)) {
      clearLine(3);
      lcd.setCursor(0, 3);
      lcd.print("ALARME 1!!");
      RTC.alarmInterrupt(ALARM_1, false);
      alarmlcd1 = "";

    }
    clockInterrupt = false;
    // Son & lumieres
  }
  displayTime();
}
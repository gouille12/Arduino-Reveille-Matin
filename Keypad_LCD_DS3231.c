/* 
24-07-2017-13h17 = Je peux entrer une heure, une date et un jour de semaine et ca s'affiche bien dans le lcd et ca se met dans des variables.
31-07-2017-14h47 = Je peux entrer l'heure/date en time_t et l'afficher sur le LCD
03-08-2017-14h35 = Un interrupt s'affiche sur le lcd au bon moment
07-08-2017-14h12 = Compte à rebours ok


3. Bouton pour la backlight (bouton physique on/off)
4. Gestion des input erronés (* = back)
5. Avoir un effet lumineux lors d'un interrupt (6 LED qui éclaire l'une après l'autre en cercle?)
6. Avoir un effet sonore lors d'un interrupt (https://diyhacking.com/arduino-audio-player/, un genre de petite mélodie type cellulaire genre?)
7. Clairer le code pour qu'il soit le plus clair possible + doc string
8. Faire un KiCad complet du circuit qui est clair
9. Alimentation à base de pile (attn 9V -> 5V direct = pas bon) + Pouvoir afficher l'état de la pile?
10. Prévoir implantation permanente (eagle + etching? ou plaque avec trous?)
11. Esthétisme du produit fini
12. Affichage sur le LCD propre
13. Test de toutes les fonctions
14. sauver le code sur github
15. Penser à ce que je recherche dans un réveille-matin
16. Assurer la précision des alarmes : doit sonner exactement 10 sec après avoir mis le dernier chiffre
18. Charger les piles au complet
19. Utiliser la fonction Timettostring pour les alarmes
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
char keyPressed;
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
  lcd.backlight(); // Penser à retirer
  lcd.setCursor(0, 0);
  lcd.print("Init...");
  delay(1000);
  lcd.clear();

  setSyncProvider(RTC.get); // sync time.h et rtc
  Serial.println("RTCsync");
  if (timeStatus() != timeSet) {
    Serial.println("Erreur");
  }


  RTC.squareWave(SQWAVE_NONE);
  pinMode(PININTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PININTERRUPT), clockTrigger, FALLING);
}

void setTime() {
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
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, alarmToSet.substring(2).toInt(), alarmToSet.substring(0, 2).toInt(), 1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_2, true); 
  alarmlcd2 = "Alarme : " + String(alarmToSet.substring(0, 2).toInt()) + ":" + String(alarmToSet.substring(2).toInt());
  lcd.clear();
  displayAlarms();
}

void clockTrigger() {
  clockInterrupt = true;
}

void setCountdown() {

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
  time_t countdownTargetTimeT = now() + countdownHours*3600 + countdownMinutes*60 + countdownSeconds;



  Serial.println(second(countdownTargetTimeT));
  Serial.println(minute(countdownTargetTimeT));
  Serial.println(hour(countdownTargetTimeT));
  RTC.setAlarm(ALM1_MATCH_HOURS, second(countdownTargetTimeT), minute(countdownTargetTimeT), hour(countdownTargetTimeT), 1);
  RTC.alarm(ALARM_1);
  RTC.alarmInterrupt(ALARM_1, true);
  alarmlcd1 = "Alarme : " + String(hour(countdownTargetTimeT)) + ":" + String(minute(countdownTargetTimeT)) + ":" + String(second(countdownTargetTimeT));
  lcd.clear();
  displayAlarms();
}

void displayTime() {
  if (actualTime != now()) {
    actualTime = now();
    lcd.setCursor(0, 1);
    lcd.print(timeTToString(actualTime));
    Serial.println(timeTToString(actualTime));
    lcd.setCursor(0, 0);
    lcd.print(dateTToString(actualTime));
    Serial.println(dateTToString(actualTime));
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
    setTime();
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
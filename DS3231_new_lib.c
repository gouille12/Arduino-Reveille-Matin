#include <Wire.h>
#include <DS3232RTC.h>
#include <Time.h>

#define DS3231_I2C_ADDRESS 0x68

void setup() {
  Serial.begin(9600);
  setSyncProvider(RTC.get); // sync time.h et rtc
  Serial.println("RTCsync");
  if (timeStatus() != timeSet) {
  	Serial.println("Erreur");
  }
}

void loop() {
	Serial.println(RTC.get());
}
#define pin_D0 2
#define pin_D1 3
#define pin_D2 4
#define pin_D3 5
#define pin_D4 6
#define pin_D5 7
#define pin_D6 9
#define pin_D7 10

#define pin_RS 11
#define pin_RW 12
#define pin_E 13


/*
E = H pour écrire dans les registres
R/W = H pour read mode, L pour write mode
RS = Choisir le registre : H = data-screen, L = instructions what to do next
D0-D7 = data qu'on envoie au registre ou qu'on lit
V0 = luminosité (potentio) - potentio est nécessaire
Vss = ground
VDD = power
A = rétroéclairage + 
K = rétroéclairage -
*/



void setup() {
	Serial.begin(9600);
	pinMode(pin_D0, OUTPUT);
	pinMode(pin_D1, OUTPUT);
	pinMode(pin_D2, OUTPUT);
	pinMode(pin_D3, OUTPUT);
	pinMode(pin_D4, OUTPUT);
	pinMode(pin_D5, OUTPUT);
	pinMode(pin_D6, OUTPUT);
	pinMode(pin_D7, OUTPUT);
	pinMode(pin_RS, OUTPUT);
	pinMode(pin_RW, OUTPUT);
	pinMode(pin_E, OUTPUT);
	digitalWrite(pin_D0, LOW);
	digitalWrite(pin_D1, LOW);
	digitalWrite(pin_D2, LOW);
	digitalWrite(pin_D3, LOW);
	digitalWrite(pin_D4, LOW);
	digitalWrite(pin_D5, LOW);
	digitalWrite(pin_D6, LOW);
	digitalWrite(pin_D7, LOW);
	digitalWrite(pin_RS, LOW);
	digitalWrite(pin_RW, LOW);
	digitalWrite(pin_E, LOW);

}

void loop() {
	/*
	D0-D7 = 0000 0011
	RS = 1
	RW = 0
	E = 1
	*/

	raising_E();
	digitalWrite(pin_D0, LOW);
	digitalWrite(pin_D1, LOW);
	digitalWrite(pin_D2, LOW);
	digitalWrite(pin_D3, LOW);
	digitalWrite(pin_D4, LOW);
	digitalWrite(pin_D5, LOW);
	digitalWrite(pin_D6, HIGH);
	digitalWrite(pin_D7, HIGH);
	digitalWrite(pin_RS, HIGH);
	digitalWrite(pin_RW, LOW);
	digitalWrite(pin_E, HIGH);

	delay(3000);
	Serial.print("changement");
	delay(1000);

	/*
	D0-D7 = 0000 0011
	RS = 1
	RW = 1*
	E = 1
	*/
	raising_E();
	digitalWrite(pin_D0, LOW);
	digitalWrite(pin_D1, LOW);
	digitalWrite(pin_D2, LOW);
	digitalWrite(pin_D3, LOW);
	digitalWrite(pin_D4, LOW);
	digitalWrite(pin_D5, LOW);
	digitalWrite(pin_D6, HIGH);
	digitalWrite(pin_D7, HIGH);
	digitalWrite(pin_RS, HIGH);
	digitalWrite(pin_RW, HIGH);
	digitalWrite(pin_E, HIGH);
	raising_E();
	delay(3000);
	Serial.print("changement");
	delay(1000);

}
void raising_E() {

	digitalWrite(pin_E, LOW);
	delay(1000);
	digitalWrite(pin_E, HIGH);
	delay(1000);
}
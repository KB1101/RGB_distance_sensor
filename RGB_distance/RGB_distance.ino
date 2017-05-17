#include <DS18B20.h>
#include <OneWire.h>
#include <LiquidCrystal.h>


#define TRIG 3							//pin wyzwalania czujnika
#define ECHO 2							//pin powrotu czyjnika
#define BAUDRATE 9600					//baudrate serial port
#define ONEWIRE_PIN 12					// pin onewire do ds18b20
#define RED_PIN 11						// kolor czerwony na pinie 11
#define GREEN_PIN 10					// kolor zielony na pinie 10
#define BLUE_PIN 9						// kolor niebieski na pinie 9
#define NIEBIESKI_KOLOR 0				// czy u¿ywaæ koloru niebieskiego 0|1
#define unit8_t byte

int odleglosc, licznik,licznik2;		//odleg³oœæ w cm
long CZAS;								//d³ugoœæ powrotnego impulsu w uS
float temp;								//temperatura

/*
* LCD RS pin to pin A0
* LCD Enable pin to pin A1
* LCD D4 pin to pin A2
* LCD D5 pin to pin A3
* LCD D6 pin to pin A4
* LCD D7 pin to pin A5
*/
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);
byte adres[8] = { 0x28, 0x28, 0x2E, 0x28, 0x2, 0x0, 0x0, 0x70 };

void setup(){

	Serial.begin(BAUDRATE);						//inicjalizaja monitora szeregowego
	pinMode(TRIG, OUTPUT);						//ustawienie pinu 3 w Arduino jako wyjœcie
	pinMode(ECHO, INPUT);						//ustawienie pinu 2 w Arduino jako wejœcie
	pinMode(RED_PIN, OUTPUT);					//ustawienie pinu 11,10,9 jako wyjscie dla taœmy RGB
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);
	lcd.begin(8, 2);							//inicjalizacja LCD 16x1 -> rozpoznawany jako 8x2
	lcd.print("Hello!");						// Na lcd: Hello!
	Serial.println("Czujnik odleglosci z LCD"); //Na serial: Czujnik odleglosci z LCD

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Test LED");

	lcd.setCursor(0, 1);
	lcd.print("s RED   ");
	analogWrite(RED_PIN, 255);
	delay(500);
	analogWrite(RED_PIN,0);

	lcd.setCursor(0, 1);
	lcd.print("s BLUE  ");
	analogWrite(BLUE_PIN, 255);
	delay(500);
	analogWrite(BLUE_PIN, 0);

	lcd.setCursor(0, 1);
	lcd.print("s GREEN ");
	analogWrite(GREEN_PIN, 255);
	delay(1000);
	analogWrite(GREEN_PIN, 0);


	licznik = 0;
	licznik2 = 0;
	lcd.clear();
	sensors.begin(10);
}



void loop(){

	pomiar_odleglosci();              //pomiar odleg³oœci

	sensors.request(adres);
	delay(80);
	temp = sensors.readTemperature(adres);

	/*byte addr[8];

	onewire.reset_search();
	if (!onewire.search(addr)) {
		Serial.print("No more addresses.\n");
		onewire.reset_search();
		return;
	}

	Serial.print("R=");
	int i;
	for (i = 0; i < 8; i++) {
		Serial.print(addr[i], HEX);
		Serial.print(" ,");
	}
	Serial.println();
	*/

	if (licznik == 2) {
		zmiana_koloru();
		wyswietl_na_lcd();
		Serial.print("Odleglosc: ");      //wyœwietlanie wyników na ekranie w pêtli co 200 ms
		Serial.print(odleglosc);
		Serial.print(" cm Temp: ");
		Serial.print(temp);
		Serial.println("*C");
		licznik = 0;
	}
	else licznik++;


}

void zmiana_koloru() {

		if (odleglosc > 250 && NIEBIESKI_KOLOR) analogWrite(BLUE_PIN, 255);
		else if (odleglosc <= 250 && odleglosc > 120) {
			analogWrite(RED_PIN, 0);
			analogWrite(GREEN_PIN, odleglosc);
			analogWrite(BLUE_PIN, 30);
		}
		else if (odleglosc <= 120 && odleglosc > 100) {
			analogWrite(RED_PIN, odleglosc / 10);
			analogWrite(GREEN_PIN, odleglosc);
			analogWrite(BLUE_PIN, 0);
		}
		else if (odleglosc <= 100 && odleglosc > 50) {
			analogWrite(RED_PIN, 200 - odleglosc);
			analogWrite(GREEN_PIN, odleglosc);
			analogWrite(BLUE_PIN, 0);
		}
		else if (odleglosc <= 50 && odleglosc > 30) {
			analogWrite(RED_PIN, 220 - odleglosc);
			analogWrite(GREEN_PIN, odleglosc + 10);
			analogWrite(BLUE_PIN, 0);
		}
		else if (odleglosc <= 30 && odleglosc > 10) {
			analogWrite(RED_PIN, 255 - odleglosc);
			analogWrite(GREEN_PIN, odleglosc);
			analogWrite(BLUE_PIN, 0);
		}
		else if (odleglosc <= 10 && odleglosc >= 5) {
			analogWrite(RED_PIN, 255 - odleglosc);
			analogWrite(GREEN_PIN, odleglosc);
			analogWrite(BLUE_PIN, 0);
		}
		else if (odleglosc < 5) {
			analogWrite(RED_PIN, 255);
			analogWrite(GREEN_PIN, 20);
			analogWrite(BLUE_PIN, 30);
		}

}

void wyswietl_na_lcd() {

		//lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("DIST=");
		lcd.print(odleglosc);
		if (odleglosc / 10 == 0) lcd.print(" ");
		if (odleglosc / 100 == 0) lcd.print(" ");

		lcd.setCursor(0, 1);
		lcd.print(" T=");
		lcd.print(temp);
		if (temp / 10 == 0) lcd.print(" ");

}

void pomiar_odleglosci(){
	digitalWrite(TRIG, HIGH);       //ustawienie stanu wysokiego na 10 uS - impuls inicjalizujacy - patrz dokumentacja
	delayMicroseconds(10);
	digitalWrite(TRIG, LOW);
	CZAS = pulseIn(ECHO, HIGH);
	odleglosc = CZAS / 58;                //szerokoœæ odbitego impulsu w uS podzielone przez 58 to odleglosc w cm - patrz dokumentacja
}
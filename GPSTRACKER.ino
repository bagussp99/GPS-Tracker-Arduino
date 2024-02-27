#include <SoftwareSerial.h>
#include <TinyGPS++.h>


TinyGPSPlus gps;
double latitude, longitude;

SoftwareSerial SIM800L(8, 7);  //RX dan TX

// Pin tombol reboot (misalnya menggunakan pin D3)
const int tombol = 4;
unsigned long buttonPressStartTime = 0;
bool isButtonPressed = false;


String response;
int lastStringLength = response.length();

String link;


//nada
#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_A5 880

const int buzzerPin = 11;


void setup() {
  Serial.begin(9600);
  Serial.println("GPS Mulai");

  // Mengatur pin tombol reboot sebagai input dengan pull-up resistor
  pinMode(tombol, INPUT_PULLUP);

  pinMode(buzzerPin, OUTPUT);

  SIM800L.begin(9600);
  SIM800L.println("AT+CMGF=1");
  Serial.println("SIM800L started at 9600");
  delay(1000);
  Serial.println("Setup Complete! SIM800L is Ready!");
  SIM800L.println("AT+CNMI=2,2,0,0,0");

  SoundWelcome();
}

void loop() {

  tekanTombol();

  if (SIM800L.available() > 0) {
    response = SIM800L.readStringUntil('\n');
  }

  if (lastStringLength != response.length()) {
    GPS();
    //Perintah ON
    if (response.indexOf("ceklokasi") != -1) {  //ganti CEK dengan keyword yang diinginkan jangan gunakan spasi
      SIM800L.println("AT+CMGF=1");             //Sets the GSM Module in Text Mode
      delay(500);
      Serial.println("ceklokasi : Kirim Pesan");       
      SIM800L.println("AT+CMGS=\"+6285156656501\"\r");  //ganti no hp yang akan di kirimi balasan sms
      delay(1000);
      SIM800L.print("Respon ceklokasi\n\n");
      SIM800L.print("Lokasi perangkat saat ini: \n");
      SIM800L.print("\nLatitude: ");
      SIM800L.print(latitude, 6);
      SIM800L.print("\nLongitude: ");
      SIM800L.print(longitude, 6);
      SIM800L.print("\nLink: ");
      SIM800L.print(link);
      SIM800L.println((char)26);  // ASCII code of CTRL+Z
      SoundBalas();
      delay(1000);
    }
  }
}

void GPS() {
  if (Serial.available()) {
    gps.encode(Serial.read());
  }
  if (gps.location.isUpdated()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    link = "www.google.com/maps/place/" + String(latitude, 6) + "%2C" + String(longitude, 6);

    Serial.print("Latitude : ");
    Serial.println(latitude, 6);
    Serial.print("Longitude : ");
    Serial.println(longitude, 6);
    Serial.print("Link Map : ");
    Serial.println(link);
  }
}

void tekanTombol() {
  if (digitalRead(tombol) == LOW) {  // Mengecek apakah tombol ditekan
    if (!isButtonPressed) {
      isButtonPressed = true;
      buttonPressStartTime = millis();
    } else {
      unsigned long buttonPressDuration = millis() - buttonPressStartTime;

      if (buttonPressDuration >= 2000) {  // Jika tombol ditekan lebih dari 3 detik
        SIM800L.println("AT+CMGF=1");     //Sets the GSM Module in Text Mode
        delay(1000);
        SoundTBL(); 
        Serial.println("Tombol : Kirim Pesan");           // Delay of 1000 milli seconds or 1 second
        SIM800L.println("AT+CMGS=\"+62878XXXXXXXX\"\r");  //ganti no hp yang akan di kirimi balasan sms
        delay(1000);
        SIM800L.print("Respon Tombol\n\n");
        SIM800L.print("Lokasi perangkat saat ini: \n");
        SIM800L.print("\nLatitude: ");
        SIM800L.print(latitude, 6);
        SIM800L.print("\nLongitude: ");
        SIM800L.print(longitude, 6);
        SIM800L.print("\nLink: ");
        SIM800L.print(link);
        SIM800L.println((char)26);  // ASCII code of CTRL+Z
        delay(1000);
      }
    }
  }
}


// Fungsi untuk memainkan nada pada buzzer
void playNote(int note, int duration) {
  tone(buzzerPin, note, duration);
  delay(duration + 50);  // Memberikan jeda sejenak setelah memainkan nada
  noTone(buzzerPin);     // Mematikan nada
}
void SoundWelcome() {
  // Memainkan nada pertama (C5)
  playNote(NOTE_C5, 200);
  delay(100);

  // Memainkan nada kedua (E5)
  playNote(NOTE_E5, 200);
  delay(100);

  // Memainkan nada ketiga (G5)
  playNote(NOTE_G5, 200);
}
void SoundTBL() {
  // Memainkan nada pertama (G5)
  playNote(NOTE_G5, 200);
  delay(100);

  // Memainkan nada kedua (A5)
  playNote(NOTE_A5, 200);
}

void SoundBalas() {
  // Memainkan nada pertama (E5)
  playNote(NOTE_E5, 100);
  delay(100);

  playNote(NOTE_E5, 100);
  delay(100);

  // Memainkan nada ketiga (A5) dengan durasi lebih panjang (400 ms)
  playNote(NOTE_A5, 400);
}
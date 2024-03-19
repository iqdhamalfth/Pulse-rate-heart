//#include <KRwifi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <arduino.h>
#include <AverageValue.h>
#define USE_ARDUINO_INTERRUPTS true  // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>   // Includes the PulseSensorPlayground Library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// //Untuk Koneksi WiFi
// char* ssid = "UG KELAPADUA HOTZONE";
// char* pass = "c0b4d1b4c4";
// char* server = "api.thingspeak.com";
// char* APIKey = "9VAT4HU8JEHVFREM";

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int oneWireBus = 4;  // sensor suhu pin

//sensor DS18B20
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
float temperatureC;

// Variables
const int PulseWire = A0;  // PulseSensorconnected to ANALOG PIN 0
int Threshold = 550;
int myBPM;

PulseSensorPlayground pulseSensor;

//sensor mq2
int MQ135Pin = A2;  //mq2 sensor pin
int Rload = 20000;
float rO = 66000;
double ppm = 417.57;
float a = 110.7432567;
float b = -2.856935538;
float minppm = 0;
float maxppm = 0;
const long MAX_VALUE_NUM = 10;
AverageValue<long> averageValue(MAX_VALUE_NUM);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();  //sensor suhu
  //setWifi(ssid, pass);

  lcd.begin();
  lcd.backlight();
  LCDinit();

  pinMode(MQ135Pin, INPUT);

  minppm = pow((1000 / 110.7432567), 1 / -2.856935538);
  maxppm = pow((10 / 110.7432567), 1 / -2.856935538);
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);
  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.
  }
}

void loop() {
  //DS18B20
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Suhu: ");
  Serial.print(temperatureC);
  Serial.println(" ºC");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C: ");
  lcd.print(temperatureC, 1);
  //lcd.print(" C");
  // delay(2000);

   // mq2 main program
  int adcRaw = analogRead(MQ135Pin);
  double rS = ((1024.0 * Rload) / adcRaw) - Rload;
  float rSrO = rS / rO;
  float ppm = a * pow((float)rS / (float)rO, b);
  averageValue.push(ppm);
  Serial.print("CO2:");
  Serial.print(ppm);
  Serial.println(" PPM");

  //lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("PPM: ");
  lcd.print(ppm, 1);
  //lcd.print(" PPM");
  //delay(2000);

  //pulse rate heart
  int myBPM = pulseSensor.getBeatsPerMinute();     // Calls function on our pulseSensor object that returns BPM as an "int".
  if (pulseSensor.sawStartOfBeat()) {              // Constantly test to see if "a beat happened".
    //Serial.println("♥  A HeartBeat Happened ! ");  // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM:");                         // Print phrase "BPM: "
    Serial.println(myBPM);                         // Print the value inside of myBPM.

    //lcd.clear();
    lcd.setCursor(8, 0);
    lcd.print("BPM: ");
    lcd.print(myBPM);
    delay(2000);
  }
  if (myBPM >= 100 && temperatureC < 33 || temperatureC > 40 && ppm >= 50) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kondisi: ");
    lcd.setCursor(0, 1);
    lcd.print("Stress Berat");
    Serial.print("Stress Berat");
    delay(2000);
  } else if (myBPM >= 90 && temperatureC >= 33 && ppm >= 45) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kondisi: ");
    lcd.setCursor(0, 1);
    lcd.print("Stress Sedang");
    Serial.print("Stress Sedang");
    delay(2000);
  } else if (myBPM >= 70 && temperatureC >= 35 && ppm >= 40) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kondisi: ");
    lcd.setCursor(0, 1);
    lcd.print("Stress Ringan");
    Serial.print("Stress Ringan");
    delay(2000);
  } else if (myBPM >= 60 && temperatureC >= 36 && ppm < 40) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kondisi Normal");
    Serial.print("Kondisi Normal");
    delay(2000);
  }
  // String konten;
  // konten = String() + "field1=" + temperatureC + "&field2=" + myBPM + "&field3=" + ppm;
  // httpPOST(server, APIKey, konten, 50, 80);
  // Serial.print("Respon: ");
  // Serial.println(getData);
  // statusPengiriman();
  // delay(1000);
}
void LCDinit() {
  lcd.setCursor(0, 0);
  lcd.print("    Hallo!   ");
  lcd.setCursor(0, 1);
  lcd.print("   Loading...  ");
  delay(3000);
}
// void LCDshow() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("C: ");
//   lcd.print(temperatureC);

//   lcd.setCursor(0, 1);
//   lcd.print("PPM: ");
//   lcd.print(ppm);

//   lcd.setCursor(6, 0);
//   lcd.print("BPM: ");
//   lcd.print(myBPM);
  
//   delay(2000);
// }
// void kondisi() {
//   if (myBPM > 100 && temperatureC < 33 && temperatureC > 40 && ppm > 50) {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Kondisi Stress Berat");
//   } else if (myBPM > 90 && myBPM <= 100 && temperatureC >= 33 && temperatureC <= 34 && ppm > 45 && ppm <= 50) {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Kondisi Stress Sedang");
//   } else if (myBPM > 70 && myBPM <= 90 && temperatureC >= 35 && temperatureC <= 36 && ppm >= 40 && ppm <= 45) {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Kondisi Stress Ringan");
//   } else if (myBPM >= 60 && myBPM <= 70 && temperatureC >= 36 && temperatureC <= 37 && ppm >= 60 && ppm <= 70) {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Kondisi Normal");
//   }
// }

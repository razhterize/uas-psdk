#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <MCP4725.h>
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>

void updateLCD();
void printLCD();
void database();

MCP4725 MCP(0x60);
Adafruit_ADS1115 ads;
LiquidCrystal_I2C lcd(0x27, 16, 2);
FirebaseData fdbo;
FirebaseAuth auth;
FirebaseConfig config;

const float maxVolt = 35;
const float maxCurrent = 6;
int writeVal = 1600;
float voltage, current, setVoltage=3, setCurrent=1;
unsigned long prevMillis;
float sets;

void setup(){
  Serial.begin(115200);
  MCP.begin();
  if (!ads.begin()){
    Serial.println("ADS Failed!");
    while(1);
  }
  ads.setGain(GAIN_ONE);
  lcd.init();
  lcd.backlight();
  WiFi.begin("12345","tahutempe");
  while(!WiFi.isConnected()){
    Serial.print(".");
    delay(200);
  }
  config.api_key = "AIzaSyAiuX8LjDc7eWa1g9MeCJDmDLJc_qKLGLw";
  config.database_url = "https://psdk-uas-87803-default-rtdb.asia-southeast1.firebasedatabase.app/";
  auth.user.email = "razhterize@gmail.com";
  auth.user.password = "visualstudio";
  Firebase.begin(&config,&auth);
  printLCD();
}

void loop(){
  current = (ads.computeVolts(ads.readADC_SingleEnded(0)) - 2.46) * 10;
  voltage = ads.computeVolts(ads.readADC_SingleEnded(1))*11.43;
  if (current < setCurrent){
    if (!(setVoltage - 0.2 <= voltage)&&!(setVoltage + 0.2 >= voltage)){
      if (voltage < setVoltage) {
      writeVal--;
      }
      else if (voltage > setVoltage){
      writeVal++;
      }
    }
  }else if (current > setCurrent){
    writeVal++;
  }
  MCP.writeDAC(writeVal);
  if (millis() - prevMillis >= 200){
    if (Firebase.ready()){
      database();
    }
    prevMillis = millis();
    updateLCD();
    Serial.print("Set Voltage   : ");
    Serial.printf("%.2f V \n", setVoltage);
    Serial.print("Set Current   : ");
    Serial.printf("%.2f A \n", setCurrent );
    Serial.print("Voltage       : ");
    Serial.printf("%.2f V \n", voltage);
    Serial.print("Current       : ");
    Serial.printf("%.2f A \n", current);
    Serial.print('\n');
    Serial.print("wirte val");Serial.println(writeVal);
  }
}

void database(){
  String a, b;
  a = Firebase.RTDB.getString(&fdbo, "/PSDK-UAS/setVolt") ? String(fdbo.to<const char *>()) : "0";
  a = a.substring(2, a.length()-2);
  setVoltage = a.toFloat();
  b = Firebase.RTDB.getString(&fdbo, "/PSDK-UAS/setCurr") ? String(fdbo.to<const char *>()) : "0";
  b = b.substring(2, b.length()-2);
  setCurrent = b.toFloat();
  Firebase.RTDB.setFloat(&fdbo, "/PSDK-UAS/voltage", voltage);
  Firebase.RTDB.setFloat(&fdbo, "/PSDK-UAS/current", current);
}

void printLCD(){
  lcd.setCursor(1, 0);
  lcd.print("V:");
  lcd.setCursor(9, 0);
  lcd.print("V:");
  lcd.setCursor(1, 1);
  lcd.print("I:");
  lcd.setCursor(9, 1);
  lcd.print("I:");
}

void updateLCD(){
  lcd.setCursor(3, 0);
  lcd.print("    ");
  lcd.setCursor(3, 0);
  lcd.print(setVoltage);

  lcd.setCursor(11, 0);
  lcd.print("    ");
  lcd.setCursor(11, 0);
  lcd.print(voltage);

  lcd.setCursor(3, 1);
  lcd.print("    ");
  lcd.setCursor(3, 1);
  lcd.print(setCurrent);

  lcd.setCursor(11, 1);
  lcd.print("    ");
  lcd.setCursor(11, 1);
  lcd.print(current);
}
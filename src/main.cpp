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
float voltage, current, setVoltage=3, setCurrent=1;
unsigned long prevMillis;

void setup(){
  WiFi.begin("12345", "tahutempe");
  config.api_key = "AIzaSyAiuX8LjDc7eWa1g9MeCJDmDLJc_qKLGLw";
  config.database_url = "https://psdk-uas-87803-default-rtdb.asia-southeast1.firebasedatabase.app/";
  auth.user.email = 'razhterize@gmail.com';
  auth.user.password = 'visualstudio';
  MCP.begin();
  lcd.init();
  lcd.backlight();
  printLCD();
}

void loop(){
  while (Firebase.ready()){
    database();
  }
  if (millis() - prevMillis >= 200){
    updateLCD();
    Serial.print("Set Voltage   : ");
    Serial.println(setVoltage);
    Serial.print("Set Current   : ");
    Serial.println(setCurrent);
    Serial.print("Voltage       : ");
    Serial.println(voltage);
    Serial.print("Current       : ");
    Serial.println(current);
  }
}

void database(){
  Firebase.RTDB.getFloat(&fdbo, "/PSU/setVolt", setVoltage) ? String(fdbo.to<float>()) : fdbo.errorReason().c_str();
  Firebase.RTDB.getFloat(&fdbo, "/PSU/SetCurrent", setCurrent) ? String(fdbo.to<float>()) : fdbo.errorReason().c_str();
  Firebase.RTDB.setFloat(&fdbo, "/PSU/voltage", voltage);
  Firebase.RTDB.setFloat(&fdbo, "/PSU/current", current);
  
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
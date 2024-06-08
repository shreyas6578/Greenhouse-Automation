#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Pin Definitions
#define DHT11_PIN 2
#define SOIL_MOISTURE_PIN A0
#define GAS_SENSOR_PIN A3
#define LDR_PIN A2

// DHT Sensor
DHT dht(DHT11_PIN, DHT11);

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables
float temp;
float humd;
int sensorValue;
float RLDR;
float Vout;
float Lux;
int sensorValue1;
int moistureLevel1;
float ppm;

String cdata;
String myString;
char rdata;

// Function Prototypes
void myTimerEvent();
String getValue(String data, char separator, int index);

void setup() {
    lcd.init();
    lcd.backlight();
    
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(GAS_SENSOR_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);

    Serial.begin(9600);
    
    dht.begin();

    Blynk.begin(auth, ssid, pass);
    timer.setInterval(5000L, myTimerEvent);
    
    Serial.println("Program started");
}

void loop() {
    Blynk.run();
    timer.run();

    if (Serial.available() > 0) {
        rdata = Serial.read();
        myString += rdata;
        Serial.print(rdata);
    }

    // Gas Sensor Reading
    sensorValue = analogRead(GAS_SENSOR_PIN);
    ppm = (float)0.714 * pow(10, (1.7 * (float)sensorValue / 1024) - 1);
    ppm = ppm * 100;

    Serial.print("Gas concentration: ");
    Serial.print(ppm);
    Serial.println(" PPM");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CNG Value");
    lcd.setCursor(0, 1);
    lcd.print(ppm);
    lcd.print(" PPM");
    delay(1000);

    // Soil Moisture Sensor Reading
    sensorValue1 = analogRead(SOIL_MOISTURE_PIN);
    moistureLevel1 = map(sensorValue1, 0, 1023, 0, 100);
    
    Serial.print("Moisture level: ");
    Serial.print(moistureLevel1);
    Serial.println("%");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil level ");
    lcd.setCursor(0, 1);
    lcd.print(moistureLevel1);
    lcd.print("%");
    delay(1000);

    // Light Sensor Reading
    sensorValue = analogRead(LDR_PIN);
    Vout = sensorValue * 0.0048828125;
    RLDR = (10000.0 * (5.0 - Vout)) / Vout;
    Lux = RLDR / 500;

    Serial.print("LIGHT: ");
    Serial.println(Lux);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LIGHT: ");
    lcd.print(Lux);
    lcd.setCursor(0, 1);
    if (Lux >= 0 && Lux <= 5) {
        lcd.print("DARK");
    } else if (Lux > 5 && Lux <= 14) {
        lcd.print("DIM");
    } else if (Lux > 14 && Lux <= 50) {
        lcd.print("BRIGHT");
    } else {
        lcd.print("VERY BRIGHT");
    }
    delay(1000);

    // Temperature and Humidity Sensor Reading
    temp = dht.readTemperature();
    humd = dht.readHumidity();

    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" Humi: ");
    Serial.println(humd);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");
    lcd.print(temp);
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humd);

    // Concatenate sensor data
    cdata = String(temp) + "," + String(humd) + "," + String(ppm) + "," + String(moistureLevel1) + "," + String(Lux);
    Serial.println(cdata);

    // Send data to NodeMCU (if used)
    // nodemcu.println(cdata);
    delay(2000);
}

void myTimerEvent() {
    // Custom event to be called every 5 seconds
}

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


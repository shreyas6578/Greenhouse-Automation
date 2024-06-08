#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Blynk credentials
char auth[] = "YourAuthToken";
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";
BlynkTimer timer;

// Variables
float firstVal, secondVal, thirdVal, soil, light;
String myString;

// Function Prototypes
String getValue(String data, char separator, int index);

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(5000L, myTimerEvent);
}

void loop() {
    Blynk.run();
    timer.run();

    if (Serial.available() > 0) {
        char rdata = Serial.read();
        myString += rdata;
        Serial.print(rdata);
    }

    if (myString.indexOf('\n') > 0) {
        String first = getValue(myString, ',', 0);
        String second = getValue(myString, ',', 1);
        String third = getValue(myString, ',', 2);
        String soilStr = getValue(myString, ',', 3);
        String lightStr = getValue(myString, ',', 4);

        firstVal = first.toFloat();
        secondVal = second.toFloat();
        thirdVal = third.toFloat();
        soil = soilStr.toFloat();
        light = lightStr.toFloat();

        Serial.print("First Value: ");
        Serial.println(firstVal);
        Serial.print("Second Value: ");
        Serial.println(secondVal);
        Serial.print("Third Value: ");
        Serial.println(thirdVal);
        Serial.print("Soil: ");
        Serial.println(soil);
        Serial.print("Light: ");
        Serial.println(light);

        myString = "";
    }
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

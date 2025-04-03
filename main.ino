
// ✅ **Blynk Credentials**
#define BLYNK_TEMPLATE_ID "blank_replace with yours"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "demo"

#define BLYNK_PRINT Serial  // Debugging Blynk

// ✅ **Include Required Libraries**
#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

// ✅ **Blynk Setup**
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "wifi ssid or name";
char pass[] = "wifi password";

// ✅ **Component Initialization**
MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial neogps(1);  // Serial1 for GPS module

// ✅ **Pin Definitions**
#define GPS_RX 16  // ESP32 RX (Connect to GPS TX)
#define GPS_TX 17  // ESP32 TX (Connect to GPS RX)
#define MPU_SDA 21 // I2C SDA
#define MPU_SCL 22 // I2C SCL

// ✅ **Step Detection Variables**
int stepCount = 0;
long lastStepTime = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin(MPU_SDA, MPU_SCL);  // Initialize I2C for MPU6050
    neogps.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);  // Initialize GPS

    Blynk.begin(auth, ssid, pass);

    // **Check MPU6050 Connection**
    mpu.initialize();
    if (!mpu.testConnection()) {
        Serial.println("❌ MPU6050 connection failed!");
        while (1);
    }
    Serial.println("✅ MPU6050 connected!");
}

void loop() {
    Blynk.run();
    
    detectActivity();
    getGPSLocation();
    float temp = getMPU6050Temperature();

    // ✅ **Send Data to Blynk**
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V2, stepCount);

    Serial.print("🌡 MPU6050 Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");

    delay(1000);
}

// ✅ **Detect Walking, Running, or Standing Still**
void detectActivity() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float accelTotal = sqrt(ax * ax + ay * ay + az * az) / 16384.0;
    long currentTime = millis();
    String activity;

    if (accelTotal > 1.2 && accelTotal <= 2.0) {
        activity = "🚶 Walking";
    } else if (accelTotal > 2.0) {
        activity = "🏃 Running";
    } else {
        activity = "🛑 Standing Still";
    }

    if (accelTotal > 1.2 && (currentTime - lastStepTime > 300)) {
        stepCount++;
        lastStepTime = currentTime;
    }

    Serial.print("Activity: "); Serial.println(activity);
    Serial.print("Step Count: "); Serial.println(stepCount);

    Blynk.virtualWrite(V7, activity);
}

// ✅ **Fetch GPS Location & Get Address**
void getGPSLocation() {
    long startTime = millis();
    while (millis() - startTime < 2000) {  // Wait for fresh GPS data
        if (neogps.available()) {
            gps.encode(neogps.read());
        }
    }

    if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();

        Serial.print("📍 Live GPS Location: ");
        Serial.print(latitude, 6);
        Serial.print(", ");
        Serial.println(longitude, 6);

        Blynk.virtualWrite(V3, latitude);
        Blynk.virtualWrite(V4, longitude);

        String mapsLink = "https://www.google.com/maps/search/?api=1&query=" + String(latitude, 6) + "," + String(longitude, 6);
        Blynk.virtualWrite(V6, mapsLink);

        // ✅ **Fetch Location Name from OpenStreetMap API**
        fetchLocationName(latitude, longitude);
    } else {
        Serial.println("❌ No valid GPS data received.");
    }
}

// ✅ **Fetch Location Name from API**
void fetchLocationName(float lat, float lon) {
    WiFiClient client;
    HTTPClient http;
    String url = "http://api.positionstack.com/v1/reverse?access_key=YOUR_ACCESS_KEY&query=" + String(lat, 6) + "," + String(lon, 6);
    
    Serial.println("🌍 Fetching location name...");
    
    http.begin(client, url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("📜 API Response: " + payload);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        if (doc.containsKey("data") && doc["data"][0]["label"] != nullptr) {
            String locationName = doc["data"][0]["label"].as<String>();
            Serial.println("📍 Location: " + locationName);
            Blynk.virtualWrite(V5, locationName);
        } else {
            Serial.println("❌ No valid location name found.");
        }
    } else {
        Serial.println("❌ Failed to fetch location name!");
    }

    http.end();
}

// ✅ **Read Temperature from MPU6050**
float getMPU6050Temperature() {
    int16_t rawTemp = mpu.getTemperature();
    float temperature = (rawTemp / 340.0) + 36.53;  
    return temperature;
}


























-----------------void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

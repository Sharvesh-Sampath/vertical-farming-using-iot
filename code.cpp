#include <WiFi.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Define pin connections
#define DHT_PIN 13             // Pin for temperature & humidity sensor
#define SOIL_MOISTURE_PIN 34   // Analog pin for soil moisture sensor
#define WATER_LEVEL_PIN 35     // Analog pin for water level sensor
#define PUMP_PIN 26            // Pin for water pump relay
#define LIGHT_PIN 27           // Pin for LED grow light

// Define thresholds
const int MOISTURE_THRESHOLD = 30;   // Moisture level threshold to trigger the pump
const int WATER_LOW_THRESHOLD = 300; // Low water level threshold (analog value)

// DHT sensor setup
#define DHTTYPE DHT22          // DHT 22 (AM2302) sensor
DHT dht(DHT_PIN, DHTTYPE);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi and Arduino Cloud variables
const char WIFI_SSID[] = "your_wifi_ssid";
const char WIFI_PASSWORD[] = "your_wifi_password";

String thing_id = "your_thing_id";  // Replace with your Thing ID
String device_id = "your_device_id"; // Replace with your Device ID

// Cloud variables
float temperature;
float humidity;
int soilMoisture;
int waterLevel;
bool pumpState = false;
bool lightState = false;

void setup() {
  // Initialize serial, LCD, DHT sensor
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  dht.begin();

  // Initialize pump and light pins
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  
  // Connect to WiFi and Arduino IoT Cloud
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ArduinoCloud.begin(thing_id, device_id, WIFI_SSID, WIFI_PASSWORD);
  
  // Set initial states
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(LIGHT_PIN, LOW);
}

void loop() {
  ArduinoCloud.update(); // Keep the cloud connection active
  
  // Read sensors
  readSensors();
  
  // Display data on LCD
  displayLCD();
  
  // Control pump based on soil moisture
  controlPump();
  
  // Control light based on cloud input
  controlLight();
  
  delay(1000);
}

// Function to read sensor data
void readSensors() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  waterLevel = analogRead(WATER_LEVEL_PIN);
  
  // Debug prints
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("Soil Moisture: "); Serial.println(soilMoisture);
  Serial.print("Water Level: "); Serial.println(waterLevel);
}

// Function to control water pump
void controlPump() {
  if (soilMoisture < MOISTURE_THRESHOLD && waterLevel > WATER_LOW_THRESHOLD) {
    pumpState = true;
    digitalWrite(PUMP_PIN, HIGH);  // Turn on pump
  } else {
    pumpState = false;
    digitalWrite(PUMP_PIN, LOW);   // Turn off pump
  }
}

// Function to control LED light
void controlLight() {
  if (lightState) {
    digitalWrite(LIGHT_PIN, HIGH);  // Turn on light
  } else {
    digitalWrite(LIGHT_PIN, LOW);   // Turn off light
  }
}

// Function to display data on LCD
void displayLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");
  
  lcd.setCursor(0, 1);
  lcd.print("Soil: ");
  lcd.print(soilMoisture);
  lcd.print(" ");
  lcd.print("Pump:");
  lcd.print(pumpState ? "ON" : "OFF");
}

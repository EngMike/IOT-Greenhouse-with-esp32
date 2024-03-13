#define BLYNK_TEMPLATE_ID "*************"
#define BLYNK_TEMPLATE_NAME "*************"
#define BLYNK_AUTH_TOKEN "*************"
#include <Wire.h>
#include <Blynk.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>  // Include the DHT library
#include <LiquidCrystal_I2C.h>

#define REPORTING_PERIOD_MS 1000  // frequency of updates sent to Blynk app in ms

#define DHTPIN 4        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

const int LIGHT_SENSOR_PIN = 36;
const int RELAY_PIN = 33;
const int ANALOG_THRESHOLD = 500;

#define RELAY_PIN_PUMP 32
#define MOISTURE_PIN 34
#define THRESHOLD 1000 // CHANGE YOUR THRESHOLD HERE


char auth[] = "*************";  // You should get Auth Token in the Blynk App.
char ssid[] = "*************";     // Your WiFi credentials.
char pass[] = "*************";



void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Initialize DHT sensor
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN_PUMP, OUTPUT);

}

void loop() {
  // LCD Readings
  float humi = dht.readHumidity();    // read humidity
  float tempC = dht.readTemperature(); // read temperature

  lcd.clear();
  // check if any reads failed
  if (isnan(humi) || isnan(tempC)) {
    lcd.setCursor(0, 0);
    lcd.print("Failed");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(tempC);
    lcd.print((char)223);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Umid: ");
    lcd.print(humi);
    lcd.print("%");
  }

  // Blynk Readings
  Blynk.run();
  readDHTSensor();

  delay(REPORTING_PERIOD_MS);  // Add a delay to control the reporting frequency

 // timer.run();

// LIGHT SENSOR WITH RELAY
  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  // Send analog value to Blynk app
  Blynk.virtualWrite(V3, analogValue);

  if (analogValue < ANALOG_THRESHOLD) {
    // Turn on Relay
    digitalWrite(RELAY_PIN, LOW);
    // Send relay state to Blynk app
    Blynk.virtualWrite(V4, 1);
  } else {
    // Turn off Relay
    digitalWrite(RELAY_PIN, HIGH);
    // Send relay state to Blynk app
    Blynk.virtualWrite(V4, 0);
  }

// SOIL MOISTURE WITH WATER PUMP
int value = analogRead(MOISTURE_PIN); // read the analog value from sensor

  if (value < THRESHOLD) {
    Serial.print("The soil is DRY => turn pump ON");
    digitalWrite(RELAY_PIN_PUMP, HIGH);
  } else {
    Serial.print("The soil is WET => turn pump OFF");
    digitalWrite(RELAY_PIN_PUMP, LOW);
  }

  Serial.print(" (");
  Serial.print(value);
  Serial.println(")");
  Blynk.virtualWrite(V5, value);
  delay(200);

} // end loop

void readDHTSensor() {
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Display sensor data on Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C\tHumidity: ");
  Serial.print(humidity);
  Serial.println("%");

  // Send sensor data to Blynk app
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
}






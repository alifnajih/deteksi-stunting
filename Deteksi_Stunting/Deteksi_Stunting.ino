#define BLYNK_TEMPLATE_ID "TMPL6p6cta2Av"
#define BLYNK_TEMPLATE_NAME "MONITORING STUNTING"
#define BLYNK_AUTH_TOKEN "80s19x0RH829-t6pRzUlQyzAUN5IGK6Q"
  
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// Blynk credentials
char ssid[] = "aku";
char pass[] = "12345678910";

// HX711 load cell pins
const int LOADCELL_DOUT_PIN = D3;
const int LOADCELL_SCK_PIN = D4;

// Ultrasonic sensor pins
const int TRIG_PIN = D5;
const int ECHO_PIN = D6;

// LCD I2C address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize HX711
HX711 scale;

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.init();
  lcd.backlight();

  // Initialize HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();
  
  // Initialize ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  Serial.println("Setup completed.");
}

void loop() {
  Blynk.run();

  // Measure weight
  float weight = scale.get_units(10);
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" grams");

  // Measure height
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;
  Serial.print("Height: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight: ");
  lcd.print(weight);
  lcd.print("g");
  lcd.setCursor(0, 1);
  lcd.print("Height: ");
  lcd.print(distance);
  lcd.print("cm");

  // Send data to Blynk
  Blynk.virtualWrite(V0, weight);
  Blynk.virtualWrite(V1, distance);

  // Define standard ranges (example values)
  float minWeight = 8000; // 8 kg
  float maxWeight = 25000; // 25 kg
  float minHeight = 65; // 65 cm
  float maxHeight = 120; // 120 cm

  // Check if values are within standard ranges
  if (weight < minWeight || weight > maxWeight || distance < minHeight || distance > maxHeight) {
    String alert = "Alert: Check Child's Health!";
    Blynk.logEvent(alert);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alert: Check");
    lcd.setCursor(0, 1);
    lcd.print("Child's Health!");
  }

  delay(2000); // Delay before next reading
}

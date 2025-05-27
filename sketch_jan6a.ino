// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6s3DGFHR-"
#define BLYNK_TEMPLATE_NAME "hello"
#define BLYNK_AUTH_TOKEN "kPNcsXsgqBQ1ADqLhqv4s1L6jf9oOUfO"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions
#define LDR1_PIN 4   // LDR1 connected to D2
#define LDR2_PIN 5   // LDR2 connected to D1
#define ACS712_PIN A0 // ACS712 current sensor on A0
#define DS18B20_PIN 12 // DS18B20 connected to D5
#define SERVO_PIN 14   // Servo connected to D6

// WiFi credentials
const char* ssid = "Hotspot1234";
const char* password = "12345678";

// Setup OneWire and DallasTemperature for DS18B20
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

// Create Servo object
Servo myServo;

// Variables for readings
int ldr1State = 0;  // State of LDR1 (0 or 1)
int ldr2State = 0;  // State of LDR2 (0 or 1)
float temperature = 0.0;
float current = 0.0;

// Servo reset position (default)
int servoResetPosition = 90;

void setup() {
  // Start Serial Monitor for debugging
  Serial.begin(115200);
  
  // Start WiFi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Setup LDR pins as inputs (Digital Pins)
  pinMode(LDR1_PIN, INPUT);
  pinMode(LDR2_PIN, INPUT);

  // Setup ACS712 pin (A0)
  pinMode(ACS712_PIN, INPUT);

  // Setup Servo
  myServo.attach(SERVO_PIN);
  myServo.write(servoResetPosition);  // Set the initial position of the servo

  // Initialize DS18B20 sensor
  sensors.begin();

  // Set up Blynk to listen for the reset command (Virtual Pin V3 for servo reset)
  Blynk.virtualWrite(V3, 0); // Initial state for reset button
}

void loop() {
  // Read LDR values (Digital sensors)
  ldr1State = digitalRead(LDR1_PIN);
  ldr2State = digitalRead(LDR2_PIN);

  // Print LDR states to Serial Monitor for debugging
  Serial.print("LDR1 State: ");
  Serial.print(ldr1State);
  Serial.print(" | LDR2 State: ");
  Serial.println(ldr2State);

  // Move servo towards the LDR with higher light value (assuming HIGH means more light)
  if (ldr1State == HIGH) {
    myServo.write(0);  // Move servo to the left (towards LDR1)
  } else if (ldr2State == HIGH) {
    myServo.write(180); // Move servo to the right (towards LDR2)
  } else {
    myServo.write(servoResetPosition);  // Stay at the reset position if no light detected
  }

  // Read temperature from DS18B20
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  // Read current from ACS712
  current = readCurrent();

  // Print temperature and current to the Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, ");

  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" A");

  // Send data to Blynk
  Blynk.virtualWrite(V1, temperature);  // Send temperature to Blynk Virtual Pin 1
  Blynk.virtualWrite(V4, current);      // Send current to Blynk Virtual Pin 2

  // Check if the reset button was pressed in Blynk (via Virtual Pin V3)
  Blynk.run();

  delay(1000); // Delay 1 second before next reading
}

// Function to read current from ACS712 sensor
float readCurrent() {
  // Read analog value from ACS712 (0-1023 range)
  int sensorValue = analogRead(ACS712_PIN);
  
  // Convert the value to voltage (since ACS712 gives 0-5V corresponding to -5A to 5A)
  float voltage = sensorValue * (5.0 / 1023.0);

  // The ACS712 outputs 2.5V when current is 0. To get the current, subtract the reference (2.5V)
  voltage = voltage - 2.5;

  // The ACS712 has a sensitivity of 185mV per ampere. So, divide by 0.185 to get the current in amperes.
  float currentAmps = voltage / 0.185;

  return currentAmps;
}

// This function will be called from Blynk when the reset button is pressed
BLYNK_WRITE(V3) {
  int resetButtonState = param.asInt(); // Read the button state (0 or 1)

  if (resetButtonState == 1) {
    // Reset servo to the initial position
    myServo.write(servoResetPosition); // Reset to 90 degrees (or the desired position)
    Serial.println("Servo reset to initial position!");
  }
}

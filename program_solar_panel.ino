#include <Servo.h>

#define LDR_KIRI A0       // Pin LDR kiri
#define LDR_KANAN A1      // Pin LDR kanan
#define ACS712_PIN A2     // Pin sensor arus ACS712
#define SERVO_PIN 9       // Pin servo

Servo servo;
int posisiServo = 90;       // Posisi awal servo (netral)
int posisiTarget = 90;      // Target posisi servo

void setup() {
  servo.attach(SERVO_PIN);
  servo.write(posisiServo); // Set posisi awal servo ke 90 derajat
  Serial.begin(9600);
}

void loop() {
  int nilaiLDRKiri = analogRead(LDR_KIRI);
  int nilaiLDRKanan = analogRead(LDR_KANAN);
  int nilaiArus = analogRead(ACS712_PIN);
  float arus = -(nilaiArus - 512) * 5.0 / 1023.0; // Konversi nilai ADC ke arus

  int selisih = nilaiLDRKanan - nilaiLDRKiri;
  int posisiBaru = map(selisih, -1023, 1023, 0, 180);
  posisiTarget = constrain(posisiBaru, 0, 180);

  if (posisiServo < posisiTarget) {
    posisiServo++;
  } else if (posisiServo > posisiTarget) {
    posisiServo--;
  }
  servo.write(posisiServo);

  // Debugging output
  Serial.print("LDR Kiri: "); Serial.print(nilaiLDRKiri);
  Serial.print(" | LDR Kanan: "); Serial.print(nilaiLDRKanan);
  Serial.print(" | Posisi Servo: "); Serial.print(posisiServo);
  Serial.print(" | Arus: "); Serial.print(arus, 2);
  Serial.println(" A");

  delay(20); // Delay untuk memperlambat pergerakan servo
}

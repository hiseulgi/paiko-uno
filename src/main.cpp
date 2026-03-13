#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// =============================================
// KONFIGURASI
// =============================================
#define SERVO_PIN     9
#define SERVO_OPEN    0    // sudut servo saat membuka (sesuaikan)
#define SERVO_CLOSE   180     // sudut servo saat menutup (sesuaikan)
#define FEEDING_DELAY 100  // durasi servo terbuka (ms)

// Interval pemberian pakan
// Untuk testing: ganti FEED_HOUR=0, FEED_MIN=1, FEED_SEC=0 (tiap 1 menit)
#define FEED_HOUR 23
#define FEED_MIN  59
#define FEED_SEC  59
 
// =============================================
// OBJEK
// =============================================
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Coba 0x3F jika 0x27 tidak jalan
Servo feederServo;
 
// =============================================
// VARIABEL
// Hanya countdown — satu sumber kebenaran.
// Nilai max: cdSec=59, cdMin=59, cdHour=23 → selalu kecil, aman selamanya.
// =============================================
unsigned long prevMillis = 0;
 
uint8_t cdHour = FEED_HOUR;  // 0–23
uint8_t cdMin  = FEED_MIN;   // 0–59
uint8_t cdSec  = FEED_SEC;   // 0–59
 
// =============================================
// FUNGSI
// =============================================
 
void printTwoDigit(uint8_t val) {
    if (val < 10) lcd.print('0');
    lcd.print(val);
}
 
void updateLCD() {
    lcd.setCursor(0, 0);
    lcd.print("Next Feed In:   ");
    lcd.setCursor(0, 1);
    printTwoDigit(cdHour);
    lcd.print(':');
    printTwoDigit(cdMin);
    lcd.print(':');
    printTwoDigit(cdSec);
    lcd.print("     ");
}
 
void showFeedingMessage() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Memberi Pakan ");
    lcd.setCursor(0, 1);
    lcd.print("  Tunggu...     ");
}
 
void resetCountdown() {
    cdHour = FEED_HOUR;
    cdMin  = FEED_MIN;
    cdSec  = FEED_SEC;
}
 
void feedFish() {
    showFeedingMessage();
 
    feederServo.write(SERVO_OPEN);
    delay(FEEDING_DELAY);
    feederServo.write(SERVO_CLOSE);
    delay(500);
 
    resetCountdown();
    updateLCD();
 
    Serial.println(">> Pakan diberikan! Countdown direset.");
}
 
// Turunkan countdown 1 detik.
// Jika sudah 00:00:00 saat fungsi dipanggil → saatnya feed.
void tickCountdown() {
    // Cek apakah countdown sudah habis
    if (cdHour == 0 && cdMin == 0 && cdSec == 0) {
        feedFish();
        return;
    }
 
    // Kurangi 1 detik dengan cascading
    if (cdSec > 0) {
        cdSec--;
    } else {
        cdSec = 59;
        if (cdMin > 0) {
            cdMin--;
        } else {
            cdMin = 59;
            cdHour--;  // aman karena sudah dicek cdHour > 0 di atas
        }
    }
 
    updateLCD();
 
    // Debug Serial Monitor
    Serial.print("CD: ");
    Serial.print(cdHour); Serial.print(":");
    Serial.print(cdMin);  Serial.print(":");
    Serial.println(cdSec);
}
 
// =============================================
// SETUP
// =============================================
void setup() {
    Serial.begin(9600);
 
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Auto Feeder   ");
    lcd.setCursor(0, 1);
    lcd.print("  Initializing..");
    delay(2000);
 
    feederServo.attach(SERVO_PIN);
    feederServo.write(SERVO_CLOSE);
 
    prevMillis = millis();
    updateLCD();
 
    Serial.println("Auto Fish Feeder Started");
}
 
// =============================================
// LOOP
// =============================================
void loop() {
    unsigned long currentMillis = millis();
 
    if (currentMillis - prevMillis >= 1000UL) {
        prevMillis += 1000UL;
        tickCountdown();
    }
}
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// =============================================
// KONFIGURASI
// =============================================
#define SERVO_PIN     9
#define SERVO_OPEN    90    // sudut servo saat membuka (sesuaikan)
#define SERVO_CLOSE   0     // sudut servo saat menutup (sesuaikan)
#define FEEDING_DELAY 1000  // durasi servo terbuka (ms)

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
// VARIABEL TIMER
// Nilai max: timerSec=59, timerMin=59, timerHour=23
// Tidak ada variabel yang terus naik tanpa batas!
// =============================================
unsigned long prevMillis = 0;  // hanya 1 unsigned long untuk millis

uint8_t timerSec  = 0;   // 0-59, reset ke 0 tiap 60 detik
uint8_t timerMin  = 0;   // 0-59, reset ke 0 tiap 60 menit
uint8_t timerHour = 0;   // 0-23, reset ke 0 tiap 24 jam (trigger pakan)

// Countdown mundur ke LCD
uint8_t cdSec  = FEED_SEC;
uint8_t cdMin  = FEED_MIN;
uint8_t cdHour = FEED_HOUR;

// =============================================
// FUNGSI HELPER
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

void resetTimer() {
    timerSec  = 0;
    timerMin  = 0;
    timerHour = 0;
    cdSec     = FEED_SEC;
    cdMin     = FEED_MIN;
    cdHour    = FEED_HOUR;
}

void feedFish() {
    showFeedingMessage();
    feederServo.write(SERVO_OPEN);
    delay(FEEDING_DELAY);
    feederServo.write(SERVO_CLOSE);
    delay(500);

    resetTimer();
    updateLCD();

    Serial.println(">> Pakan diberikan! Timer direset.");
}

// Countdown turun 1 detik: HH:MM:SS turun hingga 00:00:00
void tickCountdown() {
    if (cdSec > 0) {
        cdSec--;
    } else {
        cdSec = 59;
        if (cdMin > 0) {
            cdMin--;
        } else {
            cdMin = 59;
            if (cdHour > 0) {
                cdHour--;
            }
        }
    }
}

// Timer naik 1 detik dengan cascading reset
// detik 59 -> reset 0, naik menit
// menit 59 -> reset 0, naik jam
// jam   23 -> reset 0 = 24 jam tercapai -> beri pakan
void tickTimer() {
    timerSec++;

    if (timerSec >= 60) {
        timerSec = 0;
        timerMin++;

        if (timerMin >= 60) {
            timerMin = 0;
            timerHour++;

            if (timerHour >= 24) {
                feedFish();  // reset semua di dalam feedFish()
                return;
            }
        }
    }

    tickCountdown();
    updateLCD();

    // Debug ke Serial Monitor
    Serial.print("Timer: ");
    Serial.print(timerHour); Serial.print("h ");
    Serial.print(timerMin);  Serial.print("m ");
    Serial.print(timerSec);  Serial.print("s  |  CD: ");
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

    // Tick setiap 1000ms, pakai += agar tidak drift
    if (currentMillis - prevMillis >= 1000UL) {
        prevMillis += 1000UL;
        tickTimer();
    }
}
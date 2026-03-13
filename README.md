# 🐟 paiko-uno - Automatic Fish Feeder

An Arduino-based automatic fish feeder that dispenses food every 24 hours using a servo motor, with a live countdown displayed on a 16x2 I2C LCD.

## Features

- 24-hour countdown timer (no RTC module needed)
- Live countdown display on LCD in `HH:MM:SS` format
- Servo-driven feeding mechanism
- Memory-safe: all timer variables are bounded (`uint8_t`, max 59/23) — safe for indefinite 24/7 operation
- Non-blocking timer using `millis()` — no `delay()` in the main loop

## Hardware

| Component | Details |
|---|---|
| Microcontroller | Arduino Uno |
| Display | 16x2 LCD with I2C backpack (address `0x27` or `0x3F`) |
| Actuator | Servo motor |
| Mechanism | Feeder drum/disc connected to servo horn |

### Wiring

| Module | Arduino Pin |
|---|---|
| LCD SDA | A4 |
| LCD SCL | A5 |
| LCD VCC | 5V |
| LCD GND | GND |
| Servo Signal | D9 |
| Servo VCC | 5V |
| Servo GND | GND |

## Software

Built with [PlatformIO](https://platformio.org/). Libraries used:

- `marcoschwartz/LiquidCrystal_I2C`
- `arduino-libraries/Servo`

## Getting Started

1. Clone this repository:
   ```bash
   git clone git@github.com:hiseulgi/paiko-uno.git
   ```

2. Open with PlatformIO (VS Code extension or CLI).

3. Adjust the configuration in `src/main.cpp`:
   ```cpp
   #define SERVO_OPEN    90    // servo angle when open (adjust to your mechanism)
   #define SERVO_CLOSE   0     // servo angle when closed
   #define FEEDING_DELAY 1000  // how long the servo stays open (ms)

   #define FEED_HOUR 23        // feeding interval (23h 59m 59s ≈ 24h)
   #define FEED_MIN  59
   #define FEED_SEC  59
   ```

4. Upload to Arduino Uno via PlatformIO.

## Testing

To test quickly without waiting 24 hours, change the feed interval to 1 minute:

```cpp
#define FEED_HOUR 0
#define FEED_MIN  1
#define FEED_SEC  0
```

Open the Serial Monitor at **9600 baud** to watch the timer and countdown tick in real time.

## How It Works

```
Every 1000ms (via millis):
  timerSec++
    if timerSec >= 60  → reset to 0, timerMin++
      if timerMin >= 60  → reset to 0, timerHour++
        if timerHour >= 24 → FEED FISH, reset all
  
  countdown ticks down in parallel (HH:MM:SS → 00:00:00)
  LCD updates every second
```

No large accumulating variables — the timer is based on cascading counters, so all values stay small and bounded forever.

## Project Structure

```
auto-fish-feeder/
├── platformio.ini
├── README.md
└── src/
    └── main.cpp
```

## License

MIT
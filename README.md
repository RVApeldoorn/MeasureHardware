# MeasureHardware

This repository contains Arduino sketches for distance measurement with a Flutter app (MeasureApp) using BLE or USB serial. The app sends a start command, prompting the hardware to average multiple measurements and the send the result.

## Requirements

### Hardware
- **BLE Setup**: XIAO ESP32-C6, VL53L1X ToF sensor.
- **Serial Setup**: Arduino, HC-SR04 ultrasonic sensor.

## Hardware Connections

### BLE VL53L1X (ESP32-C6)
| VL53L1X Pin | ESP32-C6 Pin |
|-------------|--------------|
| VCC         | 3.3V         |
| GND         | GND          |
| SDA         | D4 (GPIO6)   |
| SCL         | D5 (GPIO7)   |

### Serial HC-SR04
| HC-SR04 Pin | Arduino Pin |
|-------------|-------------|
| VCC         | 5V          |
| GND         | GND         |
| Trig        | 9           |
| Echo        | 10          |

## How It Works

### 1. BLE VL53L1X (`ble_vl53l1x_esp32c6.ino`)
- **Service**: `BleMeasurementService`.
- **Communication**: BLE with UUIDs:
  - Service: `12345678-1234-5678-1234-56789abcdef0`
  - Notify: `12345678-1234-5678-1234-56789abcdef1` (sends mm digits, e.g., `500`)
  - Write: `12345678-1234-5678-1234-56789abcdef2` (receives `start`)
- **Workflow**: ESP32-C6 starts BLE server, app sends `start`, averages 10 VL53L1X measurements, sends mm digits.
- **Run**: `flutter run -d <device-id> --dart-define=MEASUREMENT_SERVICE=ble`.

### 2. Serial HC-SR04 (`serial_hcsr04.ino`)
- **Service**: `SerialMeasurementService`.
- **Communication**: USB serial (9600 baud).
- **Workflow**: Arduino waits for `start`, averages 10 HC-SR04 measurements (2–400 cm), sends mm digits.
- **Run**: `flutter run -d <device-id> --dart-define=MEASUREMENT_SERVICE=serial`.

### 3. Mock Service
- **Service**: `MockMeasurementService`.
- **Communication**: Simulated (no hardware).
- **Workflow**: Alternates sending reference and height measurements in mm.
- **Run**: `flutter run -d <device-id> --dart-define=MEASUREMENT_SERVICE=mock`.
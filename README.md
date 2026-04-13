# ESP32 Game Controller
<img width="1024" height="1536" alt="ChatGPT Image Apr 13, 2026, 09_46_19 PM" src="https://github.com/user-attachments/assets/9f024c68-d2ca-4601-a28c-96bae382e74a" />


A custom ESP32-based BLE game controller with TFT display, analog sticks, switches, and OTA update capability.

## Features

- **BLE Gamepad**: Acts as a Bluetooth Low Energy game controller compatible with Windows, Android, and other BLE HID‑supported devices.
- **TFT Display**: Real‑time visualization of all axes, switches, battery level, and connection status.
- **6 Analog Axes**:
  - Throttle, Yaw, Pitch, Roll (for flight‑sim style control)
  - Two auxiliary potentiometers (Pot1, Pot2)
- **7 Digital Inputs**:
  - ARM, MODE, SWA, SWB toggle switches
  - Three momentary buttons (BT1, BT2, BT3)
- **Battery Monitoring**: Reads battery voltage via ADC, displays smoothed voltage and percentage.
- **Over‑The‑Air (OTA) Updates**: Dedicated OTA button triggers Wi‑Fi mode for wireless firmware upload via Arduino IDE.
- **Wi‑Fi Configuration**: Connects to a predefined Wi‑Fi network when OTA is activated.

## Hardware Requirements

- ESP32 Dev Module (or any ESP32 with sufficient GPIOs)
- TFT Display (ILI9341 or compatible, driven by TFT_eSPI)
- 4x Analog Joysticks/Potentiometers (for Throttle, Yaw, Pitch, Roll)
- 2x Additional Potentiometers (Pot1, Pot2)
- 4x Toggle Switches (ARM, MODE, SWA, SWB)
- 3x Push Buttons (BT1, BT2, BT3)
- 1x OTA Button
- Battery voltage divider (connected to GPIO14)
- Appropriate pull‑up/pull‑down resistors as needed

## Pin Mapping

| Function   | GPIO | Notes                          |
|------------|------|--------------------------------|
| Throttle   | 34   | Analog input (ADC1_CH6)        |
| Yaw        | 35   | Analog input (ADC1_CH7)        |
| Pitch      | 32   | Analog input (ADC1_CH4)        |
| Roll       | 33   | Analog input (ADC1_CH5)        |
| Pot1       | 39   | Analog input (ADC1_CH3)        |
| Pot2       | 36   | Analog input (ADC1_CH0)        |
| ARM        | 13   | Digital input with internal PU |
| MODE       | 12   | Digital input with internal PU |
| SWA        | 25   | Digital input with internal PU |
| SWB        | 26   | Digital input with internal PU |
| OTA        | 15   | Digital input with internal PU |
| Battery    | 14   | Analog input (ADC2_CH1)        |
| BT1        | 1    | Digital input with internal PU |
| BT2        | 0    | Digital input with internal PU |
| BT3        | 2    | Digital input with internal PU |

## Software Dependencies

- [Arduino Core for ESP32](https://github.com/espressif/arduino-esp32)
- [BleGamepad Library](https://github.com/lemmingDev/ESP32-BLE-Gamepad)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [ArduinoOTA](https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA) (included with ESP32 core)

## Setup Instructions

1. **Install Arduino IDE** (or PlatformIO) and add the ESP32 board support.
2. **Install required libraries** via Library Manager or manually.
3. **Configure TFT_eSPI**:
   - Edit the `User_Setup.h` file in the TFT_eSPI library folder to match your display (driver, pins, rotation).
4. **Wi‑Fi Credentials**:
   - Replace `"your_SSID"` and `"your_PASSWORD"` in the sketch with your actual Wi‑Fi credentials.
5. **Upload the sketch** to your ESP32.
6. **Connect hardware** according to the pin mapping table.

## Usage

- Power on the device. The TFT will show "SYSTEM BOOTING..." then the main UI.
- Press the OTA button to enter Over‑The‑Air update mode. The ESP32 will connect to Wi‑Fi and display its IP address.
- In Arduino IDE, select the correct serial port and choose "ESP32‑TX‑Controller" from the **Tools → Port** menu for OTA upload.
- Normal operation: The device advertises as a BLE gamepad named "ESP32 TX Controller". Pair with your computer/phone.
- All analog values and switch states are shown on the display in real‑time.

## Battery Calibration

The battery reading uses a calibration factor (`calibFactor = 1.138`). Adjust this value based on your voltage divider and actual measurements.

```cpp
float calibFactor = 1.138; // Adjust as needed
```

## Project Structure

- `esp32-game-controller.ino` – Main Arduino sketch
- `README.md` – This documentation
- `.gitignore` – Git ignore file for Arduino/ESP32 projects

## License

This project is open‑source under the MIT License.

## Acknowledgments

- [ESP32-BLE-Gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad) by lemmingDev
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) by Bodmer
- Arduino and ESP32 communities
# 🚀 ESP32 BLE TX Controller (Drone/Simulator)

Custom DIY transmitter using ESP32 with:
- BLE Gamepad (for simulators like Liftoff, Velocidrone)
- TFT Display UI
- OTA Updates via WiFi
- Battery Monitoring with smoothing
- Multiple switches & buttons

---

## 📸 Features

✅ BLE Gamepad (no receiver needed)  
✅ Smooth battery voltage display  
✅ OTA firmware update button  
✅ Real-time UI (sticks, switches, buttons)  
✅ Dual potentiometer support  
✅ Compact ESP32-based design  

---

## 🔌 Hardware Used

- ESP32 Dev Board  
- 2x Joystick Modules  
- 2x Potentiometers  
- Switches (ARM, MODE, SWA, SWB)  
- 3x Push Buttons  
- 1x TFT Display (SPI - ILI9341/ST7735)  
- nRF24L01 (optional future use)  
- Battery + Voltage Divider (10k/10k)

---

## ⚡ Pin Configuration

### 🎮 Analog Inputs (ADC1)
| Function     | GPIO |
|-------------|------|
| Throttle    | 34   |
| Yaw         | 35   |
| Pitch       | 32   |
| Roll        | 33   |
| Pot1        | 39   |
| Pot2        | 36   |

### 🔘 Switches & Buttons
| Function | GPIO |
|----------|------|
| ARM      | 13   |
| MODE     | 12   |
| SWA      | 25   |
| SWB      | 26   |
| OTA BTN  | 15   |
| BTN1     | 1    |
| BTN2     | 0    |
| BTN3     | 2    |

### 🔋 Battery
| Function | GPIO |
|----------|------|
| Battery Sense | 14 |

(Use 10k/10k voltage divider)

---

### 📺 TFT Display (SPI)
| Signal | GPIO |
|--------|------|
| SCK    | 18   |
| MOSI   | 23   |
| CS     | 21   |
| DC     | 22   |
| RESET  | EN   |

---

## 📡 OTA Update

Press OTA button → enters OTA mode  
Upload via Arduino IDE → Network Port  

---

## 🎮 Usage

1. Power ON  
2. Connect via Bluetooth as **ESP32 TX Controller**  
3. Open simulator (Liftoff / Velocidrone)  
4. Map controls  
5. Fly 🚁  

---

## 🔋 Battery Calculation

Voltage is calculated using:
V = (ADC / 4095) × 3.3 × 2   

Smoothed using exponential filter.

---

## 📷 Circuit Diagram

See `/images/circuit_diagram.png`

---

## 👨‍💻 Author

Sudhir Kumar  
DIY RC & Drone Projects 🚀
🔧 Circuit Diagram (Simple Guide)

Tu image bana sakta hai (EasyEDA / KiCad), par abhi ke liye ye reference:

🧠 Core Connections

Joystick 1 (Throttle/Yaw)

VRx → GPIO34
VRy → GPIO35
VCC → 3.3V
GND → GND

Joystick 2 (Pitch/Roll)

VRx → GPIO32
VRy → GPIO33
🎚 Pots
Middle pin → GPIO39 / 36
Side → 3.3V & GND
🔘 Switches
One side → GPIO
Other side → GND

(INPUT_PULLUP used)

🔋 Battery DividerBattery +
   |
  10k
   |
   |----> GPIO14
   |
  10k
   |
  GND
  📺 TFT (SPI Shared)
ESP32      TFT
18   --->  SCK
23   --->  MOSI
21   --->  CS
22   --->  DC
EN   --->  RESET

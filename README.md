# PPM to USB Joystick Converter (Arduino Pro Micro)

This project converts an incoming **PPM RC signal** (8 channels) into a **USB Joystick/HID device** using an Arduino **SparkFun Pro Micro (5V / 16 MHz)**.  
Useful for FPV head-tracking, RC transmitters, simulators, or DIY control setups.

---

## Hardware

- **SparkFun Pro Micro (5V, 16 MHz)**
- **PPM signal source** (FPV goggles, RC receiver, etc.)
- PPM wire connected to **pin 3**

---

## Libraries Used

### arduino-timer (v2.0.1)  
https://github.com/contrem/arduino-timer

### ArduinoJoystickLibrary (v2.0)  
https://github.com/MHeironimus/ArduinoJoystickLibrary/tree/version-2.0

---

## Features

- Reads **8-channel PPM** using interrupt on **pin 3**
- Converts channels into USB joystick axes/buttons
- Fully customizable channel → joystick assignment
- Optional serial debug printing
- Works as a standard USB HID joystick on Windows/macOS/Linux

---

## PPM Signal Format

- **Start pulse (sync):** > 3000 µs  
- **Channel width:** 1000–2000 µs  
- **Channel count:** 8 (`CHANNEL_COUNT = 8`)

The sketch measures pulse width and maps values directly to joystick ranges.

---

## Wiring

| Signal | Arduino Pin |
|--------|-------------|
| PPM    | **D3**      |
| GND    | GND         |

Pull-up is enabled automatically.

---

## How It Works

1. Every falling edge triggers an interrupt.  
2. Pulse width is measured using `micros()`.  
3. A gap >3000 µs marks the start of a new PPM frame.  
4. Channel pulse widths are captured into `rxChannels[]`.  
5. `updateRC()` updates joystick axes and buttons.  
6. HID state is sent to USB.

---

## Channel → Joystick Mapping

Default mapping in `updateRC()`:

| RC Channel | Function | Joystick Axis / Button |
|------------|----------|------------------------|
| CH5        | Flaps    | X-Axis                 |
| CH4        | Rudder   | Y-Axis                 |
| CH6        | Brake    | Z-Axis (optional)      |
| CH7        | Gear     | Button 0 (optional)    |

Additional mappings can be created by editing `updateRC()`.

---

## Serial Debugging

Set:


to print channel values every second.

Set to `0` to disable.

---

## Compiling & Uploading

1. Install required libraries.  
2. Select **SparkFun Pro Micro 5V/16MHz** in Arduino IDE.  
3. Upload the sketch.  
4. Device appears as a **USB Joystick**.

---

## Customization

- Change axis/button assignment in `updateRC()`.
- Modify axis inversion or scaling.  
- Enable constant HID updates (`joyAutoSendMode = true`).  
- Increase `CHANNEL_COUNT` if needed.

---


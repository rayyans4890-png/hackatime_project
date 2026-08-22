# Hackpad Firmware — FIRMWARE.md

> **Hack Club Stardance Event**
> 5-Key Cherry MX Macropad · Seeeduino XIAO SAMD21

---

## Hardware Summary

| Component | Detail |
|-----------|--------|
| **MCU** | Seeeduino XIAO SAMD21 (ATSAMD21G18A) |
| **Core** | ARM Cortex-M0+ @ 48 MHz |
| **Flash** | 256 KB |
| **RAM** | 32 KB |
| **USB** | Native USB (CDC + HID) |
| **Keys** | 5 × Cherry MX (PCB-mount) |
| **Rotary Encoder** | None |
| **RGB LEDs** | None |
| **OLED/Display** | None |

### GPIO Pin Assignments (verified from KiCad PCB)

| Switch | XIAO Pin | Arduino Name | SAMD21 Port | PCB Net |
|--------|----------|-------------|-------------|---------|
| SW1 (top) | Pin 11 | D10 | PA06 | `Net-(U1-PA6_A10_D10_MOSI)` |
| SW2 (upper) | Pin 10 | D9 | PA05 | `Net-(U1-PA5_A9_D9_MISO)` |
| SW3 (middle) | Pin 9 | D8 | PA07 | `Net-(U1-PA7_A8_D8_SCK)` |
| SW4 (lower) | Pin 8 | D7 | PB09 | `Net-(U1-PB09_A7_D7_RX)` |
| SW5 (bottom) | Pin 5 | D4 | PA08 | `Net-(U1-PA8_A4_D4_SDA)` |

All switches are active-low: pin 2 of every switch connects to GND.
Internal pull-ups are enabled in firmware.

---

## Firmware Architecture

```
firmware/
├── platformio.ini          # Build configuration
├── hackpad.uf2             # Pre-built firmware (ready to flash)
├── hackpad.bin             # Pre-built firmware (binary)
├── FIRMWARE.md             # This file
└── src/
    ├── main.cpp            # Complete firmware source
    └── config.h            # Pin definitions and configuration
```

### Build System

The firmware is built with **PlatformIO** using the **Arduino framework** on the
Atmel SAM platform. The build targets the `seeed_xiao` board definition, which
correctly maps all SAMD21 peripherals, USB stack, and the UF2 bootloader.

> **Note on QMK:** The Hack Club tutorial references QMK firmware. However,
> QMK does not natively support the SAMD21 (ATSAMD21G18A). QMK's ARM support
> covers STM32 and RP2040 via ChibiOS, but the SAMD21 is not included. This
> firmware achieves the same result — a USB HID keyboard — using the Arduino
> framework with the SAMD21's native USB stack, which is the standard and
> recommended approach for the XIAO SAMD21.

### USB HID Implementation

The firmware implements a **USB HID Keyboard** using the Arduino `HID` library
with a custom HID report descriptor:

- **Report type:** Standard 6-key rollover (6KRO) boot keyboard
- **Report size:** 8 bytes (modifier + reserved + 6 keycodes)
- **Usage page:** Generic Desktop → Keyboard (page 0x01 / usage 0x06)

---

## Default Keymap

```
┌──────────┐
│   SW1    │ 1
├──────────┤
│   SW2    │ 2
├──────────┤
│   SW3    │ 3
├──────────┤
│   SW4    │ 4
├──────────┤
│   SW5    │ 5
└──────────┘
```

| Physical Key | HID Keycode | Usage ID | Description |
|-------------|-------------|----------|-------------|
| SW1 (top) | 1 | 0x1E | Number key 1 |
| SW2 (upper) | 2 | 0x1F | Number key 2 |
| SW3 (middle) | 3 | 0x20 | Number key 3 |
| SW4 (lower) | 4 | 0x21 | Number key 4 |
| SW5 (bottom) | 5 | 0x22 | Number key 5 |

### Why F13–F15?

F13, F14, and F15 are **unused function keys** on all major operating systems.
They are the ideal choice for macropads because:

- They **never conflict** with system shortcuts
- They can be **remapped** to any action via software:

| OS | Tool |
|----|------|
| macOS | [Karabiner-Elements](https://karabiner-elements.pqrs.org/) |
| Windows | [PowerToys Keyboard Manager](https://learn.microsoft.com/windows/powertoys/keyboard-manager) or [AutoHotkey](https://www.autohotkey.com/) |
| Linux | `xbindkeys`, `input-remapper`, or `keyd` |

### Remap Example (AutoHotkey — Windows)

```ahk
F13::MsgBox "Hello from Hackpad!"
F14::Run "https://hackclub.com"
F15::Send "Hello, World!"
```

---

## Build Instructions

### Prerequisites

- **PlatformIO CLI** (`pip install platformio`)
- Internet connection (for first-time platform/library download)

### Build

```bash
cd firmware
pio run
```

The compiled firmware will be in:
```
.pio/build/xiao_samd21/firmware.elf
.pio/build/xiao_samd21/firmware.bin
```

### Build Output

```
RAM:   7.6% (2,484 / 32,768 bytes)
Flash: 4.5% (11,792 / 262,144 bytes)
```

---

## Flashing the Firmware

### Method 1: UF2 Bootloader (Recommended)

The XIAO SAMD21 ships with a UF2 bootloader pre-installed.

1. **Enter bootloader mode:**
   - Connect the XIAO to your computer via USB-C
   - **Double-tap the RESET button** on the XIAO (the small button on the board)
   - The RGB LED will start cycling colors — this indicates bootloader mode
   - A new USB drive will appear on your computer (named `XIAO` or `XIAO_SEEED`)

2. **Flash the firmware:**
   - Copy `hackpad.uf2` to the XIAO drive:
     ```bash
     # Windows (PowerShell)
     Copy-Item firmware\hackpad.uf2 E:\  # replace E: with the XIAO drive letter

     # macOS / Linux
     cp firmware/hackpad.uf2 /media/XIAO/
     ```
   - The XIAO will automatically reboot after copying

3. **Verify:**
   - The Hackpad will enumerate as a USB keyboard
   - Press each key to verify HID output (use a key tester website or text editor)

### Method 2: PlatformIO Upload

```bash
cd firmware
pio run --target upload
```

> **Note:** The XIAO must be in bootloader mode (double-tap RESET) for upload to work.

### Method 3: bossac (Manual)

```bash
# Enter bootloader mode first, then:
bossac -p COM3 -U true -w -v firmware.bin
# (replace COM3 with your XIAO's serial port)
```

---

## LED Behavior

The XIAO SAMD21 has a single built-in RGB LED:

| State | Meaning |
|-------|---------|
| Solid blue | USB connected, normal operation |
| Color cycling | Bootloader mode (ready to flash) |
| Off | Not powered |

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Keys don't register | Verify USB enumeration (check Device Manager / `lsusb`) |
| Build fails with `Keyboard.h` not found | Ensure you're building from the `firmware/` directory |
| Upload fails | Make sure the XIAO is in bootloader mode (double-tap RESET) |
| Wrong key output | Verify pin assignments match PCB traces in `src/main.cpp` |

---

## Memory Footprint

| Resource | Used | Available | Percentage |
|----------|------|-----------|------------|
| Flash | 11,792 B | 262,144 B | 4.5% |
| RAM | 2,484 B | 32,768 B | 7.6% |

The firmware is extremely lightweight, leaving >95% of flash and >92% of RAM
available for future enhancements (RGB LED support, encoder, OLED, layers, etc.).

---

## File Manifest

| File | Purpose |
|------|---------|
| `platformio.ini` | PlatformIO build configuration |
| `src/main.cpp` | Firmware source — key scanning, HID, debounce |
| `src/config.h` | Pin definitions and configuration constants |
| `hackpad.uf2` | Pre-built UF2 firmware (ready to flash) |
| `hackpad.bin` | Pre-built binary firmware |
| `FIRMWARE.md` | This documentation file |

---

## License

This firmware was created for the Hack Club Stardance event.
Feel free to modify, remix, and redistribute.

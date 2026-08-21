/*
 * Hackpad Firmware — USB HID Keyboard
 * Hack Club Stardance Event
 *
 * MCU:  Seeeduino XIAO SAMD21 (ATSAMD21G18A, ARM Cortex-M0+, 48 MHz)
 * PCB:  3-key Cherry MX macropad (28 mm × 56 mm)
 *
 * Hardware connections (verified from KiCad PCB traces):
 *   SW1 (left)  → PA6 (XIAO Pin 11, D10 / MOSI)
 *   SW2 (mid)   → PA5 (XIAO Pin 10, D9  / MISO)
 *   SW3 (right) → PA7 (XIAO Pin 9,  D8  / SCK)
 *   All switch pin 2 → GND
 *
 * Keymap:
 *   [1] [2] [3]
 *
 * Number keys 1–3 on the keyboard.
 *
 * Build:   pio run
 * Flash:   pio run --target upload   (double-tap RESET first)
 */

#include <Arduino.h>
#include <HID.h>

/* ═══════════════════════════════════════════════════════════════════
 *  PIN MAP  (from KiCad PCB routing)
 * ═══════════════════════════════════════════════════════════════════ */

static const uint8_t KEY_PINS[] = {
    D10,    // SW1 → D10 / MOSI  (XIAO Pin 11, PA06/A10)
    D9,     // SW2 → D9  / MISO  (XIAO Pin 10, PA05/A9)
    D8,     // SW3 → D8  / SCK   (XIAO Pin 9,  PA07/A8)
};

#define NUM_KEYS      3
#define DEBOUNCE_MS   5

/* ═══════════════════════════════════════════════════════════════════
 *  HID KEYBOARD REPORT DESCRIPTOR
 *
 *  Standard 6-key rollover boot keyboard:
 *    Byte 0: modifier bitmap (unused — always 0)
 *    Byte 1: reserved
 *    Byte 2–7: up to 6 simultaneous keycodes
 *
 *  HID Usage Table (Keyboard / Keypad page 0x07):
 *    1 = 0x1E    2 = 0x1F    3 = 0x20
 * ═══════════════════════════════════════════════════════════════════ */

static const uint8_t hidReportDescriptor[] PROGMEM = {
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)

    // Modifier byte (1 byte)
    0x05, 0x07,        //   Usage Page (Keyboard / Keypad)
    0x19, 0xE0,        //   Usage Minimum (Left Control)
    0x29, 0xE7,        //   Usage Maximum (Right GUI)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x03,        //   Input (Constant)

    // Output report (LEDs) — 1 byte
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (Num Lock)
    0x29, 0x05,        //   Usage Maximum (Kana)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x05,        //   Report Count (5)
    0x91, 0x02,        //   Output (Data, Variable, Absolute)
    0x75, 0x03,        //   Report Size (3)
    0x95, 0x01,        //   Report Count (1)
    0x91, 0x03,        //   Output (Constant)

    // Key array — 6 keycodes
    0x05, 0x07,        //   Usage Page (Keyboard / Keypad)
    0x19, 0x00,        //   Usage Minimum (0)
    0x29, 0xFF,        //   Usage Maximum (255)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x06,        //   Report Count (6)
    0x81, 0x00,        //   Input (Data, Array)

    0xC0               // End Collection
};

/* HID keyboard report — 8 bytes */
typedef struct __attribute__((packed)) {
    uint8_t modifiers;   // Modifier keys bitmap (unused)
    uint8_t reserved;    // Reserved
    uint8_t keys[6];     // Up to 6 simultaneous keycodes
} KeyboardReport;

static KeyboardReport keyReport;

/* ═══════════════════════════════════════════════════════════════════
 *  KEYCODE MAP  (HID Keyboard Usage Table page 0x07)
 * ═══════════════════════════════════════════════════════════════════ */

static const uint8_t KEYCODES[NUM_KEYS] = {
    0x1E,   // 1
    0x1F,   // 2
    0x20,   // 3
};

/* ═══════════════════════════════════════════════════════════════════
 *  DEBOUNCE STATE
 * ═══════════════════════════════════════════════════════════════════ */

static bool     key_active[NUM_KEYS]  = {false, false, false};
static bool     key_prev[NUM_KEYS]    = {false, false, false};
static uint32_t debounce_t[NUM_KEYS]  = {0, 0, 0};

/* ═══════════════════════════════════════════════════════════════════
 *  HELPERS
 * ═══════════════════════════════════════════════════════════════════ */

static void sendReport(void) {
    memset(&keyReport.keys, 0, sizeof(keyReport.keys));
    uint8_t idx = 0;
    for (uint8_t i = 0; i < NUM_KEYS && idx < 6; i++) {
        if (key_active[i]) {
            keyReport.keys[idx++] = KEYCODES[i];
        }
    }
    HID().SendReport(0, &keyReport, sizeof(keyReport));
}

/* ═══════════════════════════════════════════════════════════════════
 *  SETUP
 * ═══════════════════════════════════════════════════════════════════ */

void setup() {
    for (uint8_t i = 0; i < NUM_KEYS; i++) {
        pinMode(KEY_PINS[i], INPUT_PULLUP);
    }

    memset(&keyReport, 0, sizeof(keyReport));
    static HIDSubDescriptor desc(hidReportDescriptor, sizeof(hidReportDescriptor));
    HID().AppendDescriptor(&desc);
    HID().begin();

    delay(500);  // Allow USB enumeration
}

/* ═══════════════════════════════════════════════════════════════════
 *  MAIN LOOP
 * ═══════════════════════════════════════════════════════════════════ */

void loop() {
    uint32_t now = millis();
    bool changed = false;

    for (uint8_t i = 0; i < NUM_KEYS; i++) {
        bool raw = (digitalRead(KEY_PINS[i]) == LOW);

        if (raw != key_prev[i]) {
            debounce_t[i] = now;
            key_prev[i] = raw;
        }

        if ((now - debounce_t[i]) >= DEBOUNCE_MS && raw != key_active[i]) {
            key_active[i] = raw;
            changed = true;
        }
    }

    if (changed) {
        sendReport();
    }

    delay(1);
}

/*
 * Hackpad Firmware Configuration
 * Hack Club Stardance Event
 *
 * MCU: Seeeduino XIAO SAMD21 (ATSAMD21G18A)
 * Keys: 5x Cherry MX (active-low, direct pin to GND)
 *
 * Pin assignments derived from KiCad PCB:
 *   SW1 (top)    → PA6 (XIAO Pin 11, D10/MOSI)
 *   SW2 (upper)  → PA5 (XIAO Pin 10, D9/MISO)
 *   SW3 (middle) → PA7 (XIAO Pin 9,  D8/SCK)
 *   SW4 (lower)  → PB09 (XIAO Pin 8, D7/RX)
 *   SW5 (bottom) → PA8 (XIAO Pin 5,  D4/SDA)
 */

#ifndef HACKPAD_CONFIG_H
#define HACKPAD_CONFIG_H

// ─── Hardware Identification ────────────────────────────────────
#define PRODUCT_NAME     "Hackpad"
#define MANUFACTURER     "Hackpad"
#define SERIAL_NUMBER    "HP-001"

// ─── Matrix Configuration ───────────────────────────────────────
// 5 keys, single row, direct-pin (each key has its own GPIO)
#define MATRIX_ROWS  1
#define MATRIX_COLS  5

// ─── GPIO Pin Assignments (directly from PCB traces) ────────────
// These are SAMD21 GPIO port pins, verified against PCB routing
#define KEY_ROW_PIN  0   // Not used for direct-pin, but defined for matrix

// Column pins (active-low with internal pull-ups)
#define KEY_COL_0    PA06   // SW1 → XIAO Pin 11 (D10/MOSI)
#define KEY_COL_1    PA05   // SW2 → XIAO Pin 10 (D9/MISO)
#define KEY_COL_2    PA07   // SW3 → XIAO Pin 9  (D8/SCK)
#define KEY_COL_3    PB09   // SW4 → XIAO Pin 8  (D7/RX)
#define KEY_COL_4    PA08   // SW5 → XIAO Pin 5  (D4/SDA)

// ─── Key Count ──────────────────────────────────────────────────
#define TOTAL_KEYS   5

// ─── Debounce Settings ──────────────────────────────────────────
#define DEBOUNCE_MS  5      // 5ms debounce delay

// ─── USB Settings ───────────────────────────────────────────────
#define USB_VID      0x2886  // Seeed Studio VID
#define USB_PID      0x802C  // XIAO PID

// ─── Layer Definitions ──────────────────────────────────────────
#define NUM_LAYERS   2

// Layer 0: Default - 1, 2, 3 (number keys)
// Layer 1: Fn layer - Volume Down, Mute, Volume Up

// ─── LED Configuration ──────────────────────────────────────────
// No RGB LEDs on this PCB
#define HAS_RGB_LED  0
#define HAS_OLED     0
#define HAS_ENCODER  0

#endif /* HACKPAD_CONFIG_H */

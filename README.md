# Internet of Things practices using Texas Instruments MSP430 LaunchPad with PlatformIO

## Hardware

| Item | Value |
|---|---|
| Board | [MSP-EXP430G2ET](https://www.ti.com/tool/MSP-EXP430G2ET) |
| Microcontroller | [MSP430G2553](https://www.ti.com/product/MSP430G2553) |
| Red LED (onboard) | P1.0 |
| Green LED (onboard) | P1.6 |
| Button (onboard, S2) | P1.3 |

## Project structure

Each practice has its own environment, building only the matching file in `src/`:

| Environment | File | Topic |
|---|---|---|
| `lpmsp430g2553` | `src/main.cpp` | Sandbox |
| `practice1` | `src/practice1.cpp` | Polling-based I/O |
| `practice2` | `src/practice2.cpp` | Interrupt-driven I/O |
| `practice3` | `src/practice3.cpp` | Timer A |
| `practice4` | `src/practice4.cpp` | Analog-to-digital conversion |

## Practices

### Practice 1: polling-based I/O

The button (S2) is read inside `loop()` with a delay-based debounce. Each confirmed press toggles which LED is on: red on even counts, green on odd counts.

### Practice 2: interrupt-driven I/O

S2 stands in for a conveyor belt item counter, using a falling-edge interrupt on port 1. The green LED starts on; every 5 items counted, the red and green LEDs swap state. Debounce timing accounts for the board's real clock speed (16 MHz).

### Practice 3: Timer A

A pedestrian traffic light built with Timer A: red on for 5 s, green on for 3 s, red blinking for 1 s, then the cycle repeats. `ACLK` runs off the internal VLO (about 12 kHz) so the multi-second intervals don't need an external crystal.

### Practice 4: analog-to-digital conversion

Reads the MSP430G2553's internal temperature sensor through ADC10. It records a reference reading once at startup, then compares each new sample against it: the red LED comes on if the temperature has risen, the green LED if it has fallen, with a small hysteresis margin so sensor noise doesn't flip the LEDs back and forth.

## Building and flashing

Requires [PlatformIO](https://platformio.org) (CLI or the VS Code extension).

```powershell
pio run -e practice1 # build
pio run -e practice1 -t upload # build and flash
pio device monitor -b 9600 # serial monitor (where applicable)
```

Swap `practice1` for the environment you want: `practice2`, `practice3`, `practice4`, or `lpmsp430g2553` for the sandbox.

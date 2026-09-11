#include <Arduino.h>

unsigned char counter = 0; // Number of confirmed button presses
unsigned char buttonWasPressed = 0; // Button state from the previous loop iteration

void setup() {
    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer
    P1DIR = BIT0 | BIT6; // Set P1.0 and P1.6 as outputs (red and green LEDs)
    P1REN = BIT3; // Enable the pull resistor on P1.3 (button)
    P1OUT = BIT3 | BIT0; // Pull-up on P1.3, red LED (P1.0) on at start
}

void loop() {
    unsigned char isPressed = ((P1IN & BIT3) == 0); // With the pull-up, the pin reads low when the button is pressed

    if (isPressed != buttonWasPressed) { // Button state changed since the last loop reading
        __delay_cycles(800000); // Debounce: wait ~50 ms (16 MHz clock) for the contact to settle
        isPressed = ((P1IN & BIT3) == 0); // Re-read after the delay to confirm the new state is stable, not just noise
    }

    if (isPressed && !buttonWasPressed) { // Released-to-pressed transition: a new click was confirmed
        counter++;
        if (counter % 2 == 0) {
            P1OUT = (P1OUT & ~(BIT0 | BIT6)) | BIT0; // Even count: turn off both LEDs and turn on red (P1.0)
        }
        else {
            P1OUT = (P1OUT & ~(BIT0 | BIT6)) | BIT6; // Odd count: turn off both LEDs and turn on green (P1.6)
        }
    }

    buttonWasPressed = isPressed; // Save the current button state to compare on the next loop iteration
}

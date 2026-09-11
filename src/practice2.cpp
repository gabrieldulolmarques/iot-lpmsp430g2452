#include <Arduino.h>

volatile unsigned char itemCount = 0; // Items counted since the last reset

void setup() {
    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer
    P1DIR = 0b01000001; // Set P1.0 and P1.6 as outputs (red and green LEDs)
    P1REN = 0b00001000; // Enable the pull resistor on P1.3 (button)
    P1OUT = 0b01001000; // Pull-up on P1.3, green LED (P1.6) on at start
    P1IE = 0b00001000; // Enable the interrupt on P1.3
    P1IES = 0b00001000; // Trigger on the falling edge (button pressed)
    P1IFG &= ~0b00001000; // Clear the P1.3 interrupt flag
}

void loop() {
    __bis_SR_register(CPUOFF + GIE); // Sleep (LPM0) until the next interrupt
}

// Port 1 interrupt service routine
__attribute__((interrupt(PORT1_VECTOR)))
void Port_1(void) {
    __delay_cycles(800000); // Debounce delay (~50 ms at 16 MHz)

    if (!(P1IN & 0b00001000)) { // Confirm the pin is still low, not just noise
        itemCount++; // Register the passage of one item on the conveyor belt
        if (itemCount >= 5) {
            P1OUT ^= 0b01000001; // Toggle the green (P1.6) and red (P1.0) LEDs
            itemCount = 0; // Reset the count
        }
    }

    while (!(P1IN & 0b00001000)); // Wait for release, so its bounce isn't counted

    P1IFG &= ~0b00001000; // Clear the P1.3 interrupt flag
}

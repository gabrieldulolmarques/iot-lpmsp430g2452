#include <Arduino.h>

enum SemaphoreState { RED, GREEN, YELLOW };

volatile SemaphoreState state = RED;
volatile unsigned int ticks = 0; // Counts Timer A interrupts (~0.5 s each) within the current state

void setup() {
    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer
    P1DIR = 0b01000001; // Set P1.0 and P1.6 as outputs (red and green LEDs)
    P1REN = 0b00001000; // Enable the pull resistor on P1.3
    P1OUT = 0b00001001; // Pull-up on P1.3, red LED (P1.0) on at start
    BCSCTL3 |= LFXT1S_2; // ACLK = VLO (~12 kHz), since the board's crystal is not connected by default
    TACCTL0 = CCIE; // Enable the Timer A compare interrupt (CCR0)
    TACTL = TASSEL_1 + MC_3 + ID_3; // ACLK, up/down mode, /8 -> ~1500 Hz (~666.7 us per tick)
    TACCR0 = 375; // 2 * 375 * 666.7 us ≈ 0.5 s per interrupt
}

void loop() {
    __bis_SR_register(CPUOFF + GIE); // Sleep (LPM0) until the next interrupt
}

// Timer0_A0 interrupt service routine, fired roughly every 0.5 s
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void Timer_A(void) {
    ticks++;

    switch (state) {
        case RED:
            if (ticks >= 10) { // 10 * 0.5 s = 5 s
                state = GREEN;
                ticks = 0;
                P1OUT = (P1OUT & ~0b01000001) | 0b01000000; // Turn green on, red off
            }
            break;

        case GREEN:
            if (ticks >= 6) { // 6 * 0.5 s = 3 s
                state = YELLOW;
                ticks = 0;
                P1OUT = (P1OUT & ~0b01000001) | 0b00000001; // Turn red on, green off
            }
            break;

        case YELLOW:
            P1OUT ^= 0b00000001; // Toggle red every 0.5 s
            if (ticks >= 2) { // 2 * 0.5 s = 1 s of blinking
                state = RED;
                ticks = 0;
                P1OUT |= 0b00000001; // Make sure red ends up on when the cycle restarts
            }
            break;
    }
}

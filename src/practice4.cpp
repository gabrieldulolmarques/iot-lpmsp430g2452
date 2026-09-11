#include <Arduino.h>

unsigned int calibratedTemp = 0; // Reference temperature reading, taken once at startup
unsigned int currentValue = 0; // Latest temperature reading
const unsigned int HYSTERESIS = 1; // ADC counts of dead-band around calibratedTemp, to ignore sensor noise

void configureAdc(void) {
    ADC10CTL1 = INCH_10 + ADC10DIV_3; // Channel 10 = internal temperature sensor
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
    __delay_cycles(1000); // Wait for the voltage reference to settle
    ADC10CTL0 |= ENC + ADC10SC; // Start the conversion
    __bis_SR_register(CPUOFF + GIE); // Sleep until the conversion finishes
    calibratedTemp = ADC10MEM;
}

void setup() {
    WDTCTL = WDTPW + WDTHOLD; // Stop the watchdog timer
    P1DIR = 0x01 + 0x40; // Set P1.0 and P1.6 as outputs (red and green LEDs)
    // Clock left at its default ~16 MHz (matching F_CPU) so Serial's baud
    // rate math stays correct; the ADC10 uses its own internal oscillator
    // for sampling, so it doesn't need MCLK/SMCLK reconfigured to 1 MHz.
    configureAdc();
    __enable_interrupt();
}

void loop() {
    __delay_cycles(1000);
    ADC10CTL0 |= ENC + ADC10SC; // Start a new conversion
    __bis_SR_register(CPUOFF + GIE); // Sleep until the conversion finishes
    currentValue = ADC10MEM;

    if (currentValue > calibratedTemp + HYSTERESIS) {
        P1OUT = (P1OUT & ~0b01000001) | 0b00000001; // Temperature rose: turn red on, green off
    } else if (currentValue < calibratedTemp - HYSTERESIS) {
        P1OUT = (P1OUT & ~0b01000001) | 0b01000000; // Temperature fell: turn green on, red off
    } // Within the dead-band: keep the last detected state, ignore the noise
}

// ADC10 interrupt service routine
__attribute__((interrupt(ADC10_VECTOR)))
void ADC10_ISR(void) {
    __bic_SR_register_on_exit(CPUOFF); // Wake the CPU after the conversion
}

/* 
 * File: mainPWM.c
 * Author: Grace Foxworthy
 * Revision History: Updated 10/1/2019
 */

/* Define Statements */
#define CYCLESTARTOFFTIME 500 // ms
#define CYCLEONDCTIME 250 // ms
#define CYCLEONPWMTIME 750 // ms
#define CYCLEENDOFFTIME 500 // ms

#define NUMOFCYCLES 10 // number of cycles to record

#define DUTYCYCLEPERIOD 5000 // 20ns clock counts
#define PWMPERIOD 10000 // 20ns clock counts

/* Include Statements */
#include <xc.h>
#include <p33Exxxx.h>

/* Configuration Statements */
// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config BOREN = ON               // (BOR is enabled)
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF             // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Source Selection (Fast RC Oscillator with divide-by-N with PLL module (FRCPLL))
#pragma config PWMLOCK = OFF            // PWM Lock Enable bit (PWM registers may be written without key sequence)
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)


int main(void) {
    /* Configure Clock Frequency to 50 MHz */
    // Fosc = 7.3728 x (M / (N1 x N2)) = 99.9936 MHz
    // Fcy = Fp = Fosc/2 = 49.9968 Mhz
    // Tcy = Tp = 20 ns
    // Tcy x 2^16 = 1.31 ms, Tcy x 2^32 = 85.9 s
    OSCTUN = 0b111000; // Tune FRC oscillator
    PLLFBD = 215; // M = PLLDIV + 2 = 217
    CLKDIVbits.PLLPRE = 6; // N1 = PLLPRE + 2 = 8
    CLKDIVbits.PLLPOST = 0; // N2 = 2 x (PLLPOST + 1) = 2

    /* Configure CPU Priority */
    SRbits.IPL = 0b001;
    
    /* Initialize Pins */
    LATA = 0x0000;
    TRISA = 0x0000;
    ANSELA = 0x0000;
    
    // PortB
    LATB = 0x0000;
    TRISB = 0x000C; // Programming Pins
    ANSELB = 0x0000;
    
    // PortC
    LATC = 0x0000;
    TRISC = 0x0000;
    ANSELC = 0x0000;
    
    // PortD
    LATD = 0x0000;
    TRISD = 0x0000;
    ANSELD = 0x0000;
    
    TRISDbits.TRISD6 = 1; // button digital input
    
    // PortE
    LATE = 0x0000;
    TRISE = 0x000C; // Programming Pins
    ANSELE = 0x0000;
    
    // PortF
    LATF = 0x0000;
    TRISF = 0x0000;
    ANSELF = 0x0000;
    
    // PortG
    LATG = 0x0000;
    TRISG = 0x0000;
    ANSELG = 0x0000;
    
    /* Initialize Timers */
    T1CON = 0; // Clear the control register
    T1CONbits.TCKPS = 0b00; // Timer Input Clock Prescale Select = 1:1
    PR1 = 50000; // Period = 1 ms / Tp = 1 ms / 20 ns = 50000
    IPC0bits.T1IP = 0x01; // Set the priority of the timer interrupt to the same as the CPU
    IFS0bits.T1IF = 0; // Clear the interrupt flag
    IEC0bits.T1IE = 1; // Enable the interrupt flag
    
    /* Initialize Variables */
    char button = 0; // button press indicator 
    int t = 0; // time in ms spent in cycle
    
    /* Turn OFF D3 */
    LATGbits.LATG10 = 0;
    
    /* Turn OFF D2 */
    LATGbits.LATG11 = 0;
    
    T1CONbits.TON = 1; // start 1 ms timer
    
    /* Start Loop */
    while (1){
         if ((button == 0) && (PORTDbits.RD6 == 1)) {
            // nothing happens if button is not pressed
        } else if ((button == 0) && (PORTDbits.RD6 == 0)) {
            // pressing button changes indicator variable Read from PORT and write at LAT 
            button = 1;}
        if (button == 1){
        if ( t == 0){ LATGbits.LATG11 = 1; t++;}
        else if (0 <t && t < 1000){
            //*LATGbits.LATG11 = 1;
            
            t++;
        }
        else if (t>= 1000 && t < 3000){
            LATGbits.LATG11 = 0;
            t++;
        }
        else {
            t = 0;
                
                LATGbits.LATG11 = 0; // turn off D4 */
        }
         }
                
        /* Put Device in Idle Mode */
        Idle();
        IFS0bits.T1IF = 0; // clear timer1 flag
        TMR1 = 0;
           
            
    }
  }
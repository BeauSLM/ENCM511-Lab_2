/*
 * File         : main.c
 * Assignment   : ENCM Driver Project 2
 * Author       : Beau McCartney, Quentin Jennings, Apostolos Scondrianis
 * Section      : B03 - Group 7
 * Created On   : October 17, 2021, 11:25 AM
 * Completed On :
 * Submitted on :
 */
#include "xc.h"
#include <p24F16KA101.h>

/*
 * Function void IOinit() sets up our input and output pin
 * behavior for the program we are building.
 */

int CNFlag = 0;

void IOinit() {
    TRISBbits.TRISB8 = 0; //SET GPIO RB8 as a digital output.
    TRISAbits.TRISA2 = 0; //SET GPIO RA2 as a digital input.
    TRISAbits.TRISA4 = 1; //SET GPIO RA4 as a digital input.
    TRISBbits.TRISB4 = 1; //SET GPIO RB4 as a digital input.
    CNPU2bits.CN30PUE = 1; //Enables Pull up resistor on RA2/ CN30
    CNPU1bits.CN0PUE = 1; //Enables Pull up resistor on RA4/ CN0
    CNPU1bits.CN1PUE = 1; //Enables Pull resistor on RB4 / CN1
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital
}
 void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
 {
    IFS1bits.CNIF = 0;
    CNFlag = 1;
 }
 
void IOCheck() {
    if(CNFlag == 1) { //if an interrupt is detected let's check some conditions
        if(((PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1)
                    || (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0)
                    || (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0)) 
                    || (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0)) { //two or more buttons pressed.
            LATBbits.LATB8 = 1; //LED constantly on.
        }
        else if((PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1)) {
            //Just button on RB4 GPIO is pressed - shorted.
        } else if((PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1)) {
            //Just button on RA4 GPIO is pressed - shorted
        } else if((PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0)) {
            //Just button on RB4 GPIO is pressed - shorted
        } else { //no button is pressed
            LATBbits.LATB8 = 0; //turn LED off in case no button is pressed
        }
        CNFlag = 0;
    }
}

int main(void) {


    IOinit();
    while(1) {
        IOCheck();
    }
    return 0;
}
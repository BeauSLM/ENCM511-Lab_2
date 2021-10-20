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



//Global Variables
int CNFlag = 0;
//int TMR2Flag = 0;

//sets the clock to 500kHz
void CLKinit()  
{
     SRbits.IPL = 7;  //Disable interrupts before switch
     CLKDIVbits.RCDIV = 0;  // CLK division = 0
	 //prepares a 32kHz clock switch
     __builtin_write_OSCCONH(0x55); //writes to upper half of OSCCON
     __builtin_write_OSCCONL(0x01); //writes to lower half of OSCCON
     OSCCONbits.OSWEN=1; //indicates a clock switch
     while(OSCCONbits.OSWEN==1) {} //waits until the bit switches back
	 //indicates a successful clock switch
     SRbits.IPL = 0;  //Enable interrupts after switch
}

/*
 * Function void IOinit() sets up our input and output pin
 * behavior for the program we are building.
 */
void IOinit() {
    TRISBbits.TRISB8 = 0; //SET GPIO RB8 as a digital output.
	
    TRISAbits.TRISA2 = 1; //SET GPIO RA2 as a digital input.
    TRISAbits.TRISA4 = 1; //SET GPIO RA4 as a digital input.
    TRISBbits.TRISB4 = 1; //SET GPIO RB4 as a digital input.
	
    CNPU2bits.CN30PUE = 1; //Enables Pull up resistor on RA2/ CN30
    CNPU1bits.CN0PUE = 1; //Enables Pull up resistor on RA4/ CN0
    CNPU1bits.CN1PUE = 1; //Enables Pull resistor on RB4 / CN1
	
    //AD1PCFG = 0xFFFF; // Turn all analog pins as digital
	
    IPC4bits.CNIP = 7; //sets priority for Input Change Notification
	IFS1bits.CNIF = 0; //Clear the CNI flag
    IEC1bits.CNIE = 1; //Input Change Notification Interrupts Enabled
    CNEN1bits.CN1IE = 1; //Enable input change Notif on RB4
    CNEN1bits.CN0IE = 1; //Enable input change Notif on RA4
    CNEN2bits.CN30IE = 1; //Enable input change Notif on RA2
}


//clkval = 8 for 8MHz; 
//clkval = 500 for 500kHz; 
//clkval = 32 for 32kHz; 
void Delay_ms(unsigned int time_ms) {
    //Configuring T2CON register
    T2CONbits.T32 = 0; //Operate as 16 bit timer
    T2CONbits.TCS = 0; // Set internal clock usage
    
    //Prescaler settings of T2CON register
    T2CONbits.TCKPS0 = 0; //
    T2CONbits.TCKPS1 = 1; // These two lines set prescaling to 8x
    
    //Interrupt Configuration for Timer 2
    IPC1bits.T2IP = 3; //Set Priority level = 3
    IFS0bits.T2IF = 0; //Clear Timer 2 Flag
    IEC0bits.T2IE = 1; //Timer 2 interrupt enabled

    
    PR2 = 1000*32*time_ms/16; //Number of clock cycles that need to elapse
    T2CONbits.TON = 1; //Timer 2 Starts here
    return;
    
}
void IOcheck() {
    if(CNFlag == 1) {
        if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) {
            LATBbits.LATB8 = 0; //turn LED off in case no button is pressed
        } else if((PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1)) {
            //Just button on RA4 GPIO is pressed - shorted
            Delay_ms(1000);
        } else if((PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1)) {
            Delay_ms(2000);
            //Just button on RA4 GPIO is pressed - shorted
        } else if((PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0)) {
            Delay_ms(3000);
            //Just button on RB4 GPIO is pressed - shorted
        } else { 
            LATBbits.LATB8 = 1; //turn LED on if multiple buttons are pressed
        }
        CNFlag = 0; // Set our CN Global Flag to False after we handle the interrupt
    }
}
void Toggle_LED() {
    if(PORTBbits.RB8 == 0) {
        LATBbits.LATB8 = 1;
    } else {
        LATBbits.LATB8 = 0;
    }
}

 void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
 {
    IFS1bits.CNIF = 0; //Clear interrupt Register Flag
    CNFlag = 1; // Set our CN Global Flag to True/1
    IOcheck();
 }

 
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void) {
    IFS0bits.T2IF = 0; //Clear timer 2 interrupt flag
    T2CONbits.TON = 0; //Stop timer
    Toggle_LED();
    TMR2 = 0; //Reset the TMR2 register after the interrupt occurs.
    //TMR2Flag = 1; //Timer complete
} 


int main(void) {
    IOinit();
	CLKinit();
    while(1) {
        
    }
    return 0;
}
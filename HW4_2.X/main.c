#include "SPI.h" 
#include "I2C.h"
#include <math.h>
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF  // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1              /////// slowest wdt?? PS1048576 
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock (8MHz) to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0x0101      // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

// Constants
#define NUM_SAMPS 100
#define PI 3.14159

int main() {
	 __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
   
    //do your TRIS and LAT commands here 
    TRISAbits.TRISA4 = 0;        // Pin A4 is the LED, 0 for output
    LATAbits.LATA4 = 1;          // make LED pin "high" (1) = "on"
    TRISBbits.TRISB4 = 1;        // B4 (reset button) is an input pin
   
   init_SPI1();
   
    __builtin_enable_interrupts();
     _CP0_SET_COUNT(0);
   
    // initialize counters  
    int i = 0;   
    int j = 0;
    
    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		// remember the core timer runs at half the CPU speed
          
        delay(6000);                        // controls frequency
        //setVoltage(0b1,0b01010101);         //  and voltage is set on CH1
        
        //Sine Wave
        char voltage_sine;
        if (i <= NUM_SAMPS){
            voltage_sine = floor(255*sin((i*2*PI)/NUM_SAMPS));    
            setVoltage(0b1, voltage_sine);
            i++;
        }
        else{
            i=0;    // start loop over
        }
        
        // Triangular Wave
        char voltage_tri;
        if (j <= NUM_SAMPS/2){
            voltage_tri = floor(255*(j/100));    
            setVoltage(0b0, voltage_tri);
            j++;
        }
        else if (j > NUM_SAMPS/2){
            voltage_tri = - floor(255*(j/100));
            setVoltage(0b0, voltage_tri);
            j++;
        }
        else{
            j=0;    //start loop over
        }
        
        while(!PORTBbits.RB4) {             // when user button is pressed, 
            LATAbits.LATA4 = 0;                 // green LED is off
        }
        
        if(_CP0_GET_COUNT() > 12000) {              // every x sec.. 
            LATAbits.LATA4 = !LATAbits.LATA4;       // toggle green LED
            _CP0_SET_COUNT(0);
        }
        
    }
     
}

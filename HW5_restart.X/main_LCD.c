#include "LCD.h"
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
#define NUM_SAMPS 1000
#define PI 3.14159
#define IODIR 0x00
#define IOCON 0x05
#define GPIO  0x09
#define OLAT 0x0A
#define EXPANDER 0b00100000 // for MCP23008 with A0, A1, A2 = 0,0,0

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
    // do your TRIS and LAT commands here 
    TRISAbits.TRISA4 = 0;        // Pin A4 is the GREEN LED, 0 for output
    LATAbits.LATA4 = 1;          // make GREEN LED pin "high" (1) = "on"
    TRISBbits.TRISB4 = 1;        // B4 (reset button) is an input pin
    TRISBbits.TRISB2 = 0;        // B2 and B3 set to output for LCD 
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB15 = 0;       // B15 set to output 
    // initialize peripherals and chips
   SPI1_init();
   LCD_init();
    __builtin_enable_interrupts();
     _CP0_SET_COUNT(0);
     LCD_clearScreen(BLUE);

    char textbuffer[20];
    int leet = 1337;
    sprintf(textbuffer,"Hello World! %d!",leet);
    
    
    while(1) {
        LCD_drawPixel(50, 50, WHITE);
        //LCD_drawCharacter(28, 32, 'D');
        LCD_type(28,32,textbuffer, BLACK);
        //delay(6000);                      // controls frequency
        
    }
     
}

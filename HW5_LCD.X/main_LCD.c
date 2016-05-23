#include "ILI9163C.h" 
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
#pragma config WDTPS = PS1048576              /////// slowest wdt?? PS1048576 or PS1
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
    //TRISAbits.TRISA4 = 0;        // Pin A4 is the LED, 0 for output
    //TRISBbits.TRISB4 = 1;        // B4 (reset button) is an input pin
//    TRISA = 0b1111111111101100; // pins A4(GREEN LED), and A0, A1 are outputs
//    TRISB = 0b0011111101110011; // pins B2 (I2C), B3(I2C), B7(CS-LCD), B8= input, and B14(SPI/LCD), B15(LCD) are outputs; B4 (RESET) is an input
    TRISA = 0xFFCF; 
    TRISB = 0b0001111001110011;
    LATAbits.LATA4 = 1;          // make GREEN LED pin "high" (1) = "on"
    
    SPI1_init();
    LCD_init();
    LCD_clearScreen(0);
   
    __builtin_enable_interrupts();
     _CP0_SET_COUNT(0);
     
    LCD_drawPixel(0, 0, BLACK);
    LCD_drawPixel(1, 1, BLACK);
    LCD_drawPixel(2, 2, BLACK);
    LCD_drawPixel(3, 3, BLACK);
    
    
    char length = 0;
    char textbuffer[20];
    int leet = 1337;
    sprintf(textbuffer,"Hello world %d!",leet);
             
    while(1) {
        length = sizeof(textbuffer);//size must be taken here otherwise pointer size is taken instead of 
        LCD_drawString(28,32,textbuffer);
 
	    
        delay(960000);                        // controls frequency, 24,000,000/X
        
    }
     
}

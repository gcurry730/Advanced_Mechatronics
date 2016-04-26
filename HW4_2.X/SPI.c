#include "SPI.h" 
#include <xc.h>
#define CS LATAbits.LATA0       // chip select pin = A0

void init_SPI1(void){
	ANSELAbits.ANSA0 = 0; 		// turn off analog for pin A0
   // ANSELBbits.ANSB0 = 0; 		// turn off analog for pin A0
	TRISAbits.TRISA0 = 0; 		// set A0 to output
	
	SDI1Rbits.SDI1R = 0b000; 	// Sets SDI1 to pin A1 (INPUT, not used))
	//SS1Rbits.SS1R = 0b000; 		// Sets SS1 pin to A0 
    RPA0Rbits.RPA0R = 0b0011;   // Sets A0 pin to SS1 (OUTPUT)
	RPB8Rbits.RPB8R = 0b0011;	// Sets B8 to SDO1 (OUTPUT)
	
	SPI1BRG = 0x1000;			// Fastest = 1 to see on scope 0x300, 
	SPI1CONbits.MSTEN = 1;		// Master mode
    SPI1STATbits.SPIROV = 0;    // clear overflow
    SPI1CONbits.CKE = 1;        // changes from active to idle
    SPI1CONbits.ON = 1;			// Enable SPI peripheral
}

char SPI1_IO(char write){
	SPI1BUF = write;				// writes data to buffer
	while(SPI1STATbits.SPIBUSY){ 	// this buffer = 1 once it's ready to send
	;								// wait until it is done
	}
	char read = SPI1BUF;			// reads what the DAC sent
	return read;
}

void setVoltage(char channel, char voltage){
	short data = 0b0000000000000000;
	data |= channel << 15;			// puts channel in the 16th bit by shifting it by 15
	data |= 0b111 << 12;			// makes next three bits 1 by bit-shifting them by 12
	data |= voltage << 4; 			// voltage stored in the next 8 bits
	
    CS = 0;
	SPI1_IO(data >> 8);
	SPI1_IO(data);
    CS = 1;
	
}
void delay(int time) {
    int delaytime = time; //in hz, core timer freq is half sysfreq
    int starttime;
    starttime = _CP0_GET_COUNT(); 
    while ((int)_CP0_GET_COUNT()-starttime < delaytime){
    ;
    }
   
}
#include "SPI.h" 
#include <xc.h>



void init_SPI1(void){
	ANSELAbits.ANSA0 = 0; 		// turn off analog for pin A0
	TRISAbits.TRISA0 = 0; 		// set A0 to output
	
	SDI1Rbits.SDI1R = 0b000; 	// Sets SDI1 to pin A1
	SS1Rbits.SS1R = 0b000; 		// Sets SS1 pin to A0 
	RPB8Rbits.RPB8R = 0b0011;	// Sets B8 to SDO1
	
	SPI1BRG = 1;					// Fastest
	SPI1CONbits.MSTEN = 1;		// Master mode
	SPI1CONbits.ON = 1;			// Enable SPI peripheral	
}

char SPI1_IO(char write){
	SPI1BUF = write;				// writes data to buffer
	while(SPI1STATbits.SPIBUF){ 	// this buffer = 1 once it's ready to send
	;								// wait until it is done
	}
	char read = SPI1BUF;			// reads what the DAC sent
	return read;
}

void setVoltage(char channel, char voltage){
	char data = 0b0000000000000000;
	data |= channel << 15;			// puts channel in the 16th bit by shifting it by 15
	data |= 0b111 << 12;			// makes next three bits 1 by bit-shifting them by 12
	data |= voltage << 4; 			// volatage stored in the next 8 bits
	
	SPI1_IO(8 >> data);
	SPI1_IO(data);
	
}
// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k
#include "I2C.h"
#include <xc.h>
#define ADDR 0x40 //for 0100(A2)(A1)(A0)0 = 01000000 = 0x40 (only for writing, last bit is 0)

void initI2C(void){
    // Turn off analog for pins B2, B3 using ANSELB
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    // SDA2 is fixed to B2 and SCL2 is B3
    i2c_master_setup();
}

void initExpander(void){
    // Initialize pins GP0-3 as outputs, GP4-7 as inputs
    i2c_master_start();             // make the start bit
    i2c_master_send(ADDR);          // send to specific chip address
    i2c_master_send(0x00);          // I/O register
    i2c_master_send(0b11110000); 
   // Initialize GP0-3 to 0 
    i2c_master_start();             // make the start bit
    i2c_master_send(ADDR);          // send to specific chip address
    i2c_master_send(0x09);          // output latch register
    i2c_master_send(0b00000000);    // set GP0-3 to output 0 
}

void setExpander(char pin, char level){        // pin can be 0, 1, 2, 3   Level is 0 or 1
i2c_master_start(); // make the start bit
i2c_master_send(000<1|0); // write the address, 000, shifted left by 1, or'ed with a 0 to indicate writing
i2c_master_send(0x09); // the register to write to (general purpose I/O)
i2c_master_send(level<pin); // the value to put in the register
i2c_master_stop(); // make the stop bit
}

char getExpander(void){
i2c_master_start(); // make the start bit
i2c_master_send(000<1|0); // write the address, 000, shifted left by 1, or'ed with a 0 to indicate writing
i2c_master_send(0x09); // the register to read from (general purpose I/O)
i2c_master_restart(); // make the restart bit

i2c_master_send(000<1|1); // write the address, 000, shifted left by 1, or'ed with a 1 to indicate reading
char r = i2c_master_recv(); // save the value returned
i2c_master_ack(1); // make the ack so the slave knows we got it
i2c_master_stop(); // make the stop bit
return r;
}


void i2c_master_setup(void) {
  I2C2BRG = 53;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 ((=100Khz))
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}
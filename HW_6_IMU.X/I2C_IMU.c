// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k
#include "I2C_IMU.h"
#include <xc.h>
#define GYRO 0b1101011
#define WHOAMI 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12


/////// Given Functions ///////

void i2c_master_setup(void) {
  I2C2BRG = 235;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2  Fsck = baud rate (100khz) PGD = 104ns Pblck = 48mhz
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

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

//////// General I2C functions //////

void i2c_master_write(unsigned char ADDRESS, unsigned char REGISTER, unsigned char data) {
    i2c_master_start();
    i2c_master_send(ADDRESS<<1);
    i2c_master_send(REGISTER);
    i2c_master_send(data);
    i2c_master_stop();
}

unsigned char i2c_master_read(unsigned char ADDRESS,unsigned char REGISTER){
    unsigned char r = 0;
    i2c_master_start();
    i2c_master_send(ADDRESS<<1);
    i2c_master_send(REGISTER);
    i2c_master_restart();
    i2c_master_send(ADDRESS<<1|1);
    r = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
return r;
}

////// Functions for IMU //////

void initI2C(void){
    i2c_master_setup();
    // SDA2 is fixed to B2 and SCL2 is B3
    // Turn off analog for pins B2, B3 using ANSELB
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    // disable SEQOP
    i2c_master_write(GYRO,0x05,0b00100000);
}

void init_IMU(){
    i2c_master_write(GYRO,CTRL1_XL, 0b10000000); ///1000 for 1.66 kHz sample rate, 00 for 2g sensitivity, 00 for 400kHz baud
    i2c_master_write(GYRO,CTRL2_G,0b10000000); //1000 for 1.66 kHz, 00 for 245 dps sensitivity, 
    i2c_master_write(GYRO,CTRL3_C,0b00000100); //IF_INC bit 1 will enable the ability to read multiple registers
}

unsigned char WHO_AM_I(void){
    return i2c_master_read(GYRO,WHOAMI);
}
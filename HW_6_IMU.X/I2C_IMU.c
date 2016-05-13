// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

#include "I2C_IMU.h"
#include <xc.h>

#define SLAVE_ADDRESS_W  0b11010101
#define SLAVE_ADDRESS_R  0b11010100

unsigned char WhoAreYou(){
    unsigned char read;
    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS_W);
    i2c_master_send(0x0F);  //WHO_AM_I register
    i2c_master_restart();
    i2c_master_send(SLAVE_ADDRESS_R);
    read = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return read;  
}

void initIMU(void){
    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS_W);
    i2c_master_send(0x10); // CTRL1_XL register
    i2c_master_send(0b00000001); // powers accel, low power mode?
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS_W);
    i2c_master_send(0x11); // CTRL2_G register
    i2c_master_send(0b00000001); // powers gyro, low power mode?
    i2c_master_stop();
}
void initI2C(void){
    // Turn off analog for pins B2, B3 using ANSELB
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    // SDA2 is fixed to B2 and SCL2 is B3
    i2c_master_setup();
}

void i2c_master_setup(void) {
  I2C2BRG = 390;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 ((=100Khz)) 390?
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

void delay(int time) {
    int delaytime = time; //in hz, core timer freq is half sysfreq
    int starttime;
    starttime = _CP0_GET_COUNT(); 
    while ((int)_CP0_GET_COUNT()-starttime < delaytime){ ; }
}
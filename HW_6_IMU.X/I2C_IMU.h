#ifndef I2C_IMU_H
#define I2C_IMU_H

// I2C communication library for PIC32 (master) and IMU LSM6DS33 (slave)  

unsigned char WhoAreYou(void);
void initI2C(void);
void initIMU(void);
char getIMU(void);


void i2c_master_setup(void);                // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);                // send a START signal
void i2c_master_restart(void);              // send a RESTART signal
void i2c_master_send(unsigned char byte);   // send a byte (either an address or data)
unsigned char i2c_master_recv(void);        // receive a byte of data
void i2c_master_ack(int val);               // send an ACK (0) or NACK (1)
void i2c_master_stop(void);                    // send a stop

void delay(int)

#endif
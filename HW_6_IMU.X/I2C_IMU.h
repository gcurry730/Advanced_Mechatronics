#ifndef I2C_H_IMU
#define I2C_H_IMU

// I2C communication library for PIC32 (master) and IMU (slave)  
// helps implement use I2C2 as a master without using interrupts

void i2c_master_setup(void);                // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);                // send a START signal
void i2c_master_restart(void);              // send a RESTART signal
void i2c_master_send(unsigned char byte);   // send a byte (either an address or data)
unsigned char i2c_master_recv(void);        // receive a byte of data
void i2c_master_ack(int val);               // send an ACK (0) or NACK (1)
void i2c_master_stop(void);                    // send a stop

void i2c_master_write(unsigned char ADDRESS, unsigned char REG, unsigned char data);
unsigned char i2c_master_read(unsigned char ADDRESS,unsigned char REG);

void initI2C(void);
void init_IMU(void);
unsigned char WHO_AM_I(void);
#endif
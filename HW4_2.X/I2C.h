#ifndef I2C_H
#define I2C_H

// I2C communication library for PIC32 (master) and Expander MCP23008 (slave)  

void initI2C(void);


void i2c_master_setup(void);
void i2c_master_start(void);
void i2c_master_restart(void);
void i2c_master_send(unsigned char byte);
unsigned char i2c_master_recv(void);
void i2c_master_ack(int val);
void i2c_master_stop(void); 

#endif
#ifndef SPI1_H
#define SPI1_H

// SPI communication library for PIC32 (master) and MCP4902 DAC (slave)  

void init_SPI1(void);
char SPI1_IO(char write);
void setVoltage(char channel, char voltage);
void delay(int time);
char make_sine_wave(void);

#endif
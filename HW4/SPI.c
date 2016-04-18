

void init_SPI(void)
char SPI1_IO(char write)
void setVoltage(char channel, char voltage)

void init_SPI1(void){
	ANSELAbits.ANSA0= 0; 	// turn off analog for pin A0
	ANSELAbits.ANSA1= 0;	// turn off analog for pin A1
	
	
}
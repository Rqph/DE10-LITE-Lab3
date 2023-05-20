
#include "opencores_i2c_regs.h"
#include "opencores_i2c.h"
#include "system.h"
#include "sys/alt_stdio.h"
#include "alt_types.h"
#include "io.h"
#include "unistd.h"

#define I2C_DEBUG

#define ADXL_ADDR 0x1D

#define ADXL_X0 0x32
#define ADXL_X1 0x33
#define ADXL_Y0 0x34
#define ADXL_Y1 0x35
#define ADXL_Z0 0x36
#define ADXL_Z1 0x37

void write_to_addr(int value,int addr){
	I2C_start(I2C_BASE,ADXL_ADDR,0); //set chip address and set to write/
	I2C_write(I2C_BASE,addr,0);  	// set address to 0.
	I2C_write(I2C_BASE,value,1);
}

int read_addr(int addr){
	int data=0;
	
	I2C_start(I2C_BASE,ADXL_ADDR,0); //set chip address and set to write/
	I2C_write(I2C_BASE,addr,0);  	// set address to 0.
	
	I2C_start(I2C_BASE,ADXL_ADDR,1); //set chip address in read mode
	data =  I2C_read(I2C_BASE,1);  // memory array
	
	return data;
}

int main(){
	int data1,data2,data,addr = 0;

	I2C_init(I2C_BASE,ALT_CPU_FREQ,100000);
	if (I2C_start(I2C_BASE,ADXL_ADDR,0)==0) {
		alt_printf("ADXL bien configurer\n");
		data = read_addr(0x00);
		alt_printf("ID DEVICE : 0x%x\n",data);
	}
	alt_printf("SET 0x7 sur DATA_FORMAT (0x31)\n");
	addr = 0x31;
	data = 0x7;
	
	write_to_addr(data,addr);
	usleep(1000);
	data = read_addr(addr);
	
	alt_printf("addr : 0x%x -> value : 0x%x\n",addr,data);
	
	usleep(1000);
	
	alt_printf("SET 0x8 sur POWER_CTL (0x2D)\n");
	addr = 0x2D;
	data = 0x8;
	write_to_addr(data,addr);
	usleep(1000);
	data = read_addr(addr);
	alt_printf("addr : 0x%x -> value : 0x%x\n",addr,data);
	
	usleep(1000);

	
	while (1){
		data1 = read_addr(ADXL_X1);
		alt_printf("\tDATAX1 = 0x%x ",data1);
		data2 = read_addr(ADXL_X0);
		alt_printf("\tDATAX0 = 0x%x",data2);
		data = (data2) |(data1<<8);
		alt_printf("\tDATAX = 0x%x\n",data);
		data = 0;
		
		data1 = read_addr(ADXL_Y1);
		alt_printf("\tDATAY1 = 0x%x ",data1);
		data2 = read_addr(ADXL_Y0);
		alt_printf("\tDATAY0 = 0x%x",data2);
		data = (data2) |(data1<<8);
		alt_printf("\tDATAY = 0x%x\n",data);
		data = 0;
		
		data1 = read_addr(ADXL_Z1);
		alt_printf("\tDATAZ1 = 0x%x ",data1);
		data2 = read_addr(ADXL_Z0);
		alt_printf("\tDATAZ0 = 0x%x",data2);
		data = (data2) |(data1<<8);
		alt_printf("\tDATAZ = 0x%x\n",data);
		
		usleep(1000000);
	}
	return 0;
}
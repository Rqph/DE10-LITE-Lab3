#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"

#include "opencores_i2c_regs.h"
#include "opencores_i2c.h"

#include "system.h"
#include "alt_types.h"
#include "io.h"
#include "unistd.h"

#include "sys/alt_stdio.h"
#include "sys/alt_sys_init.h"
#include "sys/alt_irq.h"
#include "sys/alt_sys_init.h"

#define I2C_DEBUG

#define ADXL_ADDR 0x1D

#define ADXL_X0 0x32
#define ADXL_X1 0x33
#define ADXL_Y0 0x34
#define ADXL_Y1 0x35
#define ADXL_Z0 0x36
#define ADXL_Z1 0x37

#define DATA_FORMAT 0x31
#define POWER_CTL 0x2D

#define OFFSET_X 2
#define OFSX 0x1E

#define OFFSET_Y 6
#define OFSY 0x1F

#define OFFSET_Z 20
#define OFSZ 0x20

////////////////////////////////////////////////////////
void write_to_addr(int value,int addr);
int read_addr(int addr);
int read_addr(int addr);
void ADXL_calibrage();
void ADXL_config();
static void handle_button_interrupts(void* context, alt_u32 id);
static void init_button_pio();
static void handle_timer_interrupt(void *context);
static void init_timer();
void affiche_7seg(short number);
void init_7seg();
short conversion(int number);
////////////////////////////////////////////////////////
int flag_data = 0;
int dataX,dataY,dataZ=0;
////////////////////////////////////////////////////////

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

void ADXL_calibrage(){
	int addr,data=0;
	addr = OFSX;
	data = OFFSET_X;
	alt_printf("SET 0x%x sur OFSX (0x%x)\n",data,addr);
	
	write_to_addr(data,addr);
	usleep(1000);
	data = read_addr(addr);
	
	alt_printf("addr : 0x%x -> value : 0x%x\n",addr,data);
	////////////////////////////////////////////////////
	alt_printf("SET 0x%x sur OFSY (0x%x)\n",data,addr);
	addr = OFSY;
	data = OFFSET_Y ;
	write_to_addr(data,addr);
	usleep(1000);
	data = read_addr(addr);
	alt_printf("addr : 0x%x -> value : 0x%x\n",addr,data);
	////////////////////////////////////////////////////
	alt_printf("SET 0x%x sur OFSZ (0x%x)\n",addr,data);
	addr = OFSZ;
	data = OFFSET_Z ;
	write_to_addr(data,addr);
	usleep(1000);
	data = read_addr(addr);
	alt_printf("addr : 0x%x -> value : 0x%x\n",addr,data);
}
void ADXL_config(){
	int addr,data=0;
	alt_printf("SET 0x7 sur DATA_FORMAT (0x31)\n");
	addr = DATA_FORMAT;
	data = 0x7;
	write_to_addr(data,addr);
	usleep(1000);
	data = read_addr(addr);
	
	alt_printf("addr : 0x%x -> value : 0x%x\n",addr,data);
	
	usleep(1000);
	
	alt_printf("SET 0x8 sur POWER_CTL (0x2D)\n");
	addr = POWER_CTL;
	data = 0x8;
	write_to_addr(data,addr);
	usleep(1000);
	data = read_addr(addr);
	alt_printf("addr : 0x%x -> value : 0x%x\n",addr,data);
	
}

static void handle_button_interrupts(void* context, alt_u32 id){
	alt_printf("IRQ Button \n");
	flag_data+=1;
	if (flag_data>2)flag_data=0;
	
	/* Reset the edge capture register. */
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BOUTON_BASE, 0x03);
}
static void init_button_pio(){
	/* Enable all 4 button interrupts. */
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BOUTON_BASE, 0x03);
	//alt_printf("MASK \n");
	/* Reset the edge capture register. */
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BOUTON_BASE, 0x03);
	//alt_printf("EDGE CAP \n");
	/* Register the ISR. */
	alt_irq_register(BOUTON_IRQ,NULL,(void*)handle_button_interrupts);
	//alt_printf("irq \n");	
}

short conversion(int number){
	return (short)number*(3.9);
}

static void handle_timer_interrupt(void *context){
	int data1,data2=0;
	alt_printf("IRQ Timer : \n");
	data1 = read_addr(ADXL_X1);
	//alt_printf("\tDATAX1 = 0x%x ",data1);
	data2 = read_addr(ADXL_X0);
	//alt_printf("\tDATAX0 = 0x%x",data2);
	dataX = (data2) |(data1<<8);
	
	data1 = read_addr(ADXL_Y1);
	//alt_printf("\tDATAY1 = 0x%x ",data1);
	data2 = read_addr(ADXL_Y0);
	//alt_printf("\tDATAY0 = 0x%x",data2);
	dataY = (data2) |(data1<<8);
	
	data1 = read_addr(ADXL_Z1);
	//alt_printf("\tDATAZ1 = 0x%x ",data1);
	data2 = read_addr(ADXL_Z0);
	//alt_printf("\tDATAZ0 = 0x%x",data2);
	dataZ = (data2) |(data1<<8);
	
	switch(flag_data){
		case 0 :
			dataX = conversion(dataX);
			affiche_7seg(dataX);
			break;
		case 1 :
			dataY = conversion(dataY);
			affiche_7seg(dataY);
			break;
		case 2 :
			dataZ = conversion(dataZ);
			affiche_7seg(dataZ);
			break;
		default :
			affiche_7seg(0);
	}
	
	alt_printf("X = 0x%x - Y = 0x%x - Z = 0x%x\n",dataX,dataY,dataZ);
	
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);
	
	
}
static void init_timer(){
	alt_irq_register(TIMER_IRQ, NULL, (void*)handle_timer_interrupt);
	
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x3);
}

void init_7seg(){
	IOWR_ALTERA_AVALON_PIO_DATA(SEG1_BASE,0);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG2_BASE,0);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG3_BASE,0);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG4_BASE,0);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG5_BASE,0);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE,0);
}
void affiche_7seg(short number){
	int flag_negatif=0;
	int i =0;
	int split[5] = {0,0,0,0,0};
	
	if (number<0) {
		number = number*(-1);
		flag_negatif = 1;
	}
	else flag_negatif = 0;
	
	while (number >= 10) {
		split[i] = number % 10;
		number = number / 10;
		i = i+1;
	}
	split[i] = number;
	
	if (flag_negatif == 1) {
		IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE,0xF);
	}
	else {
		IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE,0x0);
	}
	
	IOWR_ALTERA_AVALON_PIO_DATA(SEG1_BASE,split[0]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG2_BASE,split[1]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG3_BASE,split[2]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG4_BASE,split[3]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG5_BASE,split[4]);
}

int main(){
	
	I2C_init(I2C_BASE,ALT_CPU_FREQ,100000);
	if (I2C_start(I2C_BASE,ADXL_ADDR,0)==0) {
		alt_printf("ADXL bien configurer\n");
		int data = read_addr(0x00);
		alt_printf("ID DEVICE : 0x%x\n",data);
	}
	
	ADXL_config();
	ADXL_calibrage();
	
	init_7seg();
	init_button_pio();
	init_timer();
	
	while (1){
	}
	return 0;
}
#define __AVR_ATmega328P__
#include "giavrheader.hpp"
#include "nRF24l01.hpp"
#include "Sound.h"
#define CE_PIN	PB1
#define CSN_PIN	PB2

UART uart(0, Pin("PD0"), Pin("PD1"));
char Uartstr[100]={0,};
//RF24 radio(CE_PIN,CSN_PIN);
uint32_t Soundfocus = 0;

uint8_t SPI_TransmitReceive(uint8_t data);
void RF_pin(uint8_t pin,uint8_t onoff);
uint32_t GetTick();
void Bpin(char pin,char onoff){
	if(onoff)PORTB |= 1<<pin;
	else PORTB &= ~(1<<pin);
}
int Uart_sendByte(char ch, FILE* fil){
	while ((UCSR0A & 0x20) == 0X00);
	UDR0 = ch;
	return 0;
}
int bestVal(int a,int b,int c){
	if(a>b)
		if(a>c)return a;
		else return c;
	else if(a>c)
		return b;
	else if(b>c)
		return b;
	else return c;
}
uint32_t Ticks = 0;
uint32_t times[10] = { 0, };
int CDS0,CDS1,CDS2;
int pCDS0,pCDS1,pCDS2;
float InputVoltagebest,InputVoltage0,InputVoltage1,InputVoltage2,enviromentVoltage;
uint8_t Value, Echo, Send = 0, raiserMode = 0,count = 0;uint32_t ffangleTick=0,ffangle=800;
int main(void)
{
	FILE* fpStdio = fdevopen(Uart_sendByte, 0);
	stdout = fpStdio;
	delay_ms(500);
	DDRC = 0x00;
	DDRB = (1 << CE_PIN) | (1 << CSN_PIN);
	DDRD |= 0x08;
	Timer0_FastPWM_init(7,3,0,1);
	Timer1_init(0x03, 65536 - 250*10);
	TCCR2B = 0x02;TCNT2 = 256 - 250;
	uart.setTXEnable();
	uart.setBaudrate(9600);
	uart.setSendLength(7);
	spi.Master(2);
	sei();

	printf("Baudrate : 9600 \nUBRR = %d\nSet Baudrate : %d\n", uart.getubrr(), (int)uart.getBaudrate());
	uart.Transmit("Start!\n");

	delay_ms(300);
	uart.Transmit("Init\n");/*
	radio.begin();
	radio.setPALevel(RF24_PA_LOW);
	radio.openWritingPipe(0xFEDC123400LL);
	radio.openReadingPipe(1, 0xFEDC123401LL);
	radio.printDetails();*/
	//while(1)if(!(PIND & 0x10)){DDRD |= 0x40;TIMSK2 = 1;btn++;}else btn=0;

	while (1)
	{
		CDS0 = ADCin(0);//CDS1 = ADCin(1);CDS2 = ADCin(2);
		
		//InputVoltage0 = 3.3*CDS0 / 1024.0;
		//InputVoltage1 = 3.3*CDS1 / 1024.0;
		//InputVoltage2 = 3.3*CDS2 / 1024.0;
		//InputVoltagebest = 3.3*bestVal(CDS0,CDS1,CDS2) / 1024.0;
		//enviromentVoltage = 3.3*(CDS0+CDS1+CDS2)/3036.0;
		printf("Voltage = %d delta:%d angle:%d  %d/%d \n", CDS0,CDS0-pCDS0,(int)ffangle,1000+(int)(((double)ffangle/256)*1000),19000-(int)(((double)ffangle/256)*1000));//,CDS1,CDS2,CDS0-pCDS0,CDS1-pCDS1,CDS2-pCDS2);
		if(CDS0-pCDS0 > 75){PORTD &= ~0x40;DDRD |= 0x40;TIMSK2 = 1;ffangleTick=GetTick();ffangle=-100;}
		pCDS0 = CDS0;//pCDS1 = CDS1;pCDS2 = CDS2;
		
		PORTD |= 0x08;
		delay_us(1000+ffangle);
		PORTD &= ~0x08;
		delay_us(19000-ffangle);
		
		if(GetTick()-ffangleTick > 500 && GetTick()-ffangleTick < 2000)ffangle=(900*(GetTick()-ffangleTick-500))/1500;
		
		/*if(count++ > 200){
			count = 0;Value=0;Send = 1;
		}
		if ((InputVoltagebest > enviromentVoltage*1.1) && !raiserMode)
		{
			if (InputVoltagebest > enviromentVoltage*1.4)Value = 4;
			else if (InputVoltagebest > enviromentVoltage*1.3)Value = 3;
			else if (InputVoltagebest > enviromentVoltage*1.2)Value = 2;
			else Value = 1;
			if(InputVoltagebest > enviromentVoltage*1.1)TIMSK2 = 1;
			Send = 1;
			raiserMode = 1;
		}
		if (InputVoltagebest < enviromentVoltage && raiserMode)
		{
			raiserMode = 0;
		}
		if (Send) {
			if(Value>0)PORTD |= 0x10;
			uint32_t time = GetTick();
			while (1) {
				Echo = 0xFF;
				printf("send %d....", Value);
				if (radio.write(&Value, sizeof Value)) {
					times[0] = GetTick();
					printf("ok.");
					printf("   Ack....");
					radio.startListening();
					while (!radio.available())if (GetTick() - times[0] > 300)break;
					if (radio.read(&Echo, sizeof Echo))printf("ok."); else  printf("fail.");
					radio.stopListening();
					printf("Echo = %d \n ",Echo);
					if (Echo == Value)break;
				}
				else printf("fail.\n");
				if(GetTick() - time > 1000)break;
			}
			Send = 0;
			PORTD &= ~0x10;
		}*/
	}
	
	while(0){/*
		if(!inited && (PINB&0x01)){
			inited = 1;
			delay_ms(300);
			uart.Transmit("Init\n");
			radio.begin();
			radio.setPALevel(RF24_PA_LOW);
			radio.openWritingPipe(0xFEDC123400LL);
			radio.openReadingPipe(1,0xFEDC123401LL);
			radio.printDetails();
		}
		if(!(PINB&0x01) && inited){
			uart.Transmit("disconnected\n");
			inited = 0;
		}
		if(!(PINB&0x01)){
			uart.Transmit("a");
			delay_ms(100);
		}
		if(inited && (PINB&0x01)){
			uart.Transmit("sending....\n");
			//delay_ms(300);
			//if(radio.write((char*)"helloworld!!",12))uart.Transmit("succeed.\n");else uart.Transmit("failed.\n");
		}*/
	}
	return 0;
}
ISR(TIMER1_OVF_vect){
	TCNT1 = 65536-250;
	Ticks++;
}
uint8_t SPI_TransmitReceive(uint8_t data){
	return spi.TransmitReceive(data);
}
void RF_pin(uint8_t pin,uint8_t onoff){
	Bpin(pin,onoff);
}
uint32_t GetTick(){
	return Ticks;
}
ISR(TIMER2_OVF_vect){
	TCNT2 = 256-250;
	if(Soundfocus >= 0 && Soundfocus < Soundbuflen)OCR0A = pgm_read_byte_near(Sound+Soundfocus);
	else {Soundfocus = 0;DDRD &= ~0x40;TIMSK2 = 0;}Soundfocus++;
}
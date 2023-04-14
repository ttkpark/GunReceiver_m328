//-----------------giyong-giho's header--------------------

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>


#define DDR_SPI DDRB
#define DD_SS (PB2)
#define DD_MOSI (PB3)
#define DD_MISO (PB4)
#define DD_SCK (PB5)

#define delay_us(tine) _delay_us(tine)
#define delay_ms(tine) _delay_ms(tine)

uint16_t tcnt=0x00,tcnt2=0x00,ocr2 = 0x00;
class Pin {
public:
	uint8_t pin;
	volatile uint8_t*PORT;
	volatile uint8_t*DDR;
	volatile uint8_t*PIN;
	//@param port    :  0:PORTA 1:PORTB 2:PORTC ...7:PORTH
	//@param pin(0~8):  Port x (ex.PDx)
	Pin(uint8_t port, uint8_t _pin) {

		switch (port) {
#ifdef PORTA
		case 0:
			PORT = &PORTA; DDR = &DDRA; PIN = &PINA;
			break;
#endif
		case 1:
			PORT = &PORTB; DDR = &DDRB; PIN = &PINB;
			break;
#ifdef PORTC
		case 2:
			PORT = &PORTC; DDR = &DDRC; PIN = &PINC;
			break;
#endif
#ifdef PORTD
		case 3:
			PORT = &PORTD; DDR = &DDRD; PIN = &PIND;
			break;
#endif
#ifdef PORTE
		case 4:
			PORT = &PORTE; DDR = &DDRE; PIN = &PINE;
			break;
#endif
#ifdef PORTF
		case 5:
			PORT = &PORTF; DDR = &DDRF; PIN = &PINF;
			break;
#endif
#ifdef PORTG
		case 6:
			PORT = &PORTG; DDR = &DDRG; PIN = &PING;
			break;
#endif
#ifdef PORTH
		case 7:
			PORT = &PORTH; DDR = &DDRH; PIN = &PINH;
			break;
#endif
		default: PORT = &PORTB; DDR = &DDRB; PIN = &PINB; break;
		}
		pin = 1 << _pin;
	}
	//@param pin    :  Pin name (ex.PD0)
	Pin(const char* _pin) {

		switch (_pin[1]-'A') {
#ifdef PORTA
		case 0:
			PORT = &PORTA; DDR = &DDRA; PIN = &PINA;
			break;
#endif
		case 1:
			PORT = &PORTB; DDR = &DDRB; PIN = &PINB;
			break;
#ifdef PORTC
		case 2:
			PORT = &PORTC; DDR = &DDRC; PIN = &PINC;
			break;
#endif
#ifdef PORTD
		case 3:
			PORT = &PORTD; DDR = &DDRD; PIN = &PIND;
			break;
#endif
#ifdef PORTE
		case 4:
			PORT = &PORTE; DDR = &DDRE; PIN = &PINE;
			break;
#endif
#ifdef PORTF
		case 5:
			PORT = &PORTF; DDR = &DDRF; PIN = &PINF;
			break;
#endif
#ifdef PORTG
		case 6:
			PORT = &PORTG; DDR = &DDRG; PIN = &PING;
			break;
#endif
#ifdef PORTH
		case 7:
			PORT = &PORTH; DDR = &DDRH; PIN = &PINH;
			break;
#endif
		default: PORT = &PORTB; DDR = &DDRB; PIN = &PINB; break;
			
		}
		pin = 1 << (_pin[2] - '0');
	}

	void init(uint8_t inout) {
		if (inout)Output();
		else Input();
	}

	void setpin(uint8_t onoff) {
		*PORT = ((*PORT)&pin) | (onoff != 0)*pin;
	}
	void Hi() {
		*PORT |= pin;
	}
	void Lo() {
		*PORT &= ~pin;
	}
	void Input() {
		*DDR &= ~pin;
	}
	void Output() {
		*DDR |= pin;
	}
};

class SPI{
	public:
	char ss;
	SPI(){ss=0;}
	
	void Slave()
	{
		/* Set MISO output, all others input */
		DDR_SPI |= (1<<DD_MISO);
		/* Enable SPI */
		SPCR = (1<<SPE);
		
	}
	void ssend(unsigned char data)
	{
		/* Start transmission */
		SPDR = data;
		/* Wait for transmission complete */
		while(!(SPSR & (1<<SPIF)));
	}
	
	void Master(char pri)
	{
		/* Set MOSI and SCK output, all others input */
		DDR_SPI |= (1<<DD_MOSI)|(1<<DD_SCK)|ss;
		DDR_SPI &= ~(1<<DD_MISO);
		/* Enable SPI, Master, set clock rate users */
		SPCR = (1<<SPE)|(1<<MSTR)|pri;

		PORTB &= ~(ss);
	}
	
	unsigned char Receive()
	{
		while(!(SPSR & (1<<SPIF)));
		return SPDR;
	}
	unsigned char TransmitReceive(unsigned char data)
	{
		/* Start transmission */
		SPDR = data;
		/* Wait for transmission complete */
		while(!(SPSR & (1<<SPIF)));
		return SPDR;
	}
	
	void Receive(int longer,unsigned char *str_rxd)
	{
		for(int i=0;i<longer;i++)
		{
			*str_rxd = Receive();
			str_rxd++;
		}
	}
	void Receive(int longer,char *str_rxd){
		for(int i=0;i<longer;i++)
		{
			*str_rxd = Receive();
			str_rxd++;
		}
	}	
	void Receive(char *str_rxd)
	{
		char receive=1;
		while(receive)
		{
			receive = Receive();
			*str_rxd = receive;
			str_rxd++;
		}
	}
	void Receive(unsigned char *str_rxd)
	{
		unsigned char receive=1;
		while(receive)
		{
			receive = Receive();
			*str_rxd = receive;
			str_rxd++;
		}
	}
	
	void Transmit(unsigned char data)
	{
		PORTB |= ss;
		/* Start transmission */
		SPDR = data;
		/* Wait for transmission complete */
		while (!(SPSR & (1 << SPIF)));
		PORTB &= ~(ss);
	}
	void Transmit(char *str_txd)
	{
		PORTB |= ss;
		
		for(;*str_txd;str_txd++)
		{
			ssend(*str_txd);
		}
		
		PORTB &= ~(ss);
	}
	void Transmit(unsigned char *str_txd)
	{
		PORTB |= ss;
		
		for(;*str_txd;str_txd++)
		{
			Transmit(*str_txd);
		}
		
		PORTB &= ~(ss);
	}
	void Transmit(int longer,char *memory)
	{
		PORTB |= ss;
		
		for(int i=0;i<(longer-1);i++){
			Transmit(*memory);
			memory++;
		}
		
		PORTB &= ~(ss);
	}
	void Transmit(int longer,unsigned char *memory)
	{
		PORTB |= ss;
	
		for(int i=0;i<(longer-1);i++){
			Transmit(*memory);
			memory++;
		}
	
		PORTB &= ~(ss);
	}

}spi;

class UART{
	int16_t filter;
	volatile uint8_t *_UDR;
	volatile uint8_t *_UCSRA;
	volatile uint8_t *_UCSRB;
	volatile uint8_t *_UCSRC;
	volatile uint16_t *_UBRR;
	public:
	
	
	UART(
		int8_t portset=-1,	//portset pin,USARTn(0,1) (-1)->UART
		Pin tx= Pin("PD0"),		//TX pin,Port D
		Pin rx= Pin("PD1")		//RX pin,Port D
	){
		tx.Output();
		rx.Input();
		
		#ifndef UDR0
		#ifndef UDR1
		portset = -1;
		#endif
		#endif
		#ifndef UDR
		#ifndef UDR1
		portset = 0;
		#endif
		#endif
		if(portset == 0){//UART0
			#ifdef UDR0
				_UDR = &UDR0;
				_UCSRA = &UCSR0A;
				_UCSRB = &UCSR0B;
				_UCSRC = &UCSR0C;
				_UBRR = &UBRR0;
			#endif
		}
		else if(portset == 1){//UART1
			#ifdef UDR1
				_UDR = &UDR1;
				_UCSRA = &UCSR1A;
				_UCSRB = &UCSR1B;
				_UCSRC = &UCSR1C;
				_UBRR = &UBRR1;
			#endif
		}
		else{//UART
			#ifdef UDR
				_UDR = &UDR;
				_UCSRA = &UCSRA;
				_UCSRB = &UCSRB;
				_UCSRC = &UCSRC;
				_UBRR = &UBRR;
			#endif
		}
	}
	
	//@param ubrr: UBRR Register
	void setubrr(uint16_t ubrr) {
		*_UBRR = ubrr;
	}
	uint16_t getubrr() {
		return *_UBRR;
	}

	//@param mode: 0:Asynchronous USART  1:Synchronous USART  2:Master SPI
	void setmode(uint8_t mode ){
	
		if(mode == 1)		*_UCSRC = ((*_UCSRC) & 0x3F) | 0x40;//UMSEL = 1
		else if(mode == 2)	*_UCSRC = ((*_UCSRC) & 0x3F) | 0xC0;//UMSEL = 3
		else 				*_UCSRC = ((*_UCSRC) & 0x3F) | 0x00;//UMSEL = 0
	}
	
	//@param pairty: 0:None  1:Even  2:Odd
	void setPairty(uint8_t pairty) {
		if (pairty == 1)		*_UCSRC = ((*_UCSRC) & 0xCF) | 0x20;
		else if (pairty == 2)	*_UCSRC = ((*_UCSRC) & 0xCF) | 0x30;
		else					*_UCSRC = ((*_UCSRC) & 0xCF) | 0x00;
	}

	//@param stopbit: 0:1-bit  1:2-bit
	void setStopbit(uint8_t stopbit) {
		if (stopbit)	*_UCSRC = ((*_UCSRC) & 0xF7) | 0x08;
		else			*_UCSRC = ((*_UCSRC) & 0xF7) | 0x00;
	}

	//@param size: 5:5-bit  6:6-bit  7:7-bit  8:8-bit 9:9-bit
	void setSendLength(uint8_t size) {
		if (size == 5)			*_UCSRC = ((*_UCSRC) & 0xF8) | 0x00;// 5-bit
		else if (size == 6)		*_UCSRC = ((*_UCSRC) & 0xF8) | 0x02;// 6-bit
		else if (size == 7)		*_UCSRC = ((*_UCSRC) & 0xF8) | 0x04;// 7-bit
		else if (size == 9) { *_UCSRB = ((*_UCSRB) & 0xFB) | 0x04; *_UCSRC = ((*_UCSRC) & 0xF8) | 0x06;}// 9-bit
		else					*_UCSRC = ((*_UCSRC) & 0xF8) | 0x06;// 8-bit
	}
	
	//@param edge: 0:UCPOL=0  1:UCPOL=1
	void setXCKPolarty(uint8_t edge) {
		if (edge == 1)			*_UCSRC = ((*_UCSRC) & 0xF8) | 0x00;
		else					*_UCSRC = ((*_UCSRC) & 0xF8) | 0x06;
	}

	//@param _filter: filter (use Receive)
	void setfilter(int16_t _filter) {
		filter = _filter;
	}

	//@param ubrr: UBRR Register
	void setBaudrate(double baudrate) {
		if (!(*_UCSRC & 0x40 || *_UCSRC & 0x80)) {//UART Asynchronous mode
			if (*_UCSRA & 0x02)//U2X0 == true
				setubrr(F_CPU / (8 * baudrate) - 1);
			else //U2X0 == false
				setubrr(F_CPU / (16 * baudrate) - 1);
		}
		else if (*_UCSRC & 0x40 && *_UCSRC & 0x80)//MSPIM mode
			setubrr(F_CPU / (2 * baudrate) - 1);
	}
	double getBaudrate() {
		if (!(*_UCSRC & 0x40 || *_UCSRC & 0x80)) {//UART Asynchronous mode
			if (*_UCSRA & 0x02)//U2X0 == true
				return F_CPU / (8 * (*_UBRR + 1));
			else //U2X0 == false
				return F_CPU / (16 * (*_UBRR + 1));
		}
		else if (*_UCSRC & 0x40 && *_UCSRC & 0x80)//MSPIM mode
			return F_CPU / (2 * (*_UBRR + 1));
		return -1;
	}

	// Receive Complpte Interrupt (USART_RX_vect)
	void setRXInterrupt(uint8_t enable = 1) {
		*_UCSRB = ((*_UCSRB) & 0x7F) | (enable != 0) << 7;
	}
	// Transmit Complpte Interrupt (USART_TX_vect)
	void setTXInterrupt(uint8_t enable = 1) {
		*_UCSRB = ((*_UCSRB) & 0xBF) | (enable != 0) << 6;
	}
	// Data Register Empty Interrupt (USART_UDRE_vect)
	void setDREmptyInterrupt(uint8_t enable = 1) {
		*_UCSRB = ((*_UCSRB) & 0xDF) | (enable != 0) << 5;
	}
	// Receive Enable
	void setRXEnable(uint8_t enable = 1) {
		*_UCSRB = ((*_UCSRB) & 0xEF) | (enable != 0) << 4;
	}
	// Transmit Enable
	void setTXEnable(uint8_t enable = 1) {
		*_UCSRB = ((*_UCSRB) & 0xF7) | (enable != 0) << 3;
	}
	// Double the Transmission Speed Enable
	void setDoubleSpeed(uint8_t enable = 1) {
		*_UCSRA = ((*_UCSRA) & 0xFD) | (enable != 0) << 1;
	}
	// Status
	void getStatus(uint8_t *rx_fail, uint8_t *tx_fail,uint8_t *data_empty,	uint8_t *frame_error,uint8_t *data_overrun,uint8_t *pairty_error){
		if (rx_fail != (uint8_t*)0)			  *rx_fail = ((*_UCSRA & 0x80) == 0);
		if (tx_fail != (uint8_t*)0)			  *tx_fail = ((*_UCSRA & 0x40) == 0);
		if (data_empty != (uint8_t*)0)	   *data_empty = ((*_UCSRA & 0x20) != 0);
		if (frame_error != (uint8_t*)0)	  *frame_error = ((*_UCSRA & 0x10) != 0);
		if (data_overrun != (uint8_t*)0) *data_overrun = ((*_UCSRA & 0x08) != 0);
		if (pairty_error != (uint8_t*)0) *pairty_error = ((*_UCSRA & 0x04) != 0);
	}
		
	char Receive(){
		while((*_UCSRA & 0x80) == 0x00);
		return *_UDR;
	}
	char Receive(char NoDat){
		if((*_UCSRA & 0x80) == 0x00)return NoDat;
		else return *_UDR;
	}
	void Receive(int32_t longer, char *str_rxd){
		for(int i;i<longer;i++)
		{
			*str_rxd = Receive();
			str_rxd++;
		}
	}
	void Receive(int32_t longer,uint8_t *str_rxd){
		for(int i;i<longer;i++)
		{
			*str_rxd = Receive();
			str_rxd++;
		}
	}
	void Receive(uint8_t *Text){
		char receive=1;
		while(receive!=filter)
		{
			receive = Receive();
			*Text = receive;
			Text++;
		}
	}
	void Receive(char *Text){
		char receive=1;
		while(receive!=filter)
		{
			receive = Receive();
			*Text = receive;
			Text++;
		}
	}
	
	void Transmit(char txd){
		while((*_UCSRA & 0x20) == 0X00);
		*_UDR = txd;
	}
	void Transmit(char *str_txd){
		for(;*str_txd;str_txd++){
			Transmit(*str_txd);
		}
	}
	void Transmit(const char *str_txd){
		for(;*str_txd;str_txd++){
			Transmit(*str_txd);
		}
	}
	void Transmit(uint8_t *str_txd){
		for(;*str_txd;str_txd++){
			Transmit(*str_txd);
		}
}
	void Transmit(int32_t longer, char *memory){
		for(int i=0;i<(longer-1);i++){
			Transmit(*memory);
			memory++;
		}
	}

};

void Timer0_init(unsigned char pres,unsigned char cnt)
{
	TCCR0B = pres;
	TCNT0 = cnt;
	TIMSK0 = 0x01;
}
void Timer1_init(unsigned char pres,unsigned int cnt)
{
	TCCR1B = pres;
	TCNT1 = cnt;
	TIMSK1 = 0x01;
}
void Timer2_init(unsigned char pres,unsigned char cnt)
{
	TCCR2B = pres;
	TCNT2 = cnt;
	TIMSK2 = 0x01;
}
void Timer0_FastPWM_init(unsigned char WGM,unsigned char COMA,unsigned char COMB,unsigned char CS)
{
	TCCR0A = ((COMA&0x03)<<6)|((COMB&0x03)<<4)|(WGM&0x03);
	TCCR0B = ((WGM&0x04)<<3)|(CS&0x07);
}
char To36jin_out(char buf){
	char output=0;
	if('0'<= buf && '9' >= buf){output = buf-'0';}
	else if('a' <= buf && buf <= 'z'){output = buf -'a'+10;}
	else if('A' <= buf && buf <= 'Z'){output = buf -'A'+10;}
	else if(buf == 0){output = 0 ;}
	else output=buf;
	return output;
}
int ADCin(char adc_channel){
	ADMUX = adc_channel;  //2.56V 사용,adc channel = 0;
	ADCSRA = 0xe7; //adc enable,시작,perescaler = 128;
	delay_ms(5);
	while(ADCSRA&&(1 << ADIF)==0); //완료까지 대기.
	return ADC;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#define delay_us(tine) _delay_us(tine)
#define delay_ms(tine) _delay_ms(tine)
#include "nRF24l01.hpp"

int min(int a,int b){if(a>b)return b;else return a;}
int max(int a,int b){if(a>b)return a;else return b;}

__attribute__((weak)) uint8_t SPI_TransmitReceive(uint8_t data);
__attribute__((weak)) void RF_pin(uint8_t pin,uint8_t onoff);
__attribute__((weak)) uint32_t GetTick();

void RF24::csn(int level){RF_pin(csn_pin,level);}
void RF24::ce(int level){RF_pin(ce_pin,level);}
uint8_t RF24::read_register(uint8_t reg, uint8_t* buf, uint8_t len){
	uint8_t status;
	
	csn(0);
	status = SPI_TransmitReceive( R_REGISTER | ( REGISTER_MASK & reg ) );
	
	while ( len-- )
		*buf++ = SPI_TransmitReceive(0xff);
	
	csn(1);
	
	return status;
}
uint8_t RF24::read_register(uint8_t reg){	
	csn(0);
	SPI_TransmitReceive( R_REGISTER | ( REGISTER_MASK & reg ) );
	
	uint8_t result = SPI_TransmitReceive(0xff);
	
	csn(1);
	
	return result;
}
uint8_t RF24::write_register(uint8_t reg, uint8_t* buf, uint8_t len){
	uint8_t status;
	
	csn(0);
	status = SPI_TransmitReceive( W_REGISTER | ( REGISTER_MASK & reg ) );
	
	while ( len-- )
		SPI_TransmitReceive(*buf++);
	
	csn(1);
	
	return status;
}
uint8_t RF24::write_register(uint8_t reg, uint8_t value){
	uint8_t status;
	
	csn(0);
	status = SPI_TransmitReceive( W_REGISTER | ( REGISTER_MASK & reg ) );
	SPI_TransmitReceive(value);
	csn(1);
	
	return status;
}
uint8_t RF24::write_payload(void* buf, uint8_t len){
	uint8_t status;

	const uint8_t* current = (const uint8_t*)(buf);

	uint8_t data_len = min(len,payload_size);
	uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;
  
	//printf("[Writing %u bytes %u blanks]",data_len,blank_len);
  
	csn(0);
	status = SPI_TransmitReceive( W_TX_PAYLOAD );
	while ( data_len-- )
		SPI_TransmitReceive(*current++);
	while ( blank_len-- )
		SPI_TransmitReceive(0);
	csn(1);

	return status;
}
uint8_t RF24::read_payload(void* buf, uint8_t len){
	uint8_t status;
	uint8_t* current = (uint8_t*)(buf);

	uint8_t data_len = min(len,payload_size);
	uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;
  
	//printf("[Reading %u bytes %u blanks]",data_len,blank_len);
  
	csn(0);
	status = SPI_TransmitReceive( R_RX_PAYLOAD );
	while ( data_len-- )
		*current++ = SPI_TransmitReceive(0xff);
	while ( blank_len-- )
		SPI_TransmitReceive(0xff);
	csn(1);

	return status;
}
uint8_t RF24::flush_rx(void){
	uint8_t status;

	csn(0);
	status = SPI_TransmitReceive( FLUSH_RX );
	csn(1);

	return status;
}
uint8_t RF24::flush_tx(void){
	uint8_t status;

	csn(0);
	status = SPI_TransmitReceive( FLUSH_TX );
	csn(1);

	return status;
}
uint8_t RF24::get_status(void){
	uint8_t status;

	csn(0);
	status = SPI_TransmitReceive( NOP );
	csn(1);

	return status;
}
void RF24::print_status(uint8_t status){
	printf("STATUS\t\t = 0x%02x RX_DR=0x%x TX_DS=0x%x MAX_RT=0x%x RX_P_NO=0x%x TX_FULL=0x%x\r\n",
		status,
		(status & 1<<(RX_DR))?1:0,
		(status & 1<<(TX_DS))?1:0,
		(status & 1<<(MAX_RT))?1:0,
		((status >> RX_P_NO) & 0x7),
		(status & 1<<(TX_FULL))?1:0
		);
}
void RF24::print_observe_tx(uint8_t value){
	printf("OBSERVE_TX=0x%02x: POLS_CNT=0x%x ARC_CNT=0x%x\r\n",
			value,
			(value >> PLOS_CNT) & 0XF,
			(value >> ARC_CNT) & 0XF
		);
}
void RF24::print_byte_register(char* name, uint8_t reg, uint8_t qty){
	char extra_tab = strlen(name) < 8 ? '\t' : 0;
	if(extra_tab != 0)printf("%s\t%c =",name,extra_tab);
	else printf("%s\t =", name);
	
	while(qty--){
		printf(" 0x%02x",read_register(reg++));
	}
	printf("\r\n");
}
void RF24::print_address_register(char* name, uint8_t reg, uint8_t qty){
	char extra_tab = strlen(name) < 8 ? '\t' : 0;

	if (extra_tab != 0)printf("%s\t%c =", name, extra_tab);
	else printf("%s\t =", name);
	printf(" : ");
	
	while(qty--){
		uint8_t buffer[5];
		read_register(reg++,buffer,sizeof buffer);
		
		printf(" 0x");
		
		uint8_t* bufptr = buffer + sizeof buffer;
		while(--bufptr >= buffer){
			printf("%02x",*bufptr);
		}
	}
	printf("\r\n");
}
void RF24::toggle_features(void){
	csn(0);
	SPI_TransmitReceive( ACTIVATE );
	SPI_TransmitReceive( 0x73 );
	csn(1);
}
RF24::RF24(uint8_t _cepin, uint8_t _cspin){
	ce_pin=_cepin;
	csn_pin=_cspin;
	wide_band=true;
	p_variant=false;
	payload_size=32;
	ack_payload_available=false;
	dynamic_payloads_enabled=false;
	pipe0_reading_address=0;
}
void RF24::begin(void){
	ce(0);
	csn(1);
	delay_ms(5);
	write_register(SETUP_RETR,(0x4 << ARD) | (0xf << ARC));

	// Restore our default PA level
	setPALevel( RF24_PA_MAX ) ;

	// Determine if this is a p or non-p RF24 module and then
	// reset our data rate back to default value. This works
	// because a non-P variant won't allow the data rate to
	// be set to 250Kbps.
	if( setDataRate( RF24_250KBPS ) )
	{
		p_variant = true ;
	}

	// Then set the data rate to the slowest (and most reliable) speed supported by all
	// hardware.
	setDataRate( RF24_1MBPS ) ;

	// Initialize CRC and request 2-byte (16bit) CRC
	setCRCLength( RF24_CRC_16 ) ;

	// Disable dynamic payloads, to match dynamic_payloads_enabled setting
	write_register(DYNPD,0);

	// Reset current status
	// Notice reset and flush is the last thing we do
	write_register(STATUS,1<<(RX_DR) | 1<<(TX_DS) | 1<<(MAX_RT) );

	// Set up default configuration.  Callers can always change it later.
	// This channel should be universally safe and not bleed over into adjacent
	// spectrum.
	setChannel(76);

	// Flush buffers
	flush_rx();
	flush_tx();
}
void RF24::startListening(void){
	write_register(CONFIG, read_register(CONFIG) | 1<<(PWR_UP) | 1<<(PRIM_RX));
	write_register(STATUS, 1<<(RX_DR) | 1<<(TX_DS) | 1<<(MAX_RT) );
	
	// Restore the pipe0 adddress, if exists
	if (pipe0_reading_address)
	write_register(RX_ADDR_P0, (uint8_t*)(&(pipe0_reading_address)), 5);
	
	// Flush buffers
	flush_rx();
	flush_tx();
	
	// Go!
	ce(1);
	
	// wait for the radio to come up (130us actually only needed)
	delay_ms(1);
}
void RF24::stopListening(void){
	ce(0);
	flush_tx();
	flush_rx();
}
boolean RF24::write( void* buf, uint8_t len ){
	uint8_t result = false;

	// Begin the write
	startWrite(buf,len);

	// ------------
	// At this point we could return from a non-blocking write, and then call
	// the rest after an interrupt

	// Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
	// or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
	// is flaky and we get neither.

	// IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
	// if I tighted up the retry logic.  (Default settings will be 1500us.
	// Monitor the send
	uint8_t observe_tx;
	uint8_t status;
	uint32_t sent_at = GetTick();
	const uint32_t timeout = 500; //ms to wait for timeout
	do
	{
		status = read_register(OBSERVE_TX,&observe_tx,1);
		//IF_SERIAL_DEBUG(Serial.print(observe_tx,HEX));
	}
	while( ! ( status & ( 1<<(TX_DS) | 1<<(MAX_RT) ) ) && ( GetTick() - sent_at < timeout ) );

	// The part above is what you could recreate with your own interrupt handler,
	// and then call this when you got an interrupt
	// ------------

	// Call this when you get an interrupt
	// The status tells us three things
	// * The send was successful (TX_DS)
	// * The send failed, too many retries (MAX_RT)
	// * There is an ack packet waiting (RX_DR)
	uint8_t tx_ok, tx_fail;
	whatHappened(&tx_ok,&tx_fail,&(ack_payload_available));

	//printf("%u%u%u\r\n",tx_ok,tx_fail,ack_payload_available);

	result = tx_ok;
	//IF_SERIAL_DEBUG(Serial.print(result?"...OK.":"...Failed"));

	// Handle the ack packet
	if ( ack_payload_available )
	{
		ack_payload_length = getDynamicPayloadSize();
		//IF_SERIAL_DEBUG(Serial.print("[AckPacket]/"));
		//IF_SERIAL_DEBUG(Serial.println(ack_payload_length,DEC));
	}

	// Yay, we are done.

	// Power down
	powerDown();

	// Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
	flush_tx();

	return result;
}
boolean RF24::available(void){
	return available(0);
}
boolean RF24::read( void* buf, uint8_t len ){
	read_payload( buf, len );

	// was this the last of the data available?
	return read_register(FIFO_STATUS) & 1<<(RX_EMPTY);
}
void RF24::openWritingPipe(uint64_t address){
	// Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
	// expects it LSB first too, so we're good.

	write_register(RX_ADDR_P0, (uint8_t*)(&address), 5);
	write_register(TX_ADDR, (uint8_t*)(&address), 5);

	const uint8_t max_payload_size = 32;
	write_register(RX_PW_P0,min(payload_size,max_payload_size));
}
void RF24::openReadingPipe(uint8_t number, uint64_t address){
	if (number == 0)
		pipe0_reading_address = address;
	
	if (number <= 6)
	{
		// For pipes 2-5, only write the LSB
		if ( number < 2 )
			write_register(child_pipe[number], (uint8_t*)(&address), 5);
		else
			write_register(child_pipe[number], (uint8_t*)(&address), 1);
		
		write_register(child_payload_size[number],payload_size);
		
		// Note it would be more efficient to set all of the bits for all open
		// pipes at once.  However, I thought it would make the calling code
		// more simple to do it this way.
		write_register(EN_RXADDR,read_register(EN_RXADDR) | 1<<(child_pipe_enable[number]));
	}
}
void RF24::setRetries(uint8_t delay, uint8_t count){
	write_register(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}
void RF24::setChannel(uint8_t channel){
	const uint8_t max_channel = 127;
	write_register(RF_CH,min(channel,max_channel));
}
void RF24::setPayloadSize(uint8_t size){
	const uint8_t max_payload_size = 32;
	payload_size = min(size,max_payload_size);
}
uint8_t RF24::getPayloadSize(void){
  return payload_size;
}
uint8_t RF24::getDynamicPayloadSize(void){
	uint8_t result =0;
	
	csn(0);
	SPI_TransmitReceive( R_RX_PL_WID );
	result = SPI_TransmitReceive(0xff);
	csn(1);
	
	return result;
}
void RF24::enableAckPayload(void){
	// Enable dynamic payload throughout the system
	write_register(FEATURE,read_register(FEATURE) | 1<<(EN_DPL) );

	// If it didn't work, the features are not enabled
	if ( ! read_register(FEATURE) )
	{
		// So enable them and try again
		toggle_features();
		write_register(FEATURE,read_register(FEATURE) | 1<<(EN_DPL) );
	}

	//IF_SERIAL_DEBUG(printf("FEATURE=%i\r\n",read_register(FEATURE)));

	// Enable dynamic payload on all pipes
	//
	// Not sure the use case of only having dynamic payload on certain
	// pipes, so the library does not support it.
	write_register(DYNPD,read_register(DYNPD) | 1<<(DPL_P5) | 1<<(DPL_P4) | 1<<(DPL_P3) | 1<<(DPL_P2) | 1<<(DPL_P1) | 1<<(DPL_P0));

	dynamic_payloads_enabled = true;
}
void RF24::enableDynamicPayloads(void){
	// Enable dynamic payload throughout the system
	write_register(FEATURE,read_register(FEATURE) | 1<<(EN_DPL) );

	// If it didn't work, the features are not enabled
	if ( ! read_register(FEATURE) )
	{
		// So enable them and try again
		toggle_features();
		write_register(FEATURE,read_register(FEATURE) | 1<<(EN_DPL) );
	}

	//IF_SERIAL_DEBUG(printf("FEATURE=%i\r\n",read_register(FEATURE)));

	// Enable dynamic payload on all pipes
	//
	// Not sure the use case of only having dynamic payload on certain
	// pipes, so the library does not support it.
	write_register(DYNPD,read_register(DYNPD) | 1<<(DPL_P5) | 1<<(DPL_P4) | 1<<(DPL_P3) | 1<<(DPL_P2) | 1<<(DPL_P1) | 1<<(DPL_P0));

	dynamic_payloads_enabled = true;
}
boolean RF24::isPVariant(void) {
	return p_variant ;
}
void RF24::setAutoAck(boolean enable){
	if ( enable )
		write_register(EN_AA, 0x3f);
	else
		write_register(EN_AA, 0);
}
void RF24::setAutoAck( uint8_t pipe, boolean enable ) {
	if ( pipe <= 6 )
	{
		uint8_t en_aa = read_register( EN_AA ) ;
		if( enable )
		{
			en_aa |= 1<<(pipe) ;
		}
		else
		{
			en_aa &= ~(1<<pipe) ;
		}
		write_register( EN_AA, en_aa ) ;
	}
}
void RF24::setPALevel( rf24_pa_dbm_e level ) {
	uint8_t setup = read_register(RF_SETUP) ;
	setup &= ~(1<<(RF_PWR_LOW) | 1<<(RF_PWR_HIGH)) ;

	// switch uses RAM (evil!)
	if ( level == RF24_PA_MAX )
	{
		setup |= (1<<(RF_PWR_LOW) | 1<<(RF_PWR_HIGH)) ;
	}
	else if ( level == RF24_PA_HIGH )
	{
		setup |= 1<<(RF_PWR_HIGH) ;
	}
	else if ( level == RF24_PA_LOW )
	{
		setup |= 1<<(RF_PWR_LOW);
	}
	else if ( level == RF24_PA_MIN )
	{
		// nothing
	}
	else if ( level == RF24_PA_ERROR )
	{
		// On error, go to maximum PA
		setup |= (1<<(RF_PWR_LOW) | 1<<(RF_PWR_HIGH)) ;
	}

	write_register( RF_SETUP, setup ) ;
}
rf24_pa_dbm_e RF24::getPALevel( void ) {
	rf24_pa_dbm_e result = RF24_PA_ERROR ;
	uint8_t power = read_register(RF_SETUP) & (1<<(RF_PWR_LOW) | 1<<(RF_PWR_HIGH)) ;

	// switch uses RAM (evil!)
	if ( power == (1<<(RF_PWR_LOW) | 1<<(RF_PWR_HIGH)) )
	{
		result = RF24_PA_MAX ;
	}
	else if ( power == 1<<(RF_PWR_HIGH) )
	{
		result = RF24_PA_HIGH ;
	}
	else if ( power == 1<<(RF_PWR_LOW) )
	{
		result = RF24_PA_LOW ;
	}
	else
	{
		result = RF24_PA_MIN ;
	}

	return result ;
}
boolean RF24::setDataRate(rf24_datarate_e speed){
	boolean result = false;
	uint8_t setup = read_register(RF_SETUP) ;

	// HIGH and LOW '00' is 1Mbs - our default
	wide_band = false ;
	setup &= ~(1<<(RF_DR_LOW) | 1<<(RF_DR_HIGH)) ;
	if( speed == RF24_250KBPS )
	{
		// Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
		// Making it '10'.
		wide_band = false ;
		setup |= 1<<( RF_DR_LOW ) ;
	}
	else
	{
		// Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
		// Making it '01'
		if ( speed == RF24_2MBPS )
		{
			wide_band = true ;
			setup |= 1<<(RF_DR_HIGH);
		}
		else
		{
			// 1Mbs
			wide_band = false ;
		}
	}
	write_register(RF_SETUP,setup);

	// Verify our result
	if ( read_register(RF_SETUP) == setup )
	{
		result = true;
	}
	else
	{
		wide_band = false;
	}

	return result;
}
rf24_datarate_e RF24::getDataRate( void ) {
	rf24_datarate_e result ;
	uint8_t dr = read_register(RF_SETUP) & (1<<(RF_DR_LOW) | 1<<(RF_DR_HIGH));

	// switch uses RAM (evil!)
	// Order matters in our case below
	if ( dr == 1<<(RF_DR_LOW) )
	{
		// '10' = 250KBPS
		result = RF24_250KBPS ;
	}
	else if ( dr == 1<<(RF_DR_HIGH) )
	{
		// '01' = 2MBPS
		result = RF24_2MBPS ;
	}
	else
	{
		// '00' = 1MBPS
		result = RF24_1MBPS ;
	}
	return result ;
}
void RF24::setCRCLength(rf24_crclength_e length){
	uint8_t config = read_register(CONFIG) & ~( 1<<(CRCO) | 1<<(EN_CRC)) ;

	// switch uses RAM (evil!)
	if ( length == RF24_CRC_DISABLED )
	{
		// Do nothing, we turned it off above. 
	}
	else if ( length == RF24_CRC_8 )
	{
		config |= 1<<(EN_CRC);
	}
	else
	{
		config |= 1<<(EN_CRC);
		config |= 1<<( CRCO );
	}
	write_register( CONFIG, config ) ;
}
rf24_crclength_e RF24::getCRCLength(void){
	rf24_crclength_e result = RF24_CRC_DISABLED;
	uint8_t config = read_register(CONFIG) & ( 1<<(CRCO) | 1<<(EN_CRC)) ;

	if ( config & 1<<(EN_CRC ) )
	{
		if ( config & 1<<(CRCO) )
			result = RF24_CRC_16;
		else
			result = RF24_CRC_8;
	}

	return result;
}
void RF24::disableCRC( void ) {
	uint8_t disable = read_register(CONFIG) & ~(1<<EN_CRC) ;
	write_register( CONFIG, disable ) ;
}
void RF24::printDetails(void){
	print_status(get_status());
	
	print_address_register((char*)"RX_ADDR_P0-1",RX_ADDR_P0,2);
	print_byte_register((char*)"RX_ADDR_P2-5",RX_ADDR_P2,4);
	print_address_register((char*)"TX_ADDR",TX_ADDR,1);
	
	print_byte_register((char*)"RX_PW_P0-6",RX_PW_P0,6);
	print_byte_register((char*)"EN_AA",EN_AA,1);
	print_byte_register((char*)"EN_RXADDR",EN_RXADDR,1);
	print_byte_register((char*)"RF_CH",RF_CH,1);
	print_byte_register((char*)"RF_SETUP",RF_SETUP,1);
	print_byte_register((char*)"CONFIG",CONFIG,1);
	print_byte_register((char*)"DYNPD/FEATURE",DYNPD,2);
	
	printf("Data Rate\t = %s\r\n",rf24_datarate_e_str_P[getDataRate()]);
	printf("Model\t\t = %s\r\n",rf24_model_e_str_P[isPVariant()]);
	printf( "CRC Length\t = %s\r\n", rf24_crclength_e_str_P[getCRCLength()]);
	printf("PA Power\t = %s\r\n",rf24_pa_dbm_e_str_P[getPALevel()]);
}
void RF24::powerDown(void){
	write_register(CONFIG,read_register(CONFIG) & ~(1<<PWR_UP));
}
void RF24::powerUp(void) {
	write_register(CONFIG,read_register(CONFIG) | (1<<PWR_UP));
}
boolean RF24::available(uint8_t* pipe_num){
	uint8_t status = get_status();
	
	// Too noisy, enable if you really want lots o data!!
	//IF_SERIAL_DEBUG(print_status(status));
	
	char result = ( status & 1<<(RX_DR) );
	
	if (result)
	{
		// If the caller wants the pipe number, include that
		if ( pipe_num )
			*pipe_num = ( status >> RX_P_NO ) & 0x7;
		
		// Clear the status bit
		
		// ??? Should this REALLY be cleared now?  Or wait until we
		// actually READ the payload?
		
		write_register(STATUS,1<<(RX_DR) );
		
		// Handle ack payload receipt
		if ( status & 1<<(TX_DS) )
		{
			write_register(STATUS,1<<(TX_DS));
		}
	}

	return result;
}
void RF24::startWrite( void* buf, uint8_t len ){
	// Transmitter power-up
	write_register(CONFIG, ( read_register(CONFIG) | 1<<(PWR_UP) ) & ~(1<<PRIM_RX) );
	delay_ms(1);

	// Send the payload
	write_payload( buf, len );

	// Allons!
	ce(1);
	delay_ms(1);
	ce(0);
}
void RF24::writeAckPayload(uint8_t pipe, void* buf, uint8_t len){
	const uint8_t* current = (const uint8_t*)(buf);

	csn(0);
	SPI_TransmitReceive( W_ACK_PAYLOAD | ( pipe & 0x7 ) );
	const uint8_t max_payload_size = 32;
	uint8_t data_len = min(len,max_payload_size);
	while ( data_len-- )
	SPI_TransmitReceive(*current++);

	csn(1);
}
boolean RF24::isAckPayloadAvailable(void){
	boolean result = ack_payload_available;
	ack_payload_available = false;
	return result;
}
void RF24::whatHappened(boolean* tx_ok,boolean* tx_fail,boolean* rx_ready){
	uint8_t status = write_register(STATUS,1<<(RX_DR) | 1<<(TX_DS) | 1<<(MAX_RT) );

	// Report to the user what happened
	*tx_ok = status & 1<<(TX_DS);
	*tx_fail = status & 1<<(MAX_RT);
	*rx_ready = status & 1<<(RX_DR);
}
boolean RF24::testCarrier(void){
	return ( read_register(CD) & 1 );
}
boolean RF24::testRPD(void) {
	return ( read_register(RPD) & 1 ) ;
}
boolean RF24::isValid(void) { return ce_pin != 0xff && csn_pin != 0xff; } 
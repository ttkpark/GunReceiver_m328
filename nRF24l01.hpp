//----------------AVR nRF24l01 header v1 2018-02-20----------------
//{
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD	    0x1C
#define FEATURE	    0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5	    5
#define DPL_P4	    4
#define DPL_P3	    3
#define DPL_P2	    2
#define DPL_P1	    1
#define DPL_P0	    0
#define EN_DPL	    2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

/* Non-P omissions */
#define LNA_HCURR   0

/* P model memory Map */
#define RPD         0x09

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2

#ifdef true
#define true      1
#endif
#ifdef false
#define false     0
#endif
#ifdef null
#define null      0
#endif
//}
//

static const char rf24_datarate_e_str_0[] = "1MBPS";
static const char rf24_datarate_e_str_1[] = "2MBPS";
static const char rf24_datarate_e_str_2[] = "250KBPS";
static const char * const rf24_datarate_e_str_P[] = {
  rf24_datarate_e_str_0,
  rf24_datarate_e_str_1,
  rf24_datarate_e_str_2,
};
static const char rf24_model_e_str_0[] = "nRF24L01";
static const char rf24_model_e_str_1[] = "nRF24L01+";
static const char * const rf24_model_e_str_P[] = {
  rf24_model_e_str_0,
  rf24_model_e_str_1,
};
static const char rf24_crclength_e_str_0[] = "Disabled";
static const char rf24_crclength_e_str_1[] = "8 bits";
static const char rf24_crclength_e_str_2[] = "16 bits" ;
static const char * const rf24_crclength_e_str_P[] = {
  rf24_crclength_e_str_0,
  rf24_crclength_e_str_1,
  rf24_crclength_e_str_2,
};
static const char rf24_pa_dbm_e_str_0[] = "PA_MIN";
static const char rf24_pa_dbm_e_str_1[] = "PA_LOW";
static const char rf24_pa_dbm_e_str_2[] = "LA_MED";
static const char rf24_pa_dbm_e_str_3[] = "PA_HIGH";
static const char * const rf24_pa_dbm_e_str_P[] = { 
  rf24_pa_dbm_e_str_0,
  rf24_pa_dbm_e_str_1,
  rf24_pa_dbm_e_str_2,
  rf24_pa_dbm_e_str_3,
};
static const uint8_t child_pipe[] = {
  RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_payload_size[] = {
  RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};
static const uint8_t child_pipe_enable[] = {
  ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

typedef enum { RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e ;
typedef enum { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS } rf24_datarate_e;
typedef enum { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 } rf24_crclength_e;

#define boolean uint8_t

int min(int a,int b);
int max(int a,int b);

class RF24 
{
public:
	uint8_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
	uint8_t csn_pin; /**< SPI Chip select */
	boolean wide_band; /* 2Mbs data rate in use? */
	boolean p_variant; /* False for RF24L01 and true for RF24L01P */
	uint8_t payload_size; /**< Fixed size of payloads */
	boolean ack_payload_available; /**< Whether there is an ack payload waiting */
	boolean dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */ 
	uint8_t ack_payload_length; /**< Dynamic size of pending ack payload. */
	uint64_t pipe0_reading_address; /**< Last address set on pipe 0 for reading. */

	void csn(int mode);
	void ce(int level);
	uint8_t read_register(uint8_t reg, uint8_t* buf, uint8_t len);
	uint8_t read_register(uint8_t reg);
	uint8_t write_register(uint8_t reg, uint8_t* buf, uint8_t len);
	uint8_t write_register(uint8_t reg, uint8_t value);
	uint8_t write_payload(void* buf, uint8_t len);
	uint8_t read_payload(void* buf, uint8_t len);
	uint8_t flush_rx(void);
	uint8_t flush_tx(void);
	uint8_t get_status(void);
	void print_status(uint8_t status);
	void print_observe_tx(uint8_t value);
	void print_byte_register(char* name, uint8_t reg, uint8_t qty);
	void print_address_register(char* name, uint8_t reg, uint8_t qty);
	void toggle_features(void);
	RF24(uint8_t _cepin, uint8_t _cspin);
	void begin(void);
	void startListening(void);
	void stopListening(void);
	boolean write( void* buf, uint8_t len );
	boolean available(void);
	boolean read( void* buf, uint8_t len );
	void openWritingPipe(uint64_t address);
	void openReadingPipe(uint8_t number, uint64_t address);
	void setRetries(uint8_t delay, uint8_t count);
	void setChannel(uint8_t channel);
	void setPayloadSize(uint8_t size);
	uint8_t getPayloadSize(void);
	uint8_t getDynamicPayloadSize(void);
	void enableAckPayload(void);
	void enableDynamicPayloads(void);
	boolean isPVariant(void) ;
	void setAutoAck(boolean enable);
	void setAutoAck( uint8_t pipe, boolean enable ) ;
	void setPALevel( rf24_pa_dbm_e level ) ;
	rf24_pa_dbm_e getPALevel( void ) ;
	boolean setDataRate(rf24_datarate_e speed);
	rf24_datarate_e getDataRate( void ) ;
	void setCRCLength(rf24_crclength_e length);
	rf24_crclength_e getCRCLength(void);
	void disableCRC( void ) ;
	void printDetails(void);
	void powerDown(void);
	void powerUp(void) ;
	boolean available(uint8_t* pipe_num);
	void startWrite( void* buf, uint8_t len );
	void writeAckPayload(uint8_t pipe, void* buf, uint8_t len);
	boolean isAckPayloadAvailable(void);
	void whatHappened(boolean* tx_ok,boolean* tx_fail,boolean* rx_ready);
	boolean testCarrier(void);
	boolean testRPD(void) ;
	boolean isValid(void);
};
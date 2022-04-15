// TWI STATUS CODES
#define TWI_START 0x08
#define TWI_RESTART 0x10
#define TWI_T_ADDR_ACK 0x18
#define TWI_T_ADDR_NACK 0x20
#define TWI_T_DATA_ACK 0x28
#define TWI_T_DATA_NACK 0x30
#define TWI_ARB_LOST 0x38
#define TWI_R_ADDR_ACK 0x40
#define TWI_R_ADDR_NACK 0x48
#define TWI_R_DATA_ACK 0x50
#define TWI_R_DATA_NACK 0x58

void twi_init();

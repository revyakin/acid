
#include "modbus.h"
#include "usart.h"

/**************************************************************************************************
 * PUBLIC VARIABLE
 *************************************************************************************************/
uint16_t modbus_reg_table[MB_REGS_NUMBER];

/**************************************************************************************************
 * PRIVATE MACRO
 *************************************************************************************************/
#define MB_TRUE             1
#define MB_FALSE            0

#ifndef MB_OUR_ADDRESS
  #define MB_OUR_ADDRESS    1
#endif

#define MB_FRAME_MAX        256
#define MB_FRAME_MIN        4

#define MB_PDU_MIN          4
#define MB_PDU_ADDR_OFF     0
#define MB_PDU_FUNC_OFF     1
#define MB_PDU_DATA_OFF     2

#define MB_PDU_FUNC_READ_ADDR_OFF           ( 0 )
#define MB_PDU_FUNC_READ_REGCNT_OFF         ( 2 )
#define MB_PDU_FUNC_READ_RESP_NBYTES_OFF    ( 0 )
#define MB_PDU_FUNC_READ_RESP_REGS_OFF      ( 1 )
#define MB_FUNC_READ_REGCNT_MAX             125

#define MB_PDU_FUNC_WRITE_ADDR_OFF          ( 0 )
#define MB_PDU_FUNC_WRITE_VAL_OFF           ( 2 )

#define HI(x)   ((uint8_t) ((x) >> 8))
#define LO(x)   ((uint8_t) (x))

/**************************************************************************************************
 * PRIVATE TYPES
 *************************************************************************************************/
enum MB_FSM_STATE {
    MB_STATE_INIT,
    MB_STATE_IDLE,
    MB_STATE_RECEIVE,
    MB_STATE_PROCESS,
    MB_STATE_TRANSMITT
};

enum MB_ERROR {
    MB_ENOERROR  = 0,
    MB_EFUNCTION = 1,
    MB_EADDRESS  = 2,
    MB_EVALUE    = 3
};

/**************************************************************************************************
 * PRIVATE VARIABLE
 *************************************************************************************************/

/* Nibble lookup table 
 */
static const uint16_t crc_table[16] = {
    0x0000,0xCC01,0xD801,0x1400,0xF001,0x3C00,0x2800,0xE401,
    0xA001,0x6C00,0x7800,0xB401,0x5000,0x9C01,0x8801,0x4400 
};

static uint8_t  mb_state;
static uint8_t  mb_frame[ MB_FRAME_MAX ];
static uint8_t  mb_frame_len;
static uint8_t  mb_received;

/**************************************************************************************************
 * FUNCTION PROTOTYPES
 *************************************************************************************************/
void     modbus_init                     ( void );
void     modbus_fsm                      ( void );
void     modbus_process                  ( uint8_t p_frame[], uint8_t * frame_len  );
uint8_t  modbus_funcReadHoldingRegister  ( uint8_t p_data[],  uint8_t * data_len   );
uint8_t  modbus_funcWriteSingleRegister  ( uint8_t p_data[],  uint8_t * data_len   );
uint16_t modbus_CRC16                    ( uint8_t pData[],   uint8_t   dataLength );
void     modbus_receiverEnable           ( void );
void     modbus_receiverDisable          ( void );
uint8_t  modbus_bytesReceived            ( void );
uint8_t  modbus_frameTimeout             ( void );
uint8_t  modbus_transmissionComplete     ( void );
void     modbus_flush                    ( void );


void modbus_init( void )
{
    mb_state     = MB_STATE_INIT;
    mb_received  = 0;
    mb_frame_len = 0;
}

void modbus_fsm ( void )
{
    
    switch ( mb_state ) {

        case MB_STATE_INIT:
            
            /* Start reception frame
             */
            modbus_receiverEnable();

            /* Goto next state
             */
            mb_state = MB_STATE_IDLE;

            break;

        case MB_STATE_IDLE:
            
            /* If received first byte (few bytes) goto MBSTATE_RECEIVE state
             */
            if ( modbus_bytesReceived() > 0 ) {
               mb_state = MB_STATE_RECEIVE;
            }

            break;

        case MB_STATE_RECEIVE:

            /* IDLE character mean that we received frame
             */
            if ( modbus_frameTimeout() ) {
                                
                /* Disable receiver during the message processing
                 */
                modbus_receiverDisable();

                mb_received = modbus_bytesReceived();

                /* Check frame length. 
                 * It must be larger than MODBUS_MIN_FRAME.
                 */
                if (mb_received < MB_FRAME_MIN) {
                    mb_state = MB_STATE_INIT;
                    break;
                }

                /* Check device address. If this frame
                 * not addressed to us, then start next reception.
                 */
                if (mb_frame[ MB_PDU_ADDR_OFF ] != MB_OUR_ADDRESS) {
                    mb_state = MB_STATE_INIT;
                    break;
                }

                /* Check frame CRC. If frame is broken -
                 * start new reception
                 */
                if (modbus_CRC16(mb_frame, mb_received) != 0) {
                    mb_state = MB_STATE_INIT;
                    break;
                }

                /* Start frame processing
                 */
                mb_state = MB_STATE_PROCESS;
            }

            break;

        case MB_STATE_PROCESS:

            /* Process data
            */
            mb_frame_len = mb_received;
            modbus_process( mb_frame, &mb_frame_len );

            /* Start response transmission
             */
            modbus_flush();
            mb_state = MB_STATE_TRANSMITT;

            break;

        case MB_STATE_TRANSMITT:

            /* If transmission complete - start new reception
             */
            if (modbus_transmissionComplete()) {
                mb_state = MB_STATE_INIT;
            }

            break;
    }
}

void modbus_process( uint8_t p_frame[], uint8_t * frame_len )
{
    uint8_t  func     =   p_frame[ MB_PDU_FUNC_OFF ];
    uint8_t  *p_data  = & p_frame[ MB_PDU_DATA_OFF ];
    uint8_t  data_len =   *frame_len - MB_PDU_MIN;
    uint16_t crc16;
    uint8_t  status = MB_ENOERROR;

    switch ( func ) {
        case 0x03:
            status = modbus_funcReadHoldingRegister( p_data, &data_len );
            break;

        case 0x06:
            status = modbus_funcWriteSingleRegister( p_data, &data_len );
            break;

        default:
            status = MB_EFUNCTION;
            break;
    }

    if (status != MB_ENOERROR) {

        /* Exception response
         */
        *p_data   = status;
         data_len = 1;

    }

    /* Add header to frame
     */
    p_frame[ MB_PDU_ADDR_OFF ] = MB_OUR_ADDRESS;
    p_frame[ MB_PDU_FUNC_OFF ] = (status == MB_ENOERROR) ? func : (func | 0x80);

    /* Add frame CRC
     */
    crc16 = modbus_CRC16( p_frame, data_len + MB_PDU_MIN - 2 );

    p_frame[ MB_PDU_DATA_OFF + data_len ]     = LO( crc16 );
    p_frame[ MB_PDU_DATA_OFF + data_len + 1 ] = HI( crc16 );

    /* Return frame length
     */
    *frame_len = data_len + MB_PDU_MIN;
    
    return;
}

uint8_t modbus_funcReadHoldingRegister( uint8_t p_data[], uint8_t * data_len )
{
    uint8_t     *ptr;
    uint16_t    start_address;
    uint16_t    regs_count;

    /* First register
     */
    start_address  = p_data[ MB_PDU_FUNC_READ_ADDR_OFF ] << 8;
    start_address |= p_data[ MB_PDU_FUNC_READ_ADDR_OFF + 1];

    /* Registers count
     */
    regs_count  = p_data[ MB_PDU_FUNC_READ_REGCNT_OFF ] << 8;
    regs_count |= p_data[ MB_PDU_FUNC_READ_REGCNT_OFF + 1];

    /* Check bounds
     */
    if ((start_address + regs_count) > MB_REGS_NUMBER) {
        return MB_EADDRESS;
    }

    if (regs_count > MB_FUNC_READ_REGCNT_MAX) {
        return MB_EVALUE;
    }

    /* Create responce
     */
    p_data[ MB_PDU_FUNC_READ_RESP_NBYTES_OFF ] = 2 * regs_count;
    
    ptr = & p_data[ MB_PDU_FUNC_READ_RESP_REGS_OFF ];

    for ( int i = 0; i < regs_count; i++ ) {
        *ptr       = HI( modbus_reg_table[ start_address + i ] );            
        *(ptr + 1) = LO( modbus_reg_table[ start_address + i ] );            

        ptr += 2;
    }

    *data_len = 2 * regs_count + 1;

    return MB_ENOERROR;
}

uint8_t modbus_funcWriteSingleRegister ( uint8_t p_data[], uint8_t * data_len )
{
    uint16_t addr;
    uint16_t val;

    /* Register address
     */
    addr  = p_data[ MB_PDU_FUNC_WRITE_ADDR_OFF ] << 8;
    addr |= p_data[ MB_PDU_FUNC_WRITE_ADDR_OFF + 1 ];
    
    /* Register value
     */
    val  = p_data[ MB_PDU_FUNC_WRITE_VAL_OFF ] << 8;
    val |= p_data[ MB_PDU_FUNC_WRITE_VAL_OFF + 1 ];

    /* Check bounds
     */
    if (addr > (MB_REGS_NUMBER - 1)) {
        return MB_EADDRESS;
    }

    modbus_reg_table[ addr ] = val;

    /* We no need to change p_data to send response. Response must be the same as request.
     */

    return MB_ENOERROR;
}

uint16_t modbus_CRC16( uint8_t pData[], uint8_t dataLength )
{
    uint16_t crc = 0xffff;

    while (dataLength--) {
        crc ^= *pData;

        crc = (crc >> 4) ^ crc_table[crc & 0x0f];
        crc = (crc >> 4) ^ crc_table[crc & 0x0f];
       
        pData++;
    }

    return crc;
}

void modbus_receiverEnable( void )
{
    usart_rx_dma_start(mb_frame, MB_FRAME_MAX);
}

void modbus_receiverDisable( void )
{
    usart_rx_dma_stop();
}

uint8_t modbus_bytesReceived( void )
{
    return MB_FRAME_MAX - DMA1_Channel3->CNDTR;
}

uint8_t modbus_frameTimeout( void )
{
    if (USART3->SR & USART_SR_IDLE) {
        USART3->SR &= ~USART_SR_IDLE;
        return MB_TRUE;
    }
    
    return MB_FALSE;
}

uint8_t modbus_transmissionComplete( void )
{
    if (USART3->SR & USART_SR_TC) {
        USART3->SR &= ~USART_SR_TC;
        return MB_TRUE;
    }
    
    return MB_FALSE;
}

void modbus_flush ( void )
{
    usart_tx_dma_start( mb_frame, mb_frame_len );
}


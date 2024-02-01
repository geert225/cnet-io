#include <drivers/profibus/profibus.h>
#include <libopencm3/stm32/usart.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// Ident Nummer DP Slave. Arbitrarily chosen. This ID not found in my .GSD library.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define IDENT_HIGH_BYTE       0xC0
#define IDENT_LOW_BYTE        0xDE

#define SAP_OFFSET            128   // Service Access Point Adress Offset
#define BROADCAST_ADD         127
#define DEFAULT_ADD           126   // Delivery address

#define SD1                   0x10  // Telegram without data field
#define SD2                   0x68  // Data telegram variable
#define SD3                   0xA2  // Data telegram fixed
#define SD4                   0xDC  // Token
#define SC                    0xE5  // Short acknowledgment
#define ED                    0x16  // End

/* FC Request */
#define FDL_STATUS            0x09  // SPS: Status query
#define SRD_HIGH              0x0D  // SPS: Set outputs, read inputs
#define FCV_                  0x10
#define FCB_                  0x20
#define REQUEST_              0x40

/* FC Response */
#define FDL_STATUS_OK         0x00  // SLA: OK
#define DATA_LOW              0x08  // SLA: (Data low) Send data inputs
#define DIAGNOSE              0x0A  // SLA: (Data high) Diagnosis pending
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Service Access Points (DP Slave) MS0
///////////////////////////////////////////////////////////////////////////////////////////////////
#define SAP_SET_SLAVE_ADR     55  // Master sets slave address, slave responds with SC
#define SAP_RD_INP            56  // Master requests input data, slave sends input data
#define SAP_RD_OUTP           57  // Master requests output data, slave sends output data
#define SAP_GLOBAL_CONTROL    58  // Master Control, Slave Does not answer
#define SAP_GET_CFG           59  // Master requests config., Slave sends configuration
#define SAP_SLAVE_DIAGNOSIS   60  // Master requests diagnosis, slave sends diagnosis Daten
#define SAP_SET_PRM           61  // Master sends parameters, slave sends SC
#define SAP_CHK_CFG           62  // Master sends configuration, Slave sends SC
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// SAP: Global Control (Data Master)
///////////////////////////////////////////////////////////////////////////////////////////////////
#define CLEAR_DATA_           0x02
#define UNFREEZE_             0x04
#define FREEZE_               0x08
#define UNSYNC_               0x10
#define SYNC_                 0x20
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// SAP: Diagnosis (Answer slave)
///////////////////////////////////////////////////////////////////////////////////////////////////
/* Status Byte 1 */
#define STATION_NOT_EXISTENT_ 0x01
#define STATION_NOT_READY_    0x02
#define CFG_FAULT_            0x04
#define EXT_DIAG_             0x08  // Extended diagnosis available
#define NOT_SUPPORTED_        0x10
#define INV_SLAVE_RESPONSE_   0x20
#define PRM_FAULT_            0x40
#define MASTER_LOCK           0x80

/* Status Byte 2 */
#define STATUS_2_DEFAULT      0x04
#define PRM_REQ_              0x01
#define STAT_DIAG_            0x02
#define WD_ON_                0x08
#define FREEZE_MODE_          0x10
#define SYNC_MODE_            0x20
#define DEACTIVATED_          0x80

/* Status Byte 3 */
#define DIAG_SIZE_OK          0x00
#define DIAG_SIZE_ERROR       0x80

/* Address */
#define MASTER_ADD_DEFAULT    0xFF

/* Extended diagnosis (EXT_DIAG_ = 1) */
#define EXT_DIAG_TYPE_        0xC0  // Bit 6-7 ist Diagnose Typ
#define EXT_DIAG_GERAET       0x00  // Wenn Bit 7 und 6 = 00, dann Geraetebezogen
#define EXT_DIAG_KENNUNG      0x40  // Wenn Bit 7 und 6 = 01, dann Kennungsbezogen
#define EXT_DIAG_KANAL        0x80  // Wenn Bit 7 und 6 = 10, dann Kanalbezogen

#define EXT_DIAG_BYTE_CNT_    0x3F  // Bit 0-5 sind Anzahl der Diagnose Bytes
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// SAP: Set Parameters Request (Data master)
///////////////////////////////////////////////////////////////////////////////////////////////////
/* Command */
#define LOCK_SLAVE_           0x80  // Slave fuer andere Master gesperrt
#define UNLOCK_SLAVE_         0x40  // Slave fuer andere Master freigegeben
#define ACTIVATE_SYNC_        0x20
#define ACTIVATE_FREEZE_      0x10
#define ACTIVATE_WATCHDOG_    0x08

/* DPV1 Status Byte 1 */
#define DPV1_MODE_            0x80
#define FAIL_SAVE_MODE_       0x40
#define PUBLISHER_MODE_       0x20
#define WATCHDOG_TB_1MS       0x04

/* DPV1 Status Byte 2 */
#define PULL_PLUG_ALARM_      0x80
#define PROZESS_ALARM_        0x40
#define DIAGNOSE_ALARM_       0x20
#define VENDOR_ALARM_         0x10
#define STATUS_ALARM_         0x08
#define UPDATE_ALARM_         0x04
#define CHECK_CONFIG_MODE_    0x01

/* DPV1 Status Byte 3 */
#define PARAMETER_CMD_ON_     0x80
#define ISOCHRON_MODE_ON_     0x10
#define PARAMETER_BLOCK_      0x08
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// SAP: Check Config Request (Data Master)
///////////////////////////////////////////////////////////////////////////////////////////////////
#define CFG_DIRECTION_        0x30  // Bit 4-5 is direction. 01 = input, 10 = output, 11 = input / output
#define CFG_INPUT             0x10  // Input
#define CFG_OUTPUT            0x20  // Output
#define CFG_INPUT_OUTPUT      0x30  // Input/Output
#define CFG_SPECIAL           0x00  // Special format if more than 16/32 bytes are to be transferred

#define CFG_KONSISTENZ_       0x80  // Bit 7 is consistency. 0 = byte or word, 1 = over entire module
#define CFG_KONS_BYTE_WORT    0x00  // Byte or word
#define CFG_KONS_MODUL        0x80  // Modul

#define CFG_WIDTH_            0x40  // Bit 6 is IO width. 0 = byte (8bit), 1 = word (16bit)
#define CFG_BYTE              0x00  // Byte
#define CFG_WORD              0x40  // Word

/* Compact format */
#define CFG_BYTE_CNT_         0x0F  // Bit 0-3 are number of bytes or words. 0 = 1 byte, 1 = 2 bytes etc.

/* Special format */
#define CFG_SP_DIRECTION_     0xC0  // Bit 6-7 is direction. 01 = input, 10 = output, 11 = input / output
#define CFG_SP_VOID           0x00  // Empty space
#define CFG_SP_INPUT          0x40  // Input
#define CFG_SP_OUTPUT         0x80  // Output
#define CFG_SP_INPUT_OPTPUT   0xC0  // Input/Output

#define CFG_SP_VENDOR_CNT_    0x0F  // Bits 0-3 are the number of manufacturer-specific bytes. 0 = none

/* Special format / length byte */
#define CFG_SP_BYTE_CNT_      0x3F  // Bit 0-5 are number of bytes or words. 0 = 1 byte, 1 = 2 bytes etc.

void profibus_init(profibusHandle_t * handle, uint32_t uartPeriph){
    handle->uartPeriph = uartPeriph;
    //Start = 0 ; Parity = EVEN ; Stop = 1
    //usart_set_baudrate(handle->uartPeriph, );
    usart_set_stopbits(handle->uartPeriph, 1);
    usart_set_databits(handle->uartPeriph, 8);
    usart_set_parity(handle->uartPeriph, USART_PARITY_EVEN);
    usart_set_flow_control(handle->uartPeriph, USART_FLOWCONTROL_NONE);
    usart_set_mode(handle->uartPeriph, USART_MODE_TX_RX);
    usart_enable(handle->uartPeriph);
}

void profibus_statusupdate(profibusHandle_t * handle){
    
}

void profibus_cycleupdate(profibusHandle_t * handle){
    
}

void profibus_sendPacket(profibusHandle_t * handle){
    
}

void profibus_recivePacket(profibusHandle_t * handle){
    
}

void profibus_handleTxPacket(profibusHandle_t * handle, uint8_t telegramType){
    
}

void profibus_handleRxPacket(profibusHandle_t * handle){
    uint32_t minMessageSize = 1;
    if(handle->rxBufferLength < minMessageSize) return;

    uint8_t telegramType = handle->rxBuffer[0];
    uint8_t destAddr = 0;
    uint8_t srcAddr = 0;
    uint8_t funcCode = 0;
    uint8_t FCSData = 0;
    uint8_t PUDSize = 0;
    uint8_t SAPDest = 0;
    uint8_t SAPSrc = 0;
    uint8_t responseAddr = 0;

    //telegram type
    switch (telegramType)
    {
    case SD1: //empty telegram with no data attached = 6 bytes
        minMessageSize = 6;
        if(handle->rxBufferLength != minMessageSize) return;
        destAddr = handle->rxBuffer[1];
        srcAddr= handle->rxBuffer[2];
        funcCode = handle->rxBuffer[3];
        FCSData = handle->rxBuffer[4];
        if (!profibus_checkDest(handle, destAddr)) return;
        if (profibus_checksum(&handle->rxBuffer[1], 3) != FCSData) return;
        break;

    case SD2: //data telegram with variable data length
        minMessageSize = 6;
        if(handle->rxBufferLength != minMessageSize) return;
        PUDSize = handle->rxBuffer[1];
        destAddr = handle->rxBuffer[4];
        srcAddr = handle->rxBuffer[5];
        funcCode = handle->rxBuffer[6];
        FCSData = handle->rxBuffer[PUDSize + 4];
        if (!profibus_checkDest(handle, destAddr)) return;
        if (profibus_checksum(&handle->rxBuffer[4], PUDSize) != FCSData) return;
        break;

    case SD3: //data telegram with 5 bytes data, max. 11 bytes
        minMessageSize = 11;
        if(handle->rxBufferLength != minMessageSize) return;
        PUDSize = 8;              // DA + SA + FC + PDU
        destAddr = handle->rxBuffer[1];
        srcAddr = handle->rxBuffer[2];
        funcCode = handle->rxBuffer[3];
        FCSData = handle->rxBuffer[9];
        if (!profibus_checkDest(handle, destAddr)) return;
        if (profibus_checksum(&handle->rxBuffer[1], 8) != FCSData) break;
        break;

    case SD4: // Token with 3 Byte Data
        minMessageSize = 3;
        if(handle->rxBufferLength != minMessageSize) return;
        destAddr = handle->rxBuffer[1];
        srcAddr = handle->rxBuffer[2];
        if (!profibus_checkDest(handle, destAddr)) return;
        break;

    default:
        return;
    }

    //handle data from telegram


    if(destAddr == handle->address){
        if(funcCode == (REQUEST_ + FDL_STATUS)){
            profibus_handleTxPacket(handle, )
            //profibus_send_CMD(SD1, FDL_STATUS_OK, 0, &uart_buffer[0], 0);
            return;
        }
        //cyclic io
        if(funcCode == (REQUEST_ + FCV_ + SRD_HIGH) || funcCode == (REQUEST_ + FCV_ + FCB_ + SRD_HIGH)){
          //update internal io buffer
          //if(){
          for(int i = 0; i < PROFIOUTPUTMODULESIZE; i++){
            handle->outputModuleData[i] = handle->rxBuffer[7 + i];
            if(handle->freeze == 0 && handle->sync == 0){
              //update function
            }
          }
          //update data to controller
          if (handle->diag != 0){
            //profibus_send_CMD(SD2, DIAGNOSE, 0, &uart_buffer[7], 0);  // Request a diagnosis
          }else{
            //profibus_send_CMD(SD2, DATA_LOW, 0, &uart_buffer[7], Input_Data_size);  // send data
          }
          return;
        }
    }
}

bool profibus_checkDest(profibusHandle_t * handle, uint8_t addr){
    return (addr == handle->address || addr == (handle->address + SAP_OFFSET) || addr == BROADCAST_ADD || addr == (BROADCAST_ADD + SAP_OFFSET));
}

uint8_t profibus_checksum(int8_t *data, uint16_t length)
{
  uint8_t csum = 0;
  while(length--)
  {
    csum += data[length];
  }
  return csum;
}
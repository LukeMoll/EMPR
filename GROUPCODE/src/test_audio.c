#include <lpc17xx_i2s.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_spi.h>
#include <lpc17xx_gpio.h>
#include <stddef.h>
#include <libempr/status.h>
#include <libempr/spi.h>

/** definitions from i2s_polling **/
#define I2S_BUFFER_SIZE                     0x1000UL
#define I2S_BUFFER_SRC                  LPC_AHBRAM1_BASE
// #define I2S_BUFFER_DST                  (I2S_BUFFER_SRC+0x1000UL)
#define I2S_BUFFER_DST                  (I2S_BUFFER_SRC+I2S_BUFFER_SIZE)

// volatile uint8_t  I2STXDone = 0;
// volatile uint8_t  I2SRXDone = 0;
volatile uint32_t *I2STXBuffer = (uint32_t*)(I2S_BUFFER_SRC);
volatile uint32_t *I2SRXBuffer = (uint32_t *)(I2S_BUFFER_DST);
volatile uint32_t I2SWriteLength = 0;
// volatile uint32_t I2SReadLength = 0;

/* definitionsfrom spi_test_working */
// /CS on 0.16 (MBed pin 14)
#define CS_PORT_NUM 0
#define CS_PIN_NUM 16
SPI_CFG_Type SPI_ConfigStruct;

#define SPI_DATABIT_SIZE                16
#define SPI_BUFFER_SIZE                 0x5 
uint16_t SPI_Tx_Buf[SPI_BUFFER_SIZE];
uint16_t SPI_Rx_Buf[SPI_BUFFER_SIZE];


void I2S_Buffer_Init(void);
void setup_i2s(void);
void SPI_transmit(void);
void SPI_send(uint8_t, uint16_t);
void SPI_transmit_mbedos(void);
void I2S_transmit_loop(void);

int main(void) {
    spi_init();
    spi_cs_init(CS_PORT_NUM, CS_PIN_NUM);
    status_code(1);

    SPI_transmit_mbedos();
    status_code(2);

    setup_i2s();
    // I2S_Buffer_Init(); //(this is already done in setup_i2s - why do we need it twice?)
    status_code(3);
    I2S_transmit_loop();
    return 1;
}

void I2S_Buffer_Init(void) {
        uint32_t i;

        // for (i = 0; i < I2S_BUFFER_SIZE; i++) {
        //         I2STXBuffer[i] = i+1;
        //         I2SRXBuffer[i] = 0;
        // }
        for(i=0; i < I2S_BUFFER_SIZE; i++) {
            // I2STXBuffer[i] = (i & 1 << 8) == 0 ? 0x1FFF : 0x000F;
            I2STXBuffer[i] = i >> 10;
        }

}

void setup_i2s(void) {
    I2S_MODEConf_Type I2S_ClkConfig;
    I2S_CFG_Type I2S_ConfigStruct;
    PINSEL_CFG_Type PinCfg;

    I2S_Buffer_Init();

    /* Pin configuration:
        * Assign:      - P0.4 as I2SRX_CLK
        *                      - P0.5 as I2SRX_WS
        *                      - P0.6 as I2SRX_SDA
        *                      - P0.7 as I2STX_CLK
        *                      - P0.8 as I2STX_WS
        *                      - P0.9 as I2STX_SDA
        */
    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Pinnum = 4;
    PinCfg.Portnum = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 5;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 6;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 7;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 8;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 9;
    PINSEL_ConfigPin(&PinCfg);

    I2S_Init(LPC_I2S);

    I2S_ConfigStruct.wordwidth = I2S_WORDWIDTH_16;
    // I2S_ConfigStruct.mono = I2S_STEREO;
    I2S_ConfigStruct.mono = I2S_MONO;
    I2S_ConfigStruct.stop = I2S_STOP_ENABLE;
    I2S_ConfigStruct.reset = I2S_RESET_ENABLE;
    I2S_ConfigStruct.ws_sel = I2S_MASTER_MODE;
    I2S_ConfigStruct.mute = I2S_MUTE_DISABLE;
    I2S_Config(LPC_I2S,I2S_TX_MODE,&I2S_ConfigStruct);

    I2S_ConfigStruct.ws_sel = I2S_SLAVE_MODE;
    I2S_Config(LPC_I2S,I2S_RX_MODE,&I2S_ConfigStruct);

    /* Clock Mode Config*/
    I2S_ClkConfig.clksel = I2S_CLKSEL_FRDCLK;
    I2S_ClkConfig.fpin = I2S_4PIN_DISABLE;
    I2S_ClkConfig.mcena = I2S_MCLK_DISABLE;
    I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_TX_MODE);
    I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_RX_MODE);

    /* Set up frequency and bit rate*/
    I2S_FreqConfig(LPC_I2S, 44100, I2S_TX_MODE);
    I2S_SetBitRate(LPC_I2S, 0, I2S_RX_MODE);
    I2S_SetBitRate(LPC_I2S, 15, I2S_TX_MODE); //why is it RX mode? Aren't we transmitting?

    I2S_Start(LPC_I2S);
}


void SPI_send(uint8_t reg_addr, uint16_t reg_data) {
    reg_addr = reg_addr & 0b01111111;
    reg_data = reg_data & 0b111111111;
    uint16_t tx_data = (reg_addr << 9) | reg_data;
    
    spi_cs_set(CS_PORT_NUM, CS_PIN_NUM);
    spi_send_byte((tx_data >> 8) & 0xFF);
    spi_send_byte(tx_data & 0xFF);
    spi_cs_clear();
}

void SPI_transmit_mbedos(void) {
    // reset();                                //TLV resets
    SPI_send(0b0001111, 0); // Expected: 0x1e00 
    
    // power(0x07);                            //Power Up the TLV320, but not the MIC, ADC and LINE
    SPI_send(0b0000110, 7); // Expected: 0x0c07
    
    // format(16, STEREO);                     //16Bit I2S protocol format, STEREO
    // SPI_send(0b0000111, 0b001100000); 
    SPI_send(0b0000111, 0b000000010); 
    
    // frequency(44100);                       //Default sample frequency is 44.1kHz
    SPI_send(0b0001000, 0x20); // Expected: 0x1020
    
    // bypass(false);                          //Do not bypass device
    SPI_send(0b0000100, 0b000010010); // Expected: 0x0812

    // mute(false);                            //Not muted
    SPI_send(0b0000101, 0); // Expected: 0x0a00

    // activateDigitalInterface_();            //The digital part of the chip is active
    SPI_send(0b0001001, 0b000000001); // Expected: 0x1201

    // outputVolume(0.7, 0.7);
    SPI_send(0b0000010, 0b011010111); // Expected: 0x04ff
    SPI_send(0b0000011, 0b011010111); // Expected: 0x06ff
    /*
        just to recap, that's
         0x1e00
         0x0c07
         0x0e60
         0x1020
         0x0812
         0x0a00
         0x1201
         0x04ff
         0x06ff
    */
   
}

uint8_t wavetick = 0;

uint32_t triangle(uint8_t wavetick) {
    wavetick = wavetick & 0b01111111;
    if(wavetick <= 32) { // wavetick in interval [0,32] -> [2^,]
        return (wavetick << 26) + 0x80000000UL;
    }
    else if(wavetick <= 96) {
        return 0x180000000UL - (wavetick << 26);
    }
    else {
        return (wavetick << 26) - 0x180000000UL;
    }
}

uint32_t sawtooth(uint8_t wavetick) {
    wavetick = wavetick & 0b01111111;
    return wavetick << 25;
}

void I2S_transmit_loop(void) {
    while(1) {
        I2S_Send(LPC_I2S,sawtooth(wavetick++));
        // I2S_Send(LPC_I2S,I2STXBuffer[I2SWriteLength]);
        // I2SWriteLength +=1;
        // if(I2SWriteLength == I2S_BUFFER_SIZE) { // I2STXDone = 1;
        //         return;
        //         I2SWriteLength = 0;
        // }
    }
}
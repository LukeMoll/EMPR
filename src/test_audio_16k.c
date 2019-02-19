#include <lpc17xx_i2s.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_spi.h>
#include <lpc17xx_gpio.h>
#include <stddef.h>
#include <stdbool.h>
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

#define AR_LHPV 0b0000010
#define AR_RHPV 0b0000011
#define AR_APC  0b0000100
#define AR_DPC  0b0000101
#define AR_PDC  0b0000110
#define AR_DAIF 0b0000111
#define AR_SRC  0b0001000
#define AR_DIA  0b0001001
/**
 * Which bitrate to use (how many bits/word)
 * Not sure if IWL = 32 means 2 16 bit words or 2 32 bit words for stereo
 */
#define A_DAIF_16B 0b0000
#define A_DAIF_20B 0b0100
#define A_DAIF_24B 0b1000
#define A_DAIF_32B 0b1100

/**
 * Data format
 * MSBLA: MSB first, left-aligned
 * MSBRA: MSB first, right aligned
 * I2S: I2S format, MSB first, left-1 aligned
 * DSP: Weird TI DSP format (don't use) 
 */
#define A_DAIF_MSBLA 0b00
#define A_DAIF_MSBRA 0b01
#define A_DAIF_I2S   0b10
#define A_DAIF_DSP   0b11

/**
 * Power Down Control
 * See datasheet page 23
 * In this library 1 is ON , 0 is OFF!
 */
#define A_PDC_DEV 0b10000000
#define A_PDC_CLK 0b1000000
#define A_PDC_OSC 0b100000
#define A_PDC_OUT 0b10000
#define A_PDC_DAC 0b1000
#define A_PDC_ADC 0b100
#define A_PDC_MIC 0b10
#define A_PDC_LIN 0b1

/**
 * Sample rate control
 * MCLK seems to be measured at 12.2MHz
 * A_SR_<ADC freq>_<DAC freq>
 */
#define A_SR_96_96 0b0111
#define A_SR_48_48 0b0000
#define A_SR_32_32 0b0110
#define A_SR_8_8   0b0011
#define A_SR_48_8  0b0001
#define A_SR_8_48  0b0010


void I2S_Buffer_Init(void);
void setup_i2s(void);
void SPI_transmit(void);
void AR_send(uint8_t, uint16_t);
void SPI_transmit_mbedos(void);
void I2S_transmit_loop(void);

void audio_reset(void);
void audio_volume(uint8_t vol);
void audio_powerctl(uint8_t bits);
void audio_fmt(uint8_t bits);
void audio_smp(uint8_t rate, bool divide);
void audio_mute(bool enabled);

int main(void) {
    spi_init();
    spi_cs_init(CS_PORT_NUM, CS_PIN_NUM);
    status_code(1);

    AR_init_16k();
    status_code(2);

    setup_i2s();
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
    status_code(10);

    I2S_ConfigStruct.wordwidth = I2S_WORDWIDTH_8;
    I2S_ConfigStruct.mono = I2S_STEREO;
    // I2S_ConfigStruct.mono = I2S_MONO;
    I2S_ConfigStruct.stop = I2S_STOP_ENABLE;
    I2S_ConfigStruct.reset = I2S_RESET_ENABLE;
    I2S_ConfigStruct.ws_sel = I2S_MASTER_MODE;
    I2S_ConfigStruct.mute = I2S_MUTE_DISABLE;
    I2S_Config(LPC_I2S,I2S_TX_MODE,&I2S_ConfigStruct);
    status_code(11);

    I2S_ConfigStruct.ws_sel = I2S_SLAVE_MODE;
    I2S_Config(LPC_I2S,I2S_RX_MODE,&I2S_ConfigStruct);
    status_code(12);

    /* Clock Mode Config*/
    I2S_ClkConfig.clksel = I2S_CLKSEL_FRDCLK;
    I2S_ClkConfig.fpin = I2S_4PIN_DISABLE;
    I2S_ClkConfig.mcena = I2S_MCLK_ENABLE;
    I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_TX_MODE);
    I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_RX_MODE);
    status_code(13);

    /* Set up frequency and bit rate*/
    I2S_FreqConfig(LPC_I2S, 16000, I2S_TX_MODE);
    I2S_SetBitRate(LPC_I2S, 0, I2S_RX_MODE);
    I2S_SetBitRate(LPC_I2S, 7, I2S_TX_MODE); //why is it RX mode? Aren't we transmitting?
    status_code(14);

    I2S_Start(LPC_I2S);
    status_code(15);
}


void AR_send(uint8_t reg_addr, uint16_t reg_data) {
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
    AR_send(0b0001111, 0); // Expected: 0x1e00 
    
    // power(0x07);                            //Power Up the TLV320, but not the MIC, ADC and LINE
    AR_send(0b0000110, 7); // Expected: 0x0c07
    
    // format(16, STEREO);                     //16Bit I2S protocol format, STEREO
    // AR_send(0b0000111, 0b001100000); 
    AR_send(0b0000111, 0b000000010); 
    
    // frequency(44100);                       //Default sample frequency is 44.1kHz
    AR_send(0b0001000, 0x20); // Expected: 0x1020
    
    // bypass(false);                          //Do not bypass device
    AR_send(0b0000100, 0b000010010); // Expected: 0x0812

    // mute(false);                            //Not muted
    AR_send(0b0000101, 0); // Expected: 0x0a00

    // activateDigitalInterface_();            //The digital part of the chip is active
    AR_send(0b0001001, 0b000000001); // Expected: 0x1201

    // outputVolume(0.7, 0.7);
    AR_send(0b0000010, 0b011010111); // Expected: 0x04ff
    AR_send(0b0000011, 0b011010111); // Expected: 0x06ff
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

void AR_init_16k(void) {
    audio_reset();
    audio_powerctl(
        A_PDC_DEV | 
        A_PDC_OUT |
        A_PDC_DAC
    );
    audio_fmt(A_DAIF_16B | A_DAIF_I2S);
    audio_smp(A_SR_32_32, true);
    bool sidetone = true;
    AR_send(AR_APC, 
        (sidetone ? 0b111100000 : 0) |
        0b10000 // DAC selected, bypass disabled
    );
    audio_mute(false);
    AR_send(AR_DIA, 1);
    audio_volume(60);

    // bypass (false)
    AR_send(0b0000100, 0b000010010); // Expected: 0x0812
    // mute(false);                            //Not muted
    AR_send(0b0000101, 0); // Expected: 0x0a00
    // activateDigitalInterface_();            //The digital part of the chip is active
    AR_send(0b0001001, 0b000000001);

}

void audio_reset(void) {
    AR_send(0b0001111, 0);
}

void audio_volume(uint8_t vol) {
    vol = (vol + 0b0110000) & 0b1111111;
    AR_send(AR_LHPV, vol);
    AR_send(AR_RHPV, vol);

}

void audio_powerctl(uint8_t bits) {
    AR_send(~AR_PDC, bits);
}

void audio_fmt(uint8_t bits) {
    bits = bits & 0b1111;
    bool master = false,
         lrs = false,
         lrp = false;
    AR_send(AR_DAIF, 
        (bits) | 
        ((lrp ? 1 : 0) << 4) |
        ((lrs ? 1 : 0) << 5) |
        ((master ? 1 : 0) << 6)
    );
    
}

void audio_smp(uint8_t rate, bool divideIn) {
    rate = rate & 0b1111;
    bool divideOut = false;
    AR_send(AR_SRC, 
        (rate << 2) | // [3:0]<<2 = [5:2]
        ((divideIn ? 1 : 0) << 6 )|
        ((divideOut ? 1 : 0) << 7)
    );
}

void audio_mute(bool enabled) {
    AR_send(AR_DPC, enabled ? 1<<3:0);
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
        uint8_t byte = (wavetick++) & 0xFF;

        I2S_Send(LPC_I2S,byte);
        // I2S_Send(LPC_I2S,I2STXBuffer[I2SWriteLength]);
        // I2SWriteLength +=1;
        // if(I2SWriteLength == I2S_BUFFER_SIZE) { // I2STXDone = 1;
        //         return;
        //         I2SWriteLength = 0;
        // }
    }
}
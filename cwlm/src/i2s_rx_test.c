#include <lpc17xx_i2s>
#include <lpc_types.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_spi.h>
#include <sem.h>

uint32_t bytes[16];
#define CS_PORT_NUM 0
#define CS_PIN_NUM 16
SPI_CFG_Type SPI_ConfigStruct;
#define SPI_DATABIT_SIZE                16
#define SPI_BUFFER_SIZE                 0x5 
uint16_t SPI_Tx_Buf[SPI_BUFFER_SIZE];
uint16_t SPI_Rx_Buf[SPI_BUFFER_SIZE];

I2S_CFG_Type i2s_ConfigStruct;

void setup_spi(void);
void CS_Init(void)
void SPI_Buffer_Init(void);
void SPI_transmit_mbedos(void); 
void SPI_send(uint8_t reg_addr, uint16_t reg_data)
void i2s_receive_loop();

int main(void) {

  setup_spi();
  SPI_Buffer_Init();
  
  setup_i2s();
  
  SPI_transmit_mbedos();
  i2s_receive_loop();
  
  return(1);
  
}

void setup_i2s(void) {
    
    /* Pin configuration:
        * Assign:      - P0.4 as I2SRX_CLK
        *                      - P0.5 as I2SRX_WS
        *                      - P0.6 as I2SRX_SDA
        *                      - P0.7 as I2STX_CLK
        *                      - P0.8 as I2STX_WS
        *                      - P0.9 as I2STX_SDA
        */
        
    PINSEL_CFG_Type PinCfg;
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
    
    I2S_CFG_Type i2s_ConfigStruct;
    i2s_ConfigStruct.wordwidth = I2S_WORDWIDTH_32;
    i2s_ConfigStruct.mono = I2S_MONO;
    i2s_ConfigStruct.stop = I2S_STOP_DISABLE;
    i2s_ConfigStruct.reset = I2S_RESET_DISABLE;
    i2s_ConfigStruct.ws_sel = I2S_MASTER_MODE;
    i2s_ConfigStruct.mute = I2S_MUTE_ENABLE;
    
    I2S_Config(LPC_I2S, I2S_RX_MODE, &i2s_ConfigStruct)
    
    I2S_ConfigStruct.ws_sel = I2S_SLAVE_MODE;
    I2S_Config(LPC_I2S,I2S_TX_MODE,&I2S_ConfigStruct);
    
    I2S_FreqConfig(LPC_I2S, 44100, I2S_RX_MODE);
    I2S_SetBitRate(LPC_I2S, 0, I2S_TX_MODE);
    I2S_SetBitRate(LPC_I2S, 31, I2S_RX_MODE);
    
    I2S_MODECONFIG_Type ClkConfig;
    I2S_ClkConfig.clksel = I2S_CLKSEL_MCLK;
    I2S_ClkConfig.fpin = I2S_4PIN_ENABLE;
    I2S_ClkConfig.mcena = I2S_MCLK_DISABLE;
    I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_TX_MODE);
    I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_RX_MODE);
}

void setup_spi(void) {
    PINSEL_CFG_Type PinCfg;
    /*
        * Initialize SPI pin connect
        * P0.15 - SCK;
        * P0.16 - SSEL - used as GPIO
        * P0.17 - MISO
        * P0.18 - MOSI
        */
    PinCfg.Funcnum = 3;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = 0;
    PinCfg.Pinnum = 15;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 17;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 18;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 16;
    PinCfg.Funcnum = 0;
    PINSEL_ConfigPin(&PinCfg);


    SPI_ConfigStruct.CPHA = SPI_CPHA_FIRST;
    SPI_ConfigStruct.CPOL = SPI_CPOL_LO;
    // SPI_ConfigStruct.ClockRate = 2000000;
       SPI_ConfigStruct.ClockRate = 2000000; // in Hz
    SPI_ConfigStruct.DataOrder = SPI_DATA_MSB_FIRST;
    SPI_ConfigStruct.Databit = SPI_DATABIT_SIZE;
    SPI_ConfigStruct.Mode = SPI_MASTER_MODE;
    // Initialize SPI peripheral with parameter given in structure above
    SPI_Init(LPC_SPI, &SPI_ConfigStruct);

    SPI_Buffer_Init();
    CS_Init();
    // CS_Force(0);
}

void CS_Init(void) {
        GPIO_SetDir(CS_PORT_NUM, (1<<CS_PIN_NUM), 1);
        GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
}

void SPI_Buffer_Init(void) {
    uint16_t ctrl_addr;
    uint16_t ctrl_data;

    ctrl_addr = (0b0001111) << 9;
    ctrl_data = 0b000000000; /**
            Reset
        */
    SPI_Tx_Buf[0] = ctrl_addr | ctrl_data; // expected 1e00

    ctrl_addr = (0b0000100) << 9;
    ctrl_data = 0b000010010; /** Analog path ctrl
            // Sidetone enabled @ 0dB
            Sidetone DISABLED
            DAC selected
            Bypass disabled
            Input select: line
            Mic muted
            Mic boost 0dB
        */
    SPI_Tx_Buf[1] = ctrl_addr | ctrl_data; // expected 0812

    ctrl_addr = (0b0000101) << 9;
    ctrl_data = 0b000000001; /**
            DAC Soft mute disabled
            De-emphasis control disabled
            ADC HPF disabled
        */
    SPI_Tx_Buf[2] = ctrl_addr | ctrl_data; // expected 0a01

    ctrl_addr = (0b0000111) << 9;
    ctrl_data = 0b000000010; /**
            Slave mode
            DAC LR normal
            Input 32-bit length
            Data format I2S
        */
    SPI_Tx_Buf[3] = ctrl_addr | ctrl_data; // expected 0e02

    ctrl_addr = (0b0001001) << 9;
    ctrl_data = 0b000000001; /**
            Digital interface activated
        */

    SPI_Tx_Buf[4] = ctrl_addr | ctrl_data; // expected 1201
}

void SPI_transmit_mbedos(void) {
    // reset();                                //TLV resets
    SPI_send(0b0001111, 0); // Expected: 0x1e00 
    
    // power(0x07);                            //Power Up the TLV320, but not the OUT, DAC and LINE
    SPI_send(0b0000110, 25); // Expected: 0x0c19
    
    // format(16, STEREO);                     //16Bit I2S protocol format, STEREO
    // SPI_send(0b0000111, 0b001100000); 
    SPI_send(0b0000111, 0b000001100); 
    
    // frequency(44100);                       //Default sample frequency is 44.1kHz
    SPI_send(0b0001000, 0x20); // Expected: 0x1020

    // mute(false);                            //Not muted
    SPI_send(0b0000101, 0); // Expected: 0x0a00

    // activateDigitalInterface_();            //The digital part of the chip is active
    SPI_send(0b0001001, 0b000000001); // Expected: 0x1201
    
    // analogPathControl_(); //sets the default for analogue input DAC off, Bypass enabled, MIC normal, Input MIC
    SPI_send(0b0000100, 12) //Expected: 0x080c 

    // outputVolume(0.7, 0.7);
    SPI_send(0b0000010, 0b011010111); // Expected: 0x04ff
    SPI_send(0b0000011, 0b011010111); // Expected: 0x06ff
    /*
        just to recap, that's
         0x1e00
         0x0c19
         0x0e60
         0x1020
         0x0a00
         0x1201
         0x080c
         0x04ff
         0x06ff
    */
}

void SPI_send(uint8_t reg_addr, uint16_t reg_data) {
    reg_addr = reg_addr & 0b01111111;
    reg_data = reg_data & 0b111111111;
    uint32_t tmp;
    uint16_t tx_data = (reg_addr << 9) | reg_data;
    SPI_DATA_SETUP_Type xferConfig;
    xferConfig.tx_data = &tx_data;
    xferConfig.rx_data = &tmp;
    xferConfig.length = 2;
    CS_Force(0);
    SPI_ReadWrite(LPC_SPI, &xferConfig, SPI_TRANSFER_POLLING);
    CS_Force(1);
}

void i2s_receive_loop(void) {
    uint8_t index = 0;
    while(1) {
        if(index < 16) {
            bytes[index] = I2S_Receive(LPC_I2S);
            index ++
        }
        else {
            index = 0;
        }
    }
    return;
}

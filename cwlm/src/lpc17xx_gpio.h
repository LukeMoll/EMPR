/***********************************************************************/
/* Peripheral group ----------------------------------------------------------- */
#ifndef LPC17XX_GPIO_H_
#define LPC17XX_GPIO_H_

/* Includes ------------------------------------------------------------------- */
#include "LPC17xx.h"
#include "lpc_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* Public Macros -------------------------------------------------------------- */
#define GPIO0_Byte      ((GPIO_Byte_TypeDef *)(LPC_GPIO0_BASE))

#define GPIO1_Byte      ((GPIO_Byte_TypeDef *)(LPC_GPIO1_BASE))

#define GPIO2_Byte      ((GPIO_Byte_TypeDef *)(LPC_GPIO2_BASE))

#define GPIO3_Byte      ((GPIO_Byte_TypeDef *)(LPC_GPIO3_BASE))

#define GPIO4_Byte      ((GPIO_Byte_TypeDef *)(LPC_GPIO4_BASE))


#define GPIO0_HalfWord  ((GPIO_HalfWord_TypeDef *)(LPC_GPIO0_BASE))

#define GPIO1_HalfWord  ((GPIO_HalfWord_TypeDef *)(LPC_GPIO1_BASE))

#define GPIO2_HalfWord  ((GPIO_HalfWord_TypeDef *)(LPC_GPIO2_BASE))

#define GPIO3_HalfWord  ((GPIO_HalfWord_TypeDef *)(LPC_GPIO3_BASE))

#define GPIO4_HalfWord  ((GPIO_HalfWord_TypeDef *)(LPC_GPIO4_BASE))

/* Public Types --------------------------------------------------------------- */
typedef struct {
        __IO uint8_t FIODIR[4];         
           uint32_t RESERVED0[3];       
        __IO uint8_t FIOMASK[4];        
        __IO uint8_t FIOPIN[4];         
        __IO uint8_t FIOSET[4];         
        __O  uint8_t FIOCLR[4];         
} GPIO_Byte_TypeDef;


typedef struct {
        __IO uint16_t FIODIRL;          
        __IO uint16_t FIODIRU;          
           uint32_t RESERVED0[3];       
        __IO uint16_t FIOMASKL;         
        __IO uint16_t FIOMASKU;         
        __IO uint16_t FIOPINL;          
        __IO uint16_t FIOPINU;          
        __IO uint16_t FIOSETL;          
        __IO uint16_t FIOSETU;          
        __O  uint16_t FIOCLRL;          
        __O  uint16_t FIOCLRU;          
} GPIO_HalfWord_TypeDef;

/* Public Functions ----------------------------------------------------------- */
/* GPIO style ------------------------------- */
void GPIO_SetDir(uint8_t portNum, uint32_t bitValue, uint8_t dir);
void GPIO_SetValue(uint8_t portNum, uint32_t bitValue);
void GPIO_ClearValue(uint8_t portNum, uint32_t bitValue);
uint32_t GPIO_ReadValue(uint8_t portNum);
void GPIO_IntCmd(uint8_t portNum, uint32_t bitValue, uint8_t edgeState);
FunctionalState GPIO_GetIntStatus(uint8_t portNum, uint32_t pinNum, uint8_t edgeState);
void GPIO_ClearInt(uint8_t portNum, uint32_t bitValue);

/* FIO (word-accessible) style ------------------------------- */
void FIO_SetDir(uint8_t portNum, uint32_t bitValue, uint8_t dir);
void FIO_SetValue(uint8_t portNum, uint32_t bitValue);
void FIO_ClearValue(uint8_t portNum, uint32_t bitValue);
uint32_t FIO_ReadValue(uint8_t portNum);
void FIO_SetMask(uint8_t portNum, uint32_t bitValue, uint8_t maskValue);
void FIO_IntCmd(uint8_t portNum, uint32_t bitValue, uint8_t edgeState);
FunctionalState FIO_GetIntStatus(uint8_t portNum, uint32_t pinNum, uint8_t edgeState);
void FIO_ClearInt(uint8_t portNum, uint32_t pinNum);

/* FIO (halfword-accessible) style ------------------------------- */
void FIO_HalfWordSetDir(uint8_t portNum, uint8_t halfwordNum, uint16_t bitValue, uint8_t dir);
void FIO_HalfWordSetMask(uint8_t portNum, uint8_t halfwordNum, uint16_t bitValue, uint8_t maskValue);
void FIO_HalfWordSetValue(uint8_t portNum, uint8_t halfwordNum, uint16_t bitValue);
void FIO_HalfWordClearValue(uint8_t portNum, uint8_t halfwordNum, uint16_t bitValue);
uint16_t FIO_HalfWordReadValue(uint8_t portNum, uint8_t halfwordNum);

/* FIO (byte-accessible) style ------------------------------- */
void FIO_ByteSetDir(uint8_t portNum, uint8_t byteNum, uint8_t bitValue, uint8_t dir);
void FIO_ByteSetMask(uint8_t portNum, uint8_t byteNum, uint8_t bitValue, uint8_t maskValue);
void FIO_ByteSetValue(uint8_t portNum, uint8_t byteNum, uint8_t bitValue);
void FIO_ByteClearValue(uint8_t portNum, uint8_t byteNum, uint8_t bitValue);
uint8_t FIO_ByteReadValue(uint8_t portNum, uint8_t byteNum);

#ifdef __cplusplus
}
#endif

#endif /* LPC17XX_GPIO_H_ */

/* --------------------------------- End Of File ------------------------------ */
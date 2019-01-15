/***********************************************************************/
/* Peripheral group ----------------------------------------------------------- */
#ifndef LPC17XX_RIT_H_
#define LPC17XX_RIT_H_

/* Includes ------------------------------------------------------------------- */
#include "LPC17xx.h"
#include "lpc_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* Private Macros ------------------------------------------------------------- */
/* --------------------- BIT DEFINITIONS -------------------------------------- */
/*********************************************************************/
#define RIT_CTRL_INTEN  ((uint32_t) (1))

#define RIT_CTRL_ENCLR  ((uint32_t) _BIT(1))

#define RIT_CTRL_ENBR   ((uint32_t) _BIT(2))

#define RIT_CTRL_TEN    ((uint32_t) _BIT(3))

#define PARAM_RITx(n)   (((uint32_t *)n)==((uint32_t *)LPC_RIT))

/* Public Functions ----------------------------------------------------------- */
/* RIT Init/DeInit functions */
void RIT_Init(LPC_RIT_TypeDef *RITx);
void RIT_DeInit(LPC_RIT_TypeDef *RITx);

/* RIT config timer functions */
void RIT_TimerConfig(LPC_RIT_TypeDef *RITx, uint32_t time_interval);

/* Enable/Disable RIT functions */

//seriously, the fuck is up with LPC_RIT_TypeDef? wtf is it? where is it
void RIT_TimerClearCmd(LPC_RIT_TypeDef *RITx, FunctionalState NewState);
void RIT_Cmd(LPC_RIT_TypeDef *RITx, FunctionalState NewState);
void RIT_TimerDebugCmd(LPC_RIT_TypeDef *RITx, FunctionalState NewState);

/* RIT Interrupt functions */
IntStatus RIT_GetIntStatus(LPC_RIT_TypeDef *RITx);

#ifdef __cplusplus
}
#endif

#endif /* LPC17XX_RIT_H_ */

/* --------------------------------- End Of File ------------------------------ */
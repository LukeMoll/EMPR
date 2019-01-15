/***********************************************************************/
/* Peripheral group ----------------------------------------------------------- */
#ifndef LPC17XX_SYSTICK_H_
#define LPC17XX_SYSTICK_H_

/* Includes ------------------------------------------------------------------- */
#include "LPC17xx.h"
#include "lpc_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* Private Macros ------------------------------------------------------------- */
/*********************************************************************/
#define ST_CTRL_ENABLE          ((uint32_t)(1<<0))
#define ST_CTRL_TICKINT         ((uint32_t)(1<<1))
#define ST_CTRL_CLKSOURCE       ((uint32_t)(1<<2))
#define ST_CTRL_COUNTFLAG       ((uint32_t)(1<<16))

/*********************************************************************/
#define ST_RELOAD_RELOAD(n)             ((uint32_t)(n & 0x00FFFFFF))

/*********************************************************************/
#define ST_RELOAD_CURRENT(n)    ((uint32_t)(n & 0x00FFFFFF))

/*********************************************************************/
#define ST_CALIB_TENMS(n)               ((uint32_t)(n & 0x00FFFFFF))
#define ST_CALIB_SKEW                   ((uint32_t)(1<<30))
#define ST_CALIB_NOREF                  ((uint32_t)(1<<31))

#define CLKSOURCE_EXT                   ((uint32_t)(0))
#define CLKSOURCE_CPU                   ((uint32_t)(1))

/* Public Functions ----------------------------------------------------------- */
void SYSTICK_InternalInit(uint32_t time);
void SYSTICK_ExternalInit(uint32_t freq, uint32_t time);

void SYSTICK_Cmd(FunctionalState NewState);
void SYSTICK_IntCmd(FunctionalState NewState);
uint32_t SYSTICK_GetCurrentValue(void);
void SYSTICK_ClearCounterFlag(void);

#ifdef __cplusplus
}
#endif


#endif /* LPC17XX_SYSTICK_H_ */

/* --------------------------------- End Of File ------------------------------ */
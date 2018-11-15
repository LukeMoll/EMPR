#ifndef EMPR_PWM
#define EMPR_PWM
#include <stdint.h>
void PWM_setup_single_edge(uint8_t channel, uint32_t period, uint32_t length, uint32_t prescale);
void PWM_start(void);
#define PWM_PORT 2  //stays the same whichever channel we're using
#define PWM_FUNCNUM 1 //stays the same whichever channel we're using
#endif

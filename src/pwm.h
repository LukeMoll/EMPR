#ifndef EMPR_PWM
#define EMPR_PWM
#include <stdint.h>
void PWM_setup_timer(uint8_t channel, uint8_t prescale);
void PWM_setup_period(uint32_t period);
void PWM_setup_single_edge(uint8_t channel, uint32_t length);
PWM_setup_dual_edge(uint8_t channel, uint32_t startval, uint32_t endval);
PWM_setup_dual_edge(uint8_t channel, uint32_t startval, uint32_t endval);
void PWM_start(void);
#define PWM_PORT 2  //stays the same whichever channel we're using
#define PWM_FUNCNUM 1 //stays the same whichever channel we're using
#endif

#ifndef __SG90_H
#define __SG90_H

#define sg90_set_angle(angle) TIM1->CCR4= (uint32_t)(((float)(angle))/180*2000+500)

void sg90_init(void);

#endif 

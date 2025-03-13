#ifndef __ASM_FUNCTION_H
#define __ASM_FUNCTION_H

void asm_enable_fpu(void); //使能FPU
void asm_enable_TCM(void); //使能TCM
void asm_enable_TCM_re_rmw(void);//使能TCM的重写和读改写

#endif


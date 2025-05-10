            AREA    |.text|, CODE, READONLY
            thumb
            ALIGN
                
;变量定义
ITCMCR      EQU 0xE000EF90 
DTCMCR      EQU 0xE000EF94
    
;/*使能FPU*/
;//配置型函数,不出栈入栈了,有问题加上
asm_enable_fpu  PROC
            EXPORT asm_enable_fpu
            ;DCI 0xb403           ;push {r0,r1}
            DCI.W 0xf64e5088     ;LDR R0,=0xE000ED88
            DCI.W 0xf2ce0000
            DCI.W 0xf44f0170     ;MOV R1,#0xF00000
            DCI 0x6001           ;STR R1,[R0];
            ;DCI 0xBC03           ;pop {r0,r1}
            DCI.W 0xf3bf8f4f     ;DSB 
            DCI 0x4770           ;bx lr
            
            ENDP
                
;/*使能TCM*/
;//H7_core.h有替代其的宏
;//配置型函数,不出栈入栈了,有问题加上
;//实际上系统会自动使能
asm_enable_TCM PROC
            EXPORT asm_enable_TCM
            LDR r11, =ITCMCR
            LDR r0, [r11]
            ORR r0, r0, #0x1 ; Set ITCMCR.EN field
            STR r0, [r11]
            LDR r11, =DTCMCR
            LDR r0, [r11]
            ORR r0, r0, #0x1 ; Set DTCMCR.EN field
            STR r0, [r11]
            DSB
            ISB
            DCI 0x4770       ;bx lr
            ENDP
                
;/*使能TCM的重写和读改写*/
;//H7_core.h有替代其的宏
;//配置型函数,不出栈入栈了,有问题加上
;//注意这函数变量标号用的上一个函数的
;//系统不会自动使能re位
asm_enable_TCM_re_rmw PROC
            EXPORT asm_enable_TCM_re_rmw
            LDR r11, =ITCMCR
            LDR r0, [r11]
            ORR r0, r0, #0x1:SHL:1 ; Set ITCMCR.RMW field
            ORR r0, r0, #0x1:SHL:2 ; Set ITCMCR.RETEN field
            STR r0, [r11]
            LDR r11, =DTCMCR
            LDR r0, [r11]
            ORR r0, r0, #0x1:SHL:1 ; Set DTCMCR.RMW field
            ORR r0, r0, #0x1:SHL:2 ; Set DTCMCR.RETEN field
            STR r0, [r11]
            DSB
            ISB
            DCI 0x4770       ;bx lr
            ENDP
                
                
            ALIGN 
            END
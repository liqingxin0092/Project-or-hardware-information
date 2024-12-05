                PRESERVE8
                THUMB
; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
__Vectors       DCD     0
                DCD     Reset_Handler              ; Reset Handler
                AREA    |.text|, CODE, READONLY    
; Reset handler
Reset_Handler   PROC
                ;enable GPIOC
                ldr r0,=0x40021018
                ldr r1,[r0]
                orr r1,r1,#(1<<4)
                str r1 ,[r0]
                ;set GPIOC13 as output
                ldr r0,=0x40011004
                ldr r1,[r0]
                orr r1,r1,#(1<<20)
                str r1,[r0]
                ldr r2,=0x4001100c
loop
                ;set GPIOC13 output high
                ldr r1,[r2]
                orr r1,r1,#(1<<13)
                str r1,[r2]
                ldr r0,=1000000
                BL delay1
                ;set GPIOC13 output low
                ldr r1,[r2]
                bic r1,r1,#(1<<13)
                str r1,[r2]
                ldr r0,=1000000
                BL delay1
                b loop
delay1           
                subs r0,r0,#1
                BNE delay1
                bx lr
                ENDP  
				END
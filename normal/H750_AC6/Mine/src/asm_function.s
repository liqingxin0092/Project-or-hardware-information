            AREA    |.text|, CODE, READONLY
            thumb
            ALIGN
                
;��������
ITCMCR      EQU 0xE000EF90 
DTCMCR      EQU 0xE000EF94
    
;/*ʹ��FPU*/
;//�����ͺ���,����ջ��ջ��,���������
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
                
;/*ʹ��TCM*/
;//H7_core.h�������ĺ�
;//�����ͺ���,����ջ��ջ��,���������
;//ʵ����ϵͳ���Զ�ʹ��
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
                
;/*ʹ��TCM����д�Ͷ���д*/
;//H7_core.h�������ĺ�
;//�����ͺ���,����ջ��ջ��,���������
;//ע���⺯����������õ���һ��������
;//ϵͳ�����Զ�ʹ��reλ
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
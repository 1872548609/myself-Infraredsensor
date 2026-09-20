;/***********************************************************************************************/
;/**
;* @file                startup_ciu32f003.s
;* @author              MCU Ecosystem Development Team
;* @brief               CMSIS Cortex-M0+ Core Device Startup File for CIU32F003. 
;*                                
;*
;************************************************************************************************* 
;* @attention
;* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
;*
;*************************************************************************************************
;*/

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000
                                
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __vector_table
                EXPORT  __vectors_End
                EXPORT  __vectors_Size

__vector_table  DCD     __initial_sp                   ; Top of Stack
                DCD     Reset_Handler                  ; Reset Handler    
                DCD     NMI_Handler                    ; NMI Handler
                DCD     HardFault_Handler              ; Hard Fault Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     SVC_Handler                    ; SVCall Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     PendSV_Handler                 ; PendSV Handler
                DCD     SysTick_Handler                ; SysTick Handler

                ; Interrupts
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     FLASH_IRQHandler                      ; FLASH
                DCD     RCC_IRQHandler                        ; RCC
                DCD     EXTI0_1_IRQHandler                    ; EXTI Line 0 & 1
                DCD     EXTI2_3_IRQHandler                    ; EXTI Line 2 & 3
                DCD     EXTI4_7_IRQHandler                    ; EXTI Line 4 to 7
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     ADC_COMP_IRQHandler                   ; ADC & COMP1/2 
                DCD     TIM1_BRK_UP_TRG_COM_IRQHandler        ; TIM1 Break, Update, Trigger and COM Event
                DCD     TIM1_CC_IRQHandler                    ; TIM1 Capture Compare
                DCD     TIM3_IRQHandler                       ; TIM3
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     LPTIM1_IRQHandler                     ; LPTIM1      
                DCD     I2C1_IRQHandler                       ; I2C1
                DCD     0                                     ; Reserved
                DCD     SPI1_IRQHandler                       ; SPI1
                DCD     0                                     ; Reserved
                DCD     UART1_IRQHandler                      ; UART1
                DCD     UART2_IRQHandler                      ; UART2
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved
                DCD     0                                     ; Reserved

__vectors_End

__vectors_Size  EQU  __vectors_End - __vector_table

                AREA    |.text|, CODE, READONLY

; Reset Handler
Reset_Handler    PROC
                 EXPORT  Reset_Handler                 [WEAK]
                 IMPORT  SystemInit 
                 IMPORT  __main                  
                     
                 LDR     R0, =SystemInit
                 BLX     R0
                 LDR     R0, =__main
                 BX      R0
                 ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                    [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler              [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                    [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                 [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  FLASH_IRQHandler                    [WEAK]
                EXPORT  RCC_IRQHandler                      [WEAK]
                EXPORT  EXTI0_1_IRQHandler                  [WEAK]
                EXPORT  EXTI2_3_IRQHandler                  [WEAK]
                EXPORT  EXTI4_7_IRQHandler                  [WEAK]
                EXPORT  ADC_COMP_IRQHandler                 [WEAK]
                EXPORT  TIM1_BRK_UP_TRG_COM_IRQHandler      [WEAK]
                EXPORT  TIM1_CC_IRQHandler                  [WEAK]
                EXPORT  TIM3_IRQHandler                     [WEAK]               
                EXPORT  LPTIM1_IRQHandler                   [WEAK]
                EXPORT  I2C1_IRQHandler                     [WEAK]
                EXPORT  SPI1_IRQHandler                     [WEAK]
                EXPORT  UART1_IRQHandler                    [WEAK]
                EXPORT  UART2_IRQHandler                    [WEAK]
                        

FLASH_IRQHandler       
RCC_IRQHandler         
EXTI0_1_IRQHandler     
EXTI2_3_IRQHandler     
EXTI4_7_IRQHandler     
ADC_COMP_IRQHandler
TIM1_BRK_UP_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM3_IRQHandler         
LPTIM1_IRQHandler      
I2C1_IRQHandler        
SPI1_IRQHandler             
UART1_IRQHandler      
UART2_IRQHandler                

                B       .

                ENDP

                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END

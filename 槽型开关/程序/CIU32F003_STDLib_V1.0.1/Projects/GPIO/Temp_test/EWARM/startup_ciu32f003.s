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

                MODULE  ?cstartup
                
                ;; Forward declaration of sections.
                SECTION CSTACK:DATA:NOROOT(3)
                
                SECTION .intvec:CODE:NOROOT(2)
                
                EXTERN  __iar_program_start
                EXTERN  SystemInit
                PUBLIC  __vector_table
                
                DATA
__vector_table
                DCD     sfe(CSTACK)
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


                THUMB
                
                PUBWEAK Reset_Handler
                SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__iar_program_start
                BX      R0
                
                PUBWEAK NMI_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
                B NMI_Handler
                
                PUBWEAK HardFault_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
                B HardFault_Handler
                
                PUBWEAK SVC_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
                B SVC_Handler
                
                PUBWEAK PendSV_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
                B PendSV_Handler
                
                PUBWEAK SysTick_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
                B SysTick_Handler
                
                PUBWEAK FLASH_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
FLASH_IRQHandler
                B FLASH_IRQHandler
                
                PUBWEAK RCC_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
RCC_IRQHandler
                B RCC_IRQHandler
                
                PUBWEAK EXTI0_1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
EXTI0_1_IRQHandler
                B EXTI0_1_IRQHandler
                
                PUBWEAK EXTI2_3_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
EXTI2_3_IRQHandler
                B EXTI2_3_IRQHandler
                
                PUBWEAK EXTI4_7_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
EXTI4_7_IRQHandler
                B EXTI4_7_IRQHandler
                                             
                PUBWEAK ADC_COMP_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
ADC_COMP_IRQHandler
                B ADC_COMP_IRQHandler
                
                PUBWEAK TIM1_BRK_UP_TRG_COM_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
TIM1_BRK_UP_TRG_COM_IRQHandler
                B TIM1_BRK_UP_TRG_COM_IRQHandler
                
                PUBWEAK TIM1_CC_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
TIM1_CC_IRQHandler
                B TIM1_CC_IRQHandler
                
                PUBWEAK TIM3_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
TIM3_IRQHandler
                B TIM3_IRQHandler
                
                PUBWEAK LPTIM1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
LPTIM1_IRQHandler
                B LPTIM1_IRQHandler
                
                PUBWEAK I2C1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_IRQHandler
                B I2C1_IRQHandler
                                
                PUBWEAK SPI1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
SPI1_IRQHandler
                B SPI1_IRQHandler
                
                PUBWEAK UART1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
UART1_IRQHandler
                B UART1_IRQHandler
                
                PUBWEAK UART2_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
UART2_IRQHandler
                B UART2_IRQHandler
                
                END

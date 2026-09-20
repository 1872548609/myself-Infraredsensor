/***********************************************************************************************/
/**
* @file                startup_ciu32f003.s
* @author              MCU Ecosystem Development Team
* @brief               CMSIS Cortex-M0+ Core Device Startup File for CIU32F003. 
*                                
*
************************************************************************************************* 
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
*************************************************************************************************
*/

.syntax unified
.cpu cortex-m0plus
.fpu softvfp
.thumb

.global __vector_table
.global g_Vectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

/**
* @brief  This is the code that gets called when the processor first
*         starts execution following a reset event. Only the absolutely
*         necessary set is performed, after which the application
*         supplied main() routine is called.
* @param  None
* @retval None
*/

    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr   r0, =_estack
    mov   sp, r0          /* set stack pointer */

/* Call the clock system initialization function.*/
    bl   SystemInit

/* Copy the data segment initializers from flash to SRAM */
    ldr  r0, =_sdata
    ldr  r1, =_edata
    ldr  r2, =_sidata
    movs r3, #0
    b    LoopCopyDataInit

CopyDataInit:
    ldr  r4, [r2, r3]
    str  r4, [r0, r3]
    adds r3, r3, #4

LoopCopyDataInit:
    adds r4, r0, r3
    cmp  r4, r1
    bcc  CopyDataInit

/* Zero fill the bss segment. */
    ldr  r2, =_sbss
    ldr  r4, =_ebss
    movs r3, #0
    b    LoopFillZerobss

FillZerobss:
    str  r3, [r2]
    adds r2, r2, #4

LoopFillZerobss:
    cmp  r2, r4
    bcc  FillZerobss

/* Call static constructors */
    bl   __libc_init_array
/* Call the application s entry point.*/
    bl   main

LoopForever:
    b    LoopForever

.size Reset_Handler, .-Reset_Handler

/**
* @brief  This is the code that gets called when the processor receives an
*         unexpected interrupt.  This simply enters an infinite loop, preserving
*         the system state for examination by a debugger.
*
* @param  None
* @retval None
*/
    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
    b    Infinite_Loop
    .size Default_Handler, .-Default_Handler

/******************************************************************************
*
* The minimal vector table for a Cortex M0+.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
    .section .isr_vector,"a",%progbits
    .type g_Vectors, %object
    .size g_Vectors, .-g_Vectors

__vector_table:
g_Vectors:
    .word  _estack
    .word  Reset_Handler                         /* Reset Handler */    
    .word  NMI_Handler                           /* NMI Handler */      
    .word  HardFault_Handler                     /* Hard Fault Handler */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  SVC_Handler                           /* SVCall Handler */      
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  PendSV_Handler                        /* PendSV Handler */   
    .word  SysTick_Handler                       /* SysTick Handler */  
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved  */
    .word  0                                     /* Reserved  */
    .word  FLASH_IRQHandler                      /* FLASH */
    .word  RCC_IRQHandler                        /* RCC */
    .word  EXTI0_1_IRQHandler                    /* EXTI Line 0 & 1 */
    .word  EXTI2_3_IRQHandler                    /* EXTI Line 2 & 3 */
    .word  EXTI4_7_IRQHandler                    /* EXTI Line 4 to 7 */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  ADC_COMP_IRQHandler                   /* ADC & COMP1/2 */
    .word  TIM1_BRK_UP_TRG_COM_IRQHandler        /* TIM1 Break, Update, Trigger and COM Event */
    .word  TIM1_CC_IRQHandler                    /* TIM1 Capture Compare */
    .word  TIM3_IRQHandler                       /* TIM3 */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  LPTIM1_IRQHandler                     /* LPTIM1 */
    .word  I2C1_IRQHandler                       /* I2C1 */
    .word  0                                     /* Reserved */
    .word  SPI1_IRQHandler                       /* SPI1 */
    .word  0                                     /* Reserved */
    .word  UART1_IRQHandler                      /* UART1 */
    .word  UART2_IRQHandler                      /* UART2 */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */
    .word  0                                     /* Reserved */      
    
/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

    .weak      NMI_Handler
    .thumb_set NMI_Handler,Default_Handler
   
    .weak      HardFault_Handler
    .thumb_set HardFault_Handler,Default_Handler
   
    .weak      SVC_Handler
    .thumb_set SVC_Handler,Default_Handler
   
    .weak      PendSV_Handler
    .thumb_set PendSV_Handler,Default_Handler
   
    .weak      SysTick_Handler
    .thumb_set SysTick_Handler,Default_Handler
   
    .weak      FLASH_IRQHandler
    .thumb_set FLASH_IRQHandler,Default_Handler
   
    .weak      RCC_IRQHandler
    .thumb_set RCC_IRQHandler,Default_Handler
   
    .weak      EXTI0_1_IRQHandler
    .thumb_set EXTI0_1_IRQHandler,Default_Handler
   
    .weak      EXTI2_3_IRQHandler
    .thumb_set EXTI2_3_IRQHandler,Default_Handler
   
    .weak      EXTI4_7_IRQHandler
    .thumb_set EXTI4_7_IRQHandler,Default_Handler
      
    .weak      ADC_COMP_IRQHandler
    .thumb_set ADC_COMP_IRQHandler,Default_Handler
    
    .weak      TIM1_BRK_UP_TRG_COM_IRQHandler
    .thumb_set TIM1_BRK_UP_TRG_COM_IRQHandler,Default_Handler
    
    .weak      TIM1_CC_IRQHandler
    .thumb_set TIM1_CC_IRQHandler,Default_Handler

    .weak      TIM3_IRQHandler
    .thumb_set TIM3_IRQHandler,Default_Handler
     
    .weak      LPTIM1_IRQHandler
    .thumb_set LPTIM1_IRQHandler,Default_Handler
   
    .weak      I2C1_IRQHandler
    .thumb_set I2C1_IRQHandler,Default_Handler
   
    .weak      SPI1_IRQHandler
    .thumb_set SPI1_IRQHandler,Default_Handler
    
    .weak      UART1_IRQHandler
    .thumb_set UART1_IRQHandler,Default_Handler
   
    .weak      UART2_IRQHandler
    .thumb_set UART2_IRQHandler,Default_Handler

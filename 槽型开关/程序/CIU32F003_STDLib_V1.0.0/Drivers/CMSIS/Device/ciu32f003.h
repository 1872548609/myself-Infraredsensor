/************************************************************************************************/
/**
* @file               ciu32f003.h
* @author             MCU Ecosystem Development Team
* @brief              CMSIS Core Peripheral Access Layer Header File for
*                     CIU32F003(CM0+). 
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

#ifndef CIU32F003_H
#define CIU32F003_H


/************************************************************************************************/
/**
* @addtogroup CMSIS
* @{
*/

/** 
* @defgroup CIU32F003 CIU32F003
* @{
*/
/************************************************************************************************/

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */



/*-----------------------------------------type define------------------------------------------*/
/************************************************************************************************/
/** 
* @defgroup CIU32F003_Types CIU32F003 Types
* @brief CIU32F003 data type definition
* @{
*/
/************************************************************************************************/ 

/**
* @brief Interrupt Number Definition
*/
typedef enum IRQn
{
    /* -------------------------------Cortex-M0+ Processor Exceptions Numbers  -------------------------------------- */
    
    NonMaskableInt_IRQn              = -14,    /**< 2 Non Maskable Interrupt                                          */
    HardFault_IRQn                   = -13,    /**< 3 Cortex-M Hard Fault Interrupt                                   */
    SVC_IRQn                         = -5,     /**< 11 Cortex-M SV Call Interrupt                                     */
    PendSV_IRQn                      = -2,     /**< 14 Cortex-M Pend SV Interrupt                                     */
    SysTick_IRQn                     = -1,     /**< 15 Cortex-M System Tick Interrupt                                 */
    
    /* -------------------------------CIU32F003 Interrupt Numbers---------------------------------------------------- */
    
    FLASH_IRQn                       = 3,      /**< FLASH global interrupt                                            */
    RCC_IRQn                         = 4,      /**< RCC global interrupt                                              */
    EXTI0_1_IRQn                     = 5,      /**< EXTI line 0 & 1 interrupts                                        */
    EXTI2_3_IRQn                     = 6,      /**< EXTI line 2 & 3 interrupts                                        */
    EXTI4_7_IRQn                     = 7,      /**< EXTI line 4 to 7 interrupts                                       */
    ADC_COMP_IRQn                    = 12,     /**< ADC, COMP1 and COMP2 interrupts                                   */
    TIM1_BRK_UP_TRG_COM_IRQn         = 13,     /**< TIM1 break, update, trigger and commutation interrupts            */
    TIM1_CC_IRQn                     = 14,     /**< TIM1 capture compare interrupt                                    */
    TIM3_IRQn                        = 15,     /**< TIM3 global interrupt                                             */
    LPTIM1_IRQn                      = 20,     /**< LPTIM1 global interrupt                                           */
    I2C1_IRQn                        = 21,     /**< I2C1 globlal interrupt                                            */
    SPI1_IRQn                        = 23,     /**< SPI1 globlal interrupt                                            */
    UART1_IRQn                       = 25,     /**< UART1 globlal interrupt                                           */
    UART2_IRQn                       = 26,     /**< UART2 globlal interrupt                                           */
} IRQn_Type;

/** 
* @} 
*/

/*--------------------------------------------define--------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup CIU32F003_Constants CIU32F003 Constants
* @brief CIU32F003 constants definition
* @{
*
*/
/************************************************************************************************/    
/**
* @brief Configuration of Core Peripherals
*/
#define __CM0PLUS_REV             0x0001U /**< core revision r0p1                            */
#define __MPU_PRESENT             0U      /**< MPU not defined                               */
#define __VTOR_PRESENT            1U      /**< VTOR present                                  */
#define __NVIC_PRIO_BITS          2U      /**< number of bits used for priority levels       */
#define __Vendor_SysTickConfig    0U      /**< set to 1 if different SysTick Config is used  */

/**
* @brief  NVIC interrupt priority
*/
#define NVIC_PRIO_0         (0x00U)
#define NVIC_PRIO_1         (0x01U)    
#define NVIC_PRIO_2         (0x02U)    
#define NVIC_PRIO_3         (0x03U)

/** 
* @} 
*/

/*------------------------------------------includes--------------------------------------------*/
#include "core_cm0plus.h"                /* Cortex-M0+ processor and core peripherals */
#include "system_ciu32f003.h"            /* CIU32F003 System Header */
#include <stdint.h>

/*-----------------------------------------type define------------------------------------------*/
/************************************************************************************************/
/** 
* @addtogroup  CIU32F003_Types
* @{
*/
/************************************************************************************************/
/**
* @brief registers of ADC 
*/
typedef struct  
{
    __IO uint32_t CR;                   /**< ADC control register,                          Address offset: 0x000         */
    __IO uint32_t CFG1;                 /**< ADC configuration register 1,                  Address offset: 0x004         */
    __IO uint32_t CFG2;                 /**< ADC configuration register 2,                  Address offset: 0x008         */
         uint32_t RESERVED_0[1];        /**< Reserved,                                      Address offset: 0x00C         */
    __IO uint32_t ISR;                  /**< ADC interrupt and status register,             Address offset: 0x010         */
    __IO uint32_t IER;                  /**< ADC interrupt enable register,                 Address offset: 0x014         */
    __IO uint32_t SAMPT;                /**< ADC sampling time register,                    Address offset: 0x018         */
    __IO uint32_t CHCFG;                /**< ADC group regular sequencer register,          Address offset: 0x01C         */
    __IO uint32_t AWDGCR;               /**< ADC analog watchdog configuration register,    Address offset: 0x020         */
         uint32_t RESERVED_1[1];        /**< Reserved,                                      Address offset: 0x024         */    
    __IO uint32_t AWDGTR;               /**< ADC analog watchdog threshold register,        Address offset: 0x028         */
         uint32_t RESERVED_2[1];        /**< Reserved,                                      Address offset: 0x02C         */      
    __IO uint32_t CALFACT;              /**< ADC calibration factor register,               Address offset: 0x030         */
         uint32_t RESERVED_3[3];        /**< Reserved,                                      Address offset: 0x034 - 0x03C */
    __IO uint32_t DR;                   /**< ADC group regular data register,               Address offset: 0x040         */
         uint32_t RESERVED_4[111];      /**< Reserved,                                      Address offset: 0x044 - 0x1FC */
    __IO uint32_t CFG3;                 /**< ADC configuration register 3,                  Address offset: 0x200         */
} ADC_t;


/**
* @brief registers of COMP common control
*/
typedef struct
{
    __IO uint32_t CR;                /**< COMP common control regigster,           Address offset: 0x00 */
} COMP_COMMON_t;


/**
* @brief registers of COMP
*/
typedef struct
{
    __IO uint32_t CSR;               /**< COMP control and status register,        Address offset: 0x00 */
} COMP_t;


/**
* @brief registers of CRC
*/
typedef struct
{
    __IO uint32_t CSR;               /**< CRC control state register,              Address offset: 0x00      */
    __IO uint32_t RDR;               /**< CRC result data register,                Address offset: 0x04      */
         uint32_t RESERVED_0[30];    /**< Reserved,                                Address offset: 0x08 - 7C */
    __IO uint32_t DR;                /**< CRC data register,                       Address offset: 0x80      */
} CRC_t;


/**
* @brief registers of DBG
*/
typedef struct
{
    __IO uint32_t CR;                /**< Debug control register,                   Address offset: 0x00 */
    __IO uint32_t APB_FZ1;           /**< Debug APB1 freeze register 1,             Address offset: 0x04 */
    __IO uint32_t APB_FZ2;           /**< Debug APB1 freeze register 2,             Address offset: 0x08 */
} DBG_t;


/**
* @brief registers of EXTI
*/
typedef struct
{
    __IO uint32_t RTSR;                /**< EXTI rising trigger selection register,          Address offset: 0x00        */
    __IO uint32_t FTSR;                /**< EXTI falling trigger selection register,         Address offset: 0x04        */
    __IO uint32_t PIR;                 /**< EXTI Rising or Falling pending register,         Address offset: 0x08        */
         uint32_t RESERVED_0[17];      /**< Reserved,                                        Address offset: 0x0C - 0x4C */
    __IO uint32_t EXTICR1;             /**< EXTI external interrupt configuration register,  Address offset: 0x50        */
         uint32_t RESERVED_1[7];       /**< Reserved,                                        Address offset: 0x54 - 0x6C */
    __IO uint32_t IMR;                 /**< EXTI interrupt mask register,                    Address offset: 0x70        */
    __IO uint32_t EMR;                 /**< EXTI event mask register,                        Address offset: 0x74        */
} EXTI_t;


/**
* @brief registers of FLASH
*/
typedef struct
{
    __IO uint32_t ACR;              /**< FLASH access control register,                Address offset: 0x00        */
         uint32_t RESERVED_0[1];    /**< Reserved,                                     Address offset: 0x04        */
    __IO uint32_t CRKEY;            /**< FLASH control key register,                   Address offset: 0x08        */
    __IO uint32_t OPTKEY;           /**< FLASH option key register,                    Address offset: 0x0C        */
    __IO uint32_t SR;               /**< FLASH status register,                        Address offset: 0x10        */
    __IO uint32_t CR;               /**< FLASH control register,                       Address offset: 0x14        */
         uint32_t RESERVED_1[2];    /**< Reserved,                                     Address offset: 0x18 - 0x1C */
    __IO uint32_t OPTR1;            /**< FLASH option register 1,                      Address offset: 0x20        */
    __IO uint32_t OPTR2;            /**< FLASH option register 2,                      Address offset: 0x24        */
         uint32_t RESERVED_2[4];    /**< Reserved,                                     Address offset: 0x28 - 0x34 */
    __IO uint32_t WRP;              /**< FLASH WRP write protect area register,        Address offset: 0x38        */
} FLASH_t;


/**
* @brief registers of GPIO
*/
typedef struct
{
    __IO uint32_t MODE;            /**< GPIO port mode register,               Address offset: 0x00        */
    __IO uint32_t OTYPE;           /**< GPIO port output type register,        Address offset: 0x04        */
         uint32_t RESERVED_0[1];   /**< Reserved,                              Address offset: 0x08        */
    __IO uint32_t PUPD;            /**< GPIO port pull-up/pull-down register,  Address offset: 0x0C        */
    __IO uint32_t IDR;             /**< GPIO port input data register,         Address offset: 0x10        */
    __IO uint32_t ODR;             /**< GPIO port output data register,        Address offset: 0x14        */
    __IO uint32_t BSR;             /**< GPIO port bit set/reset  register,     Address offset: 0x18        */
         uint32_t RESERVED_1[1];   /**< Reserved,                              Address offset: 0x1C        */
    __IO uint32_t AFL;             /**< GPIO alternate function registers,     Address offset: 0x20        */
         uint32_t RESERVED_2[1];   /**< Reserved,                              Address offset: 0x24        */	
    __IO uint32_t BR;              /**< GPIO bit reset register,               Address offset: 0x28        */
} GPIO_t;


/**
* @brief registers of I2C
*/
typedef struct
{
    __IO uint32_t CR1;             /**< I2C control register 1,            Address offset: 0x00        */
    __IO uint32_t CR2;             /**< I2C control register 2,            Address offset: 0x04        */
    __IO uint32_t ADDR1;           /**< I2C own address 1 register,        Address offset: 0x08        */
         uint32_t RESERVED_0[3];   /**< reserved,                          Address offset: 0x0C - 0x14 */
    __IO uint32_t ISR;             /**< I2C interrupt status register,     Address offset: 0x18        */
    __IO uint32_t ICR;             /**< I2C interrupt clear register,      Address offset: 0x1C        */
         uint32_t RESERVED_1[1];   /**< Reserved,                          Address offset: 0x20        */
    __IO uint32_t RDR;             /**< I2C receive data register,         Address offset: 0x24        */
    __IO uint32_t TDR;             /**< I2C transmit data register,        Address offset: 0x28        */
} I2C_t;


/**
* @brief registers of IRTIM
*/
typedef struct
{
    __IO uint32_t CR;              /**< IRTIM control register              Address offset: 0x00        */  
} IRTIM_t;


/**
* @brief registers of IWDG
*/
typedef struct
{
    __IO uint32_t CR;              /**< IWDG control register,             Address offset: 0x00        */
    __IO uint32_t CFG;             /**< IWDG configuration register,       Address offset: 0x04        */
    __IO uint32_t RESERVED_0[3];   /**< Reserved,                          Address offset: 0x08 - 0x10 */
    __IO uint32_t CNT;             /**< IWDG counter register,             Address offset: 0x14        */
} IWDG_t;


/**
* @brief registers of LPTimer
*/
typedef struct
{
    __IO uint32_t ISR;              /**< LPTIM interrupt and status register,                Address offset: 0x00 */
    __IO uint32_t ICR;              /**< LPTIM interrupt clear register,                     Address offset: 0x04 */
    __IO uint32_t IER;              /**< LPTIM interrupt enable register,                    Address offset: 0x08 */
    __IO uint32_t CFG;              /**< LPTIM configuration register,                       Address offset: 0x0C */
    __IO uint32_t CR;               /**< LPTIM control register,                             Address offset: 0x10 */
    __IO uint32_t RESERVED_0[1];    /**< Reserved,                                           Address offset: 0x10 */
    __IO uint32_t ARR;              /**< LPTIM autoreload register,                          Address offset: 0x18 */
    __IO uint32_t CNT;              /**< LPTIM counter register,                             Address offset: 0x1C */
} LPTIM_t;


/**
* @brief registers of PMU
*/
typedef struct
{
    __IO uint32_t CR;                      /**< PMU control register,                       Address offset: 0x00         */
    __IO uint32_t RESERVED_0[11];          /**< Reserved,                                   Address offset: 0x04 - 0x2C  */
    __IO uint32_t FLASH_WAKEUP;            /**< PMU Flash wakeup time control,              Address offset: 0x30         */
} PMU_t;


/**
* @brief registers of RCC
*/
typedef struct
{
    __IO uint32_t CSR1;             /**< RCC clock control and status register 1,                                Address offset: 0x00            */
         uint32_t RESERVED_0[1];    /**< Reserved,                                                               Address offset: 0x04            */
    __IO uint32_t CFG;              /**< RCC clocks config register,                                             Address offset: 0x08            */
         uint32_t RESERVED_1[1];    /**< Reserved,                                                               Address offset: 0x0C            */
    __IO uint32_t IER;              /**< RCC interrupt enable register,                                          Address offset: 0x10            */
    __IO uint32_t ISR;              /**< RCC interrupt status register,                                          Address offset: 0x14            */
    __IO uint32_t ICR;              /**< RCC interrupt clear register,                                           Address offset: 0x18            */
    __IO uint32_t IOPRST;           /**< RCC IO port reset register,                                             Address offset: 0x1C            */
    __IO uint32_t AHBRST;           /**< RCC AHB peripherals reset register,                                     Address offset: 0x20            */
    __IO uint32_t APBRST1;          /**< RCC APB peripherals reset register 1,                                   Address offset: 0x24            */
    __IO uint32_t APBRST2;          /**< RCC APB peripherals reset register 2,                                   Address offset: 0x28            */
    __IO uint32_t IOPEN;            /**< RCC IO port clock enable register,                                      Address offset: 0x2C            */
    __IO uint32_t AHBEN;            /**< RCC AHB peripherals clock enable register,                              Address offset: 0x30            */
    __IO uint32_t APBEN1;           /**< RCC APB peripherals clock enable register 1,                            Address offset: 0x34            */
    __IO uint32_t APBEN2;           /**< RCC APB peripherals clock enable register 2,                            Address offset: 0x38            */
    __IO uint32_t CLKSEL;           /**< RCC peripherals independent clock source select register,               Address offset: 0x3C            */
         uint32_t RESERVED_2[1];    /**< Reserved,                                                               Address offset: 0x40            */
    __IO uint32_t CSR2;             /**< RCC clock control and status register 2,                                Address offset: 0x44            */
         uint32_t RESERVED_3[2];    /**< Reserved,                                                               Address offset: 0x48 - 0x4C     */
    __IO uint32_t RCLCAL;           /**< RCC RCL calibration register,                                           Address offset: 0x50            */    
    __IO uint32_t RCHCAL;           /**< RCC RCH calibration register,                                           Address offset: 0x54            */        
} RCC_t;


/**
* @brief registers of SPI
*/
typedef struct
{
    __IO uint32_t CR1;               /**< SPI control register 1,                       Address offset: 0x00 */
    __IO uint32_t CR2;               /**< SPI control register 2,                       Address offset: 0x04 */
         uint32_t RESERVED_0[1];     /**< Reserved,                                     Address offset: 0x08 */
    __IO uint32_t ISR;               /**< SPI interrupt status register,                Address offset: 0x0C */
    __IO uint32_t ICR;               /**< SPI interrupt status clear register,          Address offset: 0x10 */
    __IO uint32_t DR;                /**< SPI data register,                            Address offset: 0x14 */
} SPI_t;


/**
* @brief registers of Timer
*/
typedef struct
{
    __IO uint32_t CR1;             /**< TIM control register 1,                   Address offset: 0x00        */
    __IO uint32_t CR2;             /**< TIM control register 2,                   Address offset: 0x04        */
    __IO uint32_t SMC;             /**< TIM slave mode control register,          Address offset: 0x08        */
    __IO uint32_t DIER;            /**< TIM DMA/interrupt enable register,        Address offset: 0x0C        */
    __IO uint32_t SR;              /**< TIM status register,                      Address offset: 0x10        */
    __IO uint32_t EVTG;            /**< TIM event generation register,            Address offset: 0x14        */
    __IO uint32_t CCM1;            /**< TIM capture/compare mode register 1,      Address offset: 0x18        */
    __IO uint32_t CCM2;            /**< TIM capture/compare mode register 2,      Address offset: 0x1C        */
    __IO uint32_t CCEN;            /**< TIM capture/compare enable register,      Address offset: 0x20        */
    __IO uint32_t CNT;             /**< TIM counter register,                     Address offset: 0x24        */
    __IO uint32_t PSC;             /**< TIM prescaler register,                   Address offset: 0x28        */
    __IO uint32_t ARR;             /**< TIM auto-reload register,                 Address offset: 0x2C        */
    __IO uint32_t RCR;             /**< TIM repetition counter register,          Address offset: 0x30        */
    __IO uint32_t CC1;             /**< TIM capture/compare register 1,           Address offset: 0x34        */
    __IO uint32_t CC2;             /**< TIM capture/compare register 2,           Address offset: 0x38        */
    __IO uint32_t CC3;             /**< TIM capture/compare register 3,           Address offset: 0x3C        */
    __IO uint32_t CC4;             /**< TIM capture/compare register 4,           Address offset: 0x40        */
    __IO uint32_t BDT;             /**< TIM break and dead-time register,         Address offset: 0x44        */
         uint32_t RESERVED_0[2];   /**< Reserved,                                 Address offset: 0x48 - 0x4C */
    __IO uint32_t CFG;             /**< TIM option register,                      Address offset: 0x50        */
         uint32_t RESERVED_1[3];   /**< Reserved,                                 Address offset: 0x54 - 0x5C */
    __IO uint32_t AF1;             /**< TIM alternate function register 1,        Address offset: 0x60        */
         uint32_t RESERVED_2[1];   /**< Reserved,                                 Address offset: 0x64        */
    __IO uint32_t TISEL;           /**< TIM Input Selection register,             Address offset: 0x68        */
} TIM_t;


/**
* @brief registers of UART
*/
typedef struct
{
    __IO uint32_t CR1;             /**< UART control register 1,                 Address offset: 0x00        */
    __IO uint32_t CR2;             /**< UART control register 2,                 Address offset: 0x04        */
    __IO uint32_t CR3;             /**< UART control register 3,                 Address offset: 0x08        */
    __IO uint32_t BRR;             /**< UART baud rate register,                 Address offset: 0x0C        */
         uint32_t RESERVED_0[3];   /**< Reserved,                                Address offset: 0x10-0x18   */
    __IO uint32_t ISR;             /**< UART interrupt and status register,      Address offset: 0x1C        */
    __IO uint32_t ICR;             /**< UART interrupt flag clear register,      Address offset: 0x20        */
    __IO uint32_t RDR;             /**< UART receive data register,              Address offset: 0x24        */
    __IO uint32_t TDR;             /**< UART transmit data register,             Address offset: 0x28        */   
} UART_t;



/**
* @}
*/

/*--------------------------------------------define--------------------------------------------*/
/************************************************************************************************/
/** 
* @addtogroup CIU32F003_Constants
* @{
*/
/************************************************************************************************/
/* Memory Map */
#define FLASH_MEM_BASE        (0x00000000UL)  /**< FLASH Base Address */
#define SRAM_BASE             (0x20000000UL)  /**< SRAM Base Address  */
#define APB_BASE              (0x40000000UL)  /**< APB Base Address   */
#define AHB_BASE              (0x40020000UL)  /**< AHB Base Address   */
#define GPIO_BASE             (0x50000000UL)  /**< GPIO Base Address  */

/* Option Bytes */                                                               
#define FLASH_OB_OPTR1_ADDR   (0x1FFF0000UL)  /**< Option Bytes1 base address                   */
#define FLASH_OB_OPTR2_ADDR   (0x1FFF0004UL)  /**< Option Bytes2 base address                   */
#define FLASH_OB_WRP_ADDR     (0x1FFF0008UL)  /**< WRP base address                             */

/* Engineer */
#define RCH_CAL               (0x1FFF0204UL)  /**< RCH calibration value address                 */
#define RCL_CAL               (0x1FFF020CUL)  /**< RCL calibration value address                 */
#define UID_BASE              (0x1FFF0340UL)  /**< Unique device ID register base address        */
#define DEVICE_TYPE           (0x1FFF03A4UL)  /**< device type                                   */
#define USERFLASH_SIZE        (0x1FFF03A8UL)  /**< Size of user flash                            */
#define SRAM_SIZE             (0x1FFF03ACUL)  /**< Size of sram                                  */
#define BGR_CAL               (0x1FFF03C0UL)  /**< BGR calibration value address                 */


/* APB Peripherals */
#define TIM3_BASE             (APB_BASE + 0x00000400UL)
#define IWDG_BASE             (APB_BASE + 0x00003000UL)
#define UART2_BASE            (APB_BASE + 0x00004400UL)
#define I2C1_BASE             (APB_BASE + 0x00005400UL)
#define PMU_BASE              (APB_BASE + 0x00007000UL)
#define LPTIM1_BASE           (APB_BASE + 0x00007C00UL)

#define IRTIM_BASE            (APB_BASE + 0x00010000UL)
#define COMP_COMMON_BASE      (APB_BASE + 0x00010200UL) 
#define COMP1_BASE            (COMP_COMMON_BASE + 0x00000010UL)
#define COMP2_BASE            (COMP_COMMON_BASE + 0x00000014UL)

#define ADC_BASE              (APB_BASE + 0x00012400UL)
#define TIM1_BASE             (APB_BASE + 0x00012C00UL)
#define SPI1_BASE             (APB_BASE + 0x00013000UL)
#define UART1_BASE            (APB_BASE + 0x00013800UL)
#define DBG_BASE              (APB_BASE + 0x00015800UL)


/* AHB Peripherals */
#define RCC_BASE              (AHB_BASE + 0x00001000UL)
#define EXTI_BASE             (AHB_BASE + 0x00001800UL)
#define FLASH_REGISTERS_BASE  (AHB_BASE + 0x00002000UL)
#define CRC_BASE              (AHB_BASE + 0x00003000UL)

/* GPIO */
#define GPIOA_BASE            (GPIO_BASE + 0x00000000UL)
#define GPIOB_BASE            (GPIO_BASE + 0x00000400UL)
#define GPIOC_BASE            (GPIO_BASE + 0x00000800UL)


/* APB Peripherals instance*/
#define TIM1                ((TIM_t *) TIM1_BASE)
#define TIM3                ((TIM_t *) TIM3_BASE)
#define IWDG                ((IWDG_t *) IWDG_BASE)
#define UART2               ((UART_t *) UART2_BASE)
#define I2C1                ((I2C_t *) I2C1_BASE)
#define PMU                 ((PMU_t *) PMU_BASE)
#define LPTIM1              ((LPTIM_t *) LPTIM1_BASE)

#define COMP_COMMON         ((COMP_COMMON_t *) COMP_COMMON_BASE)

#define COMP1               ((COMP_t *) COMP1_BASE)
#define COMP2               ((COMP_t *) COMP2_BASE)
#define ADC                 ((ADC_t *) ADC_BASE)
#define SPI1                ((SPI_t *) SPI1_BASE)
#define UART1               ((UART_t *) UART1_BASE)
#define DBG                 ((DBG_t *) DBG_BASE)
#define IRTIM               ((IRTIM_t *) IRTIM_BASE)

/* AHB Peripherals instance*/
#define RCC                 ((RCC_t *) RCC_BASE)
#define EXTI                ((EXTI_t *) EXTI_BASE)
#define FLASH               ((FLASH_t *) FLASH_REGISTERS_BASE)
#define CRC                 ((CRC_t *) CRC_BASE)

/* GPIO instance*/
#define GPIOA               ((GPIO_t *) GPIOA_BASE)
#define GPIOB               ((GPIO_t *) GPIOB_BASE)
#define GPIOC               ((GPIO_t *) GPIOC_BASE)


/*---------------------------Peripheral Registers Bits Definition-------------------------------*/

/*-----------------------------------------------------------------------------------------------
                                       ADC Registers Bits  
------------------------------------------------------------------------------------------------*/

/* Bits for ADC_CR */
#define ADC_CR_ADEN_POS                 (0U)
#define ADC_CR_ADEN_MASK                (0x1UL << ADC_CR_ADEN_POS)             /**< 0x00000001 */
#define ADC_CR_ADEN                     ADC_CR_ADEN_MASK                       /**< ADC enable */

#define ADC_CR_ADDIS_POS                (1U)
#define ADC_CR_ADDIS_MASK               (0x1UL << ADC_CR_ADDIS_POS)            /**< 0x00000002 */
#define ADC_CR_ADDIS                    ADC_CR_ADDIS_MASK                      /**< ADC disable */

#define ADC_CR_START_POS                (2U)
#define ADC_CR_START_MASK               (0x1UL << ADC_CR_START_POS)            /**< 0x00000004 */
#define ADC_CR_START                    ADC_CR_START_MASK                      /**< ADC group regular conversion start */

#define ADC_CR_STOP_POS                 (4U)
#define ADC_CR_STOP_MASK                (0x1UL << ADC_CR_STOP_POS)             /**< 0x00000010 */
#define ADC_CR_STOP                     ADC_CR_STOP_MASK                       /**< ADC group regular conversion stop */

#define ADC_CR_CALEN_POS                (31U)
#define ADC_CR_CALEN_MASK               (0x1UL << ADC_CR_CALEN_POS)            /**< 0x80000000 */
#define ADC_CR_CALEN                    ADC_CR_CALEN_MASK                      /**< ADC calibration */

/* Bits for ADC_CFG1 */
#define ADC_CFG1_SDIR_POS               (2U)
#define ADC_CFG1_SDIR_MASK              (0x1UL << ADC_CFG1_SDIR_POS)           /**< 0x00000004 */
#define ADC_CFG1_SDIR                   ADC_CFG1_SDIR_MASK                     /**< ADC group regular sequencer scan direction */

#define ADC_CFG1_TRIGEN_POS             (9U)
#define ADC_CFG1_TRIGEN_MASK            (0x3UL << ADC_CFG1_TRIGEN_POS)         /**< 0x00000600 */
#define ADC_CFG1_TRIGEN                 ADC_CFG1_TRIGEN_MASK                   /**< ADC group regular external trigger polarity */
#define ADC_CFG1_TRIGEN_SW              (0x0UL << ADC_CFG1_TRIGEN_POS)         /**< 0x00000000 */
#define ADC_CFG1_TRIGEN_HW_EDGE_RISING  (0x1UL << ADC_CFG1_TRIGEN_POS)         /**< 0x00000200 */
#define ADC_CFG1_TRIGEN_HW_EDGE_FALLING (0x2UL << ADC_CFG1_TRIGEN_POS)         /**< 0x00000400 */
#define ADC_CFG1_TRIGEN_HW_EDGE_BOTH    (0x3UL << ADC_CFG1_TRIGEN_POS)         /**< 0x00000600 */

#define ADC_CFG1_OVRN_MOD_POS           (11U)
#define ADC_CFG1_OVRN_MOD_MASK          (0x1UL << ADC_CFG1_OVRN_MOD_POS)       /**< 0x00000800 */
#define ADC_CFG1_OVRN_MOD               ADC_CFG1_OVRN_MOD_MASK                 /**< ADC group regular overrun configuration */

#define ADC_CFG1_CONV_MOD_POS           (12U)
#define ADC_CFG1_CONV_MOD_MASK          (0x3UL << ADC_CFG1_CONV_MOD_POS)       /**< 0x00003000 */
#define ADC_CFG1_CONV_MOD               ADC_CFG1_CONV_MOD_MASK                 /**< ADC group regular sequencer conversion mode */
#define ADC_CFG1_CONV_MOD_SINGLE        (0x0UL << ADC_CFG1_CONV_MOD_POS)       /**< 0x00000000 */
#define ADC_CFG1_CONV_MOD_CONTINUOUS    (0x1UL << ADC_CFG1_CONV_MOD_POS)       /**< 0x00001000 */
#define ADC_CFG1_CONV_MOD_DISCONTINUOUS (0x2UL << ADC_CFG1_CONV_MOD_POS)       /**< 0x00002000 */

#define ADC_CFG1_WAIT_MOD_POS           (14U)
#define ADC_CFG1_WAIT_MOD_MASK          (0x1UL << ADC_CFG1_WAIT_MOD_POS)       /**< 0x00004000 */
#define ADC_CFG1_WAIT_MOD               ADC_CFG1_WAIT_MOD_MASK                 /**< ADC auto wait */

#define ADC_CFG1_TRIG_SEL_POS           (16U)
#define ADC_CFG1_TRIG_SEL_MASK          (0x3UL << ADC_CFG1_TRIG_SEL_POS)       /**< 0x00030000 */
#define ADC_CFG1_TRIG_SEL               ADC_CFG1_TRIG_SEL_MASK                 /**< ADC group regular external trigger source */
#define ADC_CFG1_TRIG_TIM1_TRGO         (0x0UL << ADC_CFG1_TRIG_SEL_POS)       /**< 0x00000000 */
#define ADC_CFG1_TRIG_TIM1_OC4_ADC      (0x1UL << ADC_CFG1_TRIG_SEL_POS)       /**< 0x00010000 */
#define ADC_CFG1_TRIG_TIM3_TRGO         (0x2UL << ADC_CFG1_TRIG_SEL_POS)       /**< 0x00020000 */
#define ADC_CFG1_TRIG_EXTI7             (0x3UL << ADC_CFG1_TRIG_SEL_POS)       /**< 0x00030000 */

/* Bits for ADC_CFG2 */
#define ADC_CFG2_VBGREN_POS            (16U)
#define ADC_CFG2_VBGREN_MASK           (0x1UL << ADC_CFG2_VBGREN_POS)          /**< 0x00010000 */
#define ADC_CFG2_VBGREN                ADC_CFG2_VBGREN_MASK                    /**< ADC internal path to VBGR enable */

#define ADC_CFG2_PRESC_POS             (24U)
#define ADC_CFG2_PRESC_MASK            (0x7UL << ADC_CFG2_PRESC_POS)           /**< 0x07000000 */
#define ADC_CFG2_PRESC                 ADC_CFG2_PRESC_MASK                     /**< ADC asynchronous clock prescaler (prescaler only for clock source asynchronous) */
#define ADC_CFG2_PRESC_DIV1            (0x0UL << ADC_CFG2_PRESC_POS)           /**< 0x00000000 */
#define ADC_CFG2_PRESC_DIV2            (0x1UL << ADC_CFG2_PRESC_POS)           /**< 0x01000000 */
#define ADC_CFG2_PRESC_DIV3            (0x2UL << ADC_CFG2_PRESC_POS)           /**< 0x02000000 */
#define ADC_CFG2_PRESC_DIV4            (0x3UL << ADC_CFG2_PRESC_POS)           /**< 0x03000000 */
#define ADC_CFG2_PRESC_DIV8            (0x4UL << ADC_CFG2_PRESC_POS)           /**< 0x04000000 */
#define ADC_CFG2_PRESC_DIV16           (0x5UL << ADC_CFG2_PRESC_POS)           /**< 0x05000000 */
#define ADC_CFG2_PRESC_DIV32           (0x6UL << ADC_CFG2_PRESC_POS)           /**< 0x06000000 */
#define ADC_CFG2_PRESC_DIV64           (0x7UL << ADC_CFG2_PRESC_POS)           /**< 0x07000000 */

/* Bits for ADC_ISR */
#define ADC_ISR_EOSAMP_POS             (1U)
#define ADC_ISR_EOSAMP_MASK            (0x1UL << ADC_ISR_EOSAMP_POS)           /**< 0x00000002 */
#define ADC_ISR_EOSAMP                 ADC_ISR_EOSAMP_MASK                     /**< ADC group regular end of sampling flag */

#define ADC_ISR_EOC_POS                (2U)
#define ADC_ISR_EOC_MASK               (0x1UL << ADC_ISR_EOC_POS)              /**< 0x00000004 */
#define ADC_ISR_EOC                    ADC_ISR_EOC_MASK                        /**< ADC group regular end of unitary conversion flag */

#define ADC_ISR_EOS_POS                (3U)
#define ADC_ISR_EOS_MASK               (0x1UL << ADC_ISR_EOS_POS)              /**< 0x00000008 */
#define ADC_ISR_EOS                    ADC_ISR_EOS_MASK                        /**< ADC group regular end of sequence conversions flag */

#define ADC_ISR_OVRN_POS               (4U)
#define ADC_ISR_OVRN_MASK              (0x1UL << ADC_ISR_OVRN_POS)             /**< 0x00000010 */
#define ADC_ISR_OVRN                   ADC_ISR_OVRN_MASK                       /**< ADC group regular overrun flag */

#define ADC_ISR_AWDG_POS               (7U)
#define ADC_ISR_AWDG_MASK              (0x1UL << ADC_ISR_AWDG_POS)            /**< 0x00000080 */
#define ADC_ISR_AWDG                   ADC_ISR_AWDG_MASK                       /**< ADC analog watchdog flag */

#define ADC_ISR_EOCAL_POS              (11U)
#define ADC_ISR_EOCAL_MASK             (0x1UL << ADC_ISR_EOCAL_POS)            /**< 0x00000800 */
#define ADC_ISR_EOCAL                  ADC_ISR_EOCAL_MASK                      /**< ADC end of calibration flag */

/* Bits for ADC_IER */
#define ADC_IER_EOSAMPIE_POS           (1U)
#define ADC_IER_EOSAMPIE_MASK          (0x1UL << ADC_IER_EOSAMPIE_POS)         /**< 0x00000002 */
#define ADC_IER_EOSAMPIE               ADC_IER_EOSAMPIE_MASK                   /**< ADC group regular end of sampling interrupt */

#define ADC_IER_EOCIE_POS              (2U)
#define ADC_IER_EOCIE_MASK             (0x1UL << ADC_IER_EOCIE_POS)            /**< 0x00000004 */
#define ADC_IER_EOCIE                  ADC_IER_EOCIE_MASK                      /**< ADC group regular end of unitary conversion interrupt */

#define ADC_IER_EOSIE_POS              (3U)
#define ADC_IER_EOSIE_MASK             (0x1UL << ADC_IER_EOSIE_POS)            /**< 0x00000008 */
#define ADC_IER_EOSIE                  ADC_IER_EOSIE_MASK                      /**< ADC group regular end of sequence conversions interrupt */

#define ADC_IER_OVRNIE_POS             (4U)
#define ADC_IER_OVRNIE_MASK            (0x1UL << ADC_IER_OVRNIE_POS)           /**< 0x00000010 */
#define ADC_IER_OVRNIE                 ADC_IER_OVRNIE_MASK                     /**< ADC group regular overrun interrupt */

#define ADC_IER_AWDGIE_POS             (7U)
#define ADC_IER_AWDGIE_MASK            (0x1UL << ADC_IER_AWDGIE_POS)           /**< 0x00000080 */
#define ADC_IER_AWDGIE                 ADC_IER_AWDGIE_MASK                     /**< ADC analog watchdog interrupt */

#define ADC_IER_EOCALIE_POS            (11U)
#define ADC_IER_EOCALIE_MASK           (0x1UL << ADC_IER_EOCALIE_POS)          /**< 0x00000800 */
#define ADC_IER_EOCALIE                ADC_IER_EOCALIE_MASK                    /**< ADC end of calibration interrupt */

/* Bits for ADC_SAMPT */
#define ADC_SAMPT_SAMPT_POS            (0U)
#define ADC_SAMPT_SAMPT_MASK           (0xFUL << ADC_SAMPT_SAMPT_POS)          /**< 0x0000000F */
#define ADC_SAMPT_SAMPT                ADC_SAMPT_SAMPT_MASK                    /**< ADC group of channels sampling time  */
#define ADC_SAMPT_SAMPT_3CYCLES        (0x1UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000001 */
#define ADC_SAMPT_SAMPT_7CYCLES        (0x2UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000002 */
#define ADC_SAMPT_SAMPT_12CYCLES       (0x3UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000003 */
#define ADC_SAMPT_SAMPT_19CYCLES       (0x4UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000004 */
#define ADC_SAMPT_SAMPT_39CYCLES       (0x5UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000005 */
#define ADC_SAMPT_SAMPT_79CYCLES       (0x6UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000006 */
#define ADC_SAMPT_SAMPT_119CYCLES      (0x7UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000007 */
#define ADC_SAMPT_SAMPT_159CYCLES      (0x8UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000008 */
#define ADC_SAMPT_SAMPT_239CYCLES      (0x9UL << ADC_SAMPT_SAMPT_POS)          /**< 0x00000009 */
#define ADC_SAMPT_SAMPT_319CYCLES      (0xAUL << ADC_SAMPT_SAMPT_POS)          /**< 0x0000000A */
#define ADC_SAMPT_SAMPT_479CYCLES      (0xBUL << ADC_SAMPT_SAMPT_POS)          /**< 0x0000000B */
#define ADC_SAMPT_SAMPT_639CYCLES      (0xCUL << ADC_SAMPT_SAMPT_POS)          /**< 0x0000000C */
#define ADC_SAMPT_SAMPT_959CYCLES      (0xDUL << ADC_SAMPT_SAMPT_POS)          /**< 0x0000000D */
#define ADC_SAMPT_SAMPT_1279CYCLES     (0xEUL << ADC_SAMPT_SAMPT_POS)          /**< 0x0000000E */
#define ADC_SAMPT_SAMPT_1919CYCLES     (0xFUL << ADC_SAMPT_SAMPT_POS)          /**< 0x0000000F */


/* Bits for ADC_CHCFG */
#define ADC_CHCFG_CHN_POS              (0U)
#define ADC_CHCFG_CHN_MASK             (0x1FFUL << ADC_CHCFG_CHN_POS)          /**< 0x000001FF */
#define ADC_CHCFG_CHN                  ADC_CHCFG_CHN_MASK                      /**< ADC group regular sequencer channels */

#define ADC_CHCFG_CHN0_POS             (0U)
#define ADC_CHCFG_CHN0_MASK            (0x1UL << ADC_CHCFG_CHN0_POS)           /**< 0x00000001 */
#define ADC_CHCFG_CHN0                 ADC_CHCFG_CHN0_MASK                     /**< ADC group regular sequencer channel 0 */

#define ADC_CHCFG_CHN1_POS             (1U)
#define ADC_CHCFG_CHN1_MASK            (0x1UL << ADC_CHCFG_CHN1_POS)           /**< 0x00000002 */
#define ADC_CHCFG_CHN1                 ADC_CHCFG_CHN1_MASK                     /**< ADC group regular sequencer channel 1 */

#define ADC_CHCFG_CHN2_POS             (2U)
#define ADC_CHCFG_CHN2_MASK            (0x1UL << ADC_CHCFG_CHN2_POS)           /**< 0x00000004 */
#define ADC_CHCFG_CHN2                 ADC_CHCFG_CHN2_MASK                     /**< ADC group regular sequencer channel 2 */

#define ADC_CHCFG_CHN3_POS             (3U)
#define ADC_CHCFG_CHN3_MASK            (0x1UL << ADC_CHCFG_CHN3_POS)           /**< 0x00000008 */
#define ADC_CHCFG_CHN3                 ADC_CHCFG_CHN3_MASK                     /**< ADC group regular sequencer channel 3 */

#define ADC_CHCFG_CHN4_POS             (4U)
#define ADC_CHCFG_CHN4_MASK            (0x1UL << ADC_CHCFG_CHN4_POS)           /**< 0x00000010 */
#define ADC_CHCFG_CHN4                 ADC_CHCFG_CHN4_MASK                     /**< ADC group regular sequencer channel 4 */

#define ADC_CHCFG_CHN5_POS             (5U)
#define ADC_CHCFG_CHN5_MASK            (0x1UL << ADC_CHCFG_CHN5_POS)           /**< 0x00000020 */
#define ADC_CHCFG_CHN5                 ADC_CHCFG_CHN5_MASK                     /**< ADC group regular sequencer channel 5 */

#define ADC_CHCFG_CHN6_POS             (6U)
#define ADC_CHCFG_CHN6_MASK            (0x1UL << ADC_CHCFG_CHN6_POS)           /**< 0x00000040 */
#define ADC_CHCFG_CHN6                 ADC_CHCFG_CHN6_MASK                     /**< ADC group regular sequencer channel 6 */

#define ADC_CHCFG_CHN7_POS             (7U)
#define ADC_CHCFG_CHN7_MASK            (0x1UL << ADC_CHCFG_CHN7_POS)           /**< 0x00000080 */
#define ADC_CHCFG_CHN7                 ADC_CHCFG_CHN7_MASK                     /**< ADC group regular sequencer channel 7 */

#define ADC_CHCFG_CHN8_POS             (8U)
#define ADC_CHCFG_CHN8_MASK            (0x1UL << ADC_CHCFG_CHN8_POS)           /**< 0x00000100 */
#define ADC_CHCFG_CHN8                 ADC_CHCFG_CHN8_MASK                     /**< ADC group regular sequencer channel 8 */

/* Bits for ADC_AWDGCR */
#define ADC_AWDGCR_CHN_POS             (0U)
#define ADC_AWDGCR_CHN_MASK            (0x1FFUL << ADC_AWDGCR_CHN_POS)         /**< 0x000001FF */
#define ADC_AWDGCR_CHN                 ADC_AWDGCR_CHN_MASK                     /**< ADC analog watchdog monitored channel selection */
                                     
#define ADC_AWDGCR_CHN0_POS            (0U)
#define ADC_AWDGCR_CHN0_MASK           (0x1UL << ADC_AWDGCR_CHN0_POS)          /**< 0x00000001 */
#define ADC_AWDGCR_CHN0                ADC_AWDGCR_CHN0_MASK                    /**< ADC analog watchdog monitored channel 0 */

#define ADC_AWDGCR_CHN1_POS            (1U)
#define ADC_AWDGCR_CHN1_MASK           (0x1UL << ADC_AWDGCR_CHN1_POS)          /**< 0x00000002 */
#define ADC_AWDGCR_CHN1                ADC_AWDGCR_CHN1_MASK                    /**< ADC analog watchdog monitored channel 1 */

#define ADC_AWDGCR_CHN2_POS            (2U)
#define ADC_AWDGCR_CHN2_MASK           (0x1UL << ADC_AWDGCR_CHN2_POS)          /**< 0x00000004 */
#define ADC_AWDGCR_CHN2                ADC_AWDGCR_CHN2_MASK                    /**< ADC analog watchdog monitored channel 2 */

#define ADC_AWDGCR_CHN3_POS            (3U)
#define ADC_AWDGCR_CHN3_MASK           (0x1UL << ADC_AWDGCR_CHN3_POS)          /**< 0x00000008 */
#define ADC_AWDGCR_CHN3                ADC_AWDGCR_CHN3_MASK                    /**< ADC analog watchdog monitored channel 3 */

#define ADC_AWDGCR_CHN4_POS            (4U)
#define ADC_AWDGCR_CHN4_MASK           (0x1UL << ADC_AWDGCR_CHN4_POS)          /**< 0x00000010 */
#define ADC_AWDGCR_CHN4                ADC_AWDGCR_CHN4_MASK                    /**< ADC analog watchdog monitored channel 4 */

#define ADC_AWDGCR_CHN5_POS            (5U)
#define ADC_AWDGCR_CHN5_MASK           (0x1UL << ADC_AWDGCR_CHN5_POS)          /**< 0x00000020 */
#define ADC_AWDGCR_CHN5                ADC_AWDGCR_CHN5_MASK                    /**< ADC analog watchdog monitored channel 5 */

#define ADC_AWDGCR_CHN6_POS            (6U)
#define ADC_AWDGCR_CHN6_MASK           (0x1UL << ADC_AWDGCR_CHN6_POS)          /**< 0x00000040 */
#define ADC_AWDGCR_CHN6                ADC_AWDGCR_CHN6_MASK                    /**< ADC analog watchdog monitored channel 6 */

#define ADC_AWDGCR_CHN7_POS            (7U)
#define ADC_AWDGCR_CHN7_MASK           (0x1UL << ADC_AWDGCR_CHN7_POS)          /**< 0x00000080 */
#define ADC_AWDGCR_CHN7                ADC_AWDGCR_CHN7_MASK                    /**< ADC analog watchdog monitored channel 7 */

#define ADC_AWDGCR_CHN8_POS            (8U)
#define ADC_AWDGCR_CHN8_MASK           (0x1UL << ADC_AWDGCR_CHN8_POS)          /**< 0x00000100 */
#define ADC_AWDGCR_CHN8                ADC_AWDGCR_CHN8_MASK                    /**< ADC analog watchdog monitored channel 8 */


/* Bits for ADC_AWDGTR */
#define ADC_AWDGTR_AWDG_LT_POS         (0U)
#define ADC_AWDGTR_AWDG_LT_MASK        (0xFFFUL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000FFF */
#define ADC_AWDGTR_AWDG_LT             ADC_AWDGTR_AWDG_LT_MASK                 /**< ADC analog watchdog threshold low */

#define ADC_AWDGTR_AWDG_LT_0           (0x001UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000001 */
#define ADC_AWDGTR_AWDG_LT_1           (0x002UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000002 */
#define ADC_AWDGTR_AWDG_LT_2           (0x004UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000004 */
#define ADC_AWDGTR_AWDG_LT_3           (0x008UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000008 */
#define ADC_AWDGTR_AWDG_LT_4           (0x010UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000010 */
#define ADC_AWDGTR_AWDG_LT_5           (0x020UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000020 */
#define ADC_AWDGTR_AWDG_LT_6           (0x040UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000040 */
#define ADC_AWDGTR_AWDG_LT_7           (0x080UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000080 */
#define ADC_AWDGTR_AWDG_LT_8           (0x100UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000100 */
#define ADC_AWDGTR_AWDG_LT_9           (0x200UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000200 */
#define ADC_AWDGTR_AWDG_LT_10          (0x400UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000400 */
#define ADC_AWDGTR_AWDG_LT_11          (0x800UL << ADC_AWDGTR_AWDG_LT_POS)     /**< 0x00000800 */

#define ADC_AWDGTR_AWDG_HT_POS         (16U)
#define ADC_AWDGTR_AWDG_HT_MASK        (0xFFFUL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x0FFF0000 */
#define ADC_AWDGTR_AWDG_HT             ADC_AWDGTR_AWDG_HT_MASK                 /**< ADC Analog watchdog threshold high */

#define ADC_AWDGTR_AWDG_HT_0           (0x001UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00010000 */
#define ADC_AWDGTR_AWDG_HT_1           (0x002UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00020000 */
#define ADC_AWDGTR_AWDG_HT_2           (0x004UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00040000 */
#define ADC_AWDGTR_AWDG_HT_3           (0x008UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00080000 */
#define ADC_AWDGTR_AWDG_HT_4           (0x010UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00100000 */
#define ADC_AWDGTR_AWDG_HT_5           (0x020UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00200000 */
#define ADC_AWDGTR_AWDG_HT_6           (0x040UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00400000 */
#define ADC_AWDGTR_AWDG_HT_7           (0x080UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x00800000 */
#define ADC_AWDGTR_AWDG_HT_8           (0x100UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x01000000 */
#define ADC_AWDGTR_AWDG_HT_9           (0x200UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x02000000 */
#define ADC_AWDGTR_AWDG_HT_10          (0x400UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x04000000 */
#define ADC_AWDGTR_AWDG_HT_11          (0x800UL << ADC_AWDGTR_AWDG_HT_POS)     /**< 0x08000000 */

/* Bits for ADC_CALFACT */
#define ADC_CALFACT_CALFACT_POS         (0U)
#define ADC_CALFACT_CALFACT_MASK        (0x3FUL << ADC_CALFACT_CALFACT_POS)    /**< 0x0000003F */
#define ADC_CALFACT_CALFACT             ADC_CALFACT_CALFACT_MASK               /**< ADC calibration factor in single-ended mode */
#define ADC_CALFACT_CALFACT_0           (0x01UL << ADC_CALFACT_CALFACT_POS)    /**< 0x00000001 */
#define ADC_CALFACT_CALFACT_1           (0x02UL << ADC_CALFACT_CALFACT_POS)    /**< 0x00000002 */
#define ADC_CALFACT_CALFACT_2           (0x04UL << ADC_CALFACT_CALFACT_POS)    /**< 0x00000004 */
#define ADC_CALFACT_CALFACT_3           (0x08UL << ADC_CALFACT_CALFACT_POS)    /**< 0x00000008 */
#define ADC_CALFACT_CALFACT_4           (0x10UL << ADC_CALFACT_CALFACT_POS)    /**< 0x00000010 */
#define ADC_CALFACT_CALFACT_5           (0x20UL << ADC_CALFACT_CALFACT_POS)    /**< 0x00000020 */

/* Bits for ADC_DR */
#define ADC_DR_DATA_POS                 (0U)
#define ADC_DR_DATA_MASK                (0xFFFUL << ADC_DR_DATA_POS)           /**< 0x00000FFF */
#define ADC_DR_DATA                     ADC_DR_DATA_MASK                       /**< ADC group regular conversion data */

/* Bits for ADC_CFG3 */
#define ADC_CFG3_MODE_POS               (1U)
#define ADC_CFG3_MODE_MASK              (0x1UL << ADC_CFG3_MODE_POS)           /**< 0x00000002 */
#define ADC_CFG3_MODE                   ADC_CFG3_MODE_MASK                     /**< ADC mode selection   */

/*-----------------------------------------------------------------------------------------------
                                       COMP Registers Bits
------------------------------------------------------------------------------------------------*/
/* Bits for COMP_CSR */
#define COMP_CSR_EN_POS            (0U)
#define COMP_CSR_EN_MASK           (0x1UL << COMP_CSR_EN_POS)                  /**< 0x00000001 */
#define COMP_CSR_EN                COMP_CSR_EN_MASK                            /**< Comparator enable */

#define COMP_CSR_INM_POS           (4U)
#define COMP_CSR_INM_MASK          (0x1UL << COMP_CSR_INM_POS)                 /**< 0x00000010 */
#define COMP_CSR_INM               COMP_CSR_INM_MASK                           /**< Comparator input minus selection */
#define COMP_CSR_INM_INT_VREF      (0x0UL << COMP_CSR_INM_POS)                 /**< 0x00000000 */
#define COMP_CSR_INM_IO            (0x1UL << COMP_CSR_INM_POS)                 /**< 0x00000010 */

#define COMP_CSR_INP_POS           (8U)
#define COMP_CSR_INP_MASK          (0x1UL << COMP_CSR_INP_POS)                 /**< 0x00000100 */
#define COMP_CSR_INP               COMP_CSR_INP_MASK                           /**< Comparator input plus selection */
#define COMP_CSR_INP_IO1           (0x0UL << COMP_CSR_INP_POS)                 /**< 0x00000000 */
#define COMP_CSR_INP_IO2           (0x1UL << COMP_CSR_INP_POS)                 /**< 0x00000100 */

#define COMP_CSR_INPMOD_POS        (13U)
#define COMP_CSR_INPMOD_MASK       (0x1UL << COMP_CSR_INPMOD_POS)              /**< 0x00002000 */
#define COMP_CSR_INPMOD            COMP_CSR_INPMOD_MASK                        /**< Pair of comparators window mode. */

#define COMP_CSR_OUTMOD_POS        (14U)
#define COMP_CSR_OUTMOD_MASK       (0x1UL << COMP_CSR_OUTMOD_POS)              /**< 0x00004000 */
#define COMP_CSR_OUTMOD            COMP_CSR_OUTMOD_MASK                        /**< Pair of comparators window output level. */

#define COMP_CSR_POL_POS           (15U)
#define COMP_CSR_POL_MASK          (0x1UL << COMP_CSR_POL_POS)                 /**< 0x00008000 */
#define COMP_CSR_POL               COMP_CSR_POL_MASK                           /**< Comparator output polarity */

#define COMP_CSR_FLTIME_POS        (25U)
#define COMP_CSR_FLTIME_MASK       (0x7UL << COMP_CSR_FLTIME_POS)              /**< 0x0E000000 */
#define COMP_CSR_FLTIME            COMP_CSR_FLTIME_MASK                        /**< Comparator filtering time */
#define COMP_CSR_FLTIME_1CYCLE     (0x0UL << COMP_CSR_FLTIME_POS)              /**< 0x00000000 */
#define COMP_CSR_FLTIME_3CYCLE     (0x1UL << COMP_CSR_FLTIME_POS)              /**< 0x02000000 */
#define COMP_CSR_FLTIME_7CYCLE     (0x2UL << COMP_CSR_FLTIME_POS)              /**< 0x04000000 */
#define COMP_CSR_FLTIME_15CYCLE    (0x3UL << COMP_CSR_FLTIME_POS)              /**< 0x06000000 */
#define COMP_CSR_FLTIME_31CYCLE    (0x4UL << COMP_CSR_FLTIME_POS)              /**< 0x08000000 */
#define COMP_CSR_FLTIME_63CYCLE    (0x5UL << COMP_CSR_FLTIME_POS)              /**< 0x0A000000 */
#define COMP_CSR_FLTIME_255CYCLE   (0x6UL << COMP_CSR_FLTIME_POS)              /**< 0x0C000000 */
#define COMP_CSR_FLTIME_1023CYCLE  (0x7UL << COMP_CSR_FLTIME_POS)              /**< 0x0E000000 */

#define COMP_CSR_FLTEN_POS         (28U)
#define COMP_CSR_FLTEN_MASK        (0x1UL << COMP_CSR_FLTEN_POS)               /**< 0x10000000 */
#define COMP_CSR_FLTEN              COMP_CSR_FLTEN_MASK                        /**< Comparator filtering enable */

#define COMP_CSR_VAL_POS           (30U)
#define COMP_CSR_VAL_MASK          (0x1UL << COMP_CSR_VAL_POS)                 /**< 0x40000000 */
#define COMP_CSR_VAL               COMP_CSR_VAL_MASK                           /**< Comparator output level */

/* Bits for COMP_CR */
#define COMP_CR_VCDIV_POS          (0U)
#define COMP_CR_VCDIV_MASK         (0xFUL << COMP_CR_VCDIV_POS)                /**< 0x0000000F */
#define COMP_CR_VCDIV              COMP_CR_VCDIV_MASK                          /**< Comparator VDDA divide selection */
#define COMP_CR_VCDIV_1DIV16       (0x0UL << COMP_CR_VCDIV_POS)                /**< 0x00000000 */
#define COMP_CR_VCDIV_2DIV16       (0x1UL << COMP_CR_VCDIV_POS)                /**< 0x00000001 */
#define COMP_CR_VCDIV_3DIV16       (0x2UL << COMP_CR_VCDIV_POS)                /**< 0x00000002 */
#define COMP_CR_VCDIV_4DIV16       (0x3UL << COMP_CR_VCDIV_POS)                /**< 0x00000003 */
#define COMP_CR_VCDIV_5DIV16       (0x4UL << COMP_CR_VCDIV_POS)                /**< 0x00000004 */
#define COMP_CR_VCDIV_6DIV16       (0x5UL << COMP_CR_VCDIV_POS)                /**< 0x00000005 */
#define COMP_CR_VCDIV_7DIV16       (0x6UL << COMP_CR_VCDIV_POS)                /**< 0x00000006 */
#define COMP_CR_VCDIV_8DIV16       (0x7UL << COMP_CR_VCDIV_POS)                /**< 0x00000007 */
#define COMP_CR_VCDIV_9DIV16       (0x8UL << COMP_CR_VCDIV_POS)                /**< 0x00000008 */
#define COMP_CR_VCDIV_10DIV16      (0x9UL << COMP_CR_VCDIV_POS)                /**< 0x00000009 */
#define COMP_CR_VCDIV_11DIV16      (0xAUL << COMP_CR_VCDIV_POS)                /**< 0x0000000A */
#define COMP_CR_VCDIV_12DIV16      (0xBUL << COMP_CR_VCDIV_POS)                /**< 0x0000000B */
#define COMP_CR_VCDIV_13DIV16      (0xCUL << COMP_CR_VCDIV_POS)                /**< 0x0000000C */

#define COMP_CR_VCSEL_POS          (6U)
#define COMP_CR_VCSEL_MASK         (0x1UL << COMP_CR_VCSEL_POS)                /**< 0x00000040 */
#define COMP_CR_VCSEL              COMP_CR_VCSEL_MASK                          /**< Comparator internal reference voltage selection */

#define COMP_CR_HYST_POS           (16)
#define COMP_CR_HYST_MASK          (0x1UL << COMP_CR_HYST_POS)                 /**< 0x00010000 */
#define COMP_CR_HYST               COMP_CR_HYST_MASK                           /**< Comparator input hysteresis function control*/

/*-----------------------------------------------------------------------------------------------
                                       CRC Registers Bits
------------------------------------------------------------------------------------------------*/

/* Bits for CRC_CSR */
#define CRC_CSR_POLY_SIZE_POS      (0U)
#define CRC_CSR_POLY_SIZE_MASK     (0x1UL << CRC_CSR_POLY_SIZE_POS)            /**< 0x00000001 */
#define CRC_CSR_POLY_SIZE          CRC_CSR_POLY_SIZE_MASK                      /**< Polynomial size bits */
#define CRC_CSR_POLY_SIZE_16       (0x0UL << CRC_CSR_POLY_SIZE_POS)            /**< 0x00000000 */
#define CRC_CSR_POLY_SIZE_32       (0x1UL << CRC_CSR_POLY_SIZE_POS)            /**< 0x00000001 */

/* Bits for CRC_RDR */
#define CRC_RDR_RESULT_POS         (0U)
#define CRC_RDR_RESULT_MASK        (0xFFFFFFFFUL << CRC_RDR_RESULT_POS)        /**< 0xFFFFFFFF */
#define CRC_RDR_RESULT             CRC_RDR_RESULT_MASK                         /**< Result data register bits */

/* Bits for CRC_DR */
#define CRC_DR_DATA_POS            (0U)
#define CRC_DR_DATA_MASK           (0xFFUL << CRC_DR_DATA_POS)                 /**< 0x000000FF */
#define CRC_DR_DATA                CRC_DR_DATA_MASK                            /**< Data register bits */

/*-----------------------------------------------------------------------------------------------
                                       DBG Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for DBG_CR */
#define DBG_CR_DBG_STOP_POS                            (0U)
#define DBG_CR_DBG_STOP_MASK                           (0x1UL << DBG_CR_DBG_STOP_POS)             /**< 0x00000001 */
#define DBG_CR_DBG_STOP                                DBG_CR_DBG_STOP_MASK

/* Bits for DBG_APB_FZ1 */
#define DBG_APB_FZ1_TIM3_HOLD_POS                      (1U)
#define DBG_APB_FZ1_TIM3_HOLD_MASK                     (0x1UL << DBG_APB_FZ1_TIM3_HOLD_POS)       /**< 0x00000002 */
#define DBG_APB_FZ1_TIM3_HOLD                          DBG_APB_FZ1_TIM3_HOLD_MASK
#define DBG_APB_FZ1_IWDG_HOLD_POS                      (18U)
#define DBG_APB_FZ1_IWDG_HOLD_MASK                     (0x1UL << DBG_APB_FZ1_IWDG_HOLD_POS)       /**< 0x00040000 */
#define DBG_APB_FZ1_IWDG_HOLD                          DBG_APB_FZ1_IWDG_HOLD_MASK
#define DBG_APB_FZ1_LPTIM1_HOLD_POS                    (29U)
#define DBG_APB_FZ1_LPTIM1_HOLD_MASK                   (0x1UL << DBG_APB_FZ1_LPTIM1_HOLD_POS)     /**< 0x20000000 */
#define DBG_APB_FZ1_LPTIM1_HOLD                        DBG_APB_FZ1_LPTIM1_HOLD_MASK

/* Bits for DBG_APB_FZ2 */
#define DBG_APB_FZ2_TIM1_HOLD_POS                      (0U)
#define DBG_APB_FZ2_TIM1_HOLD_MASK                     (0x1UL << DBG_APB_FZ2_TIM1_HOLD_POS)       /**< 0x00000001 */
#define DBG_APB_FZ2_TIM1_HOLD                          DBG_APB_FZ2_TIM1_HOLD_MASK

/*-----------------------------------------------------------------------------------------------
                                       EXTI Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for EXTI_RTSR */
#define EXTI_RTSR_RT0_POS           (0U)
#define EXTI_RTSR_RT0_MASK          (0x1UL << EXTI_RTSR_RT0_POS)              /**< 0x00000001 */
#define EXTI_RTSR_RT0               EXTI_RTSR_RT0_MASK                        /**< Rising trigger configuration for input line 0 */
#define EXTI_RTSR_RT1_POS           (1U)
#define EXTI_RTSR_RT1_MASK          (0x1UL << EXTI_RTSR_RT1_POS)              /**< 0x00000002 */
#define EXTI_RTSR_RT1               EXTI_RTSR_RT1_MASK                        /**< Rising trigger configuration for input line 1 */
#define EXTI_RTSR_RT2_POS           (2U)
#define EXTI_RTSR_RT2_MASK          (0x1UL << EXTI_RTSR_RT2_POS)              /**< 0x00000004 */
#define EXTI_RTSR_RT2               EXTI_RTSR_RT2_MASK                        /**< Rising trigger configuration for input line 2 */
#define EXTI_RTSR_RT3_POS           (3U)
#define EXTI_RTSR_RT3_MASK          (0x1UL << EXTI_RTSR_RT3_POS)              /**< 0x00000008 */
#define EXTI_RTSR_RT3               EXTI_RTSR_RT3_MASK                        /**< Rising trigger configuration for input line 3 */
#define EXTI_RTSR_RT4_POS           (4U)
#define EXTI_RTSR_RT4_MASK          (0x1UL << EXTI_RTSR_RT4_POS)              /**< 0x00000010 */
#define EXTI_RTSR_RT4               EXTI_RTSR_RT4_MASK                        /**< Rising trigger configuration for input line 4 */
#define EXTI_RTSR_RT5_POS           (5U)
#define EXTI_RTSR_RT5_MASK          (0x1UL << EXTI_RTSR_RT5_POS)              /**< 0x00000020 */
#define EXTI_RTSR_RT5               EXTI_RTSR_RT5_MASK                        /**< Rising trigger configuration for input line 5 */
#define EXTI_RTSR_RT6_POS           (6U)
#define EXTI_RTSR_RT6_MASK          (0x1UL << EXTI_RTSR_RT6_POS)              /**< 0x00000040 */
#define EXTI_RTSR_RT6               EXTI_RTSR_RT6_MASK                        /**< Rising trigger configuration for input line 6 */
#define EXTI_RTSR_RT7_POS           (7U)
#define EXTI_RTSR_RT7_MASK          (0x1UL << EXTI_RTSR_RT7_POS)              /**< 0x00000080 */
#define EXTI_RTSR_RT7               EXTI_RTSR_RT7_MASK                        /**< Rising trigger configuration for input line 7 */

#define EXTI_RTSR_RT16_POS          (16U)
#define EXTI_RTSR_RT16_MASK         (0x1UL << EXTI_RTSR_RT16_POS)             /**< 0x00010000 */
#define EXTI_RTSR_RT16              EXTI_RTSR_RT16_MASK                       /**< Rising trigger configuration for input line 16 */
#define EXTI_RTSR_RT17_POS          (17U)
#define EXTI_RTSR_RT17_MASK         (0x1UL << EXTI_RTSR_RT17_POS)             /**< 0x00020000 */
#define EXTI_RTSR_RT17              EXTI_RTSR_RT17_MASK                       /**< Rising trigger configuration for input line 17 */

/* Bits for EXTI_FTSR */
#define EXTI_FTSR_FT0_POS           (0U)
#define EXTI_FTSR_FT0_MASK          (0x1UL << EXTI_FTSR_FT0_POS)              /**< 0x00000001 */
#define EXTI_FTSR_FT0               EXTI_FTSR_FT0_MASK                        /**< Falling trigger configuration for input line 0 */
#define EXTI_FTSR_FT1_POS           (1U)
#define EXTI_FTSR_FT1_MASK          (0x1UL << EXTI_FTSR_FT1_POS)              /**< 0x00000002 */
#define EXTI_FTSR_FT1               EXTI_FTSR_FT1_MASK                        /**< Falling trigger configuration for input line 1 */
#define EXTI_FTSR_FT2_POS           (2U)
#define EXTI_FTSR_FT2_MASK          (0x1UL << EXTI_FTSR_FT2_POS)              /**< 0x00000004 */
#define EXTI_FTSR_FT2               EXTI_FTSR_FT2_MASK                        /**< Falling trigger configuration for input line 2 */
#define EXTI_FTSR_FT3_POS           (3U)
#define EXTI_FTSR_FT3_MASK          (0x1UL << EXTI_FTSR_FT3_POS)              /**< 0x00000008 */
#define EXTI_FTSR_FT3               EXTI_FTSR_FT3_MASK                        /**< Falling trigger configuration for input line 3 */
#define EXTI_FTSR_FT4_POS           (4U)
#define EXTI_FTSR_FT4_MASK          (0x1UL << EXTI_FTSR_FT4_POS)              /**< 0x00000010 */
#define EXTI_FTSR_FT4               EXTI_FTSR_FT4_MASK                        /**< Falling trigger configuration for input line 4 */
#define EXTI_FTSR_FT5_POS           (5U)
#define EXTI_FTSR_FT5_MASK          (0x1UL << EXTI_FTSR_FT5_POS)              /**< 0x00000020 */
#define EXTI_FTSR_FT5               EXTI_FTSR_FT5_MASK                        /**< Falling trigger configuration for input line 5 */
#define EXTI_FTSR_FT6_POS           (6U)
#define EXTI_FTSR_FT6_MASK          (0x1UL << EXTI_FTSR_FT6_POS)              /**< 0x00000040 */
#define EXTI_FTSR_FT6               EXTI_FTSR_FT6_MASK                        /**< Falling trigger configuration for input line 6 */
#define EXTI_FTSR_FT7_POS           (7U)
#define EXTI_FTSR_FT7_MASK          (0x1UL << EXTI_FTSR_FT7_POS)              /**< 0x00000080 */
#define EXTI_FTSR_FT7               EXTI_FTSR_FT7_MASK                        /**< Falling trigger configuration for input line 7 */

#define EXTI_FTSR_FT16_POS          (16U)
#define EXTI_FTSR_FT16_MASK         (0x1UL << EXTI_FTSR_FT16_POS)             /**< 0x00010000 */
#define EXTI_FTSR_FT16              EXTI_FTSR_FT16_MASK                       /**< Falling trigger configuration for input line 16 */
#define EXTI_FTSR_FT17_POS          (17U)
#define EXTI_FTSR_FT17_MASK         (0x1UL << EXTI_FTSR_FT17_POS)             /**< 0x00020000 */
#define EXTI_FTSR_FT17              EXTI_FTSR_FT17_MASK                       /**< Falling trigger configuration for input line 17 */

/* Bits for EXTI_PIR */
#define EXTI_PIR_PIF0_POS          (0U)
#define EXTI_PIR_PIF0_MASK         (0x1UL << EXTI_PIR_PIF0_POS)               /**< 0x00000001 */
#define EXTI_PIR_PIF0              EXTI_PIR_PIF0_MASK                         /**< Rising or Falling pending interrupt flag on line 0 */
#define EXTI_PIR_PIF1_POS          (1U)
#define EXTI_PIR_PIF1_MASK         (0x1UL << EXTI_PIR_PIF1_POS)               /**< 0x00000002 */
#define EXTI_PIR_PIF1              EXTI_PIR_PIF1_MASK                         /**< Rising or Falling pending interrupt flag on line 1 */
#define EXTI_PIR_PIF2_POS          (2U)
#define EXTI_PIR_PIF2_MASK         (0x1UL << EXTI_PIR_PIF2_POS)               /**< 0x00000004 */
#define EXTI_PIR_PIF2              EXTI_PIR_PIF2_MASK                         /**< Rising or Falling pending interrupt flag on line 2 */
#define EXTI_PIR_PIF3_POS          (3U)
#define EXTI_PIR_PIF3_MASK         (0x1UL << EXTI_PIR_PIF3_POS)               /**< 0x00000008 */
#define EXTI_PIR_PIF3              EXTI_PIR_PIF3_MASK                         /**< Rising or Falling pending interrupt flag on line 3 */
#define EXTI_PIR_PIF4_POS          (4U)
#define EXTI_PIR_PIF4_MASK         (0x1UL << EXTI_PIR_PIF4_POS)               /**< 0x00000010 */
#define EXTI_PIR_PIF4              EXTI_PIR_PIF4_MASK                         /**< Rising or Falling pending interrupt flag on line 4 */
#define EXTI_PIR_PIF5_POS          (5U)
#define EXTI_PIR_PIF5_MASK         (0x1UL << EXTI_PIR_PIF5_POS)               /**< 0x00000020 */
#define EXTI_PIR_PIF5              EXTI_PIR_PIF5_MASK                         /**< Rising or Falling pending interrupt flag on line 5 */
#define EXTI_PIR_PIF6_POS          (6U)
#define EXTI_PIR_PIF6_MASK         (0x1UL << EXTI_PIR_PIF6_POS)               /**< 0x00000040 */
#define EXTI_PIR_PIF6              EXTI_PIR_PIF6_MASK                         /**< Rising or Falling pending interrupt flag on line 6 */
#define EXTI_PIR_PIF7_POS          (7U)
#define EXTI_PIR_PIF7_MASK         (0x1UL << EXTI_PIR_PIF7_POS)               /**< 0x00000080 */
#define EXTI_PIR_PIF7              EXTI_PIR_PIF7_MASK                         /**< Rising or Falling pending interrupt flag on line 7 */

#define EXTI_PIR_PIF16_POS         (16U)
#define EXTI_PIR_PIF16_MASK        (0x1UL << EXTI_PIR_PIF16_POS)              /**< 0x00010000 */
#define EXTI_PIR_PIF16             EXTI_PIR_PIF16_MASK                        /**< Rising or Falling pending interrupt flag on line 16 */
#define EXTI_PIR_PIF17_POS         (17U)
#define EXTI_PIR_PIF17_MASK        (0x1UL << EXTI_PIR_PIF17_POS)              /**< 0x00020000 */
#define EXTI_PIR_PIF17             EXTI_PIR_PIF17_MASK                        /**< Rising or Falling pending interrupt flag on line 17 */

/* Bits for EXTI_EXTICR1 */
#define EXTI_EXTICR1_EXTI0_POS            (0U)
#define EXTI_EXTICR1_EXTI0_MASK           (0x3UL << EXTI_EXTICR1_EXTI0_POS)   /**< 0x00000003 */
#define EXTI_EXTICR1_EXTI0                EXTI_EXTICR1_EXTI0_MASK             /**< EXTI 0 configuration */
#define EXTI_EXTICR1_EXTI1_POS            (4U)
#define EXTI_EXTICR1_EXTI1_MASK           (0x3UL << EXTI_EXTICR1_EXTI1_POS)   /**< 0x00000030 */
#define EXTI_EXTICR1_EXTI1                EXTI_EXTICR1_EXTI1_MASK             /**< EXTI 1 configuration */
#define EXTI_EXTICR1_EXTI2_POS            (8U)
#define EXTI_EXTICR1_EXTI2_MASK           (0x3UL << EXTI_EXTICR1_EXTI2_POS)   /**< 0x00000300 */
#define EXTI_EXTICR1_EXTI2                EXTI_EXTICR1_EXTI2_MASK             /**< EXTI 2 configuration */
#define EXTI_EXTICR1_EXTI3_POS            (12U)
#define EXTI_EXTICR1_EXTI3_MASK           (0x3UL << EXTI_EXTICR1_EXTI3_POS)   /**< 0x00003000 */
#define EXTI_EXTICR1_EXTI3                EXTI_EXTICR1_EXTI3_MASK             /**< EXTI 3 configuration */
#define EXTI_EXTICR1_EXTI4_POS            (16U)
#define EXTI_EXTICR1_EXTI4_MASK           (0x3UL << EXTI_EXTICR1_EXTI4_POS)   /**< 0x00030000 */
#define EXTI_EXTICR1_EXTI4                EXTI_EXTICR1_EXTI4_MASK             /**< EXTI 4 configuration */
#define EXTI_EXTICR1_EXTI5_POS            (20U)
#define EXTI_EXTICR1_EXTI5_MASK           (0x3UL << EXTI_EXTICR1_EXTI5_POS)   /**< 0x00300000 */
#define EXTI_EXTICR1_EXTI5                EXTI_EXTICR1_EXTI5_MASK             /**< EXTI 5 configuration */
#define EXTI_EXTICR1_EXTI6_POS            (24U)
#define EXTI_EXTICR1_EXTI6_MASK           (0x3UL << EXTI_EXTICR1_EXTI6_POS)   /**< 0x03000000 */
#define EXTI_EXTICR1_EXTI6                EXTI_EXTICR1_EXTI6_MASK             /**< EXTI 6 configuration */
#define EXTI_EXTICR1_EXTI7_POS            (28U)
#define EXTI_EXTICR1_EXTI7_MASK           (0x3UL << EXTI_EXTICR1_EXTI7_POS)   /**< 0x30000000 */
#define EXTI_EXTICR1_EXTI7                EXTI_EXTICR1_EXTI7_MASK             /**< EXTI 7 configuration */


/* Bits for EXTI_IMR */
#define EXTI_IMR_IM0_POS            (0U)
#define EXTI_IMR_IM0_MASK           (0x1UL << EXTI_IMR_IM0_POS)               /**< 0x00000001 */
#define EXTI_IMR_IM0                EXTI_IMR_IM0_MASK                         /**< Interrupt mask on line 0 */
#define EXTI_IMR_IM1_POS            (1U)
#define EXTI_IMR_IM1_MASK           (0x1UL << EXTI_IMR_IM1_POS)               /**< 0x00000002 */
#define EXTI_IMR_IM1                EXTI_IMR_IM1_MASK                         /**< Interrupt mask on line 1 */
#define EXTI_IMR_IM2_POS            (2U)
#define EXTI_IMR_IM2_MASK           (0x1UL << EXTI_IMR_IM2_POS)               /**< 0x00000004 */
#define EXTI_IMR_IM2                EXTI_IMR_IM2_MASK                         /**< Interrupt mask on line 2 */
#define EXTI_IMR_IM3_POS            (3U)
#define EXTI_IMR_IM3_MASK           (0x1UL << EXTI_IMR_IM3_POS)               /**< 0x00000008 */
#define EXTI_IMR_IM3                EXTI_IMR_IM3_MASK                         /**< Interrupt mask on line 3 */
#define EXTI_IMR_IM4_POS            (4U)
#define EXTI_IMR_IM4_MASK           (0x1UL << EXTI_IMR_IM4_POS)               /**< 0x00000010 */
#define EXTI_IMR_IM4                EXTI_IMR_IM4_MASK                         /**< Interrupt mask on line 4 */
#define EXTI_IMR_IM5_POS            (5U)
#define EXTI_IMR_IM5_MASK           (0x1UL << EXTI_IMR_IM5_POS)               /**< 0x00000020 */
#define EXTI_IMR_IM5                EXTI_IMR_IM5_MASK                         /**< Interrupt mask on line 5 */
#define EXTI_IMR_IM6_POS            (6U)
#define EXTI_IMR_IM6_MASK           (0x1UL << EXTI_IMR_IM6_POS)               /**< 0x00000040 */
#define EXTI_IMR_IM6                EXTI_IMR_IM6_MASK                         /**< Interrupt mask on line 6 */
#define EXTI_IMR_IM7_POS            (7U)
#define EXTI_IMR_IM7_MASK           (0x1UL << EXTI_IMR_IM7_POS)               /**< 0x00000080 */
#define EXTI_IMR_IM7                EXTI_IMR_IM7_MASK                         /**< Interrupt mask on line 7 */

#define EXTI_IMR_IM16_POS           (16U)
#define EXTI_IMR_IM16_MASK          (0x1UL << EXTI_IMR_IM16_POS)              /**< 0x00010000 */
#define EXTI_IMR_IM16               EXTI_IMR_IM16_MASK                        /**< Interrupt mask on line 16 */
#define EXTI_IMR_IM17_POS           (17U)
#define EXTI_IMR_IM17_MASK          (0x1UL << EXTI_IMR_IM17_POS)              /**< 0x00020000 */
#define EXTI_IMR_IM17               EXTI_IMR_IM17_MASK                        /**< Interrupt mask on line 17 */

#define EXTI_IMR_IM30_POS           (30U)
#define EXTI_IMR_IM30_MASK          (0x1UL << EXTI_IMR_IM30_POS)              /**< 0x40000000 */
#define EXTI_IMR_IM30               EXTI_IMR_IM30_MASK                        /**< Interrupt mask on line 30 */

/* Bits for EXTI_EMR */
#define EXTI_EMR_EM0_POS            (0U)
#define EXTI_EMR_EM0_MASK           (0x1UL << EXTI_EMR_EM0_POS)               /**< 0x00000001 */
#define EXTI_EMR_EM0                EXTI_EMR_EM0_MASK                         /**< Event mask on line 0 */
#define EXTI_EMR_EM1_POS            (1U)
#define EXTI_EMR_EM1_MASK           (0x1UL << EXTI_EMR_EM1_POS)               /**< 0x00000002 */
#define EXTI_EMR_EM1                EXTI_EMR_EM1_MASK                         /**< Event mask on line 1 */
#define EXTI_EMR_EM2_POS            (2U)
#define EXTI_EMR_EM2_MASK           (0x1UL << EXTI_EMR_EM2_POS)               /**< 0x00000004 */
#define EXTI_EMR_EM2                EXTI_EMR_EM2_MASK                         /**< Event mask on line 2 */
#define EXTI_EMR_EM3_POS            (3U)
#define EXTI_EMR_EM3_MASK           (0x1UL << EXTI_EMR_EM3_POS)               /**< 0x00000008 */
#define EXTI_EMR_EM3                EXTI_EMR_EM3_MASK                         /**< Event mask on line 3 */
#define EXTI_EMR_EM4_POS            (4U)
#define EXTI_EMR_EM4_MASK           (0x1UL << EXTI_EMR_EM4_POS)               /**< 0x00000010 */
#define EXTI_EMR_EM4                EXTI_EMR_EM4_MASK                         /**< Event mask on line 4 */
#define EXTI_EMR_EM5_POS            (5U)
#define EXTI_EMR_EM5_MASK           (0x1UL << EXTI_EMR_EM5_POS)               /**< 0x00000020 */
#define EXTI_EMR_EM5                EXTI_EMR_EM5_MASK                         /**< Event mask on line 5 */
#define EXTI_EMR_EM6_POS            (6U)
#define EXTI_EMR_EM6_MASK           (0x1UL << EXTI_EMR_EM6_POS)               /**< 0x00000040 */
#define EXTI_EMR_EM6                EXTI_EMR_EM6_MASK                         /**< Event mask on line 6 */
#define EXTI_EMR_EM7_POS            (7U)
#define EXTI_EMR_EM7_MASK           (0x1UL << EXTI_EMR_EM7_POS)               /**< 0x00000080 */
#define EXTI_EMR_EM7                EXTI_EMR_EM7_MASK                         /**< Event mask on line 7 */

#define EXTI_EMR_EM16_POS           (16U)
#define EXTI_EMR_EM16_MASK          (0x1UL << EXTI_EMR_EM16_POS)              /**< 0x00010000 */
#define EXTI_EMR_EM16               EXTI_EMR_EM16_MASK                        /**< Event mask on line 16 */
#define EXTI_EMR_EM17_POS           (17U)
#define EXTI_EMR_EM17_MASK          (0x1UL << EXTI_EMR_EM17_POS)              /**< 0x00020000 */
#define EXTI_EMR_EM17               EXTI_EMR_EM17_MASK                        /**< Event mask on line 17 */

#define EXTI_EMR_EM30_POS           (30U)
#define EXTI_EMR_EM30_MASK          (0x1UL << EXTI_EMR_EM30_POS)              /**< 0x40000000 */
#define EXTI_EMR_EM30               EXTI_EMR_EM30_MASK                        /**< Event mask on line 30 */


/*-----------------------------------------------------------------------------------------------
                                       Flash Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for FLASH_ACR */
#define FLASH_ACR_LATENCY_POS                  (0U)
#define FLASH_ACR_LATENCY_MASK                 (0x1UL << FLASH_ACR_LATENCY_POS)               /**< 0x00000001 */
#define FLASH_ACR_LATENCY                      FLASH_ACR_LATENCY_MASK                         /**< Flash read access waiting_latency */
#define FLASH_ACR_LATENCY_0CLK                 (0x0UL << FLASH_ACR_LATENCY_POS)               /**< 0x00000000 */
#define FLASH_ACR_LATENCY_1CLK                 (0x1UL << FLASH_ACR_LATENCY_POS)               /**< 0x00000001 */

/* Bits for FLASH_CRKEY */
#define FLASH_CRKEY_KEY_POS                    (0U)
#define FLASH_CRKEY_KEY_MASK                   (0xFFFFFFFFUL << FLASH_CRKEY_KEY_POS)          /**< 0xFFFFFFFF */
#define FLASH_CRKEY_KEY                        FLASH_CRKEY_KEY_MASK                           /**< Flash unlock CR regigster key */

/* Bits for FLASH_OPTKEY */
#define FLASH_OPTKEY_KEY_POS                   (0U)
#define FLASH_OPTKEY_KEY_MASK                  (0xFFFFFFFFUL << FLASH_OPTKEY_KEY_POS)         /**< 0xFFFFFFFF */
#define FLASH_OPTKEY_KEY                       FLASH_OPTKEY_KEY_MASK                          /**< Flash unlock option bytes key */

/* Bits for FLASH_SR */
#define FLASH_SR_WRPERR_POS                    (4U)
#define FLASH_SR_WRPERR_MASK                   (0x1UL << FLASH_SR_WRPERR_POS)                 /**< 0x00000010 */
#define FLASH_SR_WRPERR                        FLASH_SR_WRPERR_MASK                           /**< Flash WRP error flag */

#define FLASH_SR_OPTVERR_POS                   (15U)
#define FLASH_SR_OPTVERR_MASK                  (0x1UL << FLASH_SR_OPTVERR_POS)                /**< 0x00008000 */
#define FLASH_SR_OPTVERR                       FLASH_SR_OPTVERR_MASK                          /**< Flash option bytes verify error flag */

#define FLASH_SR_BSY_POS                       (16U)
#define FLASH_SR_BSY_MASK                      (0x1UL << FLASH_SR_BSY_POS)                    /**< 0x00010000 */
#define FLASH_SR_BSY                           FLASH_SR_BSY_MASK                              /**< Flash operation status flag */

#define FLASH_SR_EOP_POS                       (24U)
#define FLASH_SR_EOP_MASK                      (0x1UL << FLASH_SR_EOP_POS)                    /**< 0x01000000 */
#define FLASH_SR_EOP                           FLASH_SR_EOP_MASK                              /**< Flash operation complete flag */

/* Bits for FLASH_CR */
#define FLASH_CR_OP_MODE_POS                   (1U)
#define FLASH_CR_OP_MODE_MASK                  (0x3UL << FLASH_CR_OP_MODE_POS)                /**< 0x00000006 */
#define FLASH_CR_OP_MODE                       FLASH_CR_OP_MODE_MASK                          /**< Flash operate mode selection */
#define FLASH_CR_OP_MODE_IDLE                  (0x0UL << FLASH_CR_OP_MODE_POS)                /**< 0x00000000 */
#define FLASH_CR_OP_MODE_PROGRAM               (0x1UL << FLASH_CR_OP_MODE_POS)                /**< 0x00000002 */
#define FLASH_CR_OP_MODE_PAGE_ERASE            (0x2UL << FLASH_CR_OP_MODE_POS)                /**< 0x00000004 */
#define FLASH_CR_OP_MODE_MASS_ERASE            (0x3UL << FLASH_CR_OP_MODE_POS)                /**< 0x00000006 */

#define FLASH_CR_EOPIE_POS                     (24U)
#define FLASH_CR_EOPIE_MASK                    (0x1UL << FLASH_CR_EOPIE_POS)                  /**< 0x01000000 */
#define FLASH_CR_EOPIE                         FLASH_CR_EOPIE_MASK                            /**< Flash operation complete interrupt_control */

#define FLASH_CR_OPERRIE_POS                   (25U)
#define FLASH_CR_OPERRIE_MASK                  (0x1UL << FLASH_CR_OPERRIE_POS)                /**< 0x02000000 */
#define FLASH_CR_OPERRIE                       FLASH_CR_OPERRIE_MASK                          /**< Flash option error interrupt_control */

#define FLASH_CR_OPTLOCK_POS                   (30U)
#define FLASH_CR_OPTLOCK_MASK                  (0x1UL << FLASH_CR_OPTLOCK_POS)                /**< 0x40000000 */
#define FLASH_CR_OPTLOCK                       FLASH_CR_OPTLOCK_MASK                          /**< Flash option bytes lock status */

#define FLASH_CR_LOCK_POS                      (31U)
#define FLASH_CR_LOCK_MASK                     (0x1UL << FLASH_CR_LOCK_POS)                   /**< 0x80000000 */
#define FLASH_CR_LOCK                          FLASH_CR_LOCK_MASK                             /**< Flash CR register lock status */

/* Bits for FLASH_OPTR1 */
#define FLASH_OPTR1_RDPRP_POS                  (0U)
#define FLASH_OPTR1_RDPRP_MASK                 (0x1UL << FLASH_OPTR1_RDPRP_POS)               /**< 0x00000001 */
#define FLASH_OPTR1_RDPRP                      FLASH_OPTR1_RDPRP_MASK                         /**< Flash RDP protect level */

#define FLASH_OPTR1_BOR_EN_POS                 (8U)
#define FLASH_OPTR1_BOR_EN_MASK                (0x1UL << FLASH_OPTR1_BOR_EN_POS)              /**< 0x00000100 */
#define FLASH_OPTR1_BOR_EN                     FLASH_OPTR1_BOR_EN_MASK                        /**< Flash BOR enable control */

#define FLASH_OPTR1_BOR_LEVEL_POS              (9U)
#define FLASH_OPTR1_BOR_LEVEL_MASK             (0x3UL << FLASH_OPTR1_BOR_LEVEL_POS)           /**< 0x00000600 */
#define FLASH_OPTR1_BOR_LEVEL                  FLASH_OPTR1_BOR_LEVEL_MASK                     /**< Flash BOR level selection */
#define FLASH_OPTR1_BOR_LEVEL_0                (0x0UL << FLASH_OPTR1_BOR_LEVEL_POS)           /**< 0x00000000 */
#define FLASH_OPTR1_BOR_LEVEL_1                (0x1UL << FLASH_OPTR1_BOR_LEVEL_POS)           /**< 0x00000200 */
#define FLASH_OPTR1_BOR_LEVEL_2                (0x2UL << FLASH_OPTR1_BOR_LEVEL_POS)           /**< 0x00000400 */
#define FLASH_OPTR1_BOR_LEVEL_3                (0x3UL << FLASH_OPTR1_BOR_LEVEL_POS)           /**< 0x00000600 */

#define FLASH_OPTR1_NRST_SWD_MODE_POS          (13U)
#define FLASH_OPTR1_NRST_SWD_MODE_MASK         (0x3UL << FLASH_OPTR1_NRST_SWD_MODE_POS)       /**< 0x00006000 */
#define FLASH_OPTR1_NRST_SWD_MODE              FLASH_OPTR1_NRST_SWD_MODE_MASK                 /**< NRST/SWD pin selection */
#define FLASH_OPTR1_NRST_SWD_MODE_0            (0x0UL << FLASH_OPTR1_NRST_SWD_MODE_POS)       /**< 0x00000000 */
#define FLASH_OPTR1_NRST_SWD_MODE_1            (0x1UL << FLASH_OPTR1_NRST_SWD_MODE_POS)       /**< 0x00002000 */
#define FLASH_OPTR1_NRST_SWD_MODE_2            (0x2UL << FLASH_OPTR1_NRST_SWD_MODE_POS)       /**< 0x00004000 */
#define FLASH_OPTR1_NRST_SWD_MODE_3            (0x3UL << FLASH_OPTR1_NRST_SWD_MODE_POS)       /**< 0x00006000 */

/* Bits for FLASH_OPTR2 */
#define FLASH_OPTR2_RST_STOP_POS               (0U)
#define FLASH_OPTR2_RST_STOP_MASK              (0x1UL << FLASH_OPTR2_RST_STOP_POS)            /**< 0x00000001 */
#define FLASH_OPTR2_RST_STOP                   FLASH_OPTR2_RST_STOP_MASK                      /**< Enter stop or deepstop generate reset control */

#define FLASH_OPTR2_IWDG_STOP_POS              (5U)
#define FLASH_OPTR2_IWDG_STOP_MASK             (0x1UL << FLASH_OPTR2_IWDG_STOP_POS)           /**< 0x00000020 */
#define FLASH_OPTR2_IWDG_STOP                  FLASH_OPTR2_IWDG_STOP_MASK                     /**< Stop or deepstop IWDG counter stop control */

/* Bits for FLASH_WRP */
#define FLASH_WRP_WRP_POS                      (0U)
#define FLASH_WRP_WRP_MASK                     (0x3FUL << FLASH_WRP_WRP_POS)                  /**< 0x0000003F */
#define FLASH_WRP_WRP                          FLASH_WRP_WRP_MASK                             /**< Flash WRP protect area */

/*-----------------------------------------------------------------------------------------------
                                       GPIO Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for GPIO_MODER */
#define GPIO_MODE_MODE0_POS            (0U)
#define GPIO_MODE_MODE0_MASK           (0x3UL << GPIO_MODE_MODE0_POS)          /**< 0x00000003 */
#define GPIO_MODE_MODE0                GPIO_MODE_MODE0_MASK
#define GPIO_MODE_MODE1_POS            (2U)
#define GPIO_MODE_MODE1_MASK           (0x3UL << GPIO_MODE_MODE1_POS)          /**< 0x0000000C */
#define GPIO_MODE_MODE1                GPIO_MODE_MODE1_MASK
#define GPIO_MODE_MODE2_POS            (4U)
#define GPIO_MODE_MODE2_MASK           (0x3UL << GPIO_MODE_MODE2_POS)          /**< 0x00000030 */
#define GPIO_MODE_MODE2                GPIO_MODE_MODE2_MASK
#define GPIO_MODE_MODE3_POS            (6U)
#define GPIO_MODE_MODE3_MASK           (0x3UL << GPIO_MODE_MODE3_POS)          /**< 0x000000C0 */
#define GPIO_MODE_MODE3                GPIO_MODE_MODE3_MASK
#define GPIO_MODE_MODE4_POS            (8U)
#define GPIO_MODE_MODE4_MASK           (0x3UL << GPIO_MODE_MODE4_POS)          /**< 0x00000300 */
#define GPIO_MODE_MODE4                GPIO_MODE_MODE4_MASK
#define GPIO_MODE_MODE5_POS            (10U)
#define GPIO_MODE_MODE5_MASK           (0x3UL << GPIO_MODE_MODE5_POS)          /**< 0x00000C00 */
#define GPIO_MODE_MODE5                GPIO_MODE_MODE5_MASK
#define GPIO_MODE_MODE6_POS            (12U)
#define GPIO_MODE_MODE6_MASK           (0x3UL << GPIO_MODE_MODE6_POS)          /**< 0x00003000 */
#define GPIO_MODE_MODE6                GPIO_MODE_MODE6_MASK
#define GPIO_MODE_MODE7_POS            (14U)
#define GPIO_MODE_MODE7_MASK           (0x3UL << GPIO_MODE_MODE7_POS)          /**< 0x0000C000 */
#define GPIO_MODE_MODE7                GPIO_MODE_MODE7_MASK

/* Bits for GPIO_OTYPER */
#define GPIO_OTYPE_OT0_POS             (0U)
#define GPIO_OTYPE_OT0_MASK            (0x1UL << GPIO_OTYPE_OT0_POS)           /**< 0x00000001 */
#define GPIO_OTYPE_OT0                 GPIO_OTYPE_OT0_MASK
#define GPIO_OTYPE_OT1_POS             (1U)
#define GPIO_OTYPE_OT1_MASK            (0x1UL << GPIO_OTYPE_OT1_POS)           /**< 0x00000002 */
#define GPIO_OTYPE_OT1                 GPIO_OTYPE_OT1_MASK
#define GPIO_OTYPE_OT2_POS             (2U)
#define GPIO_OTYPE_OT2_MASK            (0x1UL << GPIO_OTYPE_OT2_POS)           /**< 0x00000004 */
#define GPIO_OTYPE_OT2                 GPIO_OTYPE_OT2_MASK
#define GPIO_OTYPE_OT3_POS             (3U)
#define GPIO_OTYPE_OT3_MASK            (0x1UL << GPIO_OTYPE_OT3_POS)           /**< 0x00000008 */
#define GPIO_OTYPE_OT3                 GPIO_OTYPE_OT3_MASK
#define GPIO_OTYPE_OT4_POS             (4U)
#define GPIO_OTYPE_OT4_MASK            (0x1UL << GPIO_OTYPE_OT4_POS)           /**< 0x00000010 */
#define GPIO_OTYPE_OT4                 GPIO_OTYPE_OT4_MASK
#define GPIO_OTYPE_OT5_POS             (5U)
#define GPIO_OTYPE_OT5_MASK            (0x1UL << GPIO_OTYPE_OT5_POS)           /**< 0x00000020 */
#define GPIO_OTYPE_OT5                 GPIO_OTYPE_OT5_MASK
#define GPIO_OTYPE_OT6_POS             (6U)
#define GPIO_OTYPE_OT6_MASK            (0x1UL << GPIO_OTYPE_OT6_POS)           /**< 0x00000040 */
#define GPIO_OTYPE_OT6                 GPIO_OTYPE_OT6_MASK
#define GPIO_OTYPE_OT7_POS             (7U)
#define GPIO_OTYPE_OT7_MASK            (0x1UL << GPIO_OTYPE_OT7_POS)           /**< 0x00000080 */
#define GPIO_OTYPE_OT7                 GPIO_OTYPE_OT7_MASK


/* Bits for GPIO_PUPDR */
#define GPIO_PUPD_PUPD0_POS            (0U)
#define GPIO_PUPD_PUPD0_MASK           (0x3UL << GPIO_PUPD_PUPD0_POS)          /**< 0x00000003 */
#define GPIO_PUPD_PUPD0                GPIO_PUPD_PUPD0_MASK
#define GPIO_PUPD_PUPD1_POS            (2U)
#define GPIO_PUPD_PUPD1_MASK           (0x3UL << GPIO_PUPD_PUPD1_POS)          /**< 0x0000000C */
#define GPIO_PUPD_PUPD1                GPIO_PUPD_PUPD1_MASK
#define GPIO_PUPD_PUPD2_POS            (4U)
#define GPIO_PUPD_PUPD2_MASK           (0x3UL << GPIO_PUPD_PUPD2_POS)          /**< 0x00000030 */
#define GPIO_PUPD_PUPD2                GPIO_PUPD_PUPD2_MASK
#define GPIO_PUPD_PUPD3_POS            (6U)
#define GPIO_PUPD_PUPD3_MASK           (0x3UL << GPIO_PUPD_PUPD3_POS)          /**< 0x000000C0 */
#define GPIO_PUPD_PUPD3                GPIO_PUPD_PUPD3_MASK
#define GPIO_PUPD_PUPD4_POS            (8U)
#define GPIO_PUPD_PUPD4_MASK           (0x3UL << GPIO_PUPD_PUPD4_POS)          /**< 0x00000300 */
#define GPIO_PUPD_PUPD4                GPIO_PUPD_PUPD4_MASK
#define GPIO_PUPD_PUPD5_POS            (10U)
#define GPIO_PUPD_PUPD5_MASK           (0x3UL << GPIO_PUPD_PUPD5_POS)          /**< 0x00000C00 */
#define GPIO_PUPD_PUPD5                GPIO_PUPD_PUPD5_MASK
#define GPIO_PUPD_PUPD6_POS            (12U)
#define GPIO_PUPD_PUPD6_MASK           (0x3UL << GPIO_PUPD_PUPD6_POS)          /**< 0x00003000 */
#define GPIO_PUPD_PUPD6                GPIO_PUPD_PUPD6_MASK
#define GPIO_PUPD_PUPD7_POS            (14U)
#define GPIO_PUPD_PUPD7_MASK           (0x3UL << GPIO_PUPD_PUPD7_POS)          /**< 0x0000C000 */
#define GPIO_PUPD_PUPD7                GPIO_PUPD_PUPD7_MASK

/* Bits for GPIO_IDR */
#define GPIO_IDR_ID0_POS               (0U)
#define GPIO_IDR_ID0_MASK              (0x1UL << GPIO_IDR_ID0_POS)             /**< 0x00000001 */
#define GPIO_IDR_ID0                   GPIO_IDR_ID0_MASK
#define GPIO_IDR_ID1_POS               (1U)
#define GPIO_IDR_ID1_MASK              (0x1UL << GPIO_IDR_ID1_POS)             /**< 0x00000002 */
#define GPIO_IDR_ID1                   GPIO_IDR_ID1_MASK
#define GPIO_IDR_ID2_POS               (2U)
#define GPIO_IDR_ID2_MASK              (0x1UL << GPIO_IDR_ID2_POS)             /**< 0x00000004 */
#define GPIO_IDR_ID2                   GPIO_IDR_ID2_MASK
#define GPIO_IDR_ID3_POS               (3U)
#define GPIO_IDR_ID3_MASK              (0x1UL << GPIO_IDR_ID3_POS)             /**< 0x00000008 */
#define GPIO_IDR_ID3                   GPIO_IDR_ID3_MASK
#define GPIO_IDR_ID4_POS               (4U)
#define GPIO_IDR_ID4_MASK              (0x1UL << GPIO_IDR_ID4_POS)             /**< 0x00000010 */
#define GPIO_IDR_ID4                   GPIO_IDR_ID4_MASK
#define GPIO_IDR_ID5_POS               (5U)
#define GPIO_IDR_ID5_MASK              (0x1UL << GPIO_IDR_ID5_POS)             /**< 0x00000020 */
#define GPIO_IDR_ID5                   GPIO_IDR_ID5_MASK
#define GPIO_IDR_ID6_POS               (6U)
#define GPIO_IDR_ID6_MASK              (0x1UL << GPIO_IDR_ID6_POS)             /**< 0x00000040 */
#define GPIO_IDR_ID6                   GPIO_IDR_ID6_MASK
#define GPIO_IDR_ID7_POS               (7U)
#define GPIO_IDR_ID7_MASK              (0x1UL << GPIO_IDR_ID7_POS)             /**< 0x00000080 */
#define GPIO_IDR_ID7                   GPIO_IDR_ID7_MASK

/* Bits for GPIO_ODR */
#define GPIO_ODR_OD0_POS               (0U)
#define GPIO_ODR_OD0_MASK              (0x1UL << GPIO_ODR_OD0_POS)             /**< 0x00000001 */
#define GPIO_ODR_OD0                   GPIO_ODR_OD0_MASK
#define GPIO_ODR_OD1_POS               (1U)
#define GPIO_ODR_OD1_MASK              (0x1UL << GPIO_ODR_OD1_POS)             /**< 0x00000002 */
#define GPIO_ODR_OD1                   GPIO_ODR_OD1_MASK
#define GPIO_ODR_OD2_POS               (2U)
#define GPIO_ODR_OD2_MASK              (0x1UL << GPIO_ODR_OD2_POS)             /**< 0x00000004 */
#define GPIO_ODR_OD2                   GPIO_ODR_OD2_MASK
#define GPIO_ODR_OD3_POS               (3U)
#define GPIO_ODR_OD3_MASK              (0x1UL << GPIO_ODR_OD3_POS)             /**< 0x00000008 */
#define GPIO_ODR_OD3                   GPIO_ODR_OD3_MASK
#define GPIO_ODR_OD4_POS               (4U)
#define GPIO_ODR_OD4_MASK              (0x1UL << GPIO_ODR_OD4_POS)             /**< 0x00000010 */
#define GPIO_ODR_OD4                   GPIO_ODR_OD4_MASK
#define GPIO_ODR_OD5_POS               (5U)
#define GPIO_ODR_OD5_MASK              (0x1UL << GPIO_ODR_OD5_POS)             /**< 0x00000020 */
#define GPIO_ODR_OD5                   GPIO_ODR_OD5_MASK
#define GPIO_ODR_OD6_POS               (6U)
#define GPIO_ODR_OD6_MASK              (0x1UL << GPIO_ODR_OD6_POS)             /**< 0x00000040 */
#define GPIO_ODR_OD6                   GPIO_ODR_OD6_MASK
#define GPIO_ODR_OD7_POS               (7U)
#define GPIO_ODR_OD7_MASK              (0x1UL << GPIO_ODR_OD7_POS)             /**< 0x00000080 */
#define GPIO_ODR_OD7                   GPIO_ODR_OD7_MASK


/* Bits for GPIO_BSR */
#define GPIO_BSR_BS0_POS               (0U)
#define GPIO_BSR_BS0_MASK              (0x1UL << GPIO_BSR_BS0_POS)             /**< 0x00000001 */
#define GPIO_BSR_BS0                   GPIO_BSR_BS0_MASK
#define GPIO_BSR_BS1_POS               (1U)
#define GPIO_BSR_BS1_MASK              (0x1UL << GPIO_BSR_BS1_POS)             /**< 0x00000002 */
#define GPIO_BSR_BS1                   GPIO_BSR_BS1_MASK
#define GPIO_BSR_BS2_POS               (2U)
#define GPIO_BSR_BS2_MASK              (0x1UL << GPIO_BSR_BS2_POS)             /**< 0x00000004 */
#define GPIO_BSR_BS2                   GPIO_BSR_BS2_MASK
#define GPIO_BSR_BS3_POS               (3U)
#define GPIO_BSR_BS3_MASK              (0x1UL << GPIO_BSR_BS3_POS)             /**< 0x00000008 */
#define GPIO_BSR_BS3                   GPIO_BSR_BS3_MASK
#define GPIO_BSR_BS4_POS               (4U)
#define GPIO_BSR_BS4_MASK              (0x1UL << GPIO_BSR_BS4_POS)             /**< 0x00000010 */
#define GPIO_BSR_BS4                   GPIO_BSR_BS4_MASK
#define GPIO_BSR_BS5_POS               (5U)
#define GPIO_BSR_BS5_MASK              (0x1UL << GPIO_BSR_BS5_POS)             /**< 0x00000020 */
#define GPIO_BSR_BS5                   GPIO_BSR_BS5_MASK
#define GPIO_BSR_BS6_POS               (6U)
#define GPIO_BSR_BS6_MASK              (0x1UL << GPIO_BSR_BS6_POS)             /**< 0x00000040 */
#define GPIO_BSR_BS6                   GPIO_BSR_BS6_MASK
#define GPIO_BSR_BS7_POS               (7U)
#define GPIO_BSR_BS7_MASK              (0x1UL << GPIO_BSR_BS7_POS)             /**< 0x00000080 */
#define GPIO_BSR_BS7                   GPIO_BSR_BS7_MASK

#define GPIO_BSR_BR0_POS               (16U)
#define GPIO_BSR_BR0_MASK              (0x1UL << GPIO_BSR_BR0_POS)             /**< 0x00010000 */
#define GPIO_BSR_BR0                   GPIO_BSR_BR0_MASK
#define GPIO_BSR_BR1_POS               (17U)
#define GPIO_BSR_BR1_MASK              (0x1UL << GPIO_BSR_BR1_POS)             /**< 0x00020000 */
#define GPIO_BSR_BR1                   GPIO_BSR_BR1_MASK
#define GPIO_BSR_BR2_POS               (18U)
#define GPIO_BSR_BR2_MASK              (0x1UL << GPIO_BSR_BR2_POS)             /**< 0x00040000 */
#define GPIO_BSR_BR2                   GPIO_BSR_BR2_MASK
#define GPIO_BSR_BR3_POS               (19U)
#define GPIO_BSR_BR3_MASK              (0x1UL << GPIO_BSR_BR3_POS)             /**< 0x00080000 */
#define GPIO_BSR_BR3                   GPIO_BSR_BR3_MASK
#define GPIO_BSR_BR4_POS               (20U)
#define GPIO_BSR_BR4_MASK              (0x1UL << GPIO_BSR_BR4_POS)             /**< 0x00100000 */
#define GPIO_BSR_BR4                   GPIO_BSR_BR4_MASK
#define GPIO_BSR_BR5_POS               (21U)
#define GPIO_BSR_BR5_MASK              (0x1UL << GPIO_BSR_BR5_POS)             /**< 0x00200000 */
#define GPIO_BSR_BR5                   GPIO_BSR_BR5_MASK
#define GPIO_BSR_BR6_POS               (22U)
#define GPIO_BSR_BR6_MASK              (0x1UL << GPIO_BSR_BR6_POS)             /**< 0x00400000 */
#define GPIO_BSR_BR6                   GPIO_BSR_BR6_MASK
#define GPIO_BSR_BR7_POS               (23U)
#define GPIO_BSR_BR7_MASK              (0x1UL << GPIO_BSR_BR7_POS)             /**< 0x00800000 */
#define GPIO_BSR_BR7                   GPIO_BSR_BR7_MASK


/* Bits for GPIO_AFL */
#define GPIO_AFL_AFSEL0_POS           (0U)
#define GPIO_AFL_AFSEL0_MASK          (0x7UL << GPIO_AFL_AFSEL0_POS)           /**< 0x00000007 */
#define GPIO_AFL_AFSEL0               GPIO_AFL_AFSEL0_MASK
#define GPIO_AFL_AFSEL1_POS           (4U)
#define GPIO_AFL_AFSEL1_MASK          (0x7UL << GPIO_AFL_AFSEL1_POS)           /**< 0x00000070 */
#define GPIO_AFL_AFSEL1               GPIO_AFL_AFSEL1_MASK
#define GPIO_AFL_AFSEL2_POS           (8U)
#define GPIO_AFL_AFSEL2_MASK          (0x7UL << GPIO_AFL_AFSEL2_POS)           /**< 0x00000700 */
#define GPIO_AFL_AFSEL2               GPIO_AFL_AFSEL2_MASK
#define GPIO_AFL_AFSEL3_POS           (12U)
#define GPIO_AFL_AFSEL3_MASK          (0x7UL << GPIO_AFL_AFSEL3_POS)           /**< 0x00007000 */
#define GPIO_AFL_AFSEL3               GPIO_AFL_AFSEL3_MASK
#define GPIO_AFL_AFSEL4_POS           (16U)
#define GPIO_AFL_AFSEL4_MASK          (0x7UL << GPIO_AFL_AFSEL4_POS)           /**< 0x00070000 */
#define GPIO_AFL_AFSEL4               GPIO_AFL_AFSEL4_MASK
#define GPIO_AFL_AFSEL5_POS           (20U)
#define GPIO_AFL_AFSEL5_MASK          (0x7UL << GPIO_AFL_AFSEL5_POS)           /**< 0x00700000 */
#define GPIO_AFL_AFSEL5               GPIO_AFL_AFSEL5_MASK
#define GPIO_AFL_AFSEL6_POS           (24U)
#define GPIO_AFL_AFSEL6_MASK          (0x7UL << GPIO_AFL_AFSEL6_POS)           /**< 0x07000000 */
#define GPIO_AFL_AFSEL6               GPIO_AFL_AFSEL6_MASK
#define GPIO_AFL_AFSEL7_POS           (28U)
#define GPIO_AFL_AFSEL7_MASK          (0x7UL << GPIO_AFL_AFSEL7_POS)           /**< 0x70000000 */
#define GPIO_AFL_AFSEL7               GPIO_AFL_AFSEL7_MASK



/* Bits for GPIO_BR */
#define GPIO_BR_BR0_POS                (0U)
#define GPIO_BR_BR0_MASK               (0x1UL << GPIO_BR_BR0_POS)              /**< 0x00000001 */
#define GPIO_BR_BR0                    GPIO_BR_BR0_MASK
#define GPIO_BR_BR1_POS                (1U)
#define GPIO_BR_BR1_MASK               (0x1UL << GPIO_BR_BR1_POS)              /**< 0x00000002 */
#define GPIO_BR_BR1                    GPIO_BR_BR1_MASK
#define GPIO_BR_BR2_POS                (2U)
#define GPIO_BR_BR2_MASK               (0x1UL << GPIO_BR_BR2_POS)              /**< 0x00000004 */
#define GPIO_BR_BR2                    GPIO_BR_BR2_MASK
#define GPIO_BR_BR3_POS                (3U)
#define GPIO_BR_BR3_MASK               (0x1UL << GPIO_BR_BR3_POS)              /**< 0x00000008 */
#define GPIO_BR_BR3                    GPIO_BR_BR3_MASK
#define GPIO_BR_BR4_POS                (4U)
#define GPIO_BR_BR4_MASK               (0x1UL << GPIO_BR_BR4_POS)              /**< 0x00000010 */
#define GPIO_BR_BR4                    GPIO_BR_BR4_MASK
#define GPIO_BR_BR5_POS                (5U)
#define GPIO_BR_BR5_MASK               (0x1UL << GPIO_BR_BR5_POS)              /**< 0x00000020 */
#define GPIO_BR_BR5                    GPIO_BR_BR5_MASK
#define GPIO_BR_BR6_POS                (6U)
#define GPIO_BR_BR6_MASK               (0x1UL << GPIO_BR_BR6_POS)              /**< 0x00000040 */
#define GPIO_BR_BR6                    GPIO_BR_BR6_MASK
#define GPIO_BR_BR7_POS                (7U)
#define GPIO_BR_BR7_MASK               (0x1UL << GPIO_BR_BR7_POS)              /**< 0x00000080 */
#define GPIO_BR_BR7                    GPIO_BR_BR7_MASK



/*-----------------------------------------------------------------------------------------------
                                       I2C Registers Bits
------------------------------------------------------------------------------------------------*/
/* Bits for I2C_CR1 */
#define I2C_CR1_PE_POS               (0U)
#define I2C_CR1_PE_MASK              (0x1UL << I2C_CR1_PE_POS)                 /**< 0x00000001 */
#define I2C_CR1_PE                   I2C_CR1_PE_MASK                           /**< Peripheral enable */

#define I2C_CR1_BUFIE_POS            (1U)
#define I2C_CR1_BUFIE_MASK           (0x1UL << I2C_CR1_BUFIE_POS)              /**< 0x00000002 */
#define I2C_CR1_BUFIE                I2C_CR1_BUFIE_MASK                        /**< Buffer interrupt enable */

#define I2C_CR1_EVTIE_POS            (3U)
#define I2C_CR1_EVTIE_MASK           (0x1UL << I2C_CR1_EVTIE_POS)              /**< 0x00000008 */
#define I2C_CR1_EVTIE                I2C_CR1_EVTIE_MASK                        /**< Event interrupt enable */

#define I2C_CR1_ERRIE_POS            (7U)
#define I2C_CR1_ERRIE_MASK           (0x1UL << I2C_CR1_ERRIE_POS)              /**< 0x00000080 */
#define I2C_CR1_ERRIE                I2C_CR1_ERRIE_MASK                        /**< Errors interrupt enable */

#define I2C_CR1_DNF_POS              (8U)
#define I2C_CR1_DNF_MASK             (0xFUL << I2C_CR1_DNF_POS)                /**< 0x00000F00 */
#define I2C_CR1_DNF                  I2C_CR1_DNF_MASK                          /**< Digital noise filter */
#define I2C_CR1_DNF_DISABLE          (0x0UL << I2C_CR1_DNF_POS)                /**< 0x00000000 */
#define I2C_CR1_DNF_1CLK             (0x1UL << I2C_CR1_DNF_POS)                /**< 0x00000100 */
#define I2C_CR1_DNF_2CLK             (0x2UL << I2C_CR1_DNF_POS)                /**< 0x00000200 */
#define I2C_CR1_DNF_3CLK             (0x3UL << I2C_CR1_DNF_POS)                /**< 0x00000300 */
#define I2C_CR1_DNF_4CLK             (0x4UL << I2C_CR1_DNF_POS)                /**< 0x00000400 */
#define I2C_CR1_DNF_5CLK             (0x5UL << I2C_CR1_DNF_POS)                /**< 0x00000500 */
#define I2C_CR1_DNF_6CLK             (0x6UL << I2C_CR1_DNF_POS)                /**< 0x00000600 */
#define I2C_CR1_DNF_7CLK             (0x7UL << I2C_CR1_DNF_POS)                /**< 0x00000700 */
#define I2C_CR1_DNF_8CLK             (0x8UL << I2C_CR1_DNF_POS)                /**< 0x00000800 */
#define I2C_CR1_DNF_9CLK             (0x9UL << I2C_CR1_DNF_POS)                /**< 0x00000900 */
#define I2C_CR1_DNF_10CLK            (0xAUL << I2C_CR1_DNF_POS)                /**< 0x00000A00 */
#define I2C_CR1_DNF_11CLK            (0xBUL << I2C_CR1_DNF_POS)                /**< 0x00000B00 */
#define I2C_CR1_DNF_12CLK            (0xCUL << I2C_CR1_DNF_POS)                /**< 0x00000C00 */
#define I2C_CR1_DNF_13CLK            (0xDUL << I2C_CR1_DNF_POS)                /**< 0x00000D00 */
#define I2C_CR1_DNF_14CLK            (0xEUL << I2C_CR1_DNF_POS)                /**< 0x00000E00 */
#define I2C_CR1_DNF_15CLK            (0xFUL << I2C_CR1_DNF_POS)                /**< 0x00000F00 */

#define I2C_CR1_NOSTRETCH_POS        (17U)
#define I2C_CR1_NOSTRETCH_MASK       (0x1UL << I2C_CR1_NOSTRETCH_POS)          /**< 0x00020000 */
#define I2C_CR1_NOSTRETCH            I2C_CR1_NOSTRETCH_MASK                    /**< Clock stretching disable */

#define I2C_CR1_GCEN_POS             (19U)
#define I2C_CR1_GCEN_MASK            (0x1UL << I2C_CR1_GCEN_POS)               /**< 0x00080000 */
#define I2C_CR1_GCEN                 I2C_CR1_GCEN_MASK                         /**< General call enable */

/* Bits for I2C_CR2 */
#define I2C_CR2_NACK_POS             (15U)
#define I2C_CR2_NACK_MASK            (0x1UL << I2C_CR2_NACK_POS)               /**< 0x00008000 */
#define I2C_CR2_NACK                 I2C_CR2_NACK_MASK                         /**< NACK generation */

/* Bits for I2C_ADDR1 */
#define I2C_ADDR1_ADDR1_POS          (1U)
#define I2C_ADDR1_ADDR1_MASK         (0x7FUL << I2C_ADDR1_ADDR1_POS)           /**< 0x00000FE */
#define I2C_ADDR1_ADDR1              I2C_ADDR1_ADDR1_MASK                      /**< Interface  address 1 */

/* Bits for I2C_ISR */
#define I2C_ISR_TXE_POS              (0U)
#define I2C_ISR_TXE_MASK             (0x1UL << I2C_ISR_TXE_POS)                /**< 0x00000001 */
#define I2C_ISR_TXE                  I2C_ISR_TXE_MASK                          /**< Transmit data register empty */

#define I2C_ISR_TXIS_POS             (1U)
#define I2C_ISR_TXIS_MASK            (0x1UL << I2C_ISR_TXIS_POS)               /**< 0x00000002 */
#define I2C_ISR_TXIS                 I2C_ISR_TXIS_MASK                         /**< Transmit interrupt status */

#define I2C_ISR_RXNE_POS             (2U)
#define I2C_ISR_RXNE_MASK            (0x1UL << I2C_ISR_RXNE_POS)               /**< 0x00000004 */
#define I2C_ISR_RXNE                 I2C_ISR_RXNE_MASK                         /**< Receive data register not empty */

#define I2C_ISR_ADDR_POS             (3U)
#define I2C_ISR_ADDR_MASK            (0x1UL << I2C_ISR_ADDR_POS)               /**< 0x00000008 */
#define I2C_ISR_ADDR                 I2C_ISR_ADDR_MASK                         /**< Address matched (slave mode)*/

#define I2C_ISR_NACKF_POS            (4U)
#define I2C_ISR_NACKF_MASK           (0x1UL << I2C_ISR_NACKF_POS)              /**< 0x00000010 */
#define I2C_ISR_NACKF                I2C_ISR_NACKF_MASK                        /**< NACK received flag */

#define I2C_ISR_STOPF_POS            (5U)
#define I2C_ISR_STOPF_MASK           (0x1UL << I2C_ISR_STOPF_POS)              /**< 0x00000020 */
#define I2C_ISR_STOPF                I2C_ISR_STOPF_MASK                        /**< STOP detection flag */

#define I2C_ISR_BERR_POS             (8U)
#define I2C_ISR_BERR_MASK            (0x1UL << I2C_ISR_BERR_POS)               /**< 0x00000100 */
#define I2C_ISR_BERR                 I2C_ISR_BERR_MASK                         /**< Bus error */

#define I2C_ISR_OVR_POS              (10U)
#define I2C_ISR_OVR_MASK             (0x1UL << I2C_ISR_OVR_POS)                /**< 0x00000400 */
#define I2C_ISR_OVR                  I2C_ISR_OVR_MASK                          /**< Overrun/Underrun */

#define I2C_ISR_BUSY_POS             (15U)
#define I2C_ISR_BUSY_MASK            (0x1UL << I2C_ISR_BUSY_POS)               /**< 0x00008000 */
#define I2C_ISR_BUSY                 I2C_ISR_BUSY_MASK                         /**< Bus busy */

#define I2C_ISR_DIR_POS              (16U)
#define I2C_ISR_DIR_MASK             (0x1UL << I2C_ISR_DIR_POS)                /**< 0x00010000 */
#define I2C_ISR_DIR                  I2C_ISR_DIR_MASK

/* Bits for I2C_ICR */
#define I2C_ICR_ADDRCF_POS           (3U)
#define I2C_ICR_ADDRCF_MASK          (0x1UL << I2C_ICR_ADDRCF_POS)             /**< 0x00000008 */
#define I2C_ICR_ADDRCF               I2C_ICR_ADDRCF_MASK                       /**< Address matched clear flag */

#define I2C_ICR_NACKCF_POS           (4U)
#define I2C_ICR_NACKCF_MASK          (0x1UL << I2C_ICR_NACKCF_POS)             /**< 0x00000010 */
#define I2C_ICR_NACKCF               I2C_ICR_NACKCF_MASK                       /**< NACK clear flag */

#define I2C_ICR_STOPCF_POS           (5U)
#define I2C_ICR_STOPCF_MASK          (0x1UL << I2C_ICR_STOPCF_POS)             /**< 0x00000020 */
#define I2C_ICR_STOPCF               I2C_ICR_STOPCF_MASK                       /**< STOP detection clear flag */

#define I2C_ICR_BERRCF_POS           (8U)
#define I2C_ICR_BERRCF_MASK          (0x1UL << I2C_ICR_BERRCF_POS)             /**< 0x00000100 */
#define I2C_ICR_BERRCF               I2C_ICR_BERRCF_MASK                       /**< Bus error clear flag */

#define I2C_ICR_OVRCF_POS            (10U)
#define I2C_ICR_OVRCF_MASK           (0x1UL << I2C_ICR_OVRCF_POS)              /**< 0x00000400 */
#define I2C_ICR_OVRCF                I2C_ICR_OVRCF_MASK                        /**< Overrun/Underrun clear flag */

/* Bits for I2C_RDR */
#define I2C_RDR_RXDATA_POS           (0U)
#define I2C_RDR_RXDATA_MASK          (0xFFUL << I2C_RDR_RXDATA_POS)            /**< 0x000000FF */
#define I2C_RDR_RXDATA               I2C_RDR_RXDATA_MASK                       /**< 8-bit receive data */

/* Bits for I2C_TDR */
#define I2C_TDR_TXDATA_POS           (0U)
#define I2C_TDR_TXDATA_MASK          (0xFFUL << I2C_TDR_TXDATA_POS)            /**< 0x000000FF */
#define I2C_TDR_TXDATA               I2C_TDR_TXDATA_MASK                       /**< 8-bit transmit data */


/*-----------------------------------------------------------------------------------------------
                                       IRTIM Registers Bits  
------------------------------------------------------------------------------------------------*/

/* Bits for IRTIM_CR*/
#define IRTIM_CR_IR_POL_POS          (2U)
#define IRTIM_CR_IR_POL_MASK         (0x01UL << IRTIM_CR_IR_POL_POS)           /**< 0x00000004 */
#define IRTIM_CR_IR_POL              IRTIM_CR_IR_POL_MASK                      /**< ITRIM config output pol */ 

#define IRTIM_CR_IR_MODE_POS         (3U)
#define IRTIM_CR_IR_MODE_MASK        (0x03UL << IRTIM_CR_IR_MODE_POS)          /**< 0x00000018 */ 
#define IRTIM_CR_IR_MODE             IRTIM_CR_IR_MODE_MASK                     /**< IRTIM select the modulation signal */
#define IRTIM_CR_IR_MODE_TIM3_OC1    (0x0UL << IRTIM_CR_IR_MODE_POS)           /**< 0x00000000 */ 
#define IRTIM_CR_IR_MODE_UART1_TX    (0x1UL << IRTIM_CR_IR_MODE_POS)           /**< 0x00000008 */
#define IRTIM_CR_IR_MODE_UART2_TX    (0x2UL << IRTIM_CR_IR_MODE_POS)           /**< 0x00000010 */


/*-----------------------------------------------------------------------------------------------
                                       IWDG Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for IWDG_CR */
#define IWDG_CR_KEY_POS              (0U)
#define IWDG_CR_KEY_MASK             (0xFFFFUL << IWDG_CR_KEY_POS)              /**< 0x0000FFFF */
#define IWDG_CR_KEY                  IWDG_CR_KEY_MASK                           /**< KEY[15:0] bits(16-bit key value) */

/* Bits for IWDG_CFG */
#define IWDG_CFG_OVP_POS             (0U)
#define IWDG_CFG_OVP_MASK            (0x7UL << IWDG_CFG_OVP_POS)                /**< 0x00000007 */
#define IWDG_CFG_OVP                 IWDG_CFG_OVP_MASK                          /**< OVP[2:0] bits (3-bit overflowperiod value) */
#define IWDG_CFG_OVP_128             (0x0UL << IWDG_CFG_OVP_POS)                /**< 0x00000000 */
#define IWDG_CFG_OVP_256             (0x1UL << IWDG_CFG_OVP_POS)                /**< 0x00000001 */
#define IWDG_CFG_OVP_512             (0x2UL << IWDG_CFG_OVP_POS)                /**< 0x00000002 */
#define IWDG_CFG_OVP_1024            (0x3UL << IWDG_CFG_OVP_POS)                /**< 0x00000003 */
#define IWDG_CFG_OVP_2048            (0x4UL << IWDG_CFG_OVP_POS)                /**< 0x00000004 */
#define IWDG_CFG_OVP_4096            (0x5UL << IWDG_CFG_OVP_POS)                /**< 0x00000005 */
#define IWDG_CFG_OVP_8192            (0x6UL << IWDG_CFG_OVP_POS)                /**< 0x00000006 */
#define IWDG_CFG_OVP_16384           (0x7UL << IWDG_CFG_OVP_POS)                /**< 0x00000007 */

/* Bits for IWDG_CNT */
#define IWDG_CNT_CNT_POS             (0U)
#define IWDG_CNT_CNT_MASK            (0xFFFUL << IWDG_CNT_CNT_POS)              /**< 0x00000FFF */
#define IWDG_CNT_CNT                 IWDG_CNT_CNT_MASK                          /**< CNT[11:0] bits (12-bit counter value) */


/*-----------------------------------------------------------------------------------------------
                                       LPTimer Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for LPTIM_ISR */
#define LPTIM_ISR_ARRM_POS               (1U)
#define LPTIM_ISR_ARRM_MASK              (0x1UL << LPTIM_ISR_ARRM_POS)             /**< 0x00000002 */
#define LPTIM_ISR_ARRM                   LPTIM_ISR_ARRM_MASK                       /**< Autoreload match */

#define LPTIM_ISR_ITRF_POS               (15U)
#define LPTIM_ISR_ITRF_MASK              (0x1UL << LPTIM_ISR_ITRF_POS)             /**< 0x00008000 */
#define LPTIM_ISR_ITRF                   LPTIM_ISR_ITRF_MASK                       /**< ITR trigger flag  */

/* Bits for LPTIM_ICR */
#define LPTIM_ICR_ARRM_CF_POS            (1U)
#define LPTIM_ICR_ARRM_CF_MASK           (0x1UL << LPTIM_ICR_ARRM_CF_POS)          /**< 0x00000002 */
#define LPTIM_ICR_ARRM_CF                LPTIM_ICR_ARRM_CF_MASK                    /**< Autoreload match clear flag */

#define LPTIM_ICR_ITRF_CF_POS            (15U)
#define LPTIM_ICR_ITRF_CF_MASK           (0x1UL << LPTIM_ICR_ITRF_CF_POS)          /**< 0x00008000 */
#define LPTIM_ICR_ITRF_CF                LPTIM_ICR_ITRF_CF_MASK                    /**< ITR trigger edge event clear flag */

/* Bits for LPTIM_IER */
#define LPTIM_IER_ARRM_IE_POS            (1U)
#define LPTIM_IER_ARRM_IE_MASK           (0x1UL << LPTIM_IER_ARRM_IE_POS)          /**< 0x00000002 */
#define LPTIM_IER_ARRM_IE                LPTIM_IER_ARRM_IE_MASK                    /**< Autoreload match interrupt enable */

#define LPTIM_IER_ITRF_IE_POS            (15U)
#define LPTIM_IER_ITRF_IE_MASK           (0x1UL << LPTIM_IER_ITRF_IE_POS)          /**< 0x00008000 */
#define LPTIM_IER_ITRF_IE                LPTIM_IER_ITRF_IE_MASK                    /**< ITR trigger edge event interrupt enable */

/* Bits for LPTIM_CFG */
#define LPTIM_CFG_PRESC_POS               (9U)
#define LPTIM_CFG_PRESC_MASK              (0x7UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000E00 */
#define LPTIM_CFG_PRESC                   LPTIM_CFG_PRESC_MASK                     /**< PRESC[2:0] bits (clock prescaler) */
#define LPTIM_CFG_PRESC_1                 (0x0UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000000 */
#define LPTIM_CFG_PRESC_2                 (0x1UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000200 */
#define LPTIM_CFG_PRESC_4                 (0x2UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000400 */
#define LPTIM_CFG_PRESC_8                 (0x3UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000600 */
#define LPTIM_CFG_PRESC_16                (0x4UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000800 */
#define LPTIM_CFG_PRESC_32                (0x5UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000A00 */
#define LPTIM_CFG_PRESC_64                (0x6UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000C00 */
#define LPTIM_CFG_PRESC_128               (0x7UL << LPTIM_CFG_PRESC_POS)           /**< 0x00000E00 */

#define LPTIM_CFG_ITREN_POS               (16U)
#define LPTIM_CFG_ITREN_MASK              (0x1UL << LPTIM_CFG_ITREN_POS)           /**< 0x00010000              */
#define LPTIM_CFG_ITREN                   LPTIM_CFG_ITREN_MASK                     /**< ITR trigger enable      */

/* Bits for LPTIM_CR */
#define LPTIM_CR_ENABLE_POS               (0U)
#define LPTIM_CR_ENABLE_MASK              (0x1UL << LPTIM_CR_ENABLE_POS)           /**< 0x00000001 */
#define LPTIM_CR_ENABLE                   LPTIM_CR_ENABLE_MASK                     /**< LPTIM enable */

#define LPTIM_CR_SNGSTRT_POS              (1U)
#define LPTIM_CR_SNGSTRT_MASK             (0x1UL << LPTIM_CR_SNGSTRT_POS)          /**< 0x00000002 */
#define LPTIM_CR_SNGSTRT                  LPTIM_CR_SNGSTRT_MASK                    /**< Timer start in single mode */

#define LPTIM_CR_CNTSTRT_POS              (2U)
#define LPTIM_CR_CNTSTRT_MASK             (0x1UL << LPTIM_CR_CNTSTRT_POS)          /**< 0x00000004 */
#define LPTIM_CR_CNTSTRT                  LPTIM_CR_CNTSTRT_MASK                    /**< Timer start in continuous mode */

/* Bits for LPTIM_ARR */
#define LPTIM_ARR_ARR_POS                 (0U)
#define LPTIM_ARR_ARR_MASK                (0xFFFFUL << LPTIM_ARR_ARR_POS)          /**< 0x0000FFF */
#define LPTIM_ARR_ARR                     LPTIM_ARR_ARR_MASK                       /**< Auto reload register */

/* Bits for LPTIM_CNT */
#define LPTIM_CNT_CNT_POS                 (0U)
#define LPTIM_CNT_CNT_MASK                (0xFFFFUL << LPTIM_CNT_CNT_POS)          /**< 0x0000FFFF */
#define LPTIM_CNT_CNT                     LPTIM_CNT_CNT_MASK                       /**< Counter register */


/*-----------------------------------------------------------------------------------------------
                                       PMU Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for PMU_CR */
#define PMU_CR_LP_MODE_POS                (0U)
#define PMU_CR_LP_MODE_MASK               (0x1UL << PMU_CR_LP_MODE_POS)                  /**< 0x00000001     */
#define PMU_CR_LP_MODE                    PMU_CR_LP_MODE_MASK                            /**< low power mode */
#define PMU_CR_LP_MODE_STOP               (0x0UL << PMU_CR_LP_MODE_POS)                  /**< 0x00000000     */
#define PMU_CR_LP_MODE_DEEPSTOP           (0x1UL << PMU_CR_LP_MODE_POS)                  /**< 0x00000001     */

/* Bits for PMU_FLASH_WAKEUP */
#define PMU_FLASH_WAKEUP_FLASH_WAKEUP_POS       (0U)
#define PMU_FLASH_WAKEUP_FLASH_WAKEUP_MASK      (0x3UL << PMU_FLASH_WAKEUP_FLASH_WAKEUP_POS)      /**< 0x00000003        */
#define PMU_FLASH_WAKEUP_FLASH_WAKEUP           PMU_FLASH_WAKEUP_FLASH_WAKEUP_MASK                /**< falsh wakeup time */


/*-----------------------------------------------------------------------------------------------
                                       RCC Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for RCC_CSR1 */
#define RCC_CSR1_RCHON_POS                 (8U)
#define RCC_CSR1_RCHON_MASK                (0x1UL << RCC_CSR1_RCHON_POS)            /**< 0x00000100 */
#define RCC_CSR1_RCHON                     RCC_CSR1_RCHON_MASK                      /**< RCH enable */

#define RCC_CSR1_RCHRDY_POS                (10U)
#define RCC_CSR1_RCHRDY_MASK               (0x1UL << RCC_CSR1_RCHRDY_POS)           /**< 0x00000400 */
#define RCC_CSR1_RCHRDY                    RCC_CSR1_RCHRDY_MASK                     /**< RCH ready flag */

#define RCC_CSR1_EXTCLKON_POS              (16U)
#define RCC_CSR1_EXTCLKON_MASK             (0x1UL << RCC_CSR1_EXTCLKON_POS)         /**< 0x00010000 */
#define RCC_CSR1_EXTCLKON                  RCC_CSR1_EXTCLKON_MASK                   /**< EXTCLK enable */

/* Bits for RCC_CFG */
#define RCC_CFG_SYSW_POS                   (0U)
#define RCC_CFG_SYSW_MASK                  (0x7UL << RCC_CFG_SYSW_POS)                 /**< 0x00000007 */
#define RCC_CFG_SYSW                       RCC_CFG_SYSW_MASK                           /**< SYSW[2:0] system clock selection */
#define RCC_CFG_SYSW_RCHDIV6               (0x0UL << RCC_CFG_SYSW_POS)                 /**< 0x00000000 */
#define RCC_CFG_SYSW_RCHDIV3               (0x1UL << RCC_CFG_SYSW_POS)                 /**< 0x00000001 */
#define RCC_CFG_SYSW_RCH                   (0x2UL << RCC_CFG_SYSW_POS)                 /**< 0x00000002 */
#define RCC_CFG_SYSW_RCL                   (0x3UL << RCC_CFG_SYSW_POS)                 /**< 0x00000003 */
#define RCC_CFG_SYSW_EXTCLK                (0x4UL << RCC_CFG_SYSW_POS)                 /**< 0x00000004 */
                                                                                
#define RCC_CFG_SYSWS_POS                  (3U)                                     
#define RCC_CFG_SYSWS_MASK                 (0x7UL << RCC_CFG_SYSWS_POS)                /**< 0x00000038 */
#define RCC_CFG_SYSWS                      RCC_CFG_SYSWS_MASK                          /**< SYSWS[2:0] system clock select status   */
#define RCC_CFG_SYSWS_RCHDIV6              (0x0UL << RCC_CFG_SYSWS_POS)                /**< 0x00000000 RCHDIV6 used as system clock */
#define RCC_CFG_SYSWS_RCHDIV3              (0x1UL << RCC_CFG_SYSWS_POS)                /**< 0x00000008 RCHDIV3 used as system clock */
#define RCC_CFG_SYSWS_RCH                  (0x2UL << RCC_CFG_SYSWS_POS)                /**< 0x00000010 RCH used as system clock     */
#define RCC_CFG_SYSWS_RCL                  (0x3UL << RCC_CFG_SYSWS_POS)                /**< 0x00000018 RCL used as system clock     */
#define RCC_CFG_SYSWS_EXTCLK               (0x4UL << RCC_CFG_SYSWS_POS)                /**< 0x00000020 EXTCLK used as system clock  */
                                                                                                                                                                     
#define RCC_CFG_HPRE_POS                   (8U)                                       
#define RCC_CFG_HPRE_MASK                  (0x7UL << RCC_CFG_HPRE_POS)                 /**< 0x00000700 */
#define RCC_CFG_HPRE                       RCC_CFG_HPRE_MASK                           /**< HPRE[2:0] AHB clock division factor */
#define RCC_CFG_HPRE_1                     (0x0UL << RCC_CFG_HPRE_POS)                 /**< 0x00000000 */
#define RCC_CFG_HPRE_2                     (0x1UL << RCC_CFG_HPRE_POS)                 /**< 0x00000100 */
#define RCC_CFG_HPRE_4                     (0x2UL << RCC_CFG_HPRE_POS)                 /**< 0x00000200 */
#define RCC_CFG_HPRE_8                     (0x3UL << RCC_CFG_HPRE_POS)                 /**< 0x00000300 */
#define RCC_CFG_HPRE_16                    (0x4UL << RCC_CFG_HPRE_POS)                 /**< 0x00000400 */
#define RCC_CFG_HPRE_32                    (0x5UL << RCC_CFG_HPRE_POS)                 /**< 0x00000500 */
#define RCC_CFG_HPRE_64                    (0x6UL << RCC_CFG_HPRE_POS)                 /**< 0x00000600 */
#define RCC_CFG_HPRE_128                   (0x7UL << RCC_CFG_HPRE_POS)                 /**< 0x00000700 */
                                                                                    
#define RCC_CFG_PPRE_POS                   (12U)                                      
#define RCC_CFG_PPRE_MASK                  (0x7UL << RCC_CFG_PPRE_POS)                 /**< 0x00007000 */
#define RCC_CFG_PPRE                       RCC_CFG_PPRE_MASK                           /**< PPRE[2:0] APB clock division factor */
#define RCC_CFG_PPRE_1                     (0x0UL << RCC_CFG_PPRE_POS)                 /**< 0x00000000 */
#define RCC_CFG_PPRE_2                     (0x4UL << RCC_CFG_PPRE_POS)                 /**< 0x00004000 */
#define RCC_CFG_PPRE_4                     (0x5UL << RCC_CFG_PPRE_POS)                 /**< 0x00005000 */
#define RCC_CFG_PPRE_8                     (0x6UL << RCC_CFG_PPRE_POS)                 /**< 0x00006000 */
#define RCC_CFG_PPRE_16                    (0x7UL << RCC_CFG_PPRE_POS)                 /**< 0x00007000 */
                                                                                    
#define RCC_CFG_MCOSEL_POS                 (24U)                                      
#define RCC_CFG_MCOSEL_MASK                (0x7UL << RCC_CFG_MCOSEL_POS)               /**< 0x07000000 */
#define RCC_CFG_MCOSEL                     RCC_CFG_MCOSEL_MASK                         /**< MCOSEL [2:0] MCO clock output selection */
#define RCC_CFG_MCOSEL_DISABLE             (0x0UL << RCC_CFG_MCOSEL_POS)               /**< 0x00000000 */
#define RCC_CFG_MCOSEL_SYSCLK              (0x1UL << RCC_CFG_MCOSEL_POS)               /**< 0x01000000 */
#define RCC_CFG_MCOSEL_RCHDIV6             (0x3UL << RCC_CFG_MCOSEL_POS)               /**< 0x03000000 */
#define RCC_CFG_MCOSEL_EXTCLK              (0x4UL << RCC_CFG_MCOSEL_POS)               /**< 0x04000000 */
#define RCC_CFG_MCOSEL_RCL                 (0x6UL << RCC_CFG_MCOSEL_POS)               /**< 0x06000000 */
                                                                                    
#define RCC_CFG_MCOPRE_POS                 (28U)                                      
#define RCC_CFG_MCOPRE_MASK                (0x7UL << RCC_CFG_MCOPRE_POS)               /**< 0x70000000 */
#define RCC_CFG_MCOPRE                     RCC_CFG_MCOPRE_MASK                         /**< MCOPRE[2:0] MCO output clock division factor */
#define RCC_CFG_MCOPRE_DIV1                (0x0UL << RCC_CFG_MCOPRE_POS)               /**< 0x00000000 */
#define RCC_CFG_MCOPRE_DIV2                (0x1UL << RCC_CFG_MCOPRE_POS)               /**< 0x10000000 */
#define RCC_CFG_MCOPRE_DIV4                (0x2UL << RCC_CFG_MCOPRE_POS)               /**< 0x20000000 */
#define RCC_CFG_MCOPRE_DIV8                (0x3UL << RCC_CFG_MCOPRE_POS)               /**< 0x30000000 */
#define RCC_CFG_MCOPRE_DIV16               (0x4UL << RCC_CFG_MCOPRE_POS)               /**< 0x40000000 */
#define RCC_CFG_MCOPRE_DIV32               (0x5UL << RCC_CFG_MCOPRE_POS)               /**< 0x50000000 */
#define RCC_CFG_MCOPRE_DIV64               (0x6UL << RCC_CFG_MCOPRE_POS)               /**< 0x60000000 */
#define RCC_CFG_MCOPRE_DIV128              (0x7UL << RCC_CFG_MCOPRE_POS)               /**< 0x70000000 */
                                                                                                                         
/* Bits for RCC_IER */                
#define RCC_IER_RCL_RDYIE_POS              (0U)
#define RCC_IER_RCL_RDYIE_MASK             (0x1UL << RCC_IER_RCL_RDYIE_POS)            /**< 0x00000001 */
#define RCC_IER_RCL_RDYIE                  RCC_IER_RCL_RDYIE_MASK                      /**< RCL ready interrupt enable   */

#define RCC_IER_RCH_RDYIE_POS              (3U)                                       
#define RCC_IER_RCH_RDYIE_MASK             (0x1UL << RCC_IER_RCH_RDYIE_POS)            /**< 0x00000008 */
#define RCC_IER_RCH_RDYIE                  RCC_IER_RCH_RDYIE_MASK                      /**< RCH ready interrupt enable   */
                                                                                  
/* Bits for RCC_ISR */                                                              
#define RCC_ISR_RCL_RDYF_POS               (0U)                                        
#define RCC_ISR_RCL_RDYF_MASK              (0x1UL << RCC_ISR_RCL_RDYF_POS)             /**< 0x00000001 */
#define RCC_ISR_RCL_RDYF                   RCC_ISR_RCL_RDYF_MASK                       /**< RCL ready interrupt flag   */

#define RCC_ISR_RCH_RDYF_POS               (3U)                                        
#define RCC_ISR_RCH_RDYF_MASK              (0x1UL << RCC_ISR_RCH_RDYF_POS)             /**< 0x00000008 */
#define RCC_ISR_RCH_RDYF                   RCC_ISR_RCH_RDYF_MASK                       /**< RCH ready interrupt flag   */
                                                                                    
/* Bits for RCC_ICR */                                                              
#define RCC_ICR_RCL_RDYC_POS               (0U)                                        
#define RCC_ICR_RCL_RDYC_MASK              (0x1UL << RCC_ICR_RCL_RDYC_POS)             /**< 0x00000001 */
#define RCC_ICR_RCL_RDYC                   RCC_ICR_RCL_RDYC_MASK                       /**< Clear RCL ready interrupt flag   */

#define RCC_ICR_RCH_RDYC_POS               (3U)                                        
#define RCC_ICR_RCH_RDYC_MASK              (0x1UL << RCC_ICR_RCH_RDYC_POS)             /**< 0x00000008 */
#define RCC_ICR_RCH_RDYC                   RCC_ICR_RCH_RDYC_MASK                       /**< Clear RCH ready interrupt flag   */

/* Bits for RCC_IOPRST */                                                           
#define RCC_IOPRST_GPIOA_RST_POS           (0U)                                        
#define RCC_IOPRST_GPIOA_RST_MASK          (0x1UL << RCC_IOPRST_GPIOA_RST_POS)         /**< 0x00000001 */
#define RCC_IOPRST_GPIOA_RST               RCC_IOPRST_GPIOA_RST_MASK                   /**< GPIOA reset control   */

#define RCC_IOPRST_GPIOB_RST_POS           (1U)                                        
#define RCC_IOPRST_GPIOB_RST_MASK          (0x1UL << RCC_IOPRST_GPIOB_RST_POS)         /**< 0x00000002 */
#define RCC_IOPRST_GPIOB_RST               RCC_IOPRST_GPIOB_RST_MASK                   /**< GPIOB reset control   */

#define RCC_IOPRST_GPIOC_RST_POS           (2U)                                        
#define RCC_IOPRST_GPIOC_RST_MASK          (0x1UL << RCC_IOPRST_GPIOC_RST_POS)         /**< 0x00000004 */
#define RCC_IOPRST_GPIOC_RST               RCC_IOPRST_GPIOC_RST_MASK                   /**< GPIOC reset control   */
                                                                                    
/* Bits for RCC_AHBRST */                                                           
#define RCC_AHBRST_CRC_RST_POS             (12U)                                       
#define RCC_AHBRST_CRC_RST_MASK            (0x1UL << RCC_AHBRST_CRC_RST_POS)           /**< 0x00001000 */
#define RCC_AHBRST_CRC_RST                 RCC_AHBRST_CRC_RST_MASK                     /**< CRC reset control     */                                        
                                                                                    
/* Bits for RCC_APBRST1 */                                                         
#define RCC_APBRST1_TIM3_RST_POS           (1U)                                        
#define RCC_APBRST1_TIM3_RST_MASK          (0x1UL << RCC_APBRST1_TIM3_RST_POS)         /**< 0x00000002 */
#define RCC_APBRST1_TIM3_RST               RCC_APBRST1_TIM3_RST_MASK                   /**< TIM3 reset control     */
                   
#define RCC_APBRST1_UART2_RST_POS          (17U)                                       
#define RCC_APBRST1_UART2_RST_MASK         (0x1UL << RCC_APBRST1_UART2_RST_POS)        /**< 0x00020000 */
#define RCC_APBRST1_UART2_RST              RCC_APBRST1_UART2_RST_MASK                  /**< UART2 reset control    */                     
                   
#define RCC_APBRST1_I2C1_RST_POS           (21U)                                       
#define RCC_APBRST1_I2C1_RST_MASK          (0x1UL << RCC_APBRST1_I2C1_RST_POS)         /**< 0x00200000 */
#define RCC_APBRST1_I2C1_RST               RCC_APBRST1_I2C1_RST_MASK                   /**< I2C1 reset control     */
                   
#define RCC_APBRST1_LPTIM1_RST_POS          (31U)                                       
#define RCC_APBRST1_LPTIM1_RST_MASK        (0x1UL << RCC_APBRST1_LPTIM1_RST_POS)       /**< 0x80000000 */
#define RCC_APBRST1_LPTIM1_RST             RCC_APBRST1_LPTIM1_RST_MASK                 /**< LPTIM1 reset control   */
                                                                                    
/* Bits for RCC_APBRST2 */                                                          
#define RCC_APBRST2_COMP_RST_POS           (0U)                                        
#define RCC_APBRST2_COMP_RST_MASK          (0x1UL << RCC_APBRST2_COMP_RST_POS)         /**< 0x00000001 */
#define RCC_APBRST2_COMP_RST               RCC_APBRST2_COMP_RST_MASK                   /**< COMP reset control   */
                   
#define RCC_APBRST2_TIM1_RST_POS           (11U)                                        
#define RCC_APBRST2_TIM1_RST_MASK          (0x1UL << RCC_APBRST2_TIM1_RST_POS)         /**< 0x00000800 */
#define RCC_APBRST2_TIM1_RST               RCC_APBRST2_TIM1_RST_MASK                   /**< TIM1 reset control     */                                                               
                   
#define RCC_APBRST2_SPI1_RST_POS           (12U)                                       
#define RCC_APBRST2_SPI1_RST_MASK          (0x1UL << RCC_APBRST2_SPI1_RST_POS)         /**< 0x00001000 */
#define RCC_APBRST2_SPI1_RST               RCC_APBRST2_SPI1_RST_MASK                   /**< SPI1 reset control     */
                   
#define RCC_APBRST2_UART1_RST_POS          (14U)                                       
#define RCC_APBRST2_UART1_RST_MASK         (0x1UL << RCC_APBRST2_UART1_RST_POS)        /**< 0x00004000 */
#define RCC_APBRST2_UART1_RST              RCC_APBRST2_UART1_RST_MASK                  /**< UART1 reset control   */                                                                      
                   
#define RCC_APBRST2_ADC_RST_POS            (20U)                                       
#define RCC_APBRST2_ADC_RST_MASK           (0x1UL << RCC_APBRST2_ADC_RST_POS)          /**< 0x00100000 */
#define RCC_APBRST2_ADC_RST                RCC_APBRST2_ADC_RST_MASK                    /**< ADC reset control      */
                   
#define RCC_APBRST2_DBG_RST_POS            (27U)                                       
#define RCC_APBRST2_DBG_RST_MASK           (0x1UL << RCC_APBRST2_DBG_RST_POS)          /**< 0x08000000 */
#define RCC_APBRST2_DBG_RST                RCC_APBRST2_DBG_RST_MASK                    /**< DBG reset control      */
                                                                                    
/* Bits for RCC_IOPEN */                                                           
#define RCC_IOPEN_GPIOAEN_POS              (0U)                                        
#define RCC_IOPEN_GPIOAEN_MASK             (0x1UL << RCC_IOPEN_GPIOAEN_POS)            /**< 0x00000001 */
#define RCC_IOPEN_GPIOAEN                  RCC_IOPEN_GPIOAEN_MASK                      /**< GPIOA clock enable     */

#define RCC_IOPEN_GPIOBEN_POS              (1U)                                        
#define RCC_IOPEN_GPIOBEN_MASK             (0x1UL << RCC_IOPEN_GPIOBEN_POS)            /**< 0x00000002 */
#define RCC_IOPEN_GPIOBEN                  RCC_IOPEN_GPIOBEN_MASK                      /**< GPIOB clock enable     */

#define RCC_IOPEN_GPIOCEN_POS              (2U)                                        
#define RCC_IOPEN_GPIOCEN_MASK             (0x1UL << RCC_IOPEN_GPIOCEN_POS)            /**< 0x00000004 */
#define RCC_IOPEN_GPIOCEN                  RCC_IOPEN_GPIOCEN_MASK                      /**< GPIOC clock enable     */
                                                                                   
/* Bits for RCC_AHBEN */                                                            
#define RCC_AHBEN_CRCEN_POS                (12U)                                       
#define RCC_AHBEN_CRCEN_MASK               (0x1UL << RCC_AHBEN_CRCEN_POS)              /**< 0x00001000 */
#define RCC_AHBEN_CRCEN                    RCC_AHBEN_CRCEN_MASK                        /**< CRC clock enable       */
                                                                                    
/* Bits for RCC_APBEN1 */                                                           
#define RCC_APBEN1_TIM3EN_POS              (1U)                                        
#define RCC_APBEN1_TIM3EN_MASK             (0x1UL << RCC_APBEN1_TIM3EN_POS)            /**< 0x00000002 */
#define RCC_APBEN1_TIM3EN                  RCC_APBEN1_TIM3EN_MASK                      /**< TIM3 clock enable      */

#define RCC_APBEN1_UART2EN_POS             (17U)                                     
#define RCC_APBEN1_UART2EN_MASK            (0x1UL << RCC_APBEN1_UART2EN_POS)          /**< 0x00020000 */
#define RCC_APBEN1_UART2EN                 RCC_APBEN1_UART2EN_MASK                    /**< UART2 clock enable     */

#define RCC_APBEN1_I2C1EN_POS              (21U)                                     
#define RCC_APBEN1_I2C1EN_MASK             (0x1UL << RCC_APBEN1_I2C1EN_POS)            /**< 0x00200000 */
#define RCC_APBEN1_I2C1EN                  RCC_APBEN1_I2C1EN_MASK                      /**< I2C1 clock enable      */

#define RCC_APBEN1_PMUEN_POS               (28U)                                     
#define RCC_APBEN1_PMUEN_MASK              (0x1UL << RCC_APBEN1_PMUEN_POS)             /**< 0x10000000 */
#define RCC_APBEN1_PMUEN                   RCC_APBEN1_PMUEN_MASK                       /**< PMU clock enable       */                                                                        

#define RCC_APBEN1_LPTIM1EN_POS            (31U)                                     
#define RCC_APBEN1_LPTIM1EN_MASK           (0x1UL << RCC_APBEN1_LPTIM1EN_POS)          /**< 0x80000000 */
#define RCC_APBEN1_LPTIM1EN                RCC_APBEN1_LPTIM1EN_MASK                    /**< LPTIM1 clock enable    */

/* Bits for RCC_APBEN2 */
#define RCC_APBEN2_COMPEN_POS              (0U)
#define RCC_APBEN2_COMPEN_MASK             (0x1UL << RCC_APBEN2_COMPEN_POS)            /**< 0x00000001 */
#define RCC_APBEN2_COMPEN                  RCC_APBEN2_COMPEN_MASK                      /**< COMP clock enable      */

#define RCC_APBEN2_TIM1EN_POS              (11U)                                         
#define RCC_APBEN2_TIM1EN_MASK             (0x1UL << RCC_APBEN2_TIM1EN_POS)            /**< 0x00000800 */
#define RCC_APBEN2_TIM1EN                  RCC_APBEN2_TIM1EN_MASK                      /**< TIM1 clock enable      */

#define RCC_APBEN2_SPI1EN_POS              (12U)                                        
#define RCC_APBEN2_SPI1EN_MASK             (0x1UL << RCC_APBEN2_SPI1EN_POS)            /**< 0x00001000 */
#define RCC_APBEN2_SPI1EN                  RCC_APBEN2_SPI1EN_MASK                      /**< SPI1 clock enable      */

#define RCC_APBEN2_UART1EN_POS             (14U)                                        
#define RCC_APBEN2_UART1EN_MASK            (0x1UL << RCC_APBEN2_UART1EN_POS)           /**< 0x00004000 */
#define RCC_APBEN2_UART1EN                 RCC_APBEN2_UART1EN_MASK                     /**< UART1 clock enable    */

#define RCC_APBEN2_ADCEN_POS               (20U)                                        
#define RCC_APBEN2_ADCEN_MASK              (0x1UL << RCC_APBEN2_ADCEN_POS)             /**< 0x00100000 */
#define RCC_APBEN2_ADCEN                   RCC_APBEN2_ADCEN_MASK                       /**< ADC clock enable       */

#define RCC_APBEN2_DBGEN_POS               (27U)                                        
#define RCC_APBEN2_DBGEN_MASK              (0x1UL << RCC_APBEN2_DBGEN_POS)             /**< 0x08000000 */
#define RCC_APBEN2_DBGEN                   RCC_APBEN2_DBGEN_MASK                       /**< DBG clock enable       */
                                                                                     
/* Bits for RCC_CLKSEL */                                                              
#define RCC_CLKSEL_COMP1_SEL_POS           (0U)                                        
#define RCC_CLKSEL_COMP1_SEL_MASK          (0x1UL << RCC_CLKSEL_COMP1_SEL_POS)         /**< 0x00000001 */
#define RCC_CLKSEL_COMP1_SEL               RCC_CLKSEL_COMP1_SEL_MASK                   /**< COMP1 KCLK selection */ 
#define RCC_CLKSEL_COMP1_SEL_PCLK          (0x0UL << RCC_CLKSEL_COMP1_SEL_POS)         /**< 0x00000000 */
#define RCC_CLKSEL_COMP1_SEL_RCL           (0x1UL << RCC_CLKSEL_COMP1_SEL_POS)         /**< 0x00000001 */

#define RCC_CLKSEL_COMP2_SEL_POS           (1U)                                        
#define RCC_CLKSEL_COMP2_SEL_MASK          (0x1UL << RCC_CLKSEL_COMP2_SEL_POS)         /**< 0x00000002 */
#define RCC_CLKSEL_COMP2_SEL               RCC_CLKSEL_COMP2_SEL_MASK                   /**< COMP2 KCLK selection */ 
#define RCC_CLKSEL_COMP2_SEL_PCLK          (0x0UL << RCC_CLKSEL_COMP2_SEL_POS)         /**< 0x00000000 */
#define RCC_CLKSEL_COMP2_SEL_RCL           (0x1UL << RCC_CLKSEL_COMP2_SEL_POS)         /**< 0x00000002 */

#define RCC_CLKSEL_LPTIM1_SEL_POS          (18U)                                        
#define RCC_CLKSEL_LPTIM1_SEL_MASK         (0x3UL << RCC_CLKSEL_LPTIM1_SEL_POS)        /**< 0x000C0000 */
#define RCC_CLKSEL_LPTIM1_SEL              RCC_CLKSEL_LPTIM1_SEL_MASK                  /**< LPTIM1 KCLK selection  */  
#define RCC_CLKSEL_LPTIM1_SEL_PCLK         (0x0UL << RCC_CLKSEL_LPTIM1_SEL_POS)        /**< 0x00000000 */
#define RCC_CLKSEL_LPTIM1_SEL_RCL          (0x1UL << RCC_CLKSEL_LPTIM1_SEL_POS)        /**< 0x00040000 */
#define RCC_CLKSEL_LPTIM1_SEL_MCO          (0x2UL << RCC_CLKSEL_LPTIM1_SEL_POS)        /**< 0x00080000 */ 


/* Bits for RCC_CSR2 */
#define RCC_CSR2_RCLON_POS                 (0U)
#define RCC_CSR2_RCLON_MASK                (0x1UL << RCC_CSR2_RCLON_POS)               /**< 0x00000001 */
#define RCC_CSR2_RCLON                     RCC_CSR2_RCLON_MASK                         /**< RCL enable */
#define RCC_CSR2_RCLRDY_POS                (1U)                                       
#define RCC_CSR2_RCLRDY_MASK               (0x1UL << RCC_CSR2_RCLRDY_POS)              /**< 0x00000002 */
#define RCC_CSR2_RCLRDY                    RCC_CSR2_RCLRDY_MASK                        /**< RCL ready flag */

#define RCC_CSR2_LOCKUP_RSTEN_POS          (8U)                                       
#define RCC_CSR2_LOCKUP_RSTEN_MASK         (0x1UL << RCC_CSR2_LOCKUP_RSTEN_POS)        /**< 0x00000100 */
#define RCC_CSR2_LOCKUP_RSTEN              RCC_CSR2_LOCKUP_RSTEN_MASK                  /**< LOCKUP reset enable */

#define RCC_CSR2_RMVF_POS                  (16U)                                      
#define RCC_CSR2_RMVF_MASK                 (0x1UL << RCC_CSR2_RMVF_POS)                /**< 0x00010000 */
#define RCC_CSR2_RMVF                      RCC_CSR2_RMVF_MASK                          /**< Clear reset flag */
                                                           
#define RCC_CSR2_LOCKUP_RSTF_POS           (24U)                                      
#define RCC_CSR2_LOCKUP_RSTF_MASK          (0x1UL << RCC_CSR2_LOCKUP_RSTF_POS)         /**< 0x01000000 */
#define RCC_CSR2_LOCKUP_RSTF               RCC_CSR2_LOCKUP_RSTF_MASK                   /**< LOCKUP reset flag */
#define RCC_CSR2_NRST_RSTF_POS             (26U)                                      
#define RCC_CSR2_NRST_RSTF_MASK            (0x1UL << RCC_CSR2_NRST_RSTF_POS)           /**< 0x04000000 */
#define RCC_CSR2_NRST_RSTF                 RCC_CSR2_NRST_RSTF_MASK                     /**< NRST pin reset flag */ 
#define RCC_CSR2_PMU_RSTF_POS              (27U)                                      
#define RCC_CSR2_PMU_RSTF_MASK             (0x1UL << RCC_CSR2_PMU_RSTF_POS)            /**< 0x08000000 */
#define RCC_CSR2_PMU_RSTF                  RCC_CSR2_PMU_RSTF_MASK                      /**< POR or BOR reset flag */
#define RCC_CSR2_SW_RSTF_POS               (28U)                                      
#define RCC_CSR2_SW_RSTF_MASK              (0x1UL << RCC_CSR2_SW_RSTF_POS)             /**< 0x10000000 */
#define RCC_CSR2_SW_RSTF                   RCC_CSR2_SW_RSTF_MASK                       /**< Software reset flag */ 
#define RCC_CSR2_IWDG_RSTF_POS             (29U)                                      
#define RCC_CSR2_IWDG_RSTF_MASK            (0x1UL << RCC_CSR2_IWDG_RSTF_POS)           /**< 0x20000000 */
#define RCC_CSR2_IWDG_RSTF                 RCC_CSR2_IWDG_RSTF_MASK                     /**< IWDG  reset flag */
#define RCC_CSR2_LPM_RSTF_POS              (31U)                                      
#define RCC_CSR2_LPM_RSTF_MASK             (0x1UL << RCC_CSR2_LPM_RSTF_POS)            /**< 0x80000000 */
#define RCC_CSR2_LPM_RSTF                  RCC_CSR2_LPM_RSTF_MASK                      /**< Low power mode reset flag  */

/* Bits for RCC_RCL_CAL */
#define RCC_RCLCAL_RCL_CAL_POS             (0U)
#define RCC_RCLCAL_RCL_CAL_MASK            (0x0FUL << RCC_RCLCAL_RCL_CAL_POS)          /**< 0x0000000F */
#define RCC_RCLCAL_RCL_CAL                 RCC_RCLCAL_RCL_CAL_MASK  

/* Bits for RCC_RCH_CAL */
#define RCC_RCHCAL_RCH_CAL_FINE_POS        (0U)
#define RCC_RCHCAL_RCH_CAL_FINE_MASK       (0x3FUL << RCC_RCHCAL_RCH_CAL_FINE_POS)     /**< 0x0000003F */
#define RCC_RCHCAL_RCH_CAL_FINE            RCC_RCHCAL_RCH_CAL_FINE_MASK  
#define RCC_RCHCAL_RCH_CAL_COARSE_POS      (8U)
#define RCC_RCHCAL_RCH_CAL_COARSE_MASK     (0x7UL << RCC_RCHCAL_RCH_CAL_COARSE_POS)    /**< 0x00000700 */
#define RCC_RCHCAL_RCH_CAL_COARSE          RCC_RCHCAL_RCH_CAL_COARSE_MASK


/*-----------------------------------------------------------------------------------------------
                                       SPI Registers Bits
------------------------------------------------------------------------------------------------*/
/* Bits for SPI_CR1 */
#define SPI_CR1_SPE_POS                (0U)
#define SPI_CR1_SPE_MASK               (0x1UL << SPI_CR1_SPE_POS)                 /**< 0x00000001 */
#define SPI_CR1_SPE                    SPI_CR1_SPE_MASK                           /**< SPI enable */

#define SPI_CR1_BR_POS                 (1U)
#define SPI_CR1_BR_MASK                (0x7UL << SPI_CR1_BR_POS)                  /**< 0x0000000E */
#define SPI_CR1_BR                     SPI_CR1_BR_MASK                            /**< BR[2:0] bits (baud rate control) */
#define SPI_CR1_BR_PCLK_DIV_2          (0x0UL << SPI_CR1_BR_POS)                  /**< 0x00000000 */
#define SPI_CR1_BR_PCLK_DIV_4          (0x1UL << SPI_CR1_BR_POS)                  /**< 0x00000002 */
#define SPI_CR1_BR_PCLK_DIV_8          (0x2UL << SPI_CR1_BR_POS)                  /**< 0x00000004 */
#define SPI_CR1_BR_PCLK_DIV_16         (0x3UL << SPI_CR1_BR_POS)                  /**< 0x00000006 */
#define SPI_CR1_BR_PCLK_DIV_32         (0x4UL << SPI_CR1_BR_POS)                  /**< 0x00000008 */
#define SPI_CR1_BR_PCLK_DIV_64         (0x5UL << SPI_CR1_BR_POS)                  /**< 0x0000000A */
#define SPI_CR1_BR_PCLK_DIV_128        (0x6UL << SPI_CR1_BR_POS)                  /**< 0x0000000C */

#define SPI_CR1_CPHA_POS               (4U)
#define SPI_CR1_CPHA_MASK              (0x1UL << SPI_CR1_CPHA_POS)                /**< 0x00000010 */
#define SPI_CR1_CPHA                   SPI_CR1_CPHA_MASK                          /**< Clock phase */

#define SPI_CR1_CPOL_POS               (5U)
#define SPI_CR1_CPOL_MASK              (0x1UL << SPI_CR1_CPOL_POS)                /**< 0x00000020 */
#define SPI_CR1_CPOL                   SPI_CR1_CPOL_MASK                          /**< Clock polarity */

#define SPI_CR1_MSTR_POS               (6U)
#define SPI_CR1_MSTR_MASK              (0x1UL << SPI_CR1_MSTR_POS)                /**< 0x00000040 */
#define SPI_CR1_MSTR                   SPI_CR1_MSTR_MASK                          /**< Master mode selection */

#define SPI_CR1_LSBFIRST_POS           (8U)
#define SPI_CR1_LSBFIRST_MASK          (0x1UL << SPI_CR1_LSBFIRST_POS)            /**< 0x00000100 */
#define SPI_CR1_LSBFIRST               SPI_CR1_LSBFIRST_MASK                      /**< Frame format   */

#define SPI_CR1_NSSOE_POS              (10U)
#define SPI_CR1_NSSOE_MASK             (0x1UL << SPI_CR1_NSSOE_POS)               /**< 0x00000400 */
#define SPI_CR1_NSSOE                  SPI_CR1_NSSOE_MASK                         /**< NSS out enable */

#define SPI_CR1_SSM_POS                (11U)
#define SPI_CR1_SSM_MASK               (0x1UL << SPI_CR1_SSM_POS)                 /**< 0x00000800 */
#define SPI_CR1_SSM                    SPI_CR1_SSM_MASK                           /**< Slave Software NSS */

#define SPI_CR1_ERRIE_POS              (18U)
#define SPI_CR1_ERRIE_MASK             (0x1UL << SPI_CR1_ERRIE_POS)               /**< 0x00040000 */
#define SPI_CR1_ERRIE                  SPI_CR1_ERRIE_MASK                         /**< Error interrupt enable */

#define SPI_CR1_RXFNEIE_POS            (20U)
#define SPI_CR1_RXFNEIE_MASK           (0x1UL << SPI_CR1_RXFNEIE_POS)             /**< 0x00100000 */
#define SPI_CR1_RXFNEIE                SPI_CR1_RXFNEIE_MASK                       /**< RX data register not empty interrupt enable */

#define SPI_CR1_TXFEIE_POS             (25U)
#define SPI_CR1_TXFEIE_MASK            (0x1UL << SPI_CR1_TXFEIE_POS)              /**< 0x02000000 */
#define SPI_CR1_TXFEIE                 SPI_CR1_TXFEIE_MASK                        /**< TX data register empty interrupt enable */

/* Bits for SPI_CR2 */
#define SPI_CR2_NSSO_POS               (0U)
#define SPI_CR2_NSSO_MASK              (0x1UL << SPI_CR2_NSSO_POS)                /**< 0x00000001 */
#define SPI_CR2_NSSO                   SPI_CR2_NSSO_MASK                          /**< NSS out level */

/* Bits for SPI_ISR */
#define SPI_ISR_MMF_POS                (0U)
#define SPI_ISR_MMF_MASK               (0x1UL << SPI_ISR_MMF_POS)                 /**< 0x00000001 */
#define SPI_ISR_MMF                    SPI_ISR_MMF_MASK                           /**< Master mode confilct flag */

#define SPI_ISR_OVR_POS                (2U)
#define SPI_ISR_OVR_MASK               (0x1UL << SPI_ISR_OVR_POS)                 /**< 0x00000004 */
#define SPI_ISR_OVR                    SPI_ISR_OVR_MASK                           /**< RX FIFO overrun flag */

#define SPI_ISR_BUSY_POS               (4U)
#define SPI_ISR_BUSY_MASK              (0x1UL << SPI_ISR_BUSY_POS)                /**< 0x00000010 */
#define SPI_ISR_BUSY                   SPI_ISR_BUSY_MASK                          /**< Busy flag */

#define SPI_ISR_RXFNE_POS              (6U)
#define SPI_ISR_RXFNE_MASK             (0x1UL << SPI_ISR_RXFNE_POS)               /**< 0x00000040 */
#define SPI_ISR_RXFNE                  SPI_ISR_RXFNE_MASK                         /**< RX data register not empty flag */

#define SPI_ISR_TXFE_POS               (8U)
#define SPI_ISR_TXFE_MASK              (0x1UL << SPI_ISR_TXFE_POS)                /**< 0x00000100 */
#define SPI_ISR_TXFE                   SPI_ISR_TXFE_MASK                          /**< TX data register empty flag */

/* Bits for SPI_ICR */
#define SPI_ICR_MMFCF_POS              (0U)
#define SPI_ICR_MMFCF_MASK             (0x1UL << SPI_ICR_MMFCF_POS)               /**< 0x00000001 */
#define SPI_ICR_MMFCF                  SPI_ICR_MMFCF_MASK                         /**< Master mode confilct clear flag */

#define SPI_ICR_OVRCF_POS              (2U)
#define SPI_ICR_OVRCF_MASK             (0x1UL << SPI_ICR_OVRCF_POS)               /**< 0x00000004 */
#define SPI_ICR_OVRCF                  SPI_ICR_OVRCF_MASK                         /**< RX data register overrun clear flag */

/* Bits for SPI_DR */
#define SPI_DR_DR_POS                  (0U)
#define SPI_DR_DR_MASK                 (0xFFUL << SPI_DR_DR_POS)                  /**< 0x000000FF */
#define SPI_DR_DR                      SPI_DR_DR_MASK                             /**< 8 bits transmit or receive data */


/*-----------------------------------------------------------------------------------------------
                                       Timer Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for TIM_CR1 */
#define TIM_CR1_CEN_POS                   (0U)
#define TIM_CR1_CEN_MASK                  (0x1UL << TIM_CR1_CEN_POS)                   /**< 0x00000001 */
#define TIM_CR1_CEN                       TIM_CR1_CEN_MASK                             /**< Counter enable */

#define TIM_CR1_UDIS_POS                  (1U)
#define TIM_CR1_UDIS_MASK                 (0x1UL << TIM_CR1_UDIS_POS)                  /**< 0x00000002 */
#define TIM_CR1_UDIS                      TIM_CR1_UDIS_MASK                            /**< Update disable */

#define TIM_CR1_URS_POS                   (2U)
#define TIM_CR1_URS_MASK                  (0x1UL << TIM_CR1_URS_POS)                   /**< 0x00000004 */
#define TIM_CR1_URS                       TIM_CR1_URS_MASK                             /**< Update request source */

#define TIM_CR1_OPM_POS                   (3U)
#define TIM_CR1_OPM_MASK                  (0x1UL << TIM_CR1_OPM_POS)                   /**< 0x00000008 */
#define TIM_CR1_OPM                       TIM_CR1_OPM_MASK                             /**< One pulse mode */

#define TIM_CR1_DIR_POS                   (4U)
#define TIM_CR1_DIR_MASK                  (0x1UL << TIM_CR1_DIR_POS)                   /**< 0x00000010 */
#define TIM_CR1_DIR                       TIM_CR1_DIR_MASK                             /**< Direction */
                                          
#define TIM_CR1_CMS_POS                   (5U)
#define TIM_CR1_CMS_MASK                  (0x3UL << TIM_CR1_CMS_POS)                   /**< 0x00000060 */
#define TIM_CR1_CMS                       TIM_CR1_CMS_MASK                             /**< CMS[1:0] bits (center-aligned mode selection) */
#define TIM_CR1_CMS_CENTER_UP             (0x1UL << TIM_CR1_CMS_POS)                   /**< 0x00000020 */
#define TIM_CR1_CMS_CENTER_DOWN           (0x2UL << TIM_CR1_CMS_POS)                   /**< 0x00000040 */
#define TIM_CR1_CMS_CENTER_UP_DOWN        (0x3UL << TIM_CR1_CMS_POS)                   /**< 0x00000060 */
                                          
#define TIM_CR1_ARPE_POS                  (7U)
#define TIM_CR1_ARPE_MASK                 (0x1UL << TIM_CR1_ARPE_POS)                  /**< 0x00000080 */
#define TIM_CR1_ARPE                      TIM_CR1_ARPE_MASK                            /**< Auto-reload preload enable */
                                          
#define TIM_CR1_CLK_DIV_POS               (8U)
#define TIM_CR1_CLK_DIV_MASK              (0x3UL << TIM_CR1_CLK_DIV_POS)               /**< 0x00000300 */
#define TIM_CR1_CLK_DIV                   TIM_CR1_CLK_DIV_MASK                         /**< CLK_DIV[1:0] bits (clock division) */
#define TIM_CR1_CLK_DIV1                  (0x0UL << TIM_CR1_CLK_DIV_POS)               /**< 0x00000000 */
#define TIM_CR1_CLK_DIV2                  (0x1UL << TIM_CR1_CLK_DIV_POS)               /**< 0x00000100 */
#define TIM_CR1_CLK_DIV4                  (0x2UL << TIM_CR1_CLK_DIV_POS)               /**< 0x00000200 */
                                          
#define TIM_CR1_MODE_POS                  (15U)
#define TIM_CR1_MODE_MASK                 (0x1UL << TIM_CR1_MODE_POS)                  /**< 0x00008000 */
#define TIM_CR1_MODE                      TIM_CR1_MODE_MASK                            /**< work mode */
                                          
/* Bits for TIM_CR2 */                    
#define TIM_CR2_CC_PRECR_POS              (0U)
#define TIM_CR2_CC_PRECR_MASK             (0x1UL << TIM_CR2_CC_PRECR_POS)              /**< 0x00000001 */
#define TIM_CR2_CC_PRECR                  TIM_CR2_CC_PRECR_MASK                        /**< Capture/Compare preloaded control */
#define TIM_CR2_CCU_SEL_POS               (2U)
#define TIM_CR2_CCU_SEL_MASK              (0x1UL << TIM_CR2_CCU_SEL_POS)               /**< 0x00000004 */
#define TIM_CR2_CCU_SEL                   TIM_CR2_CCU_SEL_MASK                         /**< COM control update selection */
                                          
#define TIM_CR2_MM_SEL_POS                (4U)
#define TIM_CR2_MM_SEL_MASK               (0x7UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000070 */
#define TIM_CR2_MM_SEL                    TIM_CR2_MM_SEL_MASK                          /**< MM_SEL[2:0] bits (master mode melection) */
#define TIM_CR2_MM_SEL_RESET              (0x0UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000000 */
#define TIM_CR2_MM_SEL_ENABLE             (0x1UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000010 */  
#define TIM_CR2_MM_SEL_UPDATE             (0x2UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000020 */  
#define TIM_CR2_MM_SEL_CC1IF              (0x3UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000030 */  
#define TIM_CR2_MM_SEL_OC1REF             (0x4UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000040 */  
#define TIM_CR2_MM_SEL_OC2REF             (0x5UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000050 */  
#define TIM_CR2_MM_SEL_OC3REF             (0x6UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000060 */  
#define TIM_CR2_MM_SEL_OC4REF             (0x7UL << TIM_CR2_MM_SEL_POS)                /**< 0x00000070 */  
                                          
#define TIM_CR2_TI1_XOR_SEL_POS           (7U)
#define TIM_CR2_TI1_XOR_SEL_MASK          (0x1UL << TIM_CR2_TI1_XOR_SEL_POS)           /**< 0x00000080 */
#define TIM_CR2_TI1_XOR_SEL               TIM_CR2_TI1_XOR_SEL_MASK                     /**< TI1 selection */
                                          
#define TIM_CR2_OIS1_POS                  (8U)
#define TIM_CR2_OIS1_MASK                 (0x1UL << TIM_CR2_OIS1_POS)                  /**< 0x00000100 */
#define TIM_CR2_OIS1                      TIM_CR2_OIS1_MASK                            /**< Output idle state 1 (OC1 output) */
#define TIM_CR2_OIS1N_POS                 (9U)
#define TIM_CR2_OIS1N_MASK                (0x1UL << TIM_CR2_OIS1N_POS)                 /**< 0x00000200 */
#define TIM_CR2_OIS1N                     TIM_CR2_OIS1N_MASK                           /**< Output idle state 1 (OC1N output) */
#define TIM_CR2_OIS2_POS                  (10U)
#define TIM_CR2_OIS2_MASK                 (0x1UL << TIM_CR2_OIS2_POS)                  /**< 0x00000400 */
#define TIM_CR2_OIS2                      TIM_CR2_OIS2_MASK                            /**< Output idle state 2 (OC2 output) */
#define TIM_CR2_OIS2N_POS                 (11U)
#define TIM_CR2_OIS2N_MASK                (0x1UL << TIM_CR2_OIS2N_POS)                 /**< 0x00000800 */
#define TIM_CR2_OIS2N                     TIM_CR2_OIS2N_MASK                           /**< Output idle state 2 (OC2N output) */
#define TIM_CR2_OIS3_POS                  (12U)
#define TIM_CR2_OIS3_MASK                 (0x1UL << TIM_CR2_OIS3_POS)                  /**< 0x00001000 */
#define TIM_CR2_OIS3                      TIM_CR2_OIS3_MASK                            /**< Output idle state 3 (OC3 output) */
#define TIM_CR2_OIS3N_POS                 (13U)
#define TIM_CR2_OIS3N_MASK                (0x1UL << TIM_CR2_OIS3N_POS)                 /**< 0x00002000 */
#define TIM_CR2_OIS3N                     TIM_CR2_OIS3N_MASK                           /**< Output idle state 3 (OC3N output) */
#define TIM_CR2_OIS4_POS                  (14U)
#define TIM_CR2_OIS4_MASK                 (0x1UL << TIM_CR2_OIS4_POS)                  /**< 0x00004000 */
#define TIM_CR2_OIS4                      TIM_CR2_OIS4_MASK                            /**< Output idle state 4 (OC4 output) */
                                                                        
/* Bits for TIM_SMC */               
#define TIM_SMC_SM_SEL_POS                (0U)
#define TIM_SMC_SM_SEL_MASK               (0x7UL << TIM_SMC_SM_SEL_POS)                /**< 0x00000007 */
#define TIM_SMC_SM_SEL                    TIM_SMC_SM_SEL_MASK                          /**< SM_SEL[2:0] bits (slave mode selection) */
#define TIM_SMC_SM_SEL_NONE               (0x00UL << TIM_SMC_SM_SEL_POS)               /**< 0x00000000 */  
#define TIM_SMC_SM_SEL_RESET              (0x04UL << TIM_SMC_SM_SEL_POS)               /**< 0x00000004 */  
#define TIM_SMC_SM_SEL_GATED              (0x05UL << TIM_SMC_SM_SEL_POS)               /**< 0x00000005 */ 
#define TIM_SMC_SM_SEL_TRIG               (0x06UL << TIM_SMC_SM_SEL_POS)               /**< 0x00000006 */ 
#define TIM_SMC_SM_SEL_EXT_MODE1          (0x07UL << TIM_SMC_SM_SEL_POS)               /**< 0x00000007 */
                                           
#define TIM_SMC_TS_POS                    (4U)
#define TIM_SMC_TS_MASK                   (0x3UL << TIM_SMC_TS_POS)                    /**< 0x00000030 */
#define TIM_SMC_TS                        TIM_SMC_TS_MASK                              /**< TS[1:0] bits (trigger selection) */
#define TIM_SMC_TS_ITR0                   (0x00UL << TIM_SMC_TS_POS)                   /**< 0x00000000 */
#define TIM_SMC_TS_TI1F_ED                (0x01UL << TIM_SMC_TS_POS)                   /**< 0x00000010 */
#define TIM_SMC_TS_TI1FP1                 (0x02UL << TIM_SMC_TS_POS)                   /**< 0x00000020 */
#define TIM_SMC_TS_TI2FP2                 (0x03UL << TIM_SMC_TS_POS)                   /**< 0x00000030 */
                                          
#define TIM_SMC_MS_MOD_POS                (7U)
#define TIM_SMC_MS_MOD_MASK               (0x1UL << TIM_SMC_MS_MOD_POS)                /**< 0x00000080 */
#define TIM_SMC_MS_MOD                    TIM_SMC_MS_MOD_MASK                          /**< Master/Slave mode */
                                          
/* Bits for TIM_DIER */                   
#define TIM_DIER_UIE_POS                  (0U)
#define TIM_DIER_UIE_MASK                 (0x1UL << TIM_DIER_UIE_POS)                  /**< 0x00000001 */
#define TIM_DIER_UIE                      TIM_DIER_UIE_MASK                            /**< Update interrupt enable */

#define TIM_DIER_CC1IE_POS                (1U)
#define TIM_DIER_CC1IE_MASK               (0x1UL << TIM_DIER_CC1IE_POS)                /**< 0x00000002 */
#define TIM_DIER_CC1IE                    TIM_DIER_CC1IE_MASK                          /**< Capture/Compare 1 interrupt enable */
#define TIM_DIER_CC2IE_POS                (2U)
#define TIM_DIER_CC2IE_MASK               (0x1UL << TIM_DIER_CC2IE_POS)                /**< 0x00000004 */
#define TIM_DIER_CC2IE                    TIM_DIER_CC2IE_MASK                          /**< Capture/Compare 2 interrupt enable */
#define TIM_DIER_CC3IE_POS                (3U)
#define TIM_DIER_CC3IE_MASK               (0x1UL << TIM_DIER_CC3IE_POS)                /**< 0x00000008 */
#define TIM_DIER_CC3IE                    TIM_DIER_CC3IE_MASK                          /**< Capture/Compare 3 interrupt enable */
#define TIM_DIER_CC4IE_POS                (4U)
#define TIM_DIER_CC4IE_MASK               (0x1UL << TIM_DIER_CC4IE_POS)                /**< 0x00000010 */
#define TIM_DIER_CC4IE                    TIM_DIER_CC4IE_MASK                          /**< Capture/Compare 4 interrupt enable */

#define TIM_DIER_COMIE_POS                (5U)
#define TIM_DIER_COMIE_MASK               (0x1UL << TIM_DIER_COMIE_POS)                /**< 0x00000020 */
#define TIM_DIER_COMIE                    TIM_DIER_COMIE_MASK                          /**< COM interrupt enable */
#define TIM_DIER_TIE_POS                  (6U)
#define TIM_DIER_TIE_MASK                 (0x1UL << TIM_DIER_TIE_POS)                  /**< 0x00000040 */
#define TIM_DIER_TIE                      TIM_DIER_TIE_MASK                            /**< Trigger interrupt enable */
#define TIM_DIER_BIE_POS                  (7U)
#define TIM_DIER_BIE_MASK                 (0x1UL << TIM_DIER_BIE_POS)                  /**< 0x00000080 */
#define TIM_DIER_BIE                      TIM_DIER_BIE_MASK                            /**< Break interrupt enable */
                                  
/* Bits for TIM_SR */                     
#define TIM_SR_UIF_POS                    (0U)
#define TIM_SR_UIF_MASK                   (0x1UL << TIM_SR_UIF_POS)                    /**< 0x00000001 */
#define TIM_SR_UIF                        TIM_SR_UIF_MASK                              /**< Update interrupt flag */

#define TIM_SR_CC1IF_POS                  (1U)
#define TIM_SR_CC1IF_MASK                 (0x1UL << TIM_SR_CC1IF_POS)                  /**< 0x00000002 */
#define TIM_SR_CC1IF                      TIM_SR_CC1IF_MASK                            /**< Capture/Compare 1 interrupt flag */
#define TIM_SR_CC2IF_POS                  (2U)
#define TIM_SR_CC2IF_MASK                 (0x1UL << TIM_SR_CC2IF_POS)                  /**< 0x00000004 */
#define TIM_SR_CC2IF                      TIM_SR_CC2IF_MASK                            /**< Capture/Compare 2 interrupt flag */
#define TIM_SR_CC3IF_POS                  (3U)
#define TIM_SR_CC3IF_MASK                 (0x1UL << TIM_SR_CC3IF_POS)                  /**< 0x00000008 */
#define TIM_SR_CC3IF                      TIM_SR_CC3IF_MASK                            /**< Capture/Compare 3 interrupt flag */
#define TIM_SR_CC4IF_POS                  (4U)
#define TIM_SR_CC4IF_MASK                 (0x1UL << TIM_SR_CC4IF_POS)                  /**< 0x00000010 */
#define TIM_SR_CC4IF                      TIM_SR_CC4IF_MASK                            /**< Capture/Compare 4 interrupt flag */

#define TIM_SR_COMIF_POS                  (5U)
#define TIM_SR_COMIF_MASK                 (0x1UL << TIM_SR_COMIF_POS)                  /**< 0x00000020 */
#define TIM_SR_COMIF                      TIM_SR_COMIF_MASK                            /**< COM interrupt flag */
#define TIM_SR_TIF_POS                    (6U)
#define TIM_SR_TIF_MASK                   (0x1UL << TIM_SR_TIF_POS)                    /**< 0x00000040 */
#define TIM_SR_TIF                        TIM_SR_TIF_MASK                              /**< Trigger interrupt flag */
#define TIM_SR_BIF_POS                    (7U)
#define TIM_SR_BIF_MASK                   (0x1UL << TIM_SR_BIF_POS)                    /**< 0x00000080 */
#define TIM_SR_BIF                        TIM_SR_BIF_MASK                              /**< Break interrupt flag */

#define TIM_SR_CC1OF_POS                  (9U)
#define TIM_SR_CC1OF_MASK                 (0x1UL << TIM_SR_CC1OF_POS)                  /**< 0x00000200 */
#define TIM_SR_CC1OF                      TIM_SR_CC1OF_MASK                            /**< Capture/Compare 1 overcapture flag */
#define TIM_SR_CC2OF_POS                  (10U)
#define TIM_SR_CC2OF_MASK                 (0x1UL << TIM_SR_CC2OF_POS)                  /**< 0x00000400 */
#define TIM_SR_CC2OF                      TIM_SR_CC2OF_MASK                            /**< Capture/Compare 2 overcapture flag */
                                        
/* Bits for TIM_EVTG */                   
#define TIM_EVTG_UG_POS                   (0U)
#define TIM_EVTG_UG_MASK                  (0x1UL << TIM_EVTG_UG_POS)                  /**< 0x00000001 */
#define TIM_EVTG_UG                       TIM_EVTG_UG_MASK                            /**< Update generation */

#define TIM_EVTG_CC1G_POS                 (1U)
#define TIM_EVTG_CC1G_MASK                (0x1UL << TIM_EVTG_CC1G_POS)                /**< 0x00000002 */
#define TIM_EVTG_CC1G                     TIM_EVTG_CC1G_MASK                          /**< Capture/Compare 1 generation */
#define TIM_EVTG_CC2G_POS                 (2U)
#define TIM_EVTG_CC2G_MASK                (0x1UL << TIM_EVTG_CC2G_POS)                /**< 0x00000004 */
#define TIM_EVTG_CC2G                     TIM_EVTG_CC2G_MASK                          /**< Capture/Compare 2 generation */
#define TIM_EVTG_CC3G_POS                 (3U)
#define TIM_EVTG_CC3G_MASK                (0x1UL << TIM_EVTG_CC3G_POS)                /**< 0x00000008 */
#define TIM_EVTG_CC3G                     TIM_EVTG_CC3G_MASK                          /**< Capture/Compare 3 generation */
#define TIM_EVTG_CC4G_POS                 (4U)
#define TIM_EVTG_CC4G_MASK                (0x1UL << TIM_EVTG_CC4G_POS)                /**< 0x00000010 */
#define TIM_EVTG_CC4G                     TIM_EVTG_CC4G_MASK                          /**< Capture/Compare 4 generation */

#define TIM_EVTG_COMG_POS                 (5U)
#define TIM_EVTG_COMG_MASK                (0x1UL << TIM_EVTG_COMG_POS)                /**< 0x00000020 */
#define TIM_EVTG_COMG                     TIM_EVTG_COMG_MASK                          /**< Capture/Compare control update generation */
#define TIM_EVTG_TG_POS                   (6U)
#define TIM_EVTG_TG_MASK                  (0x1UL << TIM_EVTG_TG_POS)                  /**< 0x00000040 */
#define TIM_EVTG_TG                       TIM_EVTG_TG_MASK                            /**< Trigger generation */
#define TIM_EVTG_BG_POS                   (7U)
#define TIM_EVTG_BG_MASK                  (0x1UL << TIM_EVTG_BG_POS)                  /**< 0x00000080 */
#define TIM_EVTG_BG                       TIM_EVTG_BG_MASK                            /**< Break generation */

/* Bits for TIM_CCM1 */
#define TIM_CCM1_CC1S_POS                 (0U)
#define TIM_CCM1_CC1S_MASK                (0x3UL << TIM_CCM1_CC1S_POS)                /**< 0x00000003 */
#define TIM_CCM1_CC1S                     TIM_CCM1_CC1S_MASK                          /**< CC1S[1:0] bits (Capture/Compare 1 selection) */
#define TIM_CCM1_CC1S_DIRECTTI            (0x1UL << TIM_CCM1_CC1S_POS)                /**< 0x00000001 */
#define TIM_CCM1_CC1S_INDIRECTTI          (0x2UL << TIM_CCM1_CC1S_POS)                /**< 0x00000002 */
#define TIM_CCM1_CC1S_TRC                 (0x3UL << TIM_CCM1_CC1S_POS)                /**< 0x00000003 */
                                          
#define TIM_CCM1_OC1FE_POS                (2U)
#define TIM_CCM1_OC1FE_MASK               (0x1UL << TIM_CCM1_OC1FE_POS)               /**< 0x00000004 */
#define TIM_CCM1_OC1FE                    TIM_CCM1_OC1FE_MASK                         /**< Output Compare 1 fast enable */
#define TIM_CCM1_OC1PE_POS                (3U)
#define TIM_CCM1_OC1PE_MASK               (0x1UL << TIM_CCM1_OC1PE_POS)               /**< 0x00000008 */
#define TIM_CCM1_OC1PE                    TIM_CCM1_OC1PE_MASK                         /**< Output Compare 1 preload enable */
                                     
#define TIM_CCM1_OC1M_POS                 (4U)
#define TIM_CCM1_OC1M_MASK                (0x7UL << TIM_CCM1_OC1M_POS)                /**< 0x00000070 */
#define TIM_CCM1_OC1M                     TIM_CCM1_OC1M_MASK                          /**< OC1M[2:0] bits (output compare 1 mode) */
#define TIM_CCM1_OC1M_FROZEN              (0x00UL << TIM_CCM1_OC1M_POS)               /**< 0x00000000 */ 
#define TIM_CCM1_OC1M_ACTIVE              (0x01UL << TIM_CCM1_OC1M_POS)               /**< 0x00000010 */ 
#define TIM_CCM1_OC1M_INACTIVE            (0x02UL << TIM_CCM1_OC1M_POS)               /**< 0x00000020 */ 
#define TIM_CCM1_OC1M_TOGGLE              (0x03UL << TIM_CCM1_OC1M_POS)               /**< 0x00000030 */ 
#define TIM_CCM1_OC1M_FORCED_INACTIVE     (0x04UL << TIM_CCM1_OC1M_POS)               /**< 0x00000040 */ 
#define TIM_CCM1_OC1M_FORCED_ACTIVE       (0x05UL << TIM_CCM1_OC1M_POS)               /**< 0x00000050 */ 
#define TIM_CCM1_OC1M_PWM1                (0x06UL << TIM_CCM1_OC1M_POS)               /**< 0x00000060 */ 
#define TIM_CCM1_OC1M_PWM2                (0x07UL << TIM_CCM1_OC1M_POS)               /**< 0x00000070 */ 
                                     
#define TIM_CCM1_OC1CE_POS                (7U)
#define TIM_CCM1_OC1CE_MASK               (0x1UL << TIM_CCM1_OC1CE_POS)               /**< 0x00000080 */
#define TIM_CCM1_OC1CE                    TIM_CCM1_OC1CE_MASK                         /**<Output compare 1 clear enable */
                                          
#define TIM_CCM1_CC2S_POS                 (8U)
#define TIM_CCM1_CC2S_MASK                (0x3UL << TIM_CCM1_CC2S_POS)                /**< 0x00000300 */
#define TIM_CCM1_CC2S                     TIM_CCM1_CC2S_MASK                          /**< CC2S[1:0] bits (Capture/Compare 2 selection) */
#define TIM_CCM1_CC2S_DIRECTTI            (0x1UL << TIM_CCM1_CC2S_POS)                /**< 0x00000100 */
#define TIM_CCM1_CC2S_INDIRECTTI          (0x2UL << TIM_CCM1_CC2S_POS)                /**< 0x00000200 */
#define TIM_CCM1_CC2S_TRC                 (0x3UL << TIM_CCM1_CC2S_POS)                /**< 0x00000300 */
                                          
#define TIM_CCM1_OC2FE_POS                (10U)
#define TIM_CCM1_OC2FE_MASK               (0x1UL << TIM_CCM1_OC2FE_POS)               /**< 0x00000400 */
#define TIM_CCM1_OC2FE                    TIM_CCM1_OC2FE_MASK                         /**<Output compare 2 fast enable */
#define TIM_CCM1_OC2PE_POS                (11U)
#define TIM_CCM1_OC2PE_MASK               (0x1UL << TIM_CCM1_OC2PE_POS)               /**< 0x00000800 */
#define TIM_CCM1_OC2PE                    TIM_CCM1_OC2PE_MASK                         /**<Output compare 2 preload enable */
                                     
#define TIM_CCM1_OC2M_POS                 (12U)
#define TIM_CCM1_OC2M_MASK                (0x7UL << TIM_CCM1_OC2M_POS)                /**< 0x00007000 */
#define TIM_CCM1_OC2M                     TIM_CCM1_OC2M_MASK                          /**< OC2M[2:0] bits (Output compare 2 mode) */
#define TIM_CCM1_OC2M_FROZEN              (0x00UL << TIM_CCM1_OC2M_POS)               /**< 0x00000000 */ 
#define TIM_CCM1_OC2M_ACTIVE              (0x01UL << TIM_CCM1_OC2M_POS)               /**< 0x00001000 */ 
#define TIM_CCM1_OC2M_INACTIVE            (0x02UL << TIM_CCM1_OC2M_POS)               /**< 0x00002000 */ 
#define TIM_CCM1_OC2M_TOGGLE              (0x03UL << TIM_CCM1_OC2M_POS)               /**< 0x00003000 */ 
#define TIM_CCM1_OC2M_FORCED_INACTIVE     (0x04UL << TIM_CCM1_OC2M_POS)               /**< 0x00004000 */ 
#define TIM_CCM1_OC2M_FORCED_ACTIVE       (0x05UL << TIM_CCM1_OC2M_POS)               /**< 0x00005000 */ 
#define TIM_CCM1_OC2M_PWM1                (0x06UL << TIM_CCM1_OC2M_POS)               /**< 0x00006000 */ 
#define TIM_CCM1_OC2M_PWM2                (0x07UL << TIM_CCM1_OC2M_POS)               /**< 0x00007000 */ 
                                     
#define TIM_CCM1_OC2CE_POS                (15U)
#define TIM_CCM1_OC2CE_MASK               (0x1UL << TIM_CCM1_OC2CE_POS)               /**< 0x00008000 */
#define TIM_CCM1_OC2CE                    TIM_CCM1_OC2CE_MASK                         /**< Output compare 2 clear enable */
                                     
/* input and output multiplex*/      
#define TIM_CCM1_IC1PSC_POS               (2U)
#define TIM_CCM1_IC1PSC_MASK              (0x3UL << TIM_CCM1_IC1PSC_POS)              /**< 0x0000000C */
#define TIM_CCM1_IC1PSC                   TIM_CCM1_IC1PSC_MASK                        /**< IC1PSC[1:0] bits (Input capture 1 prescaler) */
#define TIM_CCM1_IC1PSC_DIV1              (0x0UL << TIM_CCM1_IC1PSC_POS)              /**< 0x00000000 */
#define TIM_CCM1_IC1PSC_DIV2              (0x1UL << TIM_CCM1_IC1PSC_POS)              /**< 0x00000004 */
#define TIM_CCM1_IC1PSC_DIV4              (0x2UL << TIM_CCM1_IC1PSC_POS)              /**< 0x00000008 */
#define TIM_CCM1_IC1PSC_DIV8              (0x3UL << TIM_CCM1_IC1PSC_POS)              /**< 0x0000000C */
                                          
#define TIM_CCM1_IC1F_POS                 (4U)
#define TIM_CCM1_IC1F_MASK                (0x7UL << TIM_CCM1_IC1F_POS)                /**< 0x00000070 */
#define TIM_CCM1_IC1F                     TIM_CCM1_IC1F_MASK                          /**< IC1F[2:0] bits (Input capture 1 filter) */
#define TIM_CCM1_IC1F_FILT_DIV1           (0x01UL << TIM_CCM1_IC1F_POS)               /**< 0x00000010 */
#define TIM_CCM1_IC1F_FILT_DIV2           (0x02UL << TIM_CCM1_IC1F_POS)               /**< 0x00000020 */
#define TIM_CCM1_IC1F_FILT_DIV4           (0x03UL << TIM_CCM1_IC1F_POS)               /**< 0x00000030 */
#define TIM_CCM1_IC1F_FILT_DIV8           (0x04UL << TIM_CCM1_IC1F_POS)               /**< 0x00000040 */
#define TIM_CCM1_IC1F_FILT_DIV16          (0x05UL << TIM_CCM1_IC1F_POS)               /**< 0x00000050 */
#define TIM_CCM1_IC1F_FILT_DIV32          (0x06UL << TIM_CCM1_IC1F_POS)               /**< 0x00000060 */
                                     
#define TIM_CCM1_IC2PSC_POS               (10U)
#define TIM_CCM1_IC2PSC_MASK              (0x3UL << TIM_CCM1_IC2PSC_POS)              /**< 0x00000C00 */
#define TIM_CCM1_IC2PSC                   TIM_CCM1_IC2PSC_MASK                        /**< IC2PSC[1:0] bits (Input capture 2 prescaler) */
#define TIM_CCM1_IC2PSC_DIV1              (0x0UL << TIM_CCM1_IC2PSC_POS)              /**< 0x00000000 */
#define TIM_CCM1_IC2PSC_DIV2              (0x1UL << TIM_CCM1_IC2PSC_POS)              /**< 0x00000400 */
#define TIM_CCM1_IC2PSC_DIV4              (0x2UL << TIM_CCM1_IC2PSC_POS)              /**< 0x00000800 */
#define TIM_CCM1_IC2PSC_DIV8              (0x3UL << TIM_CCM1_IC2PSC_POS)              /**< 0x00000C00 */
                                          
#define TIM_CCM1_IC2F_POS                 (12U)
#define TIM_CCM1_IC2F_MASK                (0x7UL << TIM_CCM1_IC2F_POS)                /**< 0x0000F000 */
#define TIM_CCM1_IC2F                     TIM_CCM1_IC2F_MASK                          /**< IC2F[2:0] bits (Input capture 2 filter) */
#define TIM_CCM1_IC2F_FILT_DIV1           (0x01UL << TIM_CCM1_IC2F_POS)               /**< 0x00001000 */
#define TIM_CCM1_IC2F_FILT_DIV2           (0x02UL << TIM_CCM1_IC2F_POS)               /**< 0x00002000 */
#define TIM_CCM1_IC2F_FILT_DIV4           (0x03UL << TIM_CCM1_IC2F_POS)               /**< 0x00003000 */
#define TIM_CCM1_IC2F_FILT_DIV8           (0x04UL << TIM_CCM1_IC2F_POS)               /**< 0x00004000 */
#define TIM_CCM1_IC2F_FILT_DIV16          (0x05UL << TIM_CCM1_IC2F_POS)               /**< 0x00005000 */
#define TIM_CCM1_IC2F_FILT_DIV32          (0x06UL << TIM_CCM1_IC2F_POS)               /**< 0x00006000 */
                                     
/* Bits for TIM_CCM2 */                                                       
#define TIM_CCM2_OC3FE_POS                (2U)
#define TIM_CCM2_OC3FE_MASK               (0x1UL << TIM_CCM2_OC3FE_POS)               /**< 0x00000004 */
#define TIM_CCM2_OC3FE                    TIM_CCM2_OC3FE_MASK                         /**< Output Compare 3 fast enable */
#define TIM_CCM2_OC3PE_POS                (3U)
#define TIM_CCM2_OC3PE_MASK               (0x1UL << TIM_CCM2_OC3PE_POS)               /**< 0x00000008 */
#define TIM_CCM2_OC3PE                    TIM_CCM2_OC3PE_MASK                         /**< Output Compare 3 preload enable */
                                     
#define TIM_CCM2_OC3M_POS                 (4U)
#define TIM_CCM2_OC3M_MASK                (0x7UL << TIM_CCM2_OC3M_POS)                /**< 0x00010070 */
#define TIM_CCM2_OC3M                     TIM_CCM2_OC3M_MASK                          /**< OC3M[2:0] bits (Output compare 3 mode) */
#define TIM_CCM2_OC3M_FROZEN              (0x00UL << TIM_CCM2_OC3M_POS)               /**< 0x00000000 */ 
#define TIM_CCM2_OC3M_ACTIVE              (0x01UL << TIM_CCM2_OC3M_POS)               /**< 0x00000010 */ 
#define TIM_CCM2_OC3M_INACTIVE            (0x02UL << TIM_CCM2_OC3M_POS)               /**< 0x00000020 */ 
#define TIM_CCM2_OC3M_TOGGLE              (0x03UL << TIM_CCM2_OC3M_POS)               /**< 0x00000030 */ 
#define TIM_CCM2_OC3M_FORCED_INACTIVE     (0x04UL << TIM_CCM2_OC3M_POS)               /**< 0x00000040 */ 
#define TIM_CCM2_OC3M_FORCED_ACTIVE       (0x05UL << TIM_CCM2_OC3M_POS)               /**< 0x00000050 */ 
#define TIM_CCM2_OC3M_PWM1                (0x06UL << TIM_CCM2_OC3M_POS)               /**< 0x00000060 */ 
#define TIM_CCM2_OC3M_PWM2                (0x07UL << TIM_CCM2_OC3M_POS)               /**< 0x00000070 */ 
                                     
#define TIM_CCM2_OC3CE_POS                (7U)
#define TIM_CCM2_OC3CE_MASK               (0x1UL << TIM_CCM2_OC3CE_POS)               /**< 0x00000080 */
#define TIM_CCM2_OC3CE                    TIM_CCM2_OC3CE_MASK                         /**< Output compare 3 clear enable */
                                                                                  
#define TIM_CCM2_OC4FE_POS                (10U)
#define TIM_CCM2_OC4FE_MASK               (0x1UL << TIM_CCM2_OC4FE_POS)               /**< 0x00000400 */
#define TIM_CCM2_OC4FE                    TIM_CCM2_OC4FE_MASK                         /**< Output compare 4 fast enable */
#define TIM_CCM2_OC4PE_POS                (11U)
#define TIM_CCM2_OC4PE_MASK               (0x1UL << TIM_CCM2_OC4PE_POS)               /**< 0x00000800 */
#define TIM_CCM2_OC4PE                    TIM_CCM2_OC4PE_MASK                         /**< Output compare 4 preload enable */
                                     
#define TIM_CCM2_OC4M_POS                 (12U)
#define TIM_CCM2_OC4M_MASK                (0x7UL << TIM_CCM2_OC4M_POS)                /**< 0x01007000 */
#define TIM_CCM2_OC4M                     TIM_CCM2_OC4M_MASK                          /**< OC4M[2:0] bits (Output compare 4 mode) */
#define TIM_CCM2_OC4M_FROZEN              (0x00UL << TIM_CCM2_OC4M_POS)               /**< 0x00000000 */ 
#define TIM_CCM2_OC4M_ACTIVE              (0x01UL << TIM_CCM2_OC4M_POS)               /**< 0x00001000 */ 
#define TIM_CCM2_OC4M_INACTIVE            (0x02UL << TIM_CCM2_OC4M_POS)               /**< 0x00002000 */ 
#define TIM_CCM2_OC4M_TOGGLE              (0x03UL << TIM_CCM2_OC4M_POS)               /**< 0x00003000 */ 
#define TIM_CCM2_OC4M_FORCED_INACTIVE     (0x04UL << TIM_CCM2_OC4M_POS)               /**< 0x00004000 */ 
#define TIM_CCM2_OC4M_FORCED_ACTIVE       (0x05UL << TIM_CCM2_OC4M_POS)               /**< 0x00005000 */ 
#define TIM_CCM2_OC4M_PWM1                (0x06UL << TIM_CCM2_OC4M_POS)               /**< 0x00006000 */ 
#define TIM_CCM2_OC4M_PWM2                (0x07UL << TIM_CCM2_OC4M_POS)               /**< 0x00007000 */ 
                                     
#define TIM_CCM2_OC4CE_POS                (15U)
#define TIM_CCM2_OC4CE_MASK               (0x1UL << TIM_CCM2_OC4CE_POS)               /**< 0x00008000 */
#define TIM_CCM2_OC4CE                    TIM_CCM2_OC4CE_MASK                         /**<Output compare 4 clear enable */
                                     
/* Bits for TIM_CCEN */
#define TIM_CCEN_CC1E_POS                 (0U)
#define TIM_CCEN_CC1E_MASK                (0x1UL << TIM_CCEN_CC1E_POS)                 /**< 0x00000001 */
#define TIM_CCEN_CC1E                     TIM_CCEN_CC1E_MASK                           /**<Capture/Compare 1 output enable */
#define TIM_CCEN_CC1P_POS                 (1U)
#define TIM_CCEN_CC1P_MASK                (0x1UL << TIM_CCEN_CC1P_POS)                 /**< 0x00000002 */
#define TIM_CCEN_CC1P                     TIM_CCEN_CC1P_MASK                           /**<Capture/Compare 1 output Polarity */
#define TIM_CCEN_CC1NE_POS                (2U)
#define TIM_CCEN_CC1NE_MASK               (0x1UL << TIM_CCEN_CC1NE_POS)                /**< 0x00000004 */
#define TIM_CCEN_CC1NE                    TIM_CCEN_CC1NE_MASK                          /**<Capture/Compare 1 complementary output enable */
#define TIM_CCEN_CC1NP_POS                (3U)
#define TIM_CCEN_CC1NP_MASK               (0x1UL << TIM_CCEN_CC1NP_POS)                /**< 0x00000008 */
#define TIM_CCEN_CC1NP                    TIM_CCEN_CC1NP_MASK                          /**<Capture/Compare 1 complementary output Polarity */

#define TIM_CCEN_CC2E_POS                 (4U)
#define TIM_CCEN_CC2E_MASK                (0x1UL << TIM_CCEN_CC2E_POS)                 /**< 0x00000010 */
#define TIM_CCEN_CC2E                     TIM_CCEN_CC2E_MASK                           /**<Capture/Compare 2 output enable */
#define TIM_CCEN_CC2P_POS                 (5U)
#define TIM_CCEN_CC2P_MASK                (0x1UL << TIM_CCEN_CC2P_POS)                 /**< 0x00000020 */
#define TIM_CCEN_CC2P                     TIM_CCEN_CC2P_MASK                           /**<Capture/Compare 2 output Polarity */
#define TIM_CCEN_CC2NE_POS                (6U)
#define TIM_CCEN_CC2NE_MASK               (0x1UL << TIM_CCEN_CC2NE_POS)                /**< 0x00000040 */
#define TIM_CCEN_CC2NE                    TIM_CCEN_CC2NE_MASK                          /**<Capture/Compare 2 complementary output enable */
#define TIM_CCEN_CC2NP_POS                (7U)
#define TIM_CCEN_CC2NP_MASK               (0x1UL << TIM_CCEN_CC2NP_POS)                /**< 0x00000080 */
#define TIM_CCEN_CC2NP                    TIM_CCEN_CC2NP_MASK                          /**<Capture/Compare 2 complementary output Polarity */

#define TIM_CCEN_CC3E_POS                 (8U)
#define TIM_CCEN_CC3E_MASK                (0x1UL << TIM_CCEN_CC3E_POS)                 /**< 0x00000100 */
#define TIM_CCEN_CC3E                     TIM_CCEN_CC3E_MASK                           /**<Capture/Compare 3 output enable */
#define TIM_CCEN_CC3P_POS                 (9U)
#define TIM_CCEN_CC3P_MASK                (0x1UL << TIM_CCEN_CC3P_POS)                 /**< 0x00000200 */
#define TIM_CCEN_CC3P                     TIM_CCEN_CC3P_MASK                           /**<Capture/Compare 3 output Polarity */
#define TIM_CCEN_CC3NE_POS                (10U)
#define TIM_CCEN_CC3NE_MASK               (0x1UL << TIM_CCEN_CC3NE_POS)                /**< 0x00000400 */
#define TIM_CCEN_CC3NE                    TIM_CCEN_CC3NE_MASK                          /**<Capture/Compare 3 complementary output enable */
#define TIM_CCEN_CC3NP_POS                (11U)
#define TIM_CCEN_CC3NP_MASK               (0x1UL << TIM_CCEN_CC3NP_POS)                /**< 0x00000800 */
#define TIM_CCEN_CC3NP                    TIM_CCEN_CC3NP_MASK                          /**<Capture/Compare 3 complementary output polarity */

#define TIM_CCEN_CC4E_POS                 (12U)
#define TIM_CCEN_CC4E_MASK                (0x1UL << TIM_CCEN_CC4E_POS)                 /**< 0x00001000 */
#define TIM_CCEN_CC4E                     TIM_CCEN_CC4E_MASK                           /**<Capture/Compare 4 output enable */
#define TIM_CCEN_CC4P_POS                 (13U)
#define TIM_CCEN_CC4P_MASK                (0x1UL << TIM_CCEN_CC4P_POS)                 /**< 0x00002000 */
#define TIM_CCEN_CC4P                     TIM_CCEN_CC4P_MASK                           /**<Capture/Compare 4 output Polarity */                                         

/* Bits for TIM_CNT */                    
#define TIM_CNT_CNT_POS                   (0U)
#define TIM_CNT_CNT_MASK                  (0xFFFFUL << TIM_CNT_CNT_POS)                /**< 0x0000FFFF */
#define TIM_CNT_CNT                       TIM_CNT_CNT_MASK                             /**<Counter value */
                                        
/* Bits for TIM_PSC */                    
#define TIM_PSC_PSC_POS                   (0U)
#define TIM_PSC_PSC_MASK                  (0xFFFFUL << TIM_PSC_PSC_POS)                /**< note£ºTIM1£º0x0000FFFF; TIM3£º0x000000FF */
#define TIM_PSC_PSC                       TIM_PSC_PSC_MASK                             /**<Prescaler value */
                                          
/* Bits for TIM_ARR */                    
#define TIM_ARR_ARR_POS                   (0U)
#define TIM_ARR_ARR_MASK                  (0xFFFFUL << TIM_ARR_ARR_POS)                /**< 0x0000FFFF */
#define TIM_ARR_ARR                       TIM_ARR_ARR_MASK                             /**< Actual auto-reload value */
                                          
/* Bits for TIM_RCR */                    
#define TIM_RCR_REP_POS                   (0U)
#define TIM_RCR_REP_MASK                  (0xFFUL << TIM_RCR_REP_POS)                  /**< 0x0000FF */
#define TIM_RCR_REP                       TIM_RCR_REP_MASK                             /**< Repetition Counter value */
                                          
/* Bits for TIM_CC1 */                    
#define TIM_CC1_CC1_POS                   (0U) 
#define TIM_CC1_CC1_MASK                  (0xFFFFUL << TIM_CC1_CC1_POS)                /**< 0x0000FFFF */
#define TIM_CC1_CC1                       TIM_CC1_CC1_MASK                             /**<Capture/Compare 1 value */
                                     
/* Bits for TIM_CC2 */                    
#define TIM_CC2_CC2_POS                   (0U)
#define TIM_CC2_CC2_MASK                  (0xFFFFUL << TIM_CC2_CC2_POS)                /**< 0x0000FFFF */
#define TIM_CC2_CC2                       TIM_CC2_CC2_MASK                             /**<Capture/Compare 2 value */
                                          
/* Bits for TIM1_CC3 */                    
#define TIM1_CC3_CC3_POS                  (0U)
#define TIM1_CC3_CC3_MASK                 (0xFFFFUL << TIM1_CC3_CC3_POS)               /**< 0x0000FFFF */
#define TIM1_CC3_CC3                      TIM1_CC3_CC3_MASK                            /**<Capture/Compare 3 value */
                                          
/* Bits for TIM1_CC4 */                    
#define TIM1_CC4_CC4_POS                  (0U)
#define TIM1_CC4_CC4_MASK                 (0xFFFFUL << TIM1_CC4_CC4_POS)               /**< 0x0000FFFF */
#define TIM1_CC4_CC4                      TIM1_CC4_CC4_MASK                            /**< Capture/Compare 4 value */
                                          
/* Bits for TIM3_CC1_MODE1 */                                                         
#define TIM3_CC1_CC1_MODE1_POS            (0U)
#define TIM3_CC1_CC1_MODE1_MASK           (0xFFUL << TIM3_CC1_CC1_MODE1_POS)                 /**< 0x000000FF */
#define TIM3_CC1_CC1_MODE1                TIM3_CC1_CC1_MODE1_MASK                            /**<Capture/Compare 1 value */

/* Bits for TIM3_MODE1_CC2 */                                                         
#define TIM3_CC2_CC2_MODE1_POS            (0U)
#define TIM3_CC2_CC2_MODE1_MASK           (0xFFUL << TIM3_CC2_CC2_MODE1_POS)                 /**< 0x000000FF */
#define TIM3_CC2_CC2_MODE1                TIM3_CC2_CC2_MODE1_MASK                            /**<Capture/Compare 2 value */

/* Bits for TIM3_MODE1_CC3 */                    
#define TIM3_CC1_CC3_MODE1_POS            (8U)
#define TIM3_CC1_CC3_MODE1_MASK           (0xFFUL << TIM3_CC1_CC3_MODE1_POS)                 /**< 0x0000FF00 */
#define TIM3_CC1_CC3_MODE1                TIM3_CC1_CC3_MODE1_MASK                            /**<Capture/Compare 3 value */
                                         
/* Bits for TIM3_MODE1_CC4 */                    
#define TIM3_CC2_CC4_MODE1_POS            (8U)
#define TIM3_CC2_CC4_MODE1_MASK           (0xFFUL << TIM3_CC2_CC4_MODE1_POS)                 /**< 0x0000FF00 */
#define TIM3_CC2_CC4_MODE1                TIM3_CC2_CC4_MODE1_MASK                            /**<Capture/Compare 4 value */

/* Bits for TIM_BDT */                    
#define TIM_BDT_DTG_POS                   (0U)
#define TIM_BDT_DTG_MASK                  (0xFFUL << TIM_BDT_DTG_POS)                 /**< 0x000000FF */
#define TIM_BDT_DTG                       TIM_BDT_DTG_MASK                            /**< DTG[0:7] bits (Dead-Time generator set-up) */
                                          
#define TIM_BDT_LOCK_POS                  (8U)
#define TIM_BDT_LOCK_MASK                 (0x3UL << TIM_BDT_LOCK_POS)                 /**< 0x00000300 */
#define TIM_BDT_LOCK                      TIM_BDT_LOCK_MASK                           /**< LOCK[1:0] bits (Lock configuration) */
#define TIM_BDT_LOCK_LEVEL1               (0x1UL << TIM_BDT_LOCK_POS)                 /**< 0x00000100 */
#define TIM_BDT_LOCK_LEVEL2               (0x2UL << TIM_BDT_LOCK_POS)                 /**< 0x00000200 */
#define TIM_BDT_LOCK_LEVEL3               (0x3UL << TIM_BDT_LOCK_POS)                 /**< 0x00000300 */
                                          
#define TIM_BDT_OSSI_POS                  (10U)
#define TIM_BDT_OSSI_MASK                 (0x1UL << TIM_BDT_OSSI_POS)                 /**< 0x00000400 */
#define TIM_BDT_OSSI                      TIM_BDT_OSSI_MASK                           /**< Off-State selection for idle mode */
#define TIM_BDT_OSSR_POS                  (11U)
#define TIM_BDT_OSSR_MASK                 (0x1UL << TIM_BDT_OSSR_POS)                 /**< 0x00000800 */
#define TIM_BDT_OSSR                      TIM_BDT_OSSR_MASK                           /**< Off-State selection for run mode */

#define TIM_BDT_BKEN_POS                  (12U)
#define TIM_BDT_BKEN_MASK                 (0x1UL << TIM_BDT_BKEN_POS)                 /**< 0x00001000 */
#define TIM_BDT_BKEN                      TIM_BDT_BKEN_MASK                           /**< Break enable for break 1 */
                                          
#define TIM_BDT_AOEN_POS                  (14U)
#define TIM_BDT_AOEN_MASK                 (0x1UL << TIM_BDT_AOEN_POS)                 /**< 0x00004000 */
#define TIM_BDT_AOEN                      TIM_BDT_AOEN_MASK                           /**< Automatic output enable */
#define TIM_BDT_MOEN_POS                  (15U)
#define TIM_BDT_MOEN_MASK                 (0x1UL << TIM_BDT_MOEN_POS)                 /**< 0x00008000 */
#define TIM_BDT_MOEN                      TIM_BDT_MOEN_MASK                           /**< Main output enable */
                                        
/* Bits for TIM_CFG */                    
#define TIM_CFG_OCREF_CLR_POS             (0U)
#define TIM_CFG_OCREF_CLR_MASK            (0x1UL << TIM_CFG_OCREF_CLR_POS)             /**< 0x00000001 */
#define TIM_CFG_OCREF_CLR                 TIM_CFG_OCREF_CLR_MASK                       /**<OCREF clear input selection */
                                                                                 
/* BK Bits for TIM1_AF1 */                
#define TIM1_AF1_BKINE_POS                (0U)
#define TIM1_AF1_BKINE_MASK               (0x1UL << TIM1_AF1_BKINE_POS)                /**< 0x00000001 */
#define TIM1_AF1_BKINE                    TIM1_AF1_BKINE_MASK                          /**< BRK BKIN input enable */
#define TIM1_AF1_BKCMP1E_POS              (1U)
#define TIM1_AF1_BKCMP1E_MASK             (0x1UL << TIM1_AF1_BKCMP1E_POS)              /**< 0x00000002 */
#define TIM1_AF1_BKCMP1E                  TIM1_AF1_BKCMP1E_MASK                        /**< BRK COMP1 enable */
#define TIM1_AF1_BKCMP2E_POS              (2U)
#define TIM1_AF1_BKCMP2E_MASK             (0x1UL << TIM1_AF1_BKCMP2E_POS)              /**< 0x00000004 */
#define TIM1_AF1_BKCMP2E                  TIM1_AF1_BKCMP2E_MASK                        /**< BRK COMP2 enable */

#define TIM1_AF1_LOCKUP_LOCK_POS          (7U)
#define TIM1_AF1_LOCKUP_LOCK_MASK         (0x1UL << TIM1_AF1_LOCKUP_LOCK_POS)          /**< 0x00000080 */
#define TIM1_AF1_LOCKUP_LOCK              TIM1_AF1_LOCKUP_LOCK_MASK                    /**< LOCKUP LOCK enable */
                                          
#define TIM1_AF1_BKINP_POS                (9U)
#define TIM1_AF1_BKINP_MASK               (0x1UL << TIM1_AF1_BKINP_POS)                /**< 0x00000200 */
#define TIM1_AF1_BKINP                    TIM1_AF1_BKINP_MASK                          /**< BRK BKIN input polarity */
#define TIM1_AF1_BKCMP1P_POS              (10U)
#define TIM1_AF1_BKCMP1P_MASK             (0x1UL << TIM1_AF1_BKCMP1P_POS)              /**< 0x00000400 */
#define TIM1_AF1_BKCMP1P                  TIM1_AF1_BKCMP1P_MASK                        /**< BRK COMP1 input polarity */
#define TIM1_AF1_BKCMP2P_POS              (11U)
#define TIM1_AF1_BKCMP2P_MASK             (0x1UL << TIM1_AF1_BKCMP2P_POS)              /**< 0x00000800 */
#define TIM1_AF1_BKCMP2P                  TIM1_AF1_BKCMP2P_MASK                        /**< BRK COMP2 input polarity */
                                                                                                                      
/* Bits for TIM_TISEL */                  
#define TIM_TISEL_TI1_SEL_POS             (0U)
#define TIM_TISEL_TI1_SEL_MASK            (0x1UL << TIM_TISEL_TI1_SEL_POS)             /**< 0x00000001 */
#define TIM_TISEL_TI1_SEL                 TIM_TISEL_TI1_SEL_MASK                       /**<TI1_SEL bits (TIM1 TI1 SEL)*/
#define TIM_TISEL_TI1_SEL_CH1             (0x0UL << TIM_TISEL_TI1_SEL_POS)             /**< 0x00000000 */
#define TIM_TISEL_TI1_SEL_COMP1           (0x1UL << TIM_TISEL_TI1_SEL_POS)             /**< 0x00000001 */                                          
                                          
#define TIM_TISEL_TI2_SEL_POS             (8U)
#define TIM_TISEL_TI2_SEL_MASK            (0x1UL << TIM_TISEL_TI2_SEL_POS)             /**< 0x00000100 */
#define TIM_TISEL_TI2_SEL                 TIM_TISEL_TI2_SEL_MASK                       /**<TI2_SEL bits (TIM1 TI2 SEL)*/                                        
#define TIM_TISEL_TI2_SEL_CH2             (0x0UL << TIM_TISEL_TI2_SEL_POS)             /**< 0x00000000 */
#define TIM_TISEL_TI2_SEL_COMP2           (0x1UL << TIM_TISEL_TI2_SEL_POS)             /**< 0x00000001 */    


/*-----------------------------------------------------------------------------------------------
                                       UART Registers Bits  
------------------------------------------------------------------------------------------------*/
/* Bits for UART_CR1 */
#define UART_CR1_UE_POS               (0U)
#define UART_CR1_UE_MASK              (0x1UL << UART_CR1_UE_POS)                     /**< 0x00000001 */
#define UART_CR1_UE                   UART_CR1_UE_MASK                               /**< UART enable */
#define UART_CR1_RE_POS               (2U)                                         
#define UART_CR1_RE_MASK              (0x1UL << UART_CR1_RE_POS)                     /**< 0x00000004 */
#define UART_CR1_RE                   UART_CR1_RE_MASK                               /**< Receiver enable */
#define UART_CR1_TE_POS               (3U)                                         
#define UART_CR1_TE_MASK              (0x1UL << UART_CR1_TE_POS)                     /**< 0x00000008 */
#define UART_CR1_TE                   UART_CR1_TE_MASK                               /**< Transmitter enable */
#define UART_CR1_RXNEIE_POS           (5U)                                         
#define UART_CR1_RXNEIE_MASK          (0x1UL << UART_CR1_RXNEIE_POS)                 /**< 0x00000020 */
#define UART_CR1_RXNEIE               UART_CR1_RXNEIE_MASK                           /**< Read data register not empty interrupt enable */
#define UART_CR1_TCIE_POS             (6U)                                         
#define UART_CR1_TCIE_MASK            (0x1UL << UART_CR1_TCIE_POS)                   /**< 0x00000040 */
#define UART_CR1_TCIE                 UART_CR1_TCIE_MASK                             /**< Transmission Complete interrupt enable */
#define UART_CR1_TXEIE_POS            (7U)                                         
#define UART_CR1_TXEIE_MASK           (0x1UL << UART_CR1_TXEIE_POS)                  /**< 0x00000080 */
#define UART_CR1_TXEIE                UART_CR1_TXEIE_MASK                            /**< Transmit Data Register empty interrupt enable */
#define UART_CR1_PEIE_POS             (8U)                                         
#define UART_CR1_PEIE_MASK            (0x1UL << UART_CR1_PEIE_POS)                   /**< 0x00000100 */
#define UART_CR1_PEIE                 UART_CR1_PEIE_MASK                             /**< PE interrupt enable */
#define UART_CR1_PTS_POS              (9U)                                         
#define UART_CR1_PTS_MASK             (0x1UL << UART_CR1_PTS_POS)                    /**< 0x00000200 */
#define UART_CR1_PTS                  UART_CR1_PTS_MASK                              /**< Parity selection */
#define UART_CR1_PEN_POS              (10U)                                        
#define UART_CR1_PEN_MASK             (0x1UL << UART_CR1_PEN_POS)                    /**< 0x00000400 */
#define UART_CR1_PEN                  UART_CR1_PEN_MASK                              /**< Parity control enable */

#define UART_CR1_WL_POS               (12U)                                        
#define UART_CR1_WL_MASK              (0x1UL << UART_CR1_WL_POS)                     
#define UART_CR1_WL                   UART_CR1_WL_MASK                               /**< Word length */
#define UART_CR1_WL8BITS              (0x0UL << UART_CR1_WL_POS)                     /**< 0x00000000 */
#define UART_CR1_WL9BITS              UART_CR1_WL_MASK                               /**< 0x00001000 */
                          
/* Bits for UART_CR2 */                                                           
#define UART_CR2_STOPBIT_POS          (13U)                                       
#define UART_CR2_STOPBIT_MASK         (0x1UL << UART_CR2_STOPBIT_POS)                /**< 0x00002000 */
#define UART_CR2_STOPBIT              UART_CR2_STOPBIT_MASK                          /**< STOP[1:0] bits (STOP bits) */
#define UART_CR2_STOPBIT_1            (0x0UL << UART_CR2_STOPBIT_POS)                /**< 0x00000000 */
#define UART_CR2_STOPBIT_2            (0x1UL << UART_CR2_STOPBIT_POS)                /**< 0x00002000 */
                          
#define UART_CR2_SWAP_POS             (15U)                                       
#define UART_CR2_SWAP_MASK            (0x1UL << UART_CR2_SWAP_POS)                   /**< 0x00008000 */
#define UART_CR2_SWAP                 UART_CR2_SWAP_MASK                             /**< SWAP TX/RX pins */
#define UART_CR2_RXIVC_POS            (16U)                                       
#define UART_CR2_RXIVC_MASK           (0x1UL << UART_CR2_RXIVC_POS)                  /**< 0x00010000 */
#define UART_CR2_RXIVC                UART_CR2_RXIVC_MASK                            /**< RX pin active level inversion */
#define UART_CR2_TXIVC_POS            (17U)                                       
#define UART_CR2_TXIVC_MASK           (0x1UL << UART_CR2_TXIVC_POS)                  /**< 0x00020000 */
#define UART_CR2_TXIVC                UART_CR2_TXIVC_MASK                            /**< TX pin active level inversion */
#define UART_CR2_DATAIVC_POS          (18U)                                       
#define UART_CR2_DATAIVC_MASK         (0x1UL << UART_CR2_DATAIVC_POS)                /**< 0x00040000 */
#define UART_CR2_DATAIVC              UART_CR2_DATAIVC_MASK                          /**< Binary data inversion */
#define UART_CR2_MSBFIRST_POS         (19U)                                       
#define UART_CR2_MSBFIRST_MASK        (0x1UL << UART_CR2_MSBFIRST_POS)               /**< 0x00080000 */
#define UART_CR2_MSBFIRST             UART_CR2_MSBFIRST_MASK                         /**< Most significant bit first */
                          
/* Bits for UART_CR3 */                                                           
#define UART_CR3_EIE_POS              (0U)                                        
#define UART_CR3_EIE_MASK             (0x1UL << UART_CR3_EIE_POS)                    /**< 0x00000001 */
#define UART_CR3_EIE                  UART_CR3_EIE_MASK                              /**< Error interrupt enable */
#define UART_CR3_HDEN_POS             (3U)                                        
#define UART_CR3_HDEN_MASK            (0x1UL << UART_CR3_HDEN_POS)                   /**< 0x00000008 */
#define UART_CR3_HDEN                 UART_CR3_HDEN_MASK                             /**< Half-Duplex selection */
#define UART_CR3_OBS_POS              (11U)                                       
#define UART_CR3_OBS_MASK             (0x1UL << UART_CR3_OBS_POS)                    /**< 0x00000800 */
#define UART_CR3_OBS                  UART_CR3_OBS_MASK                              /**< One sample bit method enable */
#define UART_CR3_ORED_POS             (12U)                                       
#define UART_CR3_ORED_MASK            (0x1UL << UART_CR3_ORED_POS)                   /**< 0x00001000 */
#define UART_CR3_ORED                 UART_CR3_ORED_MASK                             /**< Overrun disable */
                          
/* Bits for UART_BRR */                                                           
#define UART_BRR_BRR_POS              (0U)                                        
#define UART_BRR_BRR_MASK             (0xFFFFUL << UART_BRR_BRR_POS)                 /**< 0x0000FFFF */
#define UART_BRR_BRR                  UART_BRR_BRR_MASK                                  /**< UART  baud rate register [15:0] */
                         
/* Bits for UART_ISR */
#define UART_ISR_PE_POS               (0U)
#define UART_ISR_PE_MASK              (0x1UL << UART_ISR_PE_POS)                     /**< 0x00000001 */
#define UART_ISR_PE                   UART_ISR_PE_MASK                               /**< Parity error */
#define UART_ISR_FE_POS               (1U)                                          
#define UART_ISR_FE_MASK              (0x1UL << UART_ISR_FE_POS)                     /**< 0x00000002 */
#define UART_ISR_FE                   UART_ISR_FE_MASK                               /**< Framing error */
#define UART_ISR_NOISE_POS            (2U)                                          
#define UART_ISR_NOISE_MASK           (0x1UL << UART_ISR_NOISE_POS)                  /**< 0x00000004 */
#define UART_ISR_NOISE                UART_ISR_NOISE_MASK                            /**< Noise detected flag */
#define UART_ISR_ORE_POS              (3U)                                          
#define UART_ISR_ORE_MASK             (0x1UL << UART_ISR_ORE_POS)                    /**< 0x00000008 */
#define UART_ISR_ORE                  UART_ISR_ORE_MASK                              /**< OverRun error */
#define UART_ISR_RXNE_POS             (5U)                                          
#define UART_ISR_RXNE_MASK            (0x1UL << UART_ISR_RXNE_POS)                   /**< 0x00000020 */
#define UART_ISR_RXNE                 UART_ISR_RXNE_MASK                             /**< Read data register not empty */
#define UART_ISR_TC_POS               (6U)                                          
#define UART_ISR_TC_MASK              (0x1UL << UART_ISR_TC_POS)                     /**< 0x00000040 */
#define UART_ISR_TC                   UART_ISR_TC_MASK                               /**< Transmission complete */
#define UART_ISR_TXE_POS              (7U)                                          
#define UART_ISR_TXE_MASK             (0x1UL << UART_ISR_TXE_POS)                    /**< 0x00000080 */
#define UART_ISR_TXE                   UART_ISR_TXE_MASK                             /**< Transmit data register empty */
#define UART_ISR_BUSY_POS             (16U)                                         
#define UART_ISR_BUSY_MASK            (0x1UL << UART_ISR_BUSY_POS)                   /**< 0x00010000 */
#define UART_ISR_BUSY                 UART_ISR_BUSY_MASK                             /**< Busy flag */

/* Bits for UART_ICR */                                                             
#define UART_ICR_PECF_POS             (0U)                                          
#define UART_ICR_PECF_MASK            (0x1UL << UART_ICR_PECF_POS)                   /**< 0x00000001 */
#define UART_ICR_PECF                 UART_ICR_PECF_MASK                             /**< Parity error clear flag */
#define UART_ICR_FECF_POS             (1U)                                          
#define UART_ICR_FECF_MASK            (0x1UL << UART_ICR_FECF_POS)                   /**< 0x00000002 */
#define UART_ICR_FECF                 UART_ICR_FECF_MASK                             /**< Framing error clear flag */
#define UART_ICR_NOISECF_POS          (2U)                                          
#define UART_ICR_NOISECF_MASK         (0x1UL << UART_ICR_NOISECF_POS)                /**< 0x00000004 */
#define UART_ICR_NOISECF              UART_ICR_NOISECF_MASK                          /**< Noise error detected clear flag */
#define UART_ICR_ORECF_POS            (3U)                                          
#define UART_ICR_ORECF_MASK           (0x1UL << UART_ICR_ORECF_POS)                  /**< 0x00000008 */
#define UART_ICR_ORECF                UART_ICR_ORECF_MASK                            /**< OverRun error clear flag*/
#define UART_ICR_TCCF_POS             (6U)                                          
#define UART_ICR_TCCF_MASK            (0x1UL << UART_ICR_TCCF_POS)                   /**< 0x00000040 */
#define UART_ICR_TCCF                 UART_ICR_TCCF_MASK                             /**< Transmission complete clear flag */
                        
/* Bits for UART_RDR */                                                             
#define UART_RDR_RDR_POS              (0U)                                          
#define UART_RDR_RDR_MASK             (0x1FFUL << UART_RDR_RDR_POS)                  /**< 0x000001FF */
#define UART_RDR_RDR                  UART_RDR_RDR_MASK                              /**< RDR[8:0] bits (Receive data value) */
                        
/* Bits for UART_TDR */                                                             
#define UART_TDR_TDR_POS              (0U)                                          
#define UART_TDR_TDR_MASK             (0x1FFUL << UART_TDR_TDR_POS)                  /**< 0x000001FF */
#define UART_TDR_TDR                  UART_TDR_TDR_MASK                              /**< TDR[8:0] bits (Transmit data value) */



/**
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
* @}
*/

/**
* @}
*/

#endif /* CIU32F003_H */


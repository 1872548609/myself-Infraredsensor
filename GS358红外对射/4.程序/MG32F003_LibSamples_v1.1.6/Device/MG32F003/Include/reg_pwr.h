/**
 * @file    reg_pwr.h
 * @author  MegawinTech Application Team
 * @version V1.0.0
 * @date    11-March-2025
 * @brief   This flie contains all the PWR's register and its field definition.
 */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/

#ifndef __REG_PWR_H
#define __REG_PWR_H

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include <core_cm0.h>

/**
  * @brief PWR Base Address Definition
  */
#define  PWR_BASE                                  0x40007000 /*!<Base Address: 0x40007000*/

/**
  * @brief PWR Register Structure Definition
  */
typedef struct
{
    __IO uint32_t CR;                  /*!<PWR Power control register                     offset: 0x00       */
    __IO uint32_t CSR;                 /*!<PWR Power control status register              offset: 0x04       */
} PWR_TypeDef;

/**
  * @brief PWR type pointer Definition
  */
#define PWR                                       ((PWR_TypeDef *)PWR_BASE)

/**
  * @brief PWR_CR Register Bit Definition
  */
#define  PWR_CR_LPDS_Pos                          (0)
#define  PWR_CR_LPDS_Msk                          (0x1U << PWR_CR_LPDS_Pos) /*!<Low power deepsleep*/
#define  PWR_CR_LPDS                              PWR_CR_LPDS_Msk
#define  PWR_CR_PVDE_Pos                          (4)
#define  PWR_CR_PVDE_Msk                          (0x1U << PWR_CR_PVDE_Pos) /*!<Power voltage detector enable*/
#define  PWR_CR_PVDE                              PWR_CR_PVDE_Msk
#define  PWR_CR_PLS_Pos                           (9)
#define  PWR_CR_PLS_Msk                           (0xFU << PWR_CR_PLS_Pos)  /*!<PVD level selection*/
#define  PWR_CR_PLS                               PWR_CR_PLS_Msk
#define  PWR_CR_PLS_0                             (0x1U << PWR_CR_PLS_Pos)
#define  PWR_CR_PLS_1                             (0x2U << PWR_CR_PLS_Pos)
#define  PWR_CR_PLS_2                             (0x4U << PWR_CR_PLS_Pos)
#define  PWR_CR_PLS_3                             (0x8U << PWR_CR_PLS_Pos)

/**
  * @brief PWR_CSR Register Bit Definition
  */
#define  PWR_CSR_PVDO_Pos                         (2)
#define  PWR_CSR_PVDO_Msk                         (0x1U << PWR_CSR_PVDO_Pos) /*!<PVD output*/
#define  PWR_CSR_PVDO                             PWR_CSR_PVDO_Msk

#endif


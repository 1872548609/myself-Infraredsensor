/**
 * @file    crc_calculation.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _CRC_CALCULATION_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "crc_calculation.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup CRC
  * @{
  */

/**
  * @addtogroup CRC_Calculation
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define CRC_TEST_BUFFER_SIZE    114

/* Private variables **************************************************************************************************/
const uint32_t CRC_TestBuffer[CRC_TEST_BUFFER_SIZE] =
{
    0x00001021, 0x20423063, 0x408450A5, 0x60C670E7, 0x9129A14A, 0xB16BC18C,
    0xD1ADE1CE, 0xf1EF1231, 0x32732252, 0x52B54294, 0x72F762D6, 0x93398318,
    0xA35AD3BD, 0xC39CF3FF, 0xE3DE2462, 0x34430420, 0x64E674C7, 0x44A45485,
    0xA56AB54B, 0x85289509, 0xF5CFC5AC, 0xd58D3653, 0x26721611, 0x063076D7,
    0x569546B4, 0xB75BA77A, 0x97198738, 0xF7DFE7FE, 0xC7BC48C4, 0x58E56886,
    0x78A70840, 0x18612802, 0xC9CCd9ED, 0xE98Ef9AF, 0x89489969, 0xA90AB92B,
    0x4AD47AB7, 0x6A961A71, 0x0A503A33, 0x2A12DBFD, 0xFBBFEB9E, 0x9B798b58,
    0xBB3bAB1A, 0x6CA67C87, 0x5CC52C22, 0x3C030C60, 0x1C41EDAE, 0xfd8FCDEC,
    0xAd2ABD0B, 0x8D689D49, 0x7E976EB6, 0x5ED54EF4, 0x2E321E51, 0x0E70FF9F,
    0xEFBEDFDD, 0xCFFCBF1B, 0x9F598F78, 0x918881A9, 0xB1CAA1EB, 0xD10CC12D,
    0xE16F1080, 0x00A130C2, 0x20E35004, 0x40257046, 0x83B99398, 0xA3FBB3DA,
    0xC33DD31C, 0xE37FF35E, 0x129022F3, 0x32D24235, 0x52146277, 0x7256b5EA,
    0x95A88589, 0xf56EE54F, 0xD52CC50D, 0x34E224C3, 0x04817466, 0x64475424,
    0x4405A7DB, 0xB7FA8799, 0xE75FF77E, 0xC71dd73C, 0x26d336F2, 0x069116B0,
    0x76764615, 0x5634d94C, 0xC96DF90E, 0xE92f99C8, 0xB98AA9AB, 0x58444865,
    0x78066827, 0x18C008E1, 0x28A3CB7D, 0xDB5CEB3F, 0xFB1E8BF9, 0x9BD8ABBB,
    0x4A755A54, 0x6A377A16, 0x0AF11AD0, 0x2AB33A92, 0xED0FDD6C, 0xCD4DBDAA,
    0xAD8B9DE8, 0x8DC97C26, 0x5C644C45, 0x3CA22C83, 0x1CE00CC1, 0xEF1FFF3E,
    0xDF7CAF9B, 0xBFBA8FD9, 0x9FF86E17, 0x7E364E55, 0x2E933EB2, 0x0ED11EF0
};

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void CRC_Configure(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBENR_CRC, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t CRC_Caculation(void)
{
    uint8_t  i = 0;
    uint8_t  ErrorCount = 0;
    uint32_t CRC_Result = 0;

    for (i = 0; i < 10; i++)
    {
        CRC_ResetDR();

        CRC_Result = CRC_CalcCRC(0x100);

        if (0x151D1CA7 != CRC_Result)
        {
            ErrorCount++;
        }
    }

    for (i = 0; i < 10; i++)
    {
        CRC_ResetDR();

        CRC_Result = CRC_CalcBlockCRC((uint32_t *)CRC_TestBuffer, CRC_TEST_BUFFER_SIZE);

        if (0x379E9F06 != CRC_Result)
        {
            ErrorCount++;
        }
    }

    return (ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void CRC_Calculation_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    CRC_Configure();

    while (1)
    {
        if (0 == CRC_Caculation())
        {
            printf("\r\nCRC caculation is correct.");

            PLATFORM_LED_Enable(LED1, DISABLE);
        }
        else
        {
            printf("\r\nCRC caculation error!!!!!!");

            PLATFORM_LED_Enable(LED1, ENABLE);
        }

        PLATFORM_LED_Toggle(LED3);
        PLATFORM_DelayMS(500);
    }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


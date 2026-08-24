/**
 * @file    mg32_spi.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the SPI firmware functions
 */

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32_spi.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @addtogroup SPI
  * @{
  */

/** @defgroup SPI_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup SPI_Private_Functions
  * @{
  */

/**
  * @brief  Deinitializes the spi peripheral registers to their
  *         default reset values.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @retval None.
  */
void SPI_DeInit(SPI_TypeDef *spi)
{
    if (spi == SPI1)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1PERIPH_SPI1, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1PERIPH_SPI1, DISABLE);
    }
}

/**
  * @brief  Initializes the spi peripheral according to the specified
  *         parameters in the init_struct .
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  init_struct: pointer to a SPI_InitTypeDef structure
  *         that contains the configuration information for the
  *         specified SPI peripheral.
  * @retval None.
  */
void SPI_Init(SPI_TypeDef *spi, SPI_InitTypeDef *init_struct)
{
    spi->RXDNR = 0;

    if (init_struct->SPI_DataSize == SPI_DataSize_32b)
    {
        spi->GCTL |= (0x01U << SPI_GCTL_DW8_32_Pos);
    }
    else
    {
        spi->GCTL &= ~(0x01U << SPI_GCTL_DW8_32_Pos);
    }

    MODIFY_REG(spi->GCTL, SPI_GCTL_NSS_Msk, init_struct->SPI_NSS);
    MODIFY_REG(spi->GCTL, SPI_GCTL_MODE_Msk, init_struct->SPI_Mode);
    MODIFY_REG(spi->CCTL, SPI_CCTL_LSBFE_Msk, init_struct->SPI_FirstBit);
    MODIFY_REG(spi->CCTL, SPI_CCTL_CPOL_Msk, init_struct->SPI_CPOL);
    MODIFY_REG(spi->CCTL, SPI_CCTL_CPHA_Msk, init_struct->SPI_CPHA);

    spi->CCTL |= (0x01U << SPI_CCTL_SPILEN_Pos);
    MODIFY_REG(spi->SPBRG, SPI_SPBRG_SPBRG_Msk, init_struct->SPI_BaudRatePrescaler);

    if (init_struct->SPI_DataWidth >= 32)
    {
        MODIFY_REG(spi->EXTCTL, SPI_EXTCTL_EXTLEN_Msk, 0);
    }
    else
    {
        MODIFY_REG(spi->EXTCTL, SPI_EXTCTL_EXTLEN_Msk, init_struct->SPI_DataWidth);
    }
}

/**
  * @brief  Fills each init_struct member with its default value.
  * @param  init_struct: pointer to a SPI_InitTypeDef structure
  *         which will be initialized.
  * @retval None.
  */
void SPI_StructInit(SPI_InitTypeDef *init_struct)
{
    init_struct->SPI_Mode              = SPI_Mode_Slave;
    init_struct->SPI_DataSize          = SPI_DataSize_8b;
    init_struct->SPI_DataWidth         = 8;
    init_struct->SPI_CPOL              = SPI_CPOL_Low;
    init_struct->SPI_CPHA              = SPI_CPHA_1Edge;
    init_struct->SPI_NSS               = SPI_NSS_Soft;
    init_struct->SPI_BaudRatePrescaler = SPI_BaudratePrescaler_2;
    init_struct->SPI_FirstBit          = SPI_FirstBit_MSB;
}

/**
  * @brief  Enables or disables the specified SPI peripheral.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  state: new state of the spi peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void SPI_Cmd(SPI_TypeDef *spi, FunctionalState state)
{
    (state) ?                                      \
    (spi->GCTL |= (0x01U << SPI_GCTL_SPIEN_Pos)) : \
    (spi->GCTL &= ~(0x01U << SPI_GCTL_SPIEN_Pos));
}

/**
  * @brief  Transmits a Data through the spi peripheral.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  data : Data to be transmitted.
  * @retval None.
  */
void SPI_SendData(SPI_TypeDef *spi, uint32_t data)
{
    spi->TXREG = data;
}

/**
  * @brief  Returns the most recent received data by the spi peripheral.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @retval The value of the received data.
  */
uint32_t SPI_ReceiveData(SPI_TypeDef *spi)
{
    return (spi->RXREG);
}

/**
  * @brief  Slave chip csn single by selected
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  state: new state of the selected SPI CS pin
  *         request.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void SPI_CSInternalSelected(SPI_TypeDef *spi, FunctionalState state)
{
    (state) ?                                     \
    (spi->NSSR &= ~(0x01U << SPI_NSSR_NSS_Pos)) : \
    (spi->NSSR |= (0x01U << SPI_NSSR_NSS_Pos));
}

/**
  * @brief  Configures the NSS pin control mode for the selected SPI.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  nss: specifies the SPI NSS internal state.
  *         This parameter can be one of the following values:
  * @arg    SPI_NSS_SOFT: NSS pin control by software
  * @arg    SPI_NSS_HARD: NSS pin control by hardware
  * @retval None.
  */
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef *spi, uint32_t nss)
{
    MODIFY_REG(spi->GCTL, SPI_GCTL_NSS_Msk, nss);
}

/**
  * @brief  Configures the data size for the selected SPI.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  data_size: specifies the SPI data size.
  *         This parameter can be one of the following values:
  *         0 to 31, 0 = 32b, 1 = 1b, 2 = 2b
  * @arg    DataSize : 0 to 31
  * @retval None.
  * @retval None.
  */
bool SPI_DataSizeConfig(SPI_TypeDef *spi, uint8_t data_size)
{
    if (data_size > 32)
    {
        return (false);
    }

    SET_BIT(spi->CCTL, SPI_CCTL_SPILEN);
    SET_BIT(spi->CCTL, SPI_CCTL_LSBFE);

    data_size &= 0x1F;

    spi->EXTCTL = data_size;

    return (true);
}

/**
  * @brief Configures the data SizeType for the selected SPI.
  * @param spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  DataSize: selects the send and receive data register valid data.
  *         This parameter can be one of the following values:
  * @arg    SPI_DataSize_8b: 8 bits valid data
  * @arg    SPI_DataSize_32b: 32 bits valid data
  * @retval None.
  */
void SPI_DataSizeTypeConfig(SPI_TypeDef *spi, uint32_t DataSize)
{
    MODIFY_REG(spi->GCTL, SPI_GCTL_DW8_32_Msk, DataSize);
}

/**
  * @brief  Selects the data transfer direction in bi-directional mode
  *         for the specified SPI.
  * @param spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param direction: specifies the data transfer direction in
  *         bi-directional mode.
  *         This parameter can be one of the following values:
  * @arg    SPI_Enable_TX: Selects Tx transmission direction
  * @arg    SPI_Enable_RX: Selects Rx receive direction
  * @arg    SPI_Disable_TX: Disable Tx receive direction
  * @arg    SPI_Disable_RX: Disable Rx receive direction
  * @retval None.
  */
void SPI_BiDirectionalLineConfig(SPI_TypeDef *spi, uint32_t direction)
{
    if (direction == SPI_Enable_RX)
    {
        spi->GCTL |= (0x01U << SPI_GCTL_RXEN_Pos);
    }
    else if (direction == SPI_Disable_RX)
    {
        spi->GCTL &= ~(0x01U << SPI_GCTL_RXEN_Pos);
    }
    else if (direction == SPI_Enable_TX)
    {
        spi->GCTL |= (0x01U << SPI_GCTL_TXEN_Pos);
    }
    else if (direction == SPI_Disable_TX)
    {
        spi->GCTL &= ~(0x01U << SPI_GCTL_TXEN_Pos);
    }
}

/**
  * @brief  Checks whether the specified SPI flag is set or not.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  flag: specifies the SPI flag to check.
  *         This parameter can be one of the following values:
  * @arg    SPI_FLAG_RXAVL: Rx buffer has bytes flag
  * @arg    SPI_FLAG_TXEPT: Tx buffer and tx shifter empty flag
  * @arg    SPI_FLAG_TXFULL: Tx buffer full flag
  * @arg    SPI_FLAG_RXAVL_4BYTE: Receive available 4 byte data message flag.
  * @retval The new state of SPI_FLAG (SET or RESET).
  */
FlagStatus SPI_GetFlagStatus(SPI_TypeDef *spi, uint32_t flag)
{
    return ((spi->CSTAT & flag) ? SET : RESET);
}

/**
  * @brief  Checks whether the specified SPI interrupt has occurred or not.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  interrupt: specifies the SPI interrupt source to check.
  *         This parameter can be one of the following values:
  * @arg    SPI_IT_TX: Tx buffer empty interrupt
  * @arg    SPI_IT_RX: Rx buffer  interrupt
  * @arg    SPI_IT_UNDERRUN: under Error interrupt in slave mode
  * @arg    SPI_IT_RXOVER: RX OVER Error interrupt
  * @arg    SPI_IT_RXMATCH: spectials rx data numbers  interrupt
  * @arg    SPI_IT_RXFULL: Rx buffer full interrupt
  * @arg    SPI_IT_TXEPT: Tx buffer  and tx shifter empty interrupt
  * @retval The new state of SPI_IT (SET or RESET).
  */
ITStatus SPI_GetITStatus(SPI_TypeDef *spi, uint32_t interrupt)
{
    return ((spi->INTSTAT & interrupt) ? SET : RESET);
}

/**
  * @brief  Clears the spi  interrupt pending bit.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  interrupt: specifies the SPI interrupt pending bit to clear.
  * @arg    SPI_IT_TX: Tx buffer empty interrupt
  * @arg    SPI_IT_RX: Rx buffer  interrupt
  * @arg    SPI_IT_UNDERRUN: under Error interrupt in slave mode
  * @arg    SPI_IT_RXOVER: RX OVER Error interrupt
  * @arg    SPI_IT_RXMATCH: spectials rx data numbers  interrupt
  * @arg    SPI_IT_RXFULL: Rx buffer full interrupt
  * @arg    SPI_IT_TXEPT: Tx buffer  and tx shifter empty interrupt
  *         This function clears only ERR intetrrupt pending bit.
  * @retval None.
  */
void SPI_ClearITPendingBit(SPI_TypeDef *spi, uint32_t interrupt)
{
    spi->INTCLR = interrupt;
}

/**
  * @brief  SPI Hole a count Received bytes in next receive process.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  number: specifies the SPI receive Number.
  *         This parament can be 1-65535.
  *         This function can use only in SPI master single receive mode.
  * @retval None.
  */
void SPI_RxBytes(SPI_TypeDef *spi, uint16_t number)
{
    spi->RXDNR = number;
}

/**
  * @brief  slave mode tx data transmit phase adjust set.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  adjust_value: slave mode tx data transmit phase adjust enum.
  *         This parament can be :
  *         SPI_SlaveAdjust_Fast:  fast speed use
  *         SPI_SlaveAdjust_Low :  low speed use
  *         This function can use only in SPI master single receive mode.
  * @retval None.
  */
void SPI_SlaveAdjust(SPI_TypeDef *spi, uint32_t adjust_value)
{
    MODIFY_REG(spi->CCTL, SPI_CCTL_TXEDGE_Msk, adjust_value);
}

/**
  * @brief  Enables or disables all SPI interrupts.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  state: new state of all spi interrupts.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void SPI_ITCmd(SPI_TypeDef *spi, FunctionalState state)
{
    (state) ?                                      \
    (spi->GCTL |= (0x01U << SPI_GCTL_INTEN_Pos)) : \
    (spi->GCTL &= ~(0x01U << SPI_GCTL_INTEN_Pos));
}

/**
  * @brief  Enables or disables the specified SPI interrupts.
  * @param  spi: Select the SPI peripheral.
  *         This parameter can be one of the following values:
  *         SPI1.
  * @param  interrupt: specifies the SPI interrupt sources to be enabled or disabled.
  *         This parameter can be one of the following values:
  * @arg    SPI_IT_TXEPT: Transmitter empty interrupt
  * @arg    SPI_IT_RXFULL: RX FIFO full interrupt
  * @arg    SPI_IT_RXMATCH: Receive data match the RXDNR number interrupt
  * @arg    SPI_IT_RXOERR: Receive overrun error interrupt
  * @arg    SPI_IT_UNDERRUN: underrun interrupt
  * @arg    SPI_IT_RX: Receive data available interrupt
  * @arg    SPI_IT_TX: Transmit FIFO available interrupt
  * @param  state: new state of the specified spi interrupts.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void SPI_ITConfig(SPI_TypeDef *spi, uint32_t interrupt, FunctionalState state)
{
    (state) ?                   \
    (spi->INTEN |= interrupt) : \
    (spi->INTEN &= ~interrupt);
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


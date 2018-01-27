/**
 **********************************************************************************************************************
 * @file         spi.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-08-30
 * @brief        SPI C source file.
 **********************************************************************************************************************
 * @warning     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR \n
 *              IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND\n
 *              FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR\n
 *              CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n
 *              DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n
 *              DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n
 *              CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF\n
 *              THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************************************
 */

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>
#include <string.h>

#include "chip.h"

#include "spi.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define SPI_0_TX_BUFFER_SIZE    128
#define SPI_0_RX_BUFFER_SIZE    128
#define SPI_1_TX_BUFFER_SIZE    128
#define SPI_1_RX_BUFFER_SIZE    128

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** SPI-0 Transfer Setup */
static SPI_DATA_SETUP_T spi_0_xfer;
static uint16_t spi_0_tx_buffer[SPI_0_TX_BUFFER_SIZE] = {0};
static uint16_t spi_0_rx_buffer[SPI_0_RX_BUFFER_SIZE] = {0};
/** SPI-1 Transfer Setup */
static SPI_DATA_SETUP_T spi_1_xfer;
static uint16_t spi_1_tx_buffer[SPI_1_TX_BUFFER_SIZE] = {0};
static uint16_t spi_1_rx_buffer[SPI_1_RX_BUFFER_SIZE] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void spi_0_init(void)
{
    SPI_CFG_T spi_cfg;
    SPI_DELAY_CONFIG_T spi_delay_cfg;

    // Enable the clock to the Switch Matrix.
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    /*
     * Initialize SPI0 pins connect
     * SCK0: PINASSIGN3[15:8]: Select P0.27
     * MOSI0: PINASSIGN3[23:16]: Select P0.28
     * MISO0: PINASSIGN3[31:24] : Select P0.12
     * SSEL0: PINASSIGN4[7:0]: Select P0.29
     */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 27, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 28, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    //Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 29,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN));

    Chip_SWM_MovablePinAssign(SWM_SPI0_SCK_IO, 27);      // P0.27
    Chip_SWM_MovablePinAssign(SWM_SPI0_MOSI_IO, 28);     // P0.28
    Chip_SWM_MovablePinAssign(SWM_SPI0_MISO_IO, 12);     // P0.12
    //Chip_SWM_MovablePinAssign(SWM_SPI0_SSELSN_0_IO, 29);  // P0.29

    // Disable the clock to the Switch Matrix to save power .
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    // Initialize SPI Block.
    Chip_SPI_Init(LPC_SPI0);
    // Set SPI Config register.
    spi_cfg.ClkDiv      = 4;                    // Set Clock divider to maximum
    spi_cfg.Mode        = SPI_MODE_MASTER;      // Enable Master Mode
    spi_cfg.ClockMode   = SPI_CLOCK_MODE0;      // Enable Mode 0
    spi_cfg.DataOrder   = SPI_DATA_MSB_FIRST;   // Transmit MSB first
    // Slave select polarity is active low.
    spi_cfg.SSELPol     = (SPI_CFG_SPOL0_LO | SPI_CFG_SPOL1_LO | SPI_CFG_SPOL2_LO | SPI_CFG_SPOL3_LO);
    Chip_SPI_SetConfig(LPC_SPI0, &spi_cfg);
    // Set Delay register.
    
    spi_delay_cfg.PreDelay      = 0;
    spi_delay_cfg.PostDelay     = 0;
    spi_delay_cfg.FrameDelay    = 0;
    spi_delay_cfg.TransferDelay = 0;
    Chip_SPI_DelayConfig(LPC_SPI0, &spi_delay_cfg);

    // Enable SPI0.
    Chip_SPI_Enable(LPC_SPI0);

    return;
}

void spi_0_read_buffer(uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;
    uint16_t j = 0;

    spi_0_xfer.pTx = NULL;                                                          /* Transmit Buffer */
    spi_0_xfer.pRx = spi_0_rx_buffer;                                               /* Receive Buffer */
    spi_0_xfer.DataSize = 8;                                                        /* Data size in bits */
    spi_0_xfer.Length = size > SPI_0_RX_BUFFER_SIZE ? SPI_0_RX_BUFFER_SIZE : size;  /* Total frame length */
    /* Assert only SSEL0 */
    spi_0_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_0_xfer.TxCnt = 0;
    spi_0_xfer.RxCnt = 0;

    Chip_SPI_RWFrames_Blocking(LPC_SPI0, &spi_0_xfer);

    for(i = 1; i < (size + 1) && i < SPI_0_RX_BUFFER_SIZE; i++)
    {
        buffer[i - 1] = spi_0_rx_buffer[j];
        if(i % 2)
        {
            buffer[i - 1] = (uint8_t)((spi_0_rx_buffer[j] >> 8) & 0xFF);
        }
        else
        {
            buffer[i - 1] = (uint8_t)(spi_0_rx_buffer[j] & 0xFF);
        }
    }

    return;
}

void spi_0_write_buffer(uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;
    
    for(i = 0; i < size && i < SPI_0_TX_BUFFER_SIZE; i++)
    {
        spi_0_tx_buffer[i] = buffer[i];
    }    

    spi_0_xfer.pTx = spi_0_tx_buffer;                                               /* Transmit Buffer */
    spi_0_xfer.pRx = NULL;                                                          /* Receive Buffer */
    spi_0_xfer.DataSize = 8;                                                        /* Data size in bits */
    spi_0_xfer.Length = size > SPI_0_TX_BUFFER_SIZE ? SPI_0_TX_BUFFER_SIZE : size;  /* Total frame length */
    /* Assert only SSEL0 */
    spi_0_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_0_xfer.TxCnt = 0;
    spi_0_xfer.RxCnt = 0;

    Chip_SPI_WriteFrames_Blocking(LPC_SPI0, &spi_0_xfer);

    return;
}

void spi_0_write_read(uint8_t *tx, uint16_t tx_size, uint8_t *rx, uint16_t rx_size)
{
    uint16_t i = 0;
    uint16_t j = 0;

    for(i = 0; i < tx_size && i < SPI_0_TX_BUFFER_SIZE; i++)
    {
        spi_0_tx_buffer[i] = tx[i];
    }

    spi_0_xfer.pTx = spi_0_tx_buffer;       /* Transmit Buffer */
    spi_0_xfer.pRx = spi_0_rx_buffer;       /* Receive Buffer */
    spi_0_xfer.DataSize = 8;                /* Data size in bits */
    spi_0_xfer.Length = tx_size + rx_size;  /* Total frame length */
    /* Assert only SSEL0 */
    spi_0_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_0_xfer.TxCnt = 0;
    spi_0_xfer.RxCnt = 0;

    Chip_SPI_RWFrames_Blocking(LPC_SPI0, &spi_0_xfer);

    for(i = 1; i < (rx_size + 1) && i < SPI_0_RX_BUFFER_SIZE; i++)
    {
        rx[i - 1] = spi_0_rx_buffer[j];
        if(i % 2)
        {
            rx[i - 1] = (uint8_t)((spi_0_rx_buffer[j] >> 8) & 0xFF);
        }
        else
        {
            rx[i - 1] = (uint8_t)(spi_0_rx_buffer[j] & 0xFF);
        }
    }

    return;
}

void spi_1_init(void)
{
    SPI_CFG_T spi_cfg;
    SPI_DELAY_CONFIG_T spi_delay_cfg;

    // Enable the clock to the Switch Matrix.
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    /*
     * Initialize SPI0 pins connect
     * SCK0: PINASSIGN3[15:8]: Select P0.27
     * MOSI0: PINASSIGN3[23:16]: Select P0.28
     * MISO0: PINASSIGN3[31:24] : Select P0.12
     * SSEL0: PINASSIGN4[7:0]: Select P0.29
     */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 27, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 28, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    //Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 29,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN));

    Chip_SWM_MovablePinAssign(SWM_SPI1_SCK_IO, 27);      // P0.27
    Chip_SWM_MovablePinAssign(SWM_SPI1_MOSI_IO, 28);     // P0.28
    Chip_SWM_MovablePinAssign(SWM_SPI1_MISO_IO, 12);     // P0.12
    //Chip_SWM_MovablePinAssign(SWM_SPI0_SSELSN_0_IO, 29);  // P0.29

    // Disable the clock to the Switch Matrix to save power .
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    // Initialize SPI Block.
    Chip_SPI_Init(LPC_SPI1);

    // Set SPI Config register.
    spi_cfg.ClkDiv      = 4;                    // Set Clock divider to maximum
    spi_cfg.Mode        = SPI_MODE_MASTER;      // Enable Master Mode
    spi_cfg.ClockMode   = SPI_CLOCK_MODE0;      // Enable Mode 0
    spi_cfg.DataOrder   = SPI_DATA_MSB_FIRST;   // Transmit MSB first

    // Slave select polarity is active low.
    spi_cfg.SSELPol     = (SPI_CFG_SPOL0_LO | SPI_CFG_SPOL1_LO | SPI_CFG_SPOL2_LO | SPI_CFG_SPOL3_LO);
    Chip_SPI_SetConfig(LPC_SPI1, &spi_cfg);

    // Set Delay register.
    spi_delay_cfg.PreDelay      = 0;
    spi_delay_cfg.PostDelay     = 0;
    spi_delay_cfg.FrameDelay    = 0;
    spi_delay_cfg.TransferDelay = 0;
    Chip_SPI_DelayConfig(LPC_SPI1, &spi_delay_cfg);

    // Enable SPI1.
    Chip_SPI_Enable(LPC_SPI1);

    return;
}

void spi_1_read_buffer(uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;
    uint16_t j = 0;

    spi_1_xfer.pTx = NULL;                                                          /* Transmit Buffer */
    spi_1_xfer.pRx = spi_0_rx_buffer;                                               /* Receive Buffer */
    spi_1_xfer.DataSize = 8;                                                        /* Data size in bits */
    spi_1_xfer.Length = size > SPI_1_RX_BUFFER_SIZE ? SPI_1_RX_BUFFER_SIZE : size;  /* Total frame length */
    /* Assert only SSEL0 */
    spi_1_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_1_xfer.TxCnt = 0;
    spi_1_xfer.RxCnt = 0;

    Chip_SPI_RWFrames_Blocking(LPC_SPI1, &spi_1_xfer);

    for(i = 1; i < (size + 1) && i < SPI_1_RX_BUFFER_SIZE; i++)
    {
        buffer[i - 1] = spi_1_rx_buffer[j];
        if(i % 2)
        {
            buffer[i - 1] = (uint8_t)((spi_1_rx_buffer[j] >> 8) & 0xFF);
        }
        else
        {
            buffer[i - 1] = (uint8_t)(spi_1_rx_buffer[j] & 0xFF);
        }
    }

    return;
}

void spi_1_write_buffer(uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;

    for(i = 0; i < size && i < SPI_1_TX_BUFFER_SIZE; i++)
    {
        spi_1_tx_buffer[i] = buffer[i];
    }

    spi_1_xfer.pTx = spi_1_tx_buffer;                                               /* Transmit Buffer */
    spi_1_xfer.pRx = NULL;                                                          /* Receive Buffer */
    spi_1_xfer.DataSize = 8;                                                        /* Data size in bits */
    spi_1_xfer.Length = size > SPI_1_TX_BUFFER_SIZE ? SPI_1_TX_BUFFER_SIZE : size;  /* Total frame length */
    /* Assert only SSEL0 */
    spi_1_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_1_xfer.TxCnt = 0;
    spi_1_xfer.RxCnt = 0;

    Chip_SPI_WriteFrames_Blocking(LPC_SPI1, &spi_1_xfer);

    return;
}

void spi_1_write_read(uint8_t *tx, uint16_t tx_size, uint8_t *rx, uint16_t rx_size)
{
    uint16_t i = 0;
    uint16_t j = 0;

    for(i = 1; i < tx_size && i < SPI_1_TX_BUFFER_SIZE; i++)
    {
        spi_1_tx_buffer[i] = tx[i];
    }

    spi_1_xfer.pTx = spi_1_tx_buffer;       /* Transmit Buffer */
    spi_1_xfer.pRx = spi_1_rx_buffer;       /* Receive Buffer */
    spi_1_xfer.DataSize = 8;                /* Data size in bits */
    spi_1_xfer.Length = tx_size + rx_size;  /* Total frame length */
    /* Assert only SSEL0 */
    spi_1_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_1_xfer.TxCnt = 0;
    spi_1_xfer.RxCnt = 0;

    Chip_SPI_RWFrames_Blocking(LPC_SPI1, &spi_1_xfer);

    for(i = 1; i < (rx_size + 1) && i < SPI_1_RX_BUFFER_SIZE; i++)
    {
        rx[i - 1] = spi_1_rx_buffer[j];
        if(i % 2)
        {
            rx[i - 1] = (uint8_t)((spi_1_rx_buffer[j] >> 8) & 0xFF);
        }
        else
        {
            rx[i - 1] = (uint8_t)(spi_1_rx_buffer[j] & 0xFF);
        }
    }

    return;
}

/**
 * ********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

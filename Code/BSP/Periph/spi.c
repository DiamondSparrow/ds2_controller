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
#include "debug.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define SPI_0_BITRATE       1000000 //!< SPI 0 bit rate in Hz.
#define SPI_0_BUFFER_SIZE   128     //!< SPI 0 transmit and receive buffers size in bytes.

#define SPI_1_BITRATE       1000000 //!< SPI 1 bit rate in Hz.
#define SPI_1_BUFFER_SIZE   128     //!< SPI 1 transmit and receive buffers size in bytes.

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
static uint16_t spi_0_tx_buffer[SPI_0_BUFFER_SIZE] = {0};
static uint16_t spi_0_rx_buffer[SPI_0_BUFFER_SIZE] = {0};
/** SPI-1 Transfer Setup */
static SPI_DATA_SETUP_T spi_1_xfer;
static uint16_t spi_1_tx_buffer[SPI_1_BUFFER_SIZE] = {0};
static uint16_t spi_1_rx_buffer[SPI_1_BUFFER_SIZE] = {0};

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
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 16, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
//  Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN));

    Chip_SWM_MovablePortPinAssign(SWM_SPI0_SCK_IO, 0, 0);       // SCK pin P0.27
    Chip_SWM_MovablePortPinAssign(SWM_SPI0_MOSI_IO, 0, 16);     // MOSI pin P0.28
    Chip_SWM_MovablePortPinAssign(SWM_SPI0_MISO_IO, 0, 10);     // MISO pin P0.12
//  Chip_SWM_MovablePortPinAssign(SWM_SPI0_SSELSN_0_IO, 0, 9);  // SELECT pin P0.29

    // Disable the clock to the Switch Matrix to save power .
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    // Initialize SPI Block.
    Chip_SPI_Init(LPC_SPI0);

    // Set Clock divider to maximum:
    spi_cfg.ClkDiv      = Chip_SPI_CalClkRateDivider(LPC_SPI0, SPI_0_BITRATE);
    // Enable Master Mode:
    spi_cfg.Mode        = SPI_MODE_MASTER;
    // Enable  clock mode 0:
    spi_cfg.ClockMode   = SPI_CLOCK_MODE0;
    // Transmit MSB first:
    spi_cfg.DataOrder   = SPI_DATA_MSB_FIRST;
    // Slave select polarity is active low:
    spi_cfg.SSELPol     = (SPI_CFG_SPOL0_LO | SPI_CFG_SPOL1_LO | SPI_CFG_SPOL2_LO | SPI_CFG_SPOL3_LO);
    // Set SPI configuration register.
    Chip_SPI_SetConfig(LPC_SPI0, &spi_cfg);

    // Set Delay register.
    spi_delay_cfg.PreDelay      = 2;
    spi_delay_cfg.PostDelay     = 2;
    spi_delay_cfg.FrameDelay    = 2;
    spi_delay_cfg.TransferDelay = 2;
    Chip_SPI_DelayConfig(LPC_SPI0, &spi_delay_cfg);

    // Enable SPI0.
    Chip_SPI_Enable(LPC_SPI0);
    
    NVIC_DisableIRQ(SPI0_IRQn);

    return;
}

#define SPI_0_DATA_SIZE     8
#define SPI_0_SSEL          (SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3)

uint16_t spi_0_read_buffer(uint8_t *buffer, uint32_t size)
{
    uint32_t rx_cnt = 0;

    /* Clear status */
    Chip_SPI_ClearStatus(LPC_SPI0, SPI_STAT_CLR_RXOV | SPI_STAT_CLR_TXUR | SPI_STAT_CLR_SSA | SPI_STAT_CLR_SSD | SPI_STAT_FORCE_EOT);
    /* Set control information */
    Chip_SPI_SetControlInfo(LPC_SPI0, SPI_0_DATA_SIZE, SPI_0_SSEL | SPI_TXCTL_EOF);

    while (rx_cnt < size)
    {
        /* Wait for TxReady */
        while (!(Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_TXRDY)) {}
        /* Send dummy data */
        if(rx_cnt == (size - 1))
        {
            Chip_SPI_SendLastFrame(LPC_SPI0, 0xFFFF, SPI_0_DATA_SIZE, SPI_TXDATCTL_SSEL_MASK);
        }
        else
        {
            Chip_SPI_SendMidFrame(LPC_SPI0, 0xFFFF);
        }
        /* Wait for receive data */
        while (!(Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_RXRDY)) {}
        /* Receive data */
        buffer[rx_cnt] = Chip_SPI_ReceiveFrame(LPC_SPI0);
        rx_cnt++;
    }

    /* Check overrun error */
    if (Chip_SPI_GetStatus(LPC_SPI0) & (SPI_STAT_RXOV | SPI_STAT_TXUR))
    {
        return 0;
    }

    return rx_cnt;
}

uint32_t spi_0_write_buffer(uint8_t *buffer, uint32_t size)
{
    uint32_t tx_cnt = 0;

    /* Clear status */
    Chip_SPI_ClearStatus(LPC_SPI0, SPI_STAT_CLR_RXOV | SPI_STAT_CLR_TXUR | SPI_STAT_CLR_SSA | SPI_STAT_CLR_SSD | SPI_STAT_FORCE_EOT);
    /* Set control information */
    Chip_SPI_SetControlInfo(LPC_SPI0, SPI_0_DATA_SIZE, SPI_0_SSEL | SPI_TXCTL_EOF | SPI_TXCTL_RXIGNORE);

    while (tx_cnt < size)
    {
        /* Wait for TxReady */
        while (!(Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_TXRDY)) { }
        /* Send data */
        if (tx_cnt == (size - 1))
        {
            Chip_SPI_SendLastFrame_RxIgnore(LPC_SPI0, (uint16_t)buffer[tx_cnt], SPI_0_DATA_SIZE, SPI_0_SSEL);
        }
        else
        {
            Chip_SPI_SendMidFrame(LPC_SPI0, (uint16_t)buffer[tx_cnt]);
        }
        tx_cnt++;
    }

    /* Make sure the last frame sent completely */
    while (!(Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_SSD)) {}
    Chip_SPI_ClearStatus(LPC_SPI0, SPI_STAT_CLR_SSD);

    /* Check overrun error */
    if (Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_TXUR)
    {
        return 0;
    }

    return tx_cnt;
}

uint32_t spi_0_write_read(uint8_t *tx, uint32_t tx_size, uint8_t *rx, uint32_t rx_size)
{
    uint32_t tx_cnt = 0;
    uint32_t rx_cnt = 0;
    uint32_t status = 0;
    uint32_t size = tx_size + rx_size;

    /* Clear status */
    Chip_SPI_ClearStatus(LPC_SPI0, SPI_STAT_CLR_RXOV | SPI_STAT_CLR_TXUR | SPI_STAT_CLR_SSA | SPI_STAT_CLR_SSD | SPI_STAT_FORCE_EOT);
    /* Set control information. */
    Chip_SPI_SetControlInfo(LPC_SPI0, SPI_0_DATA_SIZE, SPI_0_SSEL| SPI_TXCTL_EOF);

    while ((tx_cnt < size) || (rx_cnt < size))
    {
        status = Chip_SPI_GetStatus(LPC_SPI0);
        /* In case of TxReady */
        if ((status & SPI_STAT_TXRDY) && (tx_cnt < size))
        {
            if(tx_cnt == (size - 1))
            {
                Chip_SPI_SendLastFrame(LPC_SPI0, tx[tx_cnt], SPI_0_DATA_SIZE, SPI_0_SSEL);
            }
            else
            {
                Chip_SPI_SendMidFrame(LPC_SPI0, tx[tx_cnt]);
            }
            tx_cnt++;
        }
        /* In case of Rx ready */
        if ((status & SPI_STAT_RXRDY) && (rx_cnt < size))
        {
            rx[rx_cnt] = Chip_SPI_ReceiveFrame(LPC_SPI0);
            rx_cnt++;
        }
    }

    /* Check error */
    if (Chip_SPI_GetStatus(LPC_SPI0) & (SPI_STAT_RXOV | SPI_STAT_TXUR))
    {
        return 0;
    }

    return tx_cnt;
}

void spi_1_init(void)
{
    SPI_CFG_T spi_cfg;
    SPI_DELAY_CONFIG_T spi_delay_cfg;

    // Enable the clock to the Switch Matrix.
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    /*
     * Initialize SPI1 pins connect
     * SCK0: PINASSIGN3[15:8]: Select P0.27
     * MOSI0: PINASSIGN3[23:16]: Select P0.28
     * MISO0: PINASSIGN3[31:24] : Select P0.12
     * SSEL0: PINASSIGN4[7:0]: Select P0.29
     */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 27, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 28, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
//  Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 29,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN));

    Chip_SWM_MovablePinAssign(SWM_SPI1_SCK_IO, 27);      // SCK pin P0.27
    Chip_SWM_MovablePinAssign(SWM_SPI1_MOSI_IO, 28);     // MOSI pin P0.28
    Chip_SWM_MovablePinAssign(SWM_SPI1_MISO_IO, 12);     // MISO pin P0.12
//  Chip_SWM_MovablePinAssign(SWM_SPI0_SSELSN_0_IO, 29); // SELECT pin P0.29

    // Disable the clock to the Switch Matrix to save power .
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    // Initialize SPI Block.
    Chip_SPI_Init(LPC_SPI1);

    // Set Clock divider to maximum:
    spi_cfg.ClkDiv      = Chip_SPI_CalClkRateDivider(LPC_SPI1, SPI_1_BITRATE);
    // Enable Master Mode:
    spi_cfg.Mode        = SPI_MODE_MASTER;
    // Enable clock mode:
    spi_cfg.ClockMode   = SPI_CLOCK_MODE0;
    // Transmit MSB first:
    spi_cfg.DataOrder   = SPI_DATA_MSB_FIRST;
    // Slave select polarity is active low:
    spi_cfg.SSELPol     = (SPI_CFG_SPOL0_LO | SPI_CFG_SPOL1_LO | SPI_CFG_SPOL2_LO | SPI_CFG_SPOL3_LO);
    // Set SPI configuration:
    Chip_SPI_SetConfig(LPC_SPI1, &spi_cfg);

    // Set Delay register.
    spi_delay_cfg.PreDelay      = 2;
    spi_delay_cfg.PostDelay     = 2;
    spi_delay_cfg.FrameDelay    = 2;
    spi_delay_cfg.TransferDelay = 2;
    Chip_SPI_DelayConfig(LPC_SPI1, &spi_delay_cfg);

    // Enable SPI1.
    Chip_SPI_Enable(LPC_SPI1);

    return;
}

void spi_1_read_buffer(uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;

    if(size > SPI_1_BUFFER_SIZE)
    {
        return;
    }

    spi_1_xfer.pTx = NULL;              /* Transmit Buffer */
    spi_1_xfer.pRx = spi_1_rx_buffer;   /* Receive Buffer */
    spi_1_xfer.DataSize = 8;            /* Data size in bits */
    spi_1_xfer.Length = size;           /* Total frame length */
    /* Assert only SSEL0 */
    spi_1_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_1_xfer.TxCnt = 0;
    spi_1_xfer.RxCnt = 0;

    Chip_SPI_ReadFrames_Blocking(LPC_SPI1, (SPI_DATA_SETUP_T *)&spi_1_xfer);

    for(i = 0; i < size; i++)
    {
        buffer[i] = (uint8_t)(spi_1_rx_buffer[i] & 0xFF);
    }

    return;
}

void spi_1_write_buffer(uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;

    if(size > SPI_1_BUFFER_SIZE)
    {
        return;
    }

    for(i = 0; i < size; i++)
    {
        spi_1_tx_buffer[i] = buffer[i];
    }

    spi_1_xfer.pTx = spi_1_tx_buffer;   /* Transmit Buffer */
    spi_1_xfer.pRx = NULL;              /* Receive Buffer */
    spi_1_xfer.DataSize = 8;            /* Data size in bits */
    spi_1_xfer.Length = size            /* Total frame length */
    /* Assert only SSEL0 */
    spi_1_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_1_xfer.TxCnt = 0;
    spi_1_xfer.RxCnt = 0;

    Chip_SPI_WriteFrames_Blocking(LPC_SPI1, (SPI_DATA_SETUP_T *)&spi_1_xfer);

    return;
}

void spi_1_write_read(uint8_t *tx, uint16_t tx_size, uint8_t *rx, uint16_t rx_size)
{
    uint16_t i = 0;

    if((tx_size + rx_size) > SPI_1_BUFFER_SIZE)
    {
        return;
    }

    for(i = 0; i < (tx_size + rx_size); i++)
    {
        if(i < tx_size)
        {
            spi_1_tx_buffer[i] = tx[i];
        }
        else
        {
            spi_1_tx_buffer[i] = 0xFFFF;
        }
    }

    spi_1_xfer.pTx = spi_1_tx_buffer;       /* Transmit Buffer */
    spi_1_xfer.pRx = spi_1_rx_buffer;       /* Receive Buffer */
    spi_1_xfer.DataSize = 8;                /* Data size in bits */
    spi_1_xfer.Length = tx_size + rx_size;  /* Total frame length */
    /* Assert only SSEL0 */
    spi_1_xfer.ssel = SPI_TXCTL_ASSERT_SSEL0 | SPI_TXCTL_DEASSERT_SSEL1 | SPI_TXCTL_DEASSERT_SSEL2 | SPI_TXCTL_DEASSERT_SSEL3;
    spi_1_xfer.TxCnt = 0;
    spi_1_xfer.RxCnt = 0;

    Chip_SPI_RWFrames_Blocking(LPC_SPI1, (SPI_DATA_SETUP_T *)&spi_1_xfer);

    for(i = 0; i < (rx_size + tx_size); i++)
    {
        rx[i] = (uint8_t)(spi_1_rx_buffer[i] & 0xFF);
    }

    return;
}

/**
 * ********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

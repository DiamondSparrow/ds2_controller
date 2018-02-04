/**
 **********************************************************************************************************************
 * @file         nrf24l01.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-09-07
 * @brief       Ultra low power 2.4GHz RF NRF24L01+ C source file.
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

#include "nrf24l01.h"
#include "periph/spi.h"
#include "periph/gpio.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** NRF24L01+ registers: */
#define NRF24L01_REG_CONFIG         0x00    //!< Configuration register.
#define NRF24L01_REG_EN_AA          0x01    //!< Enable 'Auto Acknowledgment' function.
#define NRF24L01_REG_EN_RXADDR      0x02    //!< Enabled RX addresses.
#define NRF24L01_REG_SETUP_AW       0x03    //!< Setup of Address Widths (common for all data pipes).
#define NRF24L01_REG_SETUP_RETR     0x04    //!< Setup of automatic retransmission.
#define NRF24L01_REG_RF_CH          0x05    //!< RF Channel.
#define NRF24L01_REG_RF_SETUP       0x06    //!< RF setup register.
#define NRF24L01_REG_STATUS         0x07    //!< Status register.
#define NRF24L01_REG_OBSERVE_TX     0x08    //!< Transmit observe register.
#define NRF24L01_REG_RPD            0x09    //!< Received power detector.
#define NRF24L01_REG_RX_ADDR_P0     0x0A    //!< Receive address data pipe 0. 5 Bytes maximum length.
#define NRF24L01_REG_RX_ADDR_P1     0x0B    //!< Receive address data pipe 1. 5 Bytes maximum length.
#define NRF24L01_REG_RX_ADDR_P2     0x0C    //!< Receive address data pipe 2. Only LSB.
#define NRF24L01_REG_RX_ADDR_P3     0x0D    //!< Receive address data pipe 3. Only LSB.
#define NRF24L01_REG_RX_ADDR_P4     0x0E    //!< Receive address data pipe 4. Only LSB.
#define NRF24L01_REG_RX_ADDR_P5     0x0F    //!< Receive address data pipe 5. Only LSB.
#define NRF24L01_REG_TX_ADDR        0x10    //!< Transmit address. Used for a PTX device only
#define NRF24L01_REG_RX_PW_P0       0x11    //!< Number of bytes in RX payload in data pipe 0 (1 to 32 bytes).
#define NRF24L01_REG_RX_PW_P1       0x12    //!< Number of bytes in RX payload in data pipe 1 (1 to 32 bytes).
#define NRF24L01_REG_RX_PW_P2       0x13    //!< Number of bytes in RX payload in data pipe 2 (1 to 32 bytes).
#define NRF24L01_REG_RX_PW_P3       0x14    //!< Number of bytes in RX payload in data pipe 3 (1 to 32 bytes).
#define NRF24L01_REG_RX_PW_P4       0x15    //!< Number of bytes in RX payload in data pipe 4 (1 to 32 bytes).
#define NRF24L01_REG_RX_PW_P5       0x16    //!< Number of bytes in RX payload in data pipe 5 (1 to 32 bytes).
#define NRF24L01_REG_FIFO_STATUS    0x17    //!< FIFO status register.
#define NRF24L01_REG_DYNPD          0x1C    //!< Enable dynamic payload length.
#define NRF24L01_REG_FEATURE        0x1D    //!< Feature register.

/** Registers default values */
#define NRF24L01_REG_DEFAULT_VAL_CONFIG         0x08
#define NRF24L01_REG_DEFAULT_VAL_EN_AA          0x3F
#define NRF24L01_REG_DEFAULT_VAL_EN_RXADDR      0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_AW       0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_RETR     0x03
#define NRF24L01_REG_DEFAULT_VAL_RF_CH          0x02
#define NRF24L01_REG_DEFAULT_VAL_RF_SETUP       0x0E
#define NRF24L01_REG_DEFAULT_VAL_STATUS         0x0E
#define NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX     0x00
#define NRF24L01_REG_DEFAULT_VAL_RPD            0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2     0xC3
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3     0xC4
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4     0xC5
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5     0xC6
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4      0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P0       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P1       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P2       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P3       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P4       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P5       0x00
#define NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS    0x11
#define NRF24L01_REG_DEFAULT_VAL_DYNPD          0x00
#define NRF24L01_REG_DEFAULT_VAL_FEATURE        0x00

/** Configuration register. */
#define NRF24L01_MASK_RX_DR     6
#define NRF24L01_MASK_TX_DS     5
#define NRF24L01_MASK_MAX_RT    4
#define NRF24L01_EN_CRC         3
#define NRF24L01_CRCO           2
#define NRF24L01_PWR_UP         1
#define NRF24L01_PRIM_RX        0

/** RF setup register. */
#define NRF24L01_PLL_LOCK       4
#define NRF24L01_RF_DR_LOW      5
#define NRF24L01_RF_DR_HIGH     3
#define NRF24L01_RF_DR          3
#define NRF24L01_RF_PWR         1 //2 bits

/** General status register. */
#define NRF24L01_RX_DR          6   //!< Data Ready RX FIFO interrupt. Asserted when new data arrives RX FIFO.
#define NRF24L01_TX_DS          5   //!< Data Sent TX FIFO interrupt. Asserted when packet transmitted on TX.
#define NRF24L01_MAX_RT         4   //!< Maximum number of TX retransmits interrupt Write 1 to clear bit.
#define NRF24L01_RX_P_NO        1   //!< Data pipe number for the payload available for reading from RX_FIFO.
#define NRF24L01_TX_FULL        0   //!< TX FIFO full flag.

/** FIFO status*/
#define NRF24L01_TX_REUSE       6
#define NRF24L01_FIFO_FULL      5
#define NRF24L01_TX_EMPTY       4
#define NRF24L01_RX_FULL        1
#define NRF24L01_RX_EMPTY       0

/** Configuration */
#define NRF24L01_CONFIG                     ((1 << NRF24L01_EN_CRC) | (0 << NRF24L01_CRCO))

/** Instruction Mnemonics */
#define NRF24L01_REGISTER_MASK              0x1F

#define NRF24L01_READ_REGISTER_MASK(reg)    (0x00 | (NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_WRITE_REGISTER_MASK(reg)   (0x20 | (NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_R_RX_PAYLOAD_MASK          0x61
#define NRF24L01_W_TX_PAYLOAD_MASK          0xA0
#define NRF24L01_FLUSH_TX_MASK              0xE1
#define NRF24L01_FLUSH_RX_MASK              0xE2
#define NRF24L01_REUSE_TX_PL_MASK           0xE3
#define NRF24L01_ACTIVATE_MASK              0x50
#define NRF24L01_R_RX_PL_WID_MASK           0x60
#define NRF24L01_NOP_MASK                   0xFF

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
/** Pins configuration */
#define NRF24L01_CE_LOW             gpio_output_low(GPIO_DISPLAY_DC)
#define NRF24L01_CE_HIGH            gpio_output_high(GPIO_DISPLAY_DC)
#define NRF24L01_CSN_LOW            gpio_output_low(GPIO_DISPLAY_SELECT)
#define NRF24L01_CSN_HIGH           gpio_output_high(GPIO_DISPLAY_SELECT)

/** Clear interrupt flags */
#define NRF24L01_CLEAR_INTERRUPTS   do { nrf24l01_write_register(NRF24L01_REG_STATUS, 0x70); } while (0)

/** Gets interrupt status from device */
#define NRF24L01_GET_INTERRUPTS     nrf24l01_get_status()

/** Flush TX FIFO. */
#define NRF24L01_FLUSH_TX           do { NRF24L01_CSN_LOW; spi_0_write_buffer((uint8_t []){NRF24L01_FLUSH_TX_MASK}, 1); NRF24L01_CSN_HIGH; } while (0)
/** Flush RX FIFO. */
#define NRF24L01_FLUSH_RX           do { NRF24L01_CSN_LOW; spi_0_write_buffer((uint8_t []){NRF24L01_FLUSH_RX_MASK}, 1); NRF24L01_CSN_HIGH; } while (0)


/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
/**
 * @brief   NRF24L01+ configuration data structure.
 */
typedef struct
{
    uint8_t payload_size;           //!< Payload size in bytes.
    uint8_t channel;                //!< Selected channel.
    nrf24l01_tx_power_t tx_power;   //!< Output power, see @ref nrf24l01_tx_power_t.
    nrf24l01_data_rate_t data_rate; //!< Data rate, see @ref nrf24l01_data_rate_t.
} nrf24l01_cfg_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
nrf24l01_cfg_t nrf24l01_config;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
void nrf24l01_init_io(void);
uint8_t nrf24l01_read_register(uint8_t reg);
void nrf24l01_read_register_multi(uint8_t reg, uint8_t *data, uint8_t count);
uint8_t nrf24l01_read_bit(uint8_t reg, uint8_t bit);
void nrf24l01_write_register(uint8_t reg, uint8_t value);
void nrf24l01_write_register_multi(uint8_t reg, uint8_t *data, uint8_t count);
void nrf24l01_write_bit(uint8_t reg, uint8_t bit, uint8_t value);
void nrf24l01_software_reset(void);
uint8_t nrf24l01_rx_fifo_empty(void);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
uint8_t nrf24l01_init(uint8_t channel, uint8_t payload_size)
{
    // Initialize IO pins.
    nrf24l01_init_io();

    // Max payload is 32 bytes.
    if (payload_size > 32)
    {
        payload_size = 32;
    }

    // Fill structure.
    nrf24l01_config.channel         = !channel; // Set channel to some different value for nrf24l01_set_channel() function
    nrf24l01_config.payload_size    = payload_size;
    nrf24l01_config.tx_power        = NRF24L01_TX_POWER_0DBM;
    nrf24l01_config.data_rate       = NRF24L01_DATA_RATE_1M;
    
    /* Reset nRF24L01+ to power on registers values */
    nrf24l01_software_reset();

    /* Channel select */
    nrf24l01_set_channel(channel);

    /* Set pipeline to max possible 32 bytes */
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P0, nrf24l01_config.payload_size); // Auto-ACK pipe.
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P1, nrf24l01_config.payload_size); // Data payload pipe.
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P2, nrf24l01_config.payload_size);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P3, nrf24l01_config.payload_size);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P4, nrf24l01_config.payload_size);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P5, nrf24l01_config.payload_size);

    // Set RF settings (data rate, output power).
    nrf24l01_set_rf(nrf24l01_config.data_rate, nrf24l01_config.tx_power);

    // Config register.
    nrf24l01_write_register(NRF24L01_REG_CONFIG, NRF24L01_CONFIG);

    // Enable auto-acknowledgment for all pipes.
    nrf24l01_write_register(NRF24L01_REG_EN_AA, 0x3F);

    // Enable RX addresses.
    nrf24l01_write_register(NRF24L01_REG_EN_RXADDR, 0x3F);

    // Auto retransmit delay: 1000 (4x250) us and Up to 15 retransmit trials.
    nrf24l01_write_register(NRF24L01_REG_SETUP_RETR, 0x4F);

    // Dynamic length configurations: No dynamic length.
    nrf24l01_write_register(NRF24L01_REG_DYNPD, 0x00);

    // Clear FIFOs.
    NRF24L01_FLUSH_TX;
    NRF24L01_FLUSH_RX;

    // Clear interrupts.
    NRF24L01_CLEAR_INTERRUPTS;

    // Go to RX mode.
    nrf24l01_power_up_rx();

    // Return OK.
    return 1;
}

void nrf24l01_set_my_address(uint8_t *addr)
{
    NRF24L01_CE_LOW;
    nrf24l01_write_register_multi(NRF24L01_REG_RX_ADDR_P1, addr, 5);
    NRF24L01_CE_HIGH;

    return;
}

void nrf24l01_set_tx_address(uint8_t *addr)
{
    nrf24l01_write_register_multi(NRF24L01_REG_RX_ADDR_P0, addr, 5);
    nrf24l01_write_register_multi(NRF24L01_REG_TX_ADDR, addr, 5);

    return;
}

uint8_t nrf24l01_get_retransmissions_count(void)
{
    /* Low 4 bits */
    return nrf24l01_read_register(NRF24L01_REG_OBSERVE_TX) & 0x0F;
}

void nrf24l01_power_up_tx(void)
{
    NRF24L01_CLEAR_INTERRUPTS;
    nrf24l01_write_register(NRF24L01_REG_CONFIG, (NRF24L01_CONFIG | (0 << NRF24L01_PRIM_RX) | (1 << NRF24L01_PWR_UP)));

    return;
}

void nrf24l01_power_up_rx(void)
{
    // Disable RX/TX mode.
    NRF24L01_CE_LOW;
    // Clear RX buffer.
    NRF24L01_FLUSH_RX;
    // Clear interrupts.
    NRF24L01_CLEAR_INTERRUPTS;
    // Setup RX mode.
    nrf24l01_write_register(NRF24L01_REG_CONFIG, (NRF24L01_CONFIG | (1 << NRF24L01_PWR_UP) | (1 << NRF24L01_PRIM_RX)));
    // Start listening.
    NRF24L01_CE_HIGH;

    return;
}

void nrf24l01_power_down(void)
{
    NRF24L01_CE_LOW;
    nrf24l01_write_bit(NRF24L01_REG_CONFIG, NRF24L01_PWR_UP, 0);

    return;
}

nrf24l01_tx_status_t nrf24l01_get_tx_status(void)
{
    uint8_t status = nrf24l01_get_status();

    //DEBUG("Status: %02X", status);
    
    if((status & (1 << NRF24L01_TX_DS)))
    {
        // Successfully sent.
        return NRF24L01_TX_STATUS_OK;
    }
    else if((status & (1 << NRF24L01_MAX_RT)))
    {
        // Message lost.
        return NRF24L01_TX_STATUS_LOST;
    }

    // Still sending.
    return NRF24L01_TX_STATUS_SENDING;
}

void nrf24l01_transmit(uint8_t *data)
{
    uint8_t count = nrf24l01_config.payload_size;
    uint8_t buffer[33] = {0};

    buffer[0] = NRF24L01_W_TX_PAYLOAD_MASK;
    memcpy(&buffer[1], data, count);
    
    /* Chip enable put to low, disable it */
    NRF24L01_CE_LOW;

    /* Go to power up tx mode */
    nrf24l01_power_up_tx();

    /* Clear TX FIFO from NRF24L01+ */
    NRF24L01_FLUSH_TX;

    /* Send payload to nRF24L01+ */
    NRF24L01_CSN_LOW;
    /* Send write payload command */
    //spi_0_write_buffer((uint8_t []){NRF24L01_W_TX_PAYLOAD_MASK}, 1);
    /* Fill payload with data*/
    spi_0_write_buffer(buffer, count + 1);
    /* Disable SPI */
    NRF24L01_CSN_HIGH;

    /* Send data! */
    NRF24L01_CE_HIGH;

    return;
}

uint8_t nrf24l01_data_ready(void)
{
    uint8_t status = nrf24l01_get_status();

    if((status & (1 << NRF24L01_RX_DR)))
    {
        return 1;
    }

    return !nrf24l01_rx_fifo_empty();
}

void nrf24l01_get_data(uint8_t *data)
{
    // Pull down chip select.
    NRF24L01_CSN_LOW;

    // Send read payload command.
    spi_0_write_buffer((uint8_t []){NRF24L01_R_RX_PAYLOAD_MASK}, 1);
    // Read payload.
    spi_0_read_buffer(data, nrf24l01_config.payload_size);
    // Pull up chip select.
    NRF24L01_CSN_HIGH;

    // Reset status register, clear RX_DR interrupt flag .
    nrf24l01_write_register(NRF24L01_REG_STATUS, (1 << NRF24L01_RX_DR));

    return;
}

void nrf24l01_set_channel(uint8_t channel)
{
    if(channel <= 125 && channel != nrf24l01_config.channel)
    {
        // Store new channel setting .
        nrf24l01_config.channel = channel;
        // Write channel.
        nrf24l01_write_register(NRF24L01_REG_RF_CH, channel);
    }

    return;
}

void nrf24l01_set_rf(nrf24l01_data_rate_t data_rate, nrf24l01_tx_power_t tx_power)
{
    uint8_t tmp = 0;

    nrf24l01_config.data_rate = data_rate;
    nrf24l01_config.tx_power = tx_power;

    if(data_rate == NRF24L01_DATA_RATE_2M)
    {
        tmp |= 1 << NRF24L01_RF_DR_HIGH;
    }
    else if(data_rate == NRF24L01_DATA_RATE_250K)
    {
        tmp |= 1 << NRF24L01_RF_DR_LOW;
    }
    /* If 1Mbps, all bits set to 0 */

    if(tx_power == NRF24L01_TX_POWER_0DBM)
    {
        tmp |= 3 << NRF24L01_RF_PWR;
    }
    else if(tx_power == NRF24L01_TX_POWER_M6DBM)
    {
        tmp |= 2 << NRF24L01_RF_PWR;
    }
    else if(tx_power == NRF24L01_TX_POWER_M12DBM)
    {
        tmp |= 1 << NRF24L01_RF_PWR;
    }

    nrf24l01_write_register(NRF24L01_REG_RF_SETUP, tmp);

    return;
}

uint8_t nrf24l01_get_status(void)
{
    uint8_t status = 0;

    NRF24L01_CSN_LOW;
    /* First received byte is always status register */
    spi_0_read_buffer((uint8_t *)&status, 1);
    /* Pull up chip select */
    NRF24L01_CSN_HIGH;

    return status;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
void nrf24l01_init_io(void)
{
    // CSN high = disable SPI.
    NRF24L01_CSN_HIGH;

    // CE low = disable TX/RX.
    NRF24L01_CE_LOW;

    return;
}

uint8_t nrf24l01_read_register(uint8_t reg)
{
    uint8_t tx[1] = {0};
    uint8_t rx[3] = {0, 0};
    tx[0] = NRF24L01_READ_REGISTER_MASK(reg);
    NRF24L01_CSN_LOW;
    spi_0_write_read((uint8_t *)tx, 1, (uint8_t *)rx, 1);
    NRF24L01_CSN_HIGH;

    return rx[1];
}

void nrf24l01_read_register_multi(uint8_t reg, uint8_t *data, uint8_t count)
{
    NRF24L01_CSN_LOW;
    spi_0_write_read((uint8_t []){NRF24L01_READ_REGISTER_MASK(reg)}, 1, data, count);
    NRF24L01_CSN_HIGH;

    return;
}

uint8_t nrf24l01_read_bit(uint8_t reg, uint8_t bit)
{
    uint8_t tmp = 0;

    tmp = nrf24l01_read_register(reg);

    if(tmp & bit)
    {
        return 1;
    }

    return 0;

}

void nrf24l01_write_register(uint8_t reg, uint8_t value)
{
    uint8_t buff[2] = {0};

    buff[0] = NRF24L01_WRITE_REGISTER_MASK(reg);
    buff[1] = value;

    NRF24L01_CSN_LOW;
    spi_0_write_buffer((uint8_t *)buff, 2);
    NRF24L01_CSN_HIGH;

    return;
}

void nrf24l01_write_register_multi(uint8_t reg, uint8_t *data, uint8_t count)
{
    NRF24L01_CSN_LOW;
    spi_0_write_buffer((uint8_t []){NRF24L01_WRITE_REGISTER_MASK(reg)}, 1);
    spi_0_write_buffer((uint8_t *)data, count);
    NRF24L01_CSN_HIGH;

    return;
}

void nrf24l01_write_bit(uint8_t reg, uint8_t bit, uint8_t value)
{
    uint8_t tmp = 0;

    /* Read register */
    tmp = nrf24l01_read_register(reg);

    // Make operation.
    if(value)
    {
        tmp |= 1 << bit;
    }
    else
    {
        tmp &= ~(1 << bit);
    }

    // Write back.
    nrf24l01_write_register(reg, tmp);

    return;
}

void nrf24l01_software_reset(void)
{
    uint8_t data[5] = {0};

    nrf24l01_write_register(NRF24L01_REG_CONFIG,        NRF24L01_REG_DEFAULT_VAL_CONFIG);
    nrf24l01_write_register(NRF24L01_REG_EN_AA,         NRF24L01_REG_DEFAULT_VAL_EN_AA);
    nrf24l01_write_register(NRF24L01_REG_EN_RXADDR,     NRF24L01_REG_DEFAULT_VAL_EN_RXADDR);
    nrf24l01_write_register(NRF24L01_REG_SETUP_AW,      NRF24L01_REG_DEFAULT_VAL_SETUP_AW);
    nrf24l01_write_register(NRF24L01_REG_SETUP_RETR,    NRF24L01_REG_DEFAULT_VAL_SETUP_RETR);
    nrf24l01_write_register(NRF24L01_REG_RF_CH,         NRF24L01_REG_DEFAULT_VAL_RF_CH);
    nrf24l01_write_register(NRF24L01_REG_RF_SETUP,      NRF24L01_REG_DEFAULT_VAL_RF_SETUP);
    nrf24l01_write_register(NRF24L01_REG_STATUS,        NRF24L01_REG_DEFAULT_VAL_STATUS);
    nrf24l01_write_register(NRF24L01_REG_OBSERVE_TX,    NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX);
    nrf24l01_write_register(NRF24L01_REG_RPD,           NRF24L01_REG_DEFAULT_VAL_RPD);

    //P0
    data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4;
    nrf24l01_write_register_multi(NRF24L01_REG_RX_ADDR_P0, data, 5);

    //P1
    data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4;
    nrf24l01_write_register_multi(NRF24L01_REG_RX_ADDR_P1, data, 5);

    nrf24l01_write_register(NRF24L01_REG_RX_ADDR_P2,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2);
    nrf24l01_write_register(NRF24L01_REG_RX_ADDR_P3,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3);
    nrf24l01_write_register(NRF24L01_REG_RX_ADDR_P4,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4);
    nrf24l01_write_register(NRF24L01_REG_RX_ADDR_P5,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5);

    //TX
    data[0] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4;
    nrf24l01_write_register_multi(NRF24L01_REG_TX_ADDR, data, 5);

    nrf24l01_write_register(NRF24L01_REG_RX_PW_P0,      NRF24L01_REG_DEFAULT_VAL_RX_PW_P0);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P1,      NRF24L01_REG_DEFAULT_VAL_RX_PW_P1);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P2,      NRF24L01_REG_DEFAULT_VAL_RX_PW_P2);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P3,      NRF24L01_REG_DEFAULT_VAL_RX_PW_P3);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P4,      NRF24L01_REG_DEFAULT_VAL_RX_PW_P4);
    nrf24l01_write_register(NRF24L01_REG_RX_PW_P5,      NRF24L01_REG_DEFAULT_VAL_RX_PW_P5);
    nrf24l01_write_register(NRF24L01_REG_FIFO_STATUS,   NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS);
    nrf24l01_write_register(NRF24L01_REG_DYNPD,         NRF24L01_REG_DEFAULT_VAL_DYNPD);
    nrf24l01_write_register(NRF24L01_REG_FEATURE,       NRF24L01_REG_DEFAULT_VAL_FEATURE);

    return;
}

uint8_t nrf24l01_rx_fifo_empty(void)
{
    uint8_t reg = nrf24l01_read_register(NRF24L01_REG_FIFO_STATUS);

    return ((reg & (1 << NRF24L01_RX_EMPTY)) ? 1 : 0);
}


/**
 **********************************************************************************************************************
 * @file        nrf24l01.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-09-07
 * @brief       Ultra low power 2.4GHz RF NRF24L01+ C header file.
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

#ifndef NRF24L01_H_
#define NRF24L01_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
/** Interrupt masks. */
#define NRF24L01_IRQ_DATA_READY 0x40    //!< Data ready for receive.
#define NRF24L01_IRQ_TX_OK      0x20    //!< Transmission went OK.
#define NRF24L01_IRQ_MAX_RT     0x10    //!< Max retransmissions reached, last transmission failed.

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief  Transmission status enumeration.
 */
typedef enum
{
    NRF24L01_TX_STATUS_LOST     = 0x00, //!< Message is lost, reached maximum number of retransmissions.
    NRF24L01_TX_STATUS_OK       = 0x01, //!< Message sent successfully.
    NRF24L01_TX_STATUS_SENDING  = 0xFF, //!< Message is still sending.
} nrf24l01_tx_status_t;

/**
 * @brief  Data rate enumeration.
 */
typedef enum
{
    NRF24L01_DATA_RATE_2M,      //!< Data rate set to 2 Mbps.
    NRF24L01_DATA_RATE_1M,      //!< Data rate set to 1 Mbps.
    NRF24L01_DATA_RATE_250K,    //!< Data rate set to 250 kbps.
} nrf24l01_data_rate_t;

/**
 * @brief  Output power enumeration.
 */
typedef enum
{
    NRF24L01_TX_POWER_M18DBM,   //!< Output power set to -18 dBm.
    NRF24L01_TX_POWER_M12DBM,   //!< Output power set to -12 dBm.
    NRF24L01_TX_POWER_M6DBM,    //!< Output power set to -6 dBm.
    NRF24L01_TX_POWER_0DBM,     //!< Output power set to 0 dBm.
} nrf24l01_tx_power_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/

/**
 * @brief   Initializes NRF24L01+ module.
 *
 * @param   channel         channel you will use for communication, from 0 to 125 eg. working frequency from
 *                          2.4 to 2.525 GHz
 * @param   payload_size    maximum data to be sent in one packet from one NRF to another.
 *
 * @note    Maximal payload size is 32 bytes.
 *
 * @retval  State of initialization.
 * @retval  0   failed.
 * @retval  1   success.
 */
uint8_t nrf24l01_init(uint8_t channel, uint8_t payload_size);

/**
 * @brief   Sets own address. This is used for settings own id when communication with other modules.
 *
 * @note    "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *          if you want to get successful communication.
 *
 * @param   addr    Pointer to 5-bytes length array with address.
 */
void nrf24l01_set_my_address(uint8_t *addr);

/**
 * @brief   Sets address you will communicate with.
 *
 * @note    "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *          if you want to get successful communication.
 *
 * @param   addr    Pointer to 5-bytes length array with address.
 */
void nrf24l01_set_tx_address(uint8_t *addr);

/**
 * @brief   Gets number of retransmissions needed in last transmission.
 *
 * @retval  Number of retransmissions, between 0 and 15.
 */
uint8_t nrf24l01_get_retransmissions_count(void);

/**
 * @brief   Sets NRF24L01+ to TX mode
 *
 * @note    In this mode NRF24L01+ is able to send data to another NRF module.
 */
void nrf24l01_power_up_tx(void);

/**
 * @brief   Sets NRF24L01+ to RX mode
 *
 * @note    In this mode NRF24L01+ is able to receive data from another NRF module.
 *          This is default mode and should be used all the time, except when sending data.
 */
void nrf24l01_power_up_rx(void);

/**
 * @brief   Sets NRF24L01+ to power down mode
 *
 * @note    In power down mode, you are not able to transmit/receive data.
 *          You can wake up device using @ref nrf24l01_power_up_tx() or @ref nrf24l01_power_up_rx() functions.
 */
void nrf24l01_power_down(void);

/**
 * @brief   Gets transmissions status.
 *
 * @retval  Transmission status. See @ref nrf24l01_tx_status_t.
 */
nrf24l01_tx_status_t nrf24l01_get_tx_status(void);

/**
 * @brief   Transmits data with NRF24L01+ to another NRF module
 *
 * @param   data    Pointer to 8-bit array with data.
 *
 * @note    Maximum length of array can be the same as "payload_size" parameter on initialization.
 */
void nrf24l01_transmit(uint8_t *data);

/**
 * @brief   Checks if data is ready to be read from NRF24L01+.
 *
 * @return  Data ready status.
 * @retval  0   No data available for receive in bufferReturns.
 * @retval  1   Data is ready to be collected.
 */
uint8_t nrf24l01_data_ready(void);

/**
 * @brief   Gets data from NRF24L01+.
 *
 * @param   data    Pointer to 8-bits array where data from NRF24L01+ will be saved.
 */
void nrf24l01_get_data(uint8_t *data);

/**
 * @brief   Sets working channel.
 *
 * @note    Channel value is just an offset in units MHz from 2.4GHz
 *          For example, if you select channel 65, then operation frequency will be set to 2.465GHz.
 *
 * @param   channel RF channel where device will operate.
 */
void nrf24l01_set_channel(uint8_t channel);

/**
 * @brief   Sets RF parameters for NRF24L01+
 *
 * @param   data_rate   Data rate selection for NRF module. See @ref nrf24l01_data_rate_t
 * @param   tx_power    Output power selection for NRF module. See @ref nrf24l01_tx_power_t
 */
void nrf24l01_set_rf(nrf24l01_data_rate_t data_rate, nrf24l01_tx_power_t tx_power);

/**
 * @brief  Gets NRF24L01+ status register value.
 *
 * @retval Status register from NRF24L01+.
 */
uint8_t nrf24l01_get_status(void);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01_H_ */

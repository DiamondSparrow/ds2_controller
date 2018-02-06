/**
 **********************************************************************************************************************
 * @file         radio.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2018-02-06
 * @brief        Radio C source file.
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
#include <stdbool.h>
#include <string.h>

#include "radio/radio.h"
#include "radio/nrf24l01.h"

#include "cmsis_os2.h"
#include "debug.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Sensors thread attributes. */
const osThreadAttr_t radio_thread_attr =
{
    .name = "RADIO",
    .stack_size = 1024,
    .priority = osPriorityNormal,
};

const uint8_t radio_my_address[NRF24L01_ADDRESS_SIZE] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
const uint8_t radio_peer_address[NRF24L01_ADDRESS_SIZE] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define RADIO_CHANNEL       1
#define RADIO_PAYLAOD_SIZE  32

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Radio thread ID. */
osThreadId_t radio_thread_id;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool radio_init(void)
{
    // Create sensors thread.
    if((radio_thread_id = osThreadNew(&radio_thread, NULL, &radio_thread_attr)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}

void radio_thread(void *arguments)
{
    uint8_t data[RADIO_PAYLAOD_SIZE] = {0};
    uint8_t count = 0;
    nrf24l01_tx_status_t status = NRF24L01_TX_STATUS_LOST;

    nrf24l01_init(RADIO_CHANNEL, RADIO_PAYLAOD_SIZE);
    nrf24l01_set_my_address((uint8_t *)radio_my_address);
    nrf24l01_set_tx_address((uint8_t *)radio_peer_address);

    while(1)
    {
        if(nrf24l01_data_ready())
        {
            nrf24l01_get_data(data);
            DEBUG_RADIO("Received data:");
            debug_send_hex_os(data, RADIO_PAYLAOD_SIZE);
            // TODO: command parser
            // TODO: form response
            data[0]++;
            nrf24l01_transmit(data);
            while(1)
            {
                osDelay(1);
                status = nrf24l01_get_tx_status();
                if(status != NRF24L01_TX_STATUS_SENDING)
                {
                    break;
                }
            }
            switch(status)
            {
                case NRF24L01_TX_STATUS_OK:
                    count = nrf24l01_get_retransmissions_count();
                    DEBUG_RADIO("Transmit: OK (0x%02X, %d).", status, count);
                    break;
                case NRF24L01_TX_STATUS_LOST:
                    count = nrf24l01_get_retransmissions_count();
                    DEBUG_RADIO("Transmit: LOST (0x%02X, %d).", status, count);
                    break;
                case NRF24L01_TX_STATUS_SENDING:
                    DEBUG_RADIO("Transmit: SENDING (0x%02X).", status);
                    break;
                default:
                    DEBUG_RADIO("Transmit: ERROR (0x%02X).", status);
                    break;
            }
            memset(data, 0, sizeof(data));
            nrf24l01_power_up_rx();
        }
        osDelay(1);
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

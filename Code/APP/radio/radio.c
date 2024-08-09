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
#include <math.h>

#include "radio/radio.h"
#include "radio/nrf24l01.h"

#include "cmsis_os2.h"
#include "debug.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define RADIO_CHANNEL           1
#define RADIO_PAYLAOD_SIZE      32
#define RADIO_TRANSMIT_TMO_MS   10  //!< Radio data transmit timeout in milliseconds.
#define RADIO_RECEIVE_TMO_MS    10  //!< Radio data receive timeout in milliseconds.

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Sensors thread attributes. */
const osThreadAttr_t radio_thread_attr =
{
    .name = "RADIO",
    .stack_size = 1024,
    .priority = osPriorityAboveNormal,
};

const uint8_t radio_my_address[NRF24L01_ADDRESS_SIZE] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
const uint8_t radio_peer_address[NRF24L01_ADDRESS_SIZE] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Radio thread ID. */
osThreadId_t radio_thread_id;
/** Radio data structure. */
volatile radio_data_t radio_data = {0};
/** Radio data buffer. */
static uint8_t radio_data_buffer[RADIO_PAYLAOD_SIZE] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static bool radio_transmit_handler(void);
static bool radio_receive_handler(void);
static bool radio_packet_builder(uint8_t *packet, uint8_t size);
static bool radio_packet_parser(uint8_t *packet, uint8_t size);
static void radio_quality_check(void);

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
    nrf24l01_init(RADIO_CHANNEL, RADIO_PAYLAOD_SIZE);
    nrf24l01_set_my_address((uint8_t *)radio_my_address);
    nrf24l01_set_tx_address((uint8_t *)radio_peer_address);

    nrf24l01_power_up_rx();

    while(1)
    {
        if(radio_receive_handler() == true)
        {
            radio_transmit_handler();
            nrf24l01_power_up_rx();
        }
        osDelay(10);
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static bool radio_transmit_handler(void)
{
    uint8_t c = RADIO_TRANSMIT_TMO_MS;

    nrf24l01_tx_status_t status = NRF24L01_TX_STATUS_LOST;

    if(radio_packet_builder(radio_data_buffer, RADIO_PAYLAOD_SIZE) == false)
    {
        return false;
    }
    nrf24l01_transmit(radio_data_buffer);
    while((c--))
    {
        osDelay(1);
        status = nrf24l01_get_tx_status();
        if(status != NRF24L01_TX_STATUS_SENDING)
        {
            break;
        }
    }

    radio_data.tx_counter++;
    radio_quality_check();

    switch(status)
    {
        case NRF24L01_TX_STATUS_OK:
            DEBUG_RADIO("Transmit: OK (%d/%d, %d %%).",
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            break;
        case NRF24L01_TX_STATUS_LOST:
            DEBUG_RADIO("Transmit: LOST (%d/%d, %d %%).",
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            radio_data.tx_lost_counter++;
            break;
        case NRF24L01_TX_STATUS_SENDING:
            DEBUG_RADIO("Transmit: SENDING (%d/%d, %d %%).",
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            radio_data.tx_lost_counter++;
            break;
        default:
            DEBUG_RADIO("Transmit: ERROR 0x%02X (%d/%d, %d %%).",
                        status,
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            radio_data.tx_lost_counter++;
            break;
    }

    return status == NRF24L01_TX_STATUS_OK ? true : false;
}
static bool radio_receive_handler(void)
{
    if(!nrf24l01_data_ready())
    {
        return false;
    }

    nrf24l01_get_data(radio_data_buffer);
    DEBUG_RADIO("Received data:");
    debug_send_hex_os(radio_data_buffer, RADIO_PAYLAOD_SIZE);
    if(radio_packet_parser(radio_data_buffer, RADIO_PAYLAOD_SIZE) == false)
    {
        return false;
    }

    radio_data.rx_counter++;
    return true;
}
static bool radio_packet_builder(uint8_t *packet, uint8_t size)
{
    if(packet == NULL || size ==0)
    {
        return false;
    }

    packet[0]++;

    return true;
}

static bool radio_packet_parser(uint8_t *packet, uint8_t size)
{
    if(packet == NULL || size ==0)
    {
        return false;
    }

    return true;
}


static void radio_quality_check(void)
{
    uint8_t rtr = 0;
    rtr = nrf24l01_get_retransmissions_count();

    if(radio_data.tx_counter)
    {
        if(rtr <= 15)
        {
            radio_data.rtr_acumulator += rtr;
        }
    }
    else
    {
        radio_data.rtr_acumulator = rtr;
        radio_data.tx_counter = 1;
    }

    radio_data.rtr_current = rtr;
    radio_data.rtr = (uint32_t)((float)radio_data.rtr_acumulator / (float)radio_data.tx_counter);
    radio_data.quality = (uint32_t)(100.0 - ((float)radio_data.rtr * (100.0 / 15.0)));

    return;
}

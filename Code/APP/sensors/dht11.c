/**
 **********************************************************************************************************************
 * @file         dht11.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-10-03
 * @brief        This is C source file template.
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

#include "dht11.h"
#include "chip.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
uint8_t dht11_data[5] = {0};

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void dht11_delay(uint32_t us);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void dht11_init(void)
{
    Chip_IOCON_PinMuxSet(LPC_IOCON, DHT11_PORT, DHT11_PIN, (IOCON_MODE_PULLUP | IOCON_FUNC0));
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, DHT11_PORT, DHT11_PIN);

    return;
}

bool dht11_read(dht11_data_t *data)
{
    uint8_t last = 1;
    uint8_t counter = 0;
    uint8_t j = 0;
    uint8_t i = 0;

    /* Pull pin down for 18 milliseconds. */
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, DHT11_PORT, DHT11_PIN);
    Chip_GPIO_SetPinState(LPC_GPIO, DHT11_PORT, DHT11_PIN, false);
    dht11_delay(18000);
    /* Then pull it up for 40 microseconds. */
    Chip_GPIO_SetPinState(LPC_GPIO, DHT11_PORT, DHT11_PIN, true);
    dht11_delay(40);
    /* Prepare to read the pin. */
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, DHT11_PORT, DHT11_PIN);

    /* Detect change and read data */
    for(i = 0; i < 85; i++)
    {
        counter = 0;
        while(Chip_GPIO_ReadPortBit(LPC_GPIO, DHT11_PORT, DHT11_PIN) == last)
        {
            counter++;
            dht11_delay(1);
            if(counter == 255)
            {
                break;
            }
        }
        last = Chip_GPIO_ReadPortBit(LPC_GPIO, DHT11_PORT, DHT11_PIN);
        if(counter == 255)
        {
            break;
        }
        /* Ignore first 3 transitions */
        if((i >= 4) && (i % 2 == 0))
        {
            /* Shove each bit into the storage bytes */
            dht11_data[j / 8] <<= 1;
            if(counter > 16)
            {
                dht11_data[j / 8] |= 1;
            }
            j++;
        }
    }

    /*
     * Check we read 40 bits (8bit x 5 ) + verify checksum in the last byte.
     */
    if((j >= 40) &&
        (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF)))
    {
        data->humidity = dht11_data[0];
        data->temperature = dht11_data[2];
        //DEBUG("Humidity = %d.%d %% Temperature = %d.%d degC", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3]);
        return true;
    }

    return false;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void dht11_delay(uint32_t us)
{
    uint32_t delay = (SystemCoreClock / 1000000) / 6;
    /* Go to clock cycles */
    delay *= us;

    /* Wait till done */
    while(delay--)
    {
        __nop();
    }

    return;
}

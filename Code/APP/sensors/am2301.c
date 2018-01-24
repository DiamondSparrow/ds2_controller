/**
 **********************************************************************************************************************
 * @file         am2301.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-09-01
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

#include "am2301.h"
#include "periph/gpio.h"
#include "chip.h"

#include "debug.h"
#include "cmsis_os2.h" 

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

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void am2301_delay(uint32_t us);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
am2301_ret_t am2301_init(void)
{
    Chip_IOCON_PinMuxSet(LPC_IOCON, AM2301_PORT, AM2301_PIN, (IOCON_MODE_PULLUP | IOCON_FUNC0));
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, AM2301_PORT, AM2301_PIN);

    return AM2301_OK;
}

am2301_ret_t am2301_read(am2301_data_t *data)
{
    uint32_t time = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t d[5] = {0};

    /* Pin output */
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, AM2301_PORT, AM2301_PIN);
    /* Set pin low for ~800-1000 us */
    Chip_GPIO_SetPinState(LPC_GPIO, AM2301_PORT, AM2301_PIN, false);
    am2301_delay(1000);
    /* Set pin high to ~30 us */
    Chip_GPIO_SetPinState(LPC_GPIO, AM2301_PORT, AM2301_PIN, true);
    am2301_delay(30);

    /* Read mode */
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, AM2301_PORT, AM2301_PIN);

    time = 0;
    /* Wait 20us for acknowledge, low signal */
    while(Chip_GPIO_ReadPortBit(LPC_GPIO, AM2301_PORT, AM2301_PIN))
    {
        if(time > 20)
        {
            return AM2301_CONNECTION_ERROR;
        }
        /* Increase time */
        time++;
        /* Wait 1 us */
        am2301_delay(1);
    }

    time = 0;
    /* Wait high signal, about 80-85us long (measured with logic analyzer) */
    while(!Chip_GPIO_ReadPortBit(LPC_GPIO, AM2301_PORT, AM2301_PIN))
    {
        if(time > 85)
        {
            return AM2301_WAITHIGH_ERROR;
        }
        /* Increase time */
        time++;
        /* Wait 1 us */
        am2301_delay(1);
    }

    time = 0;
    /* Wait low signal, about 80-85us long (measured with logic analyzer) */
    while(Chip_GPIO_ReadPortBit(LPC_GPIO, AM2301_PORT, AM2301_PIN))
    {
        if(time > 85)
        {
            return AM2301_WAITLOW_ERROR;
        }
        /* Increase time */
        time++;
        /* Wait 1 us */
        am2301_delay(1);
    }

    /* Read 5 bytes */
    for(j = 0; j < 5; j++)
    {
        d[j] = 0;
        for(i = 8; i > 0; i--)
        {
            /* We are in low signal now, wait for high signal and measure time */
            time = 0;
            /* Wait high signal, about 57-63us long (measured with logic analyzer) */
            while(!Chip_GPIO_ReadPortBit(LPC_GPIO, AM2301_PORT, AM2301_PIN))
            {
                if(time > 75)
                {
                    return AM2301_WAITHIGH_LOOP_ERROR;
                }
                /* Increase time */
                time++;
                /* Wait 1 us */
                am2301_delay(1);
            }
            /* High signal detected, start measure high signal, it can be 26us for 0 or 70us for 1 */
            time = 0;
            /* Wait low signal, between 26 and 70us long (measured with logic analyzer) */
            while(Chip_GPIO_ReadPortBit(LPC_GPIO, AM2301_PORT, AM2301_PIN))
            {
                if(time > 90)
                {
                    return AM2301_WAITLOW_LOOP_ERROR;
                }
                /* Increase time */
                time++;
                /* Wait 1 us */
                am2301_delay(1);
            }

            if(time > 18 && time < 37)
            {
                /* We read 0 */
            }
            else
            {
                /* We read 1 */
                d[j] |= 1 << (i - 1);
            }
        }
    }

    /* Check for parity */
    if(((d[0] + d[1] + d[2] + d[3]) & 0xFF) != d[4])
    {
        /* Parity error, data not valid */
        return AM2301_PARITY_ERROR;
    }

    /* Set humidity */
    data->humidity = d[0] << 8 | d[1];
    /* Negative temperature */
    if(d[2] & 0x80)
    {
        data->temperature = -((d[2] & 0x7F) << 8 | d[3]);
    }
    else
    {
        data->temperature = (d[2]) << 8 | d[3];
    }

    /* Data OK */
    return AM2301_OK;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void am2301_delay(uint32_t us)
{
    uint32_t delay = (SystemCoreClock / 1000000) / 5;
    /* Go to clock cycles */
    delay *= us;

    /* Wait till done */
    while(delay--)
    {
        __nop();
    }

    return;
}

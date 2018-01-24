/**
 **********************************************************************************************************************
 * @file         sensors.c
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
#include <stdbool.h>

#include "sensors/sensors.h"

#include "sensors/bh1750.h"
#include "sensors/am2301.h"
#include "sensors/dht11.h"
#include "sensors/filters.h"

#include "debug.h"
#include "cmsis_os2.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Sensors thread attributes. */
const osThreadAttr_t sensors_thread_attr =
{
    .name = "SENSORS",
    .stack_size = 1024,
    .priority = osPriorityNormal,
};

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Display thread ID. */
osThreadId_t sensors_thread_id;
filters_low_pass_t sensors_light_lp_filter = {0};
volatile sensors_data_t sensors_data = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool sensors_init(void)
{
    sensors_data.light.state = bh1750_init(BH1750_MODE_CONT_HIGH_RES);
    dht11_init();

    // Create sensors thread.
    if((sensors_thread_id = osThreadNew(&sensors_thread, NULL, &sensors_thread_attr)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}


void sensors_thread(void *arguments)
{
    dht11_data_t dht11_data = {0};
    uint16_t light = 0;

    osDelay(10);

    while(1)
    {
        if(sensors_data.light.state)
        {
            light = bh1750_read_level();
            if(light != UINT16_MAX)
            {
                sensors_data.light.value = light;
                sensors_data.light.value_lp = (uint16_t)filter_low_pass(&sensors_light_lp_filter, (double)light, 0.25);
            }
            else
            {
                sensors_data.light.state = false;
            }
        }
        else
        {
            sensors_data.light.state = bh1750_init(BH1750_MODE_CONT_HIGH_RES);
        }
        osDelay(1);
        if(dht11_read(&dht11_data) == true)
        {
            sensors_data.humidity.state = true;
            sensors_data.humidity.value = dht11_data.humidity;
            sensors_data.temperature.state = true;
            sensors_data.temperature.value = dht11_data.temperature;
        }
        else
        {
            dht11_init();
            sensors_data.humidity.state = false;
            sensors_data.temperature.state = false;
        }
        /*
        DEBUG("Sensors data: %d,%d; %d,%d; %d,%d;",
            sensors_data.light.state, sensors_data.light.value,
            sensors_data.humidity.state, sensors_data.humidity.value,
            sensors_data.temperature.state, sensors_data.temperature.value);
        */
        osDelay(100);
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

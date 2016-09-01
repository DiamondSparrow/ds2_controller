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

#include "display/display.h"

#include "cmsis_os.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
/** Define application thread */
osThreadDef(sensors_thread, osPriorityNormal, 1, 1024);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct
{
    struct
    {
        bool state;
        uint16_t value;
    } light;
} sensors_data_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Display thread ID. */
osThreadId sensors_thread_id;

sensors_data_t sensors_data = {0};

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
    if(sensors_data.light.state == false)
    {
        return false;
    }

    // Create display thread.
    if((sensors_thread_id = osThreadCreate(osThread(sensors_thread), NULL)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}


void sensors_thread(void const *arg)
{
    osDelay(2000);

    while(1)
    {
        osDelay(150);
        if(sensors_data.light.state)
        {
            sensors_data.light.value = bh1750_read_level();
            display_menu_set(DISPLAY_MENU_ID_LIGHT);
        }
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

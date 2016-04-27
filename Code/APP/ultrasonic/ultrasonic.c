/**
 **********************************************************************************************************************
 * @file         ultrasonic.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-14
 * @brief        Ultrasonic (HC SR-04) C source file.
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

#include "cmsis_os.h"
#include "bsp.h"

#include "ultrasonic.h"
#include "debug.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct
{
    gpio_t triger;
    gpio_t echo;
    uint32_t range;
} ultrasonic_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
ultrasonic_t ultrasonic_list[ULTRASONIC_ID_LAST] =
{
    {GPIO_ULTRASONIC_1_TRIGER, GPIO_ULTRASONIC_1_ECHO, 0},
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void ultrasonic_delay_us(uint32_t count);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool ultrasonic_init(void)
{
    uint8_t i = 0;

    for(i = 0; i < ULTRASONIC_ID_LAST; i++)
    {
        gpio_output(ultrasonic_list[i].triger);
        gpio_output_set(ultrasonic_list[i].triger, false);
        gpio_input(ultrasonic_list[i].echo);
    }

    return true;
}

uint32_t ultrasonic_read(ultrasonic_id_t id)
{
    uint32_t high = 0;
    uint32_t low = 0;
    uint32_t range = 0;
    uint32_t timeout = 0;

    gpio_output(ultrasonic_list[id].triger);
    gpio_output_high(ultrasonic_list[id].triger);
    ultrasonic_delay_us(20);
    gpio_output_low(ultrasonic_list[id].triger);

    do
    {
        if(gpio_input_get(GPIO_ULTRASONIC_1_ECHO) == true)
        {
            high = osKernelSysTick() / (SystemCoreClock / 1000000);
            timeout = 0;
            break;
        }
        else
        {
            timeout++;
            if(timeout == 0xFFF)
            {
                return 0;
            }
        }
    }
    while(1);
    do
    {
        if(gpio_input_get(GPIO_ULTRASONIC_1_ECHO) == false)
        {
            low = osKernelSysTick() /  (SystemCoreClock / 1000000);
            break;
        }
        else
        {
            timeout++;
            if(timeout == 0xFFFF)
            {
                return 0;
            }
        }
    }
    while(1);

    range = (low - high);
    range = ((float)range * 0.0165);

    return range;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void ultrasonic_delay_us(uint32_t us)
{
    /* Go to clock cycles */
    us *= (SystemCoreClock / 1000000) / 8;

    /* Wait till done */
    while(us--)
    {
        __nop();
    }

    return;
}

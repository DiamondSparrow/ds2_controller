/**
 **********************************************************************************************************************
 * @file         gpio.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-10
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

#include "chip.h"

#include "gpio.h"

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
    uint8_t port;   //!< GPIO port number.
    uint8_t pin;    //!< GPIO Pin number.
    bool dir;       //!< Pin directions: 0 - input, 1 - output.
    bool state;     //!< Pin state: 0 - low, 1 - high.
} gpio_item_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
gpio_item_t gpio_list[GPIO_LAST] =
{
    {.port = 0, .pin =  25, .dir = true, .state = false,},
    {.port = 0, .pin =  3,  .dir = true, .state = false,},
    {.port = 1, .pin =  1,  .dir = true, .state = false,},
};
/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void gpio_init(void)
{
    uint8_t i = 0;

    Chip_GPIO_Init(LPC_GPIO);

    for(i = 0; i < GPIO_LAST; i++)
    {
        Chip_GPIO_SetPinDIR(LPC_GPIO, gpio_list[i].port, gpio_list[i].pin, gpio_list[i].dir);
        if(gpio_list[i].dir == true)
        {
            Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[i].port, gpio_list[i].pin, gpio_list[i].state);
        }
    }

    return;
}

void gpio_output(gpio_t gpio)
{
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin);

    return;
}

void gpio_output_set(gpio_t gpio, bool state)
{
    Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin, gpio_list[gpio].state);

    return;
}

void gpio_output_low(gpio_t gpio)
{
    Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin, false);

    return;
}

void gpio_output_high(gpio_t gpio)
{
    Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin, true);

    return;
}

void gpio_input(gpio_t gpio)
{

    Chip_GPIO_SetPinDIRInput(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin);

    return;
}

bool gpio_input_get(gpio_t gpio)
{

    return Chip_GPIO_ReadPortBit(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin);
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

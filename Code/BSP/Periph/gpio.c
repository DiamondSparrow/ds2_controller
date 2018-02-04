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
const gpio_item_t gpio_list[GPIO_LAST] =
{
    {.port = 0, .pin =  25, .dir = true,  .state = true,},  // GPIO_LED_RED
    {.port = 0, .pin =  3,  .dir = true,  .state = true,},  // GPIO_LED_GREEN
    {.port = 1, .pin =  1,  .dir = true,  .state = true,},  // GPIO_LED_BLUE
    /*
    {.port = 0, .pin =  2,  .dir = true,  .state = false,}, // GPIO_ULTRASONIC_1_TRIGER
    {.port = 0, .pin =  30, .dir = false, .state = false,}, // GPIO_ULTRASONIC_1_ECHO
    {.port = 0, .pin =  16, .dir = true,  .state = false,}, // GPIO_AM2301
    {.port = 1, .pin =  9,  .dir = false, .state = false,}, // GPIO_SW_JS
    {.port = 1, .pin =  4,  .dir = false, .state = false,}, // GPIO_SW_LEFT
    {.port = 1, .pin =  5,  .dir = false, .state = false,}, // GPIO_SW_RIGH
    {.port = 0, .pin =  9,  .dir = true,  .state = true,},  // GPIO_NRF214L01_CE
    {.port = 0, .pin =  27, .dir = true,  .state = true,},  // GPIO_NRF214L01_CSN
    {.port = 0, .pin =  8,  .dir = true,  .state = false,}, // GPIO_MOTOR_LEFT_EN
    {.port = 0, .pin =  0,  .dir = true,  .state = false,}, // GPIO_MOTOR_LEFT_INA
    {.port = 0, .pin =  24, .dir = true,  .state = false,}, // GPIO_MOTOR_LEFT_INB
    {.port = 0, .pin =  7,  .dir = true,  .state = false,}, // GPIO_MOTOR_RIGHT_EN
    {.port = 0, .pin =  10, .dir = true,  .state = false,}, // GPIO_MOTOR_RIGHT_INA
    {.port = 1, .pin =  0,  .dir = true,  .state = false,}, // GPIO_MOTOR_RIGHT_INB
    //{.port = 1, .pin =  6,  .dir = true,  .state = true,},  // GPIO_DISPLAY_RESTART
    */
    {.port = 0, .pin =  12,  .dir = true,  .state = true,},  // GPIO_DISPLAY_DC
    {.port = 0, .pin =  9,  .dir = true,  .state = false,},  // GPIO_DISPLAY_SELECT
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
/*
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 7, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 9, (IOCON_MODE_PULLUP | IOCON_HYS_EN | IOCON_S_MODE_0CLK));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 4, (IOCON_MODE_PULLUP | IOCON_HYS_EN | IOCON_S_MODE_0CLK));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 5, (IOCON_MODE_PULLUP | IOCON_HYS_EN | IOCON_S_MODE_0CLK));
*/
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
    Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin, state);

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

void gpio_output_toggle(gpio_t gpio)
{
    Chip_GPIO_SetPinToggle(LPC_GPIO, gpio_list[gpio].port, gpio_list[gpio].pin);

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

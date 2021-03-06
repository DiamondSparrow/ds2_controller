/**
 **********************************************************************************************************************
 * @file        gpio.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-04-10
 * @brief       This is C header file template.
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

#ifndef GPIO_H_
#define GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef enum
{
    GPIO_LED_RED,
    GPIO_LED_GREEN,
    GPIO_LED_BLUE,
    GPIO_ULTRASONIC_1_TRIGER,
    GPIO_ULTRASONIC_1_ECHO,
    GPIO_AM2301,
    GPIO_SW_JS,
    GPIO_SW_LEFT,
    GPIO_SW_RIGHT,
    GPIO_NRF214L01_CE,
    GPIO_NRF214L01_CSN,
    GPIO_MOTOR_LEFT_EN,
    GPIO_MOTOR_LEFT_INA,
    GPIO_MOTOR_LEFT_INB,
    GPIO_MOTOR_RIGHT_EN,
    GPIO_MOTOR_RIGHT_INA,
    GPIO_MOTOR_RIGHT_INB,
    GPIO_DISPLAY_RESTART,
    GPIO_DISPLAY_DC,
    GPIO_DISPLAY_SELECT,
    GPIO_LAST,
} gpio_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
void gpio_init(void);
void gpio_output(gpio_t gpio);
void gpio_output_set(gpio_t gpio, bool state);
void gpio_output_low(gpio_t gpio);
void gpio_output_high(gpio_t gpio);
void gpio_output_toggle(gpio_t gpio);
void gpio_input(gpio_t gpio);
bool gpio_input_get(gpio_t gpio);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H_ */

/**
 **********************************************************************************************************************
 * @file         joystick.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-10-16
 * @brief        Joystick C source file.
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
#include <math.h>

#include "sensors/joystick.h"

#include "periph/adc.h"
#include "periph/gpio.h"

#include "cmsis_os2.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define JOYSTICK_RESOLUTION     1024
#define JOYSTICK_ADC_RES        4096
#define JOYSTICK_PI             3.14159265358979f
#define JOYSTICK_CAL_COUNT      10
#define JOYSTICK_X_INVERT       1
#define JOYSTICK_Y_INVERT       0


/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct
{
    adc_id_t x;
    adc_id_t y;
    gpio_t sw;
    uint16_t x_zero;
    uint16_t y_zero;
} joystick_config_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
joystick_config_t joystick_config[JOYSTICK_ID_LAST] =
{
    {
        .x = ADC_ID_JS_X,
        .y = ADC_ID_JS_Y,
        .sw = GPIO_SW_JS,
        .x_zero = (JOYSTICK_ADC_RES / 2),
        .y_zero = (JOYSTICK_ADC_RES / 2),
    },
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
bool joystick_init(void)
{
    uint8_t i = 0;

    for(i = 0; i < JOYSTICK_ID_LAST; i++)
    {
        joystick_calibrate((joystick_id_t)i);
    }

    return true;
}

void joystick_calibrate(joystick_id_t id)
{
    uint8_t i = JOYSTICK_CAL_COUNT;
    uint32_t x = 0;
    uint32_t y = 0;

    while(i--)
    {
        x += adc_get_value_raw(joystick_config[id].x);
        y += adc_get_value_raw(joystick_config[id].y);
        osDelay(1);
    }
    joystick_config[id].x_zero = x / JOYSTICK_CAL_COUNT;
    joystick_config[id].y_zero = y / JOYSTICK_CAL_COUNT;

    return;
}

int16_t joystick_get_x(joystick_id_t id)
{
    uint32_t x = adc_get_value_raw(joystick_config[id].x);

    if(x > joystick_config[id].x_zero)
    {
#if JOYSTICK_X_INVERT
        return -(((x - joystick_config[id].x_zero) * JOYSTICK_RESOLUTION) / (JOYSTICK_ADC_RES - joystick_config[id].x_zero));
#else
        return (((x - joystick_config[id].x_zero) * JOYSTICK_RESOLUTION) / (JOYSTICK_ADC_RES - joystick_config[id].x_zero));
#endif
    }

    if(x < joystick_config[id].x_zero)
    {
#if JOYSTICK_X_INVERT
        return (((joystick_config[id].x_zero - x) * JOYSTICK_RESOLUTION) / joystick_config[id].x_zero);
#else
        return -(((joystick_config[id].x_zero - x) * JOYSTICK_RESOLUTION) / joystick_config[id].x_zero);
#endif
    }

    return 0;
}

int16_t joystick_get_y(joystick_id_t id)
{
    uint32_t y = adc_get_value_raw(joystick_config[id].y);

    if(y > joystick_config[id].y_zero)
    {
#if JOYSTICK_Y_INVERT
        return -(((y - joystick_config[id].y_zero) * JOYSTICK_RESOLUTION) / (JOYSTICK_ADC_RES - joystick_config[id].y_zero));
#else
        return (((y - joystick_config[id].y_zero) * JOYSTICK_RESOLUTION) / (JOYSTICK_ADC_RES - joystick_config[id].y_zero));
#endif
    }

    if(y < joystick_config[id].y_zero)
    {
#if JOYSTICK_Y_INVERT
        return (((joystick_config[id].y_zero - y) * JOYSTICK_RESOLUTION) / joystick_config[id].y_zero);
#else
        return -(((joystick_config[id].y_zero - y) * JOYSTICK_RESOLUTION) / joystick_config[id].y_zero);
#endif
    }

    return 0;
}

void joystick_get_vector(joystick_id_t id, int32_t *magnitude, int32_t *direction)
{
    double magn = 0;
    double dir = 0;
    int16_t x = joystick_get_x(id);
    int16_t y = joystick_get_y(id);

    magn = sqrt(pow(x, 2) + pow(y, 2));
    if(magn > 1024)
    {
        magn = JOYSTICK_RESOLUTION;
    }

    dir = atan2(x, y);
    if(dir < 0)
    {
        dir += 2 * JOYSTICK_PI;
    }
    dir = dir * 180 / JOYSTICK_PI;

    *magnitude = (int32_t)magn;
    *direction = (int32_t)dir;

    return;
}

bool joystick_get_sw(joystick_id_t id)
{
    return gpio_input_get(joystick_config[id].sw);
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

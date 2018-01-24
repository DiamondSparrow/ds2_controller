/**
 **********************************************************************************************************************
 * @file         servo.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-13
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

#include "cmsis_os2.h"

#include "servo.h"
#include "bsp.h"
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
    pwm_id_t pwm;   //!< ID of PWM channel.
    uint32_t min;   //!< PWM duty cycle at -90 degrees.
    uint32_t zero;  //!< PWM duty cycle at 0 degrees.
    uint32_t max;   //!< PWM duty cycle at +90 degrees.
} servo_config_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static const servo_config_t servo_config[SERVO_ID_LAST] =
{
    {.pwm = PWM_ID_SERVO_PAN,  .min = 54000, .zero = 108000, .max = 162000},
    {.pwm = PWM_ID_SERVO_TILT, .min = 54000, .zero = 108000, .max = 162000},
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

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
bool servo_init(void)
{
    uint8_t i = 0;

    for(i = 0; i < SERVO_ID_LAST; i++)
    {
        servo_set((servo_id_t)i, 0);
    }

    return true;
}

bool servo_set(servo_id_t id, int8_t angle)
{
    int32_t duty_cycle = 0;

    if(angle < SERVO_ANGLE_MIN || angle > SERVO_ANGLE_MAX)
    {
        return false;
    }

#if SERVO_ANGLE_INVERT
    angle *= (-1);
#endif

    if(angle >= SERVO_ANGLE_MIN && angle < SERVO_ANGLE_ZERO)
    {
        duty_cycle = (servo_config[id].zero - servo_config[id].min);
        duty_cycle *= angle;
        duty_cycle /= 90;
        duty_cycle = (servo_config[id].zero + duty_cycle);
    }
    else if (angle > SERVO_ANGLE_ZERO && angle <= SERVO_ANGLE_MAX)
    {
        duty_cycle = (servo_config[id].max - servo_config[id].zero);
        duty_cycle *= angle;
        duty_cycle /= 90;
        duty_cycle = (servo_config[id].zero + duty_cycle);
    }
    else
    {
        duty_cycle = servo_config[id].zero;
    }

    if(duty_cycle > servo_config[id].max)
    {
        duty_cycle = servo_config[id].max;
    }
    if(duty_cycle < servo_config[id].min)
    {
        duty_cycle = servo_config[id].min;
    }

    pwm_set(servo_config[id].pwm, (uint32_t)duty_cycle);
    osDelay(10);
    pwm_set(servo_config[id].pwm, 0);

    return true;
}

void servo_set_all(int8_t pan_angle, int8_t tilt_angle)
{
    servo_set(SERVO_ID_PAN, pan_angle);
    servo_set(SERVO_ID_TILT, tilt_angle);

    return;
}

void servo_test(servo_id_t id)
{
    int8_t i = 0;

    for(i = SERVO_ANGLE_MIN; i <= SERVO_ANGLE_MAX; i++)
    {
        servo_set((servo_id_t)id, i);
        osDelay(5);
    }

    for(i = SERVO_ANGLE_MAX; i >= SERVO_ANGLE_MIN; i--)
    {
        servo_set((servo_id_t)id, i);
        osDelay(5);
    }

    servo_set((servo_id_t)id, 0);

    return;
}

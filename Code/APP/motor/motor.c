/**
 **********************************************************************************************************************
 * @file         motor.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-09-12
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

#include "motor.h"
#include "vhn2sp30.h"

#include "cmsis_os.h"
#include "debug.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
osThreadDef(motor_thread, osPriorityNormal, 1, 256);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct
{
    vhn2sp30_t drive;
    int16_t target;
    int16_t current;
} motor_data_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
osThreadId motor_thread_id;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
motor_data_t motor_data[MOTOR_ID_LAST] =
{
    // MOTOR_ID_LEFT
    {
        .drive =
        {
            .in_a = GPIO_MOTOR_LEFT_INA,
            .in_b = GPIO_MOTOR_LEFT_INB,
            .en = GPIO_MOTOR_LEFT_EN,
            .pwm = PWM_ID_MOTOR_LEFT,
            .cs = 0
        },
        .target = 0, .current = 0,
    },
    // MOTOR_ID_RIGHT
    {
        .drive =
        {
            .in_a = GPIO_MOTOR_RIGHT_INA,
            .in_b = GPIO_MOTOR_RIGHT_INB,
            .en = GPIO_MOTOR_RIGHT_EN,
            .pwm = PWM_ID_MOTOR_RIGHT,
            .cs = 0
        },
        .target = 0, .current = 0,
    },
};

uint8_t motor_ramp = 0;

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool motor_init()
{
    uint8_t i = 0;

    for(i = 0; i < MOTOR_ID_LAST; i++)
    {
        vhn2sp30_init(&motor_data[MOTOR_ID_LEFT].drive);
        vhn2sp30_init(&motor_data[MOTOR_ID_RIGHT].drive);
    }
    
    if((motor_thread_id = osThreadCreate(osThread(motor_thread), NULL)) == NULL)
    {
        return false;
    }

    return true;
}

void motor_thread(void const *arg)
{
    uint8_t i = 0;

    while(1)
    {
        if(motor_ramp > 0)
        {
            for(i = 0; i < MOTOR_ID_LAST; i++)
            {
                if(motor_data[i].target > motor_data[i].current)
                {
                    motor_data[i].current++;
                }
                else if(motor_data[i].target < motor_data[i].current)
                {
                    motor_data[i].current--;
                }
                else
                {
                    continue;
                }
                if(motor_data[i].current > 0)
                {
                    vhn2sp30_run_cw(&motor_data[i].drive, motor_data[i].current);
                }
                else if (motor_data[i].current < 0)
                {
                    vhn2sp30_run_ccw(&motor_data[i].drive, motor_data[i].current * (-1));
                }
                else
                {
                    vhn2sp30_neutral(&motor_data[i].drive);
                }
            }
            osDelay(motor_ramp);
        }
        else
        {
            osDelay(100);
        }
    }
}

void motor_ramp_set(uint16_t ramp)
{
    motor_ramp = ramp;

    return;
}

void motor_forward(motor_id_t motor, uint8_t speed)
{
    speed = speed > 100 ? 100 : speed;

    if(motor_ramp == 0)
    {
        motor_data[motor].target = speed;
        motor_data[motor].current = speed;
        vhn2sp30_run_cw(&motor_data[motor].drive, speed);
    }
    else
    {
        motor_data[motor].target = speed;
    }

    return;
}

void motor_backward(motor_id_t motor, uint8_t speed)
{
    speed = speed > 100 ? 100 : speed;

    if(motor_ramp == 0)
    {
        motor_data[motor].target = speed;
        motor_data[motor].current = speed;
        vhn2sp30_run_ccw(&motor_data[motor].drive, speed);
    }
    else
    {
        motor_data[motor].target = speed * (-1);
    }

    return;
}

void motor_brake(motor_id_t motor)
{
    motor_data[motor].target = 0;
    motor_data[motor].current = 0;
    vhn2sp30_brake_vcc(&motor_data[motor].drive);

    return;
}

void motor_neutral(motor_id_t motor)
{
    motor_data[motor].target = 0;
    motor_data[motor].current = 0;
    vhn2sp30_brake_gnd(&motor_data[motor].drive);

    return;
}

void motor_test(motor_id_t motor, uint8_t ramp)
{
    uint8_t speed = 0;
    uint8_t speed_dir = 0;

    while(1)
    {
        if(speed_dir == 0)
        {
            speed++;
            motor_forward(motor, speed);
            if(speed >= 100)
            {
                speed_dir = 1;
                osDelay(1000);
            }
        }
        else
        {
            speed--;
            motor_forward(motor, speed);
            if(speed == 0)
            {
                speed_dir = 0;
                osDelay(1000);
                break;
            }
        }
        osDelay(ramp);
    }
    while(1)
    {
        if(speed_dir == 0)
        {
            speed++;
            motor_backward(motor, speed);
            if(speed >= 100)
            {
                speed_dir = 1;
                osDelay(1000);
            }
        }
        else
        {
            speed--;
            motor_backward(motor, speed);
            if(speed == 0)
            {
                speed_dir = 0;
                osDelay(1000);
                break;
            }
        }
        osDelay(ramp);
    }
}

void motor_test_ramp(motor_id_t motor, uint8_t ramp)
{
    motor_ramp_set(ramp);

    motor_test(motor, ramp * 2);

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

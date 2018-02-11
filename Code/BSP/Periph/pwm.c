/**
 **********************************************************************************************************************
 * @file         pwm.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-12
 * @brief        Pulse Width Modulation (PWM) C source file.
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

#include "chip.h"

#include "pwm.h"

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
    LPC_SCT_T *sct;
    CHIP_SWM_PIN_MOVABLE_T pin_mov;
    uint8_t pin;
    uint8_t port;
    uint8_t index;
} pwm_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static const pwm_t pwm_config[PWM_ID_LAST] =
{
    {.sct = LPC_SCT0, .pin_mov = SWM_SCT0_OUT0_O, .port = 0, .pin = 28, .index = 1},
    {.sct = LPC_SCT0, .pin_mov = SWM_SCT0_OUT1_O, .port = 0, .pin = 27, .index = 2},
    {.sct = LPC_SCT2, .pin_mov = SWM_SCT2_OUT0_O, .port = 0, .pin = 16, .index = 1},
    {.sct = LPC_SCT2, .pin_mov = SWM_SCT2_OUT1_O, .port = 1, .pin = 3,  .index = 2},
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
static void pwm_0_init(void);
static void pwm_2_init(void);

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void pwm_0_init(void)
{
    uint8_t i = 0;
    uint8_t p = 0;

    /* Setup Board specific output pin */
    /* Enable SWM clock before altering SWM */
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    /* Connect SCT outputs. */
    for(i = 0; i < PWM_ID_LAST; i++)
    {
        if(pwm_config[i].sct != LPC_SCT0)
        {
            continue;
        }
        Chip_SWM_MovablePortPinAssign(pwm_config[i].pin_mov, pwm_config[i].port, pwm_config[i].pin);
        p++;
    }
    /* Disable SWM clock after altering SWM */
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    if(!p)
    {
        // No PWM-0 pins required.
        return;
    }

    Chip_SCTPWM_Init(LPC_SCT0);
    Chip_SCTPWM_SetRate(LPC_SCT0, PWM_0_RATE);

    for(i = 0; i < PWM_ID_LAST; i++)
    {
        if(pwm_config[i].sct != LPC_SCT0)
        {
            continue;
        }
        /* Use SCT0_OUT1 pin */
        Chip_SCTPWM_SetOutPin(pwm_config[i].sct, pwm_config[i].index, i);
        /* Start with 0% duty cycle */
        Chip_SCTPWM_SetDutyCycle(pwm_config[i].sct, pwm_config[i].index, 0);
    }

    Chip_SCTPWM_Start(LPC_SCT0);

    return;
}

static void pwm_2_init(void)
{
    uint8_t i = 0;
    uint8_t p = 0;

    /* Setup Board specific output pin */
    /* Enable SWM clock before altering SWM */
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    /* Connect SCT outputs. */
    for(i = 0; i < PWM_ID_LAST; i++)
    {
        if(pwm_config[i].sct != LPC_SCT2)
        {
            continue;
        }
        Chip_SWM_MovablePortPinAssign(pwm_config[i].pin_mov, pwm_config[i].port, pwm_config[i].pin);
        p++;
    }
    /* Disable SWM clock after altering SWM */
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    if(!p)
    {
        // No PWM-2 pins required.
        return;
    }

    Chip_SCTPWM_Init(LPC_SCT2);
    Chip_SCTPWM_SetRate(LPC_SCT2, PWM_2_RATE);

    for(i = 0; i < PWM_ID_LAST; i++)
    {
        if(pwm_config[i].sct != LPC_SCT2)
        {
            continue;
        }
        /* Use SCT2_OUTx pin. */
        Chip_SCTPWM_SetOutPin(pwm_config[i].sct, pwm_config[i].index, i - 2);
        /* Start with 0% duty cycle */
        Chip_SCTPWM_SetDutyCycle(pwm_config[i].sct, pwm_config[i].index, 0);
    }

    Chip_SCTPWM_Start(LPC_SCT2);

    return;
}


/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void pwm_init(void)
{
    pwm_0_init();
    pwm_2_init();

    return;
}

uint32_t pwm_get_duty_cycle(pwm_id_t id)
{
    return Chip_SCTPWM_GetDutyCycle(pwm_config[id].sct, pwm_config[id].index);
}

void pwm_set(pwm_id_t id, uint32_t duty_cycle)
{
    Chip_SCTPWM_SetDutyCycle(pwm_config[id].sct, pwm_config[id].index, duty_cycle);

    return;
}

void pwm_set_percentage(pwm_id_t id, uint8_t percentage)
{
    uint32_t duty_cycle = Chip_SCTPWM_PercentageToTicks(pwm_config[id].sct, percentage);

    Chip_SCTPWM_SetDutyCycle(pwm_config[id].sct, pwm_config[id].index, duty_cycle);

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

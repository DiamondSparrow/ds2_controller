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
    uint8_t index;
} pwm_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static const pwm_t sct_pwm_list[PWM_ID_LAST] =
{
    {.sct = LPC_SCT0, .pin_mov = SWM_SCT0_OUT0_O, .pin = 29, .index = 1},
    {.sct = LPC_SCT0, .pin_mov = SWM_SCT0_OUT1_O, .pin = 9, .index = 2},
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
void pwm_init(void)
{
    uint8_t i =0;

    Chip_SCTPWM_Init(LPC_SCT0);
    Chip_SCTPWM_SetRate(LPC_SCT0, PWM_0_RATE);

    /* Setup Board specific output pin */
    /* Enable SWM clock before altering SWM */
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    /* Connect SCT outputs to PIO0.x*/
    for(i = 0; i < PWM_ID_LAST; i++)
    {
        Chip_SWM_MovablePinAssign(sct_pwm_list[i].pin_mov, sct_pwm_list[i].pin);
    }
    /* Disable SWM clock after altering SWM */
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    for(i = 0; i < PWM_ID_LAST; i++)
    {
        /* Use SCT0_OUT1 pin */
        Chip_SCTPWM_SetOutPin(sct_pwm_list[i].sct, sct_pwm_list[i].index, i);
        /* Start with 0% duty cycle */
        Chip_SCTPWM_SetDutyCycle(sct_pwm_list[i].sct, sct_pwm_list[i].index, 0);
    }

    Chip_SCTPWM_Start(LPC_SCT0);

    return;
}

uint32_t pwm_get_duty_cycle(pwm_id_t id)
{
    return Chip_SCTPWM_GetDutyCycle(sct_pwm_list[id].sct, sct_pwm_list[id].index);
}

void pwm_set(pwm_id_t id, uint32_t duty_cycle)
{
    Chip_SCTPWM_SetDutyCycle(sct_pwm_list[id].sct, sct_pwm_list[id].index, duty_cycle);

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

/**
 **********************************************************************************************************************
 * @file        vhn2sp30.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-09-12
 * @brief       H-bridge Motor drive VHN2SP30 C source file.
 **********************************************************************************************************************
 * @warning     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR \n
 *              IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND\n
 *              FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR\n
 *              CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n
 *              DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n
 *              drive, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n
 *              CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF\n
 *              THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************************************
 */

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>

#include "vhn2sp30.h"

#include "periph/gpio.h"
#include "periph/pwm.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void vhn2sp30_init(vhn2sp30_t *drive)
{
    gpio_output(drive->in_a);
    gpio_output_low(drive->in_a);
    gpio_output(drive->in_b);
    gpio_output_low(drive->in_b);
    gpio_output(drive->en);
    gpio_output_low(drive->en);
    pwm_set(drive->pwm, 0);

    return;
}

void vhn2sp30_brake_vcc(vhn2sp30_t *drive)
{
    gpio_output_high(drive->in_a);
    gpio_output_high(drive->in_b);
    gpio_output_high(drive->en);
    vhn2sp30_io_pwm(drive, 0);

    return;
}

void vhn2sp30_brake_gnd(vhn2sp30_t *drive)
{
    gpio_output_low(drive->in_a);
    gpio_output_low(drive->in_b);
    gpio_output_low(drive->en);
    vhn2sp30_io_pwm(drive, 0);

    return;
}

void vhn2sp30_run_cw(vhn2sp30_t *drive, uint8_t speed)
{
    vhn2sp30_io_enable(drive);
    vhn2so30_io_cw(drive);
    vhn2sp30_io_pwm(drive, speed);

    return;
}

void vhn2sp30_run_ccw(vhn2sp30_t *drive, uint8_t speed)
{
    vhn2sp30_io_enable(drive);
    vhn2so30_io_ccw(drive);
    vhn2sp30_io_pwm(drive, speed);

    return;
}

void vhn2sp30_neutral(vhn2sp30_t *drive)
{
    gpio_output_low(drive->in_a);
    gpio_output_low(drive->in_b);
    gpio_output_low(drive->en);
    pwm_set(drive->pwm, 0);

    return;
}

void vhn2so30_io_cw(vhn2sp30_t *drive)
{
    gpio_output_high(drive->in_a);
    gpio_output_low(drive->in_b);

    return;
}

void vhn2so30_io_ccw(vhn2sp30_t *drive)
{
    gpio_output_low(drive->in_a);
    gpio_output_high(drive->in_b);

    return;
}

void vhn2sp30_io_pwm(vhn2sp30_t *drive, uint8_t pwm)
{
    pwm_set_percentage(drive->pwm, pwm > 100 ? 100 : pwm);

    return;
}

void vhn2sp30_io_enable(vhn2sp30_t *drive)
{
    gpio_output_high(drive->en);

    return;
}

void vhn2sp30_io_disable(vhn2sp30_t *drive)
{
    gpio_output_low(drive->en);

    return;
}

uint32_t vhn2sp30_io_cs(vhn2sp30_t *drive)
{
    return (uint32_t)((adc_get_value_volt(drive->cs) *VHN2SP30_CURRENT_SENSE_COEF) / VHN2SP30_CURRENT_SENSE_RESISTOR);
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

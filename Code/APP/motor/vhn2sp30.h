/**
 **********************************************************************************************************************
 * @file        vhn2sp30.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-09-12
 * @brief       H-bridge Motor drive VHN2SP30 C header file.
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

#ifndef VHN2SP30_H_
#define VHN2SP30_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>

#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
#define VHN2SP30_CURRENT_SENSE_RESISTOR 1500    //!< Current sense resistance in Ohms.
#define VHN2SP30_CURRENT_SENSE_COEF     11370   //!< Current sense coefficient K1 or K2 by manual, typical value.

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef struct
{
    gpio_id_t in_a;
    gpio_id_t in_b;
    gpio_id_t en;
    pwm_id_t pwm;
    adc_id_t cs;
} vhn2sp30_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
void vhn2sp30_init(vhn2sp30_t *drive);

void vhn2sp30_brake_vcc(vhn2sp30_t *drive);
void vhn2sp30_brake_gnd(vhn2sp30_t *drive);
void vhn2sp30_run_cw(vhn2sp30_t *drive, uint8_t speed);
void vhn2sp30_run_ccw(vhn2sp30_t *drive, uint8_t speed);
void vhn2sp30_neutral(vhn2sp30_t *drive);

void vhn2so30_io_cw(vhn2sp30_t *drive);
void vhn2so30_io_ccw(vhn2sp30_t *drive);
void vhn2sp30_io_pwm(vhn2sp30_t *drive, uint8_t pwm);
void vhn2sp30_io_enable(vhn2sp30_t *drive);
void vhn2sp30_io_disable(vhn2sp30_t *drive);
uint32_t vhn2sp30_io_cs(vhn2sp30_t *drive);

#ifdef __cplusplus
}
#endif

#endif /* VHN2SP30_H_ */

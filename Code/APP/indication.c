/**
 **********************************************************************************************************************
 * @file         indication.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         Apr 6, 2016
 * @brief        Indication control C source file.
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
#include "bsp.h"
#include "cmsis_os.h"

#include "indication.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
osTimerDef(indication, indication_handle);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
osTimerId indication_timer_id;
indication_t indication_flag = INDICATION_NA;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Turn on ref led.
 */
static void indication_red_on(void);

/**
 * @brief   Turn off ref led.
 */
static void indication_red_off(void);

/**
 * @brief   Turn on green led.
 */
static void indication_green_on(void);

/**
 * @brief   Turn off green led.
 */
static void indication_green_off(void);

/**
 * @brief   Turn on blue led.
 */
static void indication_blue_on(void);

/**
 * @brief   Turn off blue led.
 */
static void indication_blue_off(void);

/**
 * @brief   Indication 'INDICATION_NA' control.
 */
static void indication_cntrl_na(void);

/**
 * @brief   Indication 'INDICATION_RED' control.
 */
static void indication_cntrl_red(void);

/**
 * @brief   Indication 'INDICATION_GREEN' control.
 */
static void indication_cntrl_green(void);

/**
 * @brief   Indication 'INDICATION_BLUE' control.
 */
static void indication_cntrl_blue(void);

/**
 * @brief   Indication 'INDICATION_BOOT' control.
 */
static void indication_cntrl_boot(void);

/**
 * @brief   Indication 'INDICATION_INIT' control.
 */
static void indication_cntrl_init(void);

/**
 * @brief   Indication 'INDICATION_STANDBY' control.
 */
static void indication_cntrl_standby(void);

/**
 * @brief   Indication 'INDICATION_IDLE' control.
 */
static void indication_cntrl_idle(void);

/**
 * @brief   Indication 'INDICATION_IDLE' control.
 */
static void indication_cntrl_fault(void);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool indication_init(void)
{
    if((indication_timer_id = osTimerCreate(osTimer(indication), osTimerPeriodic, NULL)) == NULL)
    {
        return false;
    }

    if((osTimerStart(indication_timer_id, INDICATION_PERIOD)) != osOK)
    {
        return false;
    }

    return true;
}

void indication_set(indication_t indication)
{
    __disable_irq();
    indication_flag = indication;
    __enable_irq();

    return;
}

void indication_set_blocking(indication_t indication)
{
    __disable_irq();
    indication_flag = indication;
    indication_handle(NULL);
    __enable_irq();

    return;
}

void indication_handle(void const *arg)
{
    indication_t indication = indication_flag;

    switch(indication)
    {
        default:
        case INDICATION_NA:
            indication_cntrl_na();
            break;
        case INDICATION_RED:
            indication_cntrl_red();
            break;
        case INDICATION_GREEN:
            indication_cntrl_green();
            break;
        case INDICATION_BLUE:
            indication_cntrl_blue();
            break;
        case INDICATION_BOOT:
            indication_cntrl_boot();
            break;
        case INDICATION_INIT:
            indication_cntrl_init();
            break;
        case INDICATION_STANDBY:
            indication_cntrl_standby();
            break;
        case INDICATION_IDLE:
            indication_cntrl_idle();
            break;
        case INDICATION_FAULT:
            indication_cntrl_fault();
            break;
    }

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void indication_red_on(void)
{
    gpio_output_low(GPIO_LED_RED);
    
    return;
}

static void indication_red_off(void)
{
    gpio_output_high(GPIO_LED_RED);

    return;
}

static void indication_green_on(void)
{
    gpio_output_low(GPIO_LED_GREEN);

    return;
}

static void indication_green_off(void)
{
    gpio_output_high(GPIO_LED_GREEN);

    return;
}

static void indication_blue_on(void)
{
    gpio_output_low(GPIO_LED_BLUE);

    return;
}

static void indication_blue_off(void)
{
    gpio_output_high(GPIO_LED_BLUE);

    return;
}

static void indication_cntrl_na(void)
{
    indication_red_off();
    indication_green_off();
    indication_blue_off();

    return;
}

static void indication_cntrl_red(void)
{
    indication_red_on();
    indication_green_off();
    indication_blue_off();

    return;
}

static void indication_cntrl_green(void)
{
    indication_red_off();
    indication_green_on();
    indication_blue_off();

    return;
}

static void indication_cntrl_blue(void)
{
    indication_red_off();
    indication_green_off();
    indication_blue_on();

    return;
}

static void indication_cntrl_boot(void)
{
    indication_red_off();
    indication_green_off();
    indication_blue_on();

    return;
}

static void indication_cntrl_init(void)
{
    indication_red_off();
    indication_green_on();
    indication_blue_off();

    return;
}

static void indication_cntrl_standby(void)
{
    indication_red_off();
    indication_green_off();
    indication_blue_on();
    osDelay(50);

    indication_red_off();
    indication_green_off();
    indication_blue_off();

    return;
}

static void indication_cntrl_idle(void)
{
    indication_red_off();
    indication_green_on();
    indication_blue_off();
    osDelay(50);

    indication_red_off();
    indication_green_off();
    indication_blue_off();

    return;
}

static void indication_cntrl_fault(void)
{
    indication_red_on();
    indication_green_off();
    indication_blue_off();
    osDelay(500);

    indication_red_off();
    indication_green_off();
    indication_blue_off();

    return;
}

/**
 **********************************************************************************************************************
 * @file         buttons
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         Feb 12, 2017
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
#include <stdio.h>

#include "buttons.h"

#include "periph/gpio.h"
#include "display/display.h"
#include "debug.h"

#include "cmsis_os2.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define BUTTONS_PRESS_DURATION          50  //!< Buttons press duration in milliseconds.
#define BUTTONS_LONG_PRESS_DURATION     500 //!< Button long press duration in milliseconds.

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
/**
 * @brief   Buttons states enumerator.
 */
typedef enum
{
    BUTTONS_STATE_RELEASED,     //!< Button is release.
    BUTTONS_STATE_PRESSED,      //!< Buttons is pressed by short time defined at @ref BUTTONS_PRESS_DURATION.
    BUTTONS_STATE_PRESSED_LONG, //!< Buttons is pressed by long time defined at @ref BUTTONS_PRESS_DURATION.
} buttons_state_t;

/**
 * @brief   Buttons callback function prototype
 *
 * @note    This callback function will be called when button state changes.
 */
typedef void (*buttons_callback_t)(buttons_id_t id);

/**
 * @brief   Buttons data structure.
 */
typedef struct
{
    gpio_id_t           gpio;       /**< Button GPIO ID. See @ref gpio_id_t. */
    buttons_state_t     state;      /**< Current button state. See @ref buttons_state_t. */
    uint32_t            counter;    /**< Button counter. */
    buttons_callback_t  callback;   /**< Button function callback. See @ref buttons_callback_t.*/
} buttons_data_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Buttons thread ID. */
osThreadId_t buttons_thread_id;
/** Buttons data. See @ref buttons_data_t. */
buttons_data_t buttons_data[BUTTONS_ID_LAST] =
{
    {.gpio = GPIO_ID_SW_RIGHT, .state = BUTTONS_STATE_RELEASED, .counter = 0, .callback = NULL}, // BUTTONS_ID_VIEW_RIGHT
    {.gpio = GPIO_ID_SW_LEFT,  .state = BUTTONS_STATE_RELEASED, .counter = 0, .callback = NULL}, // BUTTONS_ID_VIEW_LEFT
};

/** Application thread attributes. */
const osThreadAttr_t buttons_thread_attr =
{
    .name = "BUTTONS",
    .stack_size = 512,
    .priority = osPriorityNormal,
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   View right (@ref BUTTONS_ID_VIEW_RIGHT) button callback function.
 *
 * @param   id  Button ID. See @ref buttons_id_t.
 */
static void buttons_cb_view_right(buttons_id_t id);

/**
 * @brief   View left (@ref BUTTONS_ID_VIEW_LEFT) button callback function.
 *
 * @param   id  Button ID. See @ref buttons_id_t.
 */
static void buttons_cb_view_left(buttons_id_t id);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool buttons_init(void)
{
    buttons_data[BUTTONS_ID_VIEW_RIGHT].callback = &buttons_cb_view_right;
    buttons_data[BUTTONS_ID_VIEW_LEFT].callback = &buttons_cb_view_left;

    // Create application thread.
    if((buttons_thread_id = osThreadNew(&buttons_thread, NULL, &buttons_thread_attr)) == NULL)
    {
        return false;
    }

    return true;
}

void buttons_thread(void *arguments)
{
    buttons_id_t b = (buttons_id_t)0;

    while(1)
    {
        for(b = (buttons_id_t)0; b < BUTTONS_ID_LAST; b++)
        {
            if(gpio_input_get(buttons_data[b].gpio) != true)
            {
                buttons_data[b].counter++;
                if(buttons_data[b].counter > BUTTONS_LONG_PRESS_DURATION && buttons_data[b].state != BUTTONS_STATE_PRESSED_LONG)
                {
                    //DEBUG("Button-%d long pressed.", b);
                    buttons_data[b].state = BUTTONS_STATE_PRESSED_LONG;
                    if(buttons_data[b].callback)
                    {
                        buttons_data[b].callback(b);
                    }
                }
            }
            else
            {
                if(buttons_data[b].counter >= BUTTONS_PRESS_DURATION && buttons_data[b].counter < BUTTONS_LONG_PRESS_DURATION)
                {
                    buttons_data[b].state = BUTTONS_STATE_PRESSED;
                    //DEBUG("Button-%d pressed.", b);
                    if(buttons_data[b].callback)
                    {
                        buttons_data[b].callback(b);
                    }
                }
                buttons_data[b].state = BUTTONS_STATE_RELEASED;
                buttons_data[b].counter = 0;
            }
        }
        osDelay(1);
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void buttons_cb_view_right(buttons_id_t id)
{
    buttons_data_t *button = &buttons_data[id];
    display_menu_id_t menu = display_get_menu();

    switch(button->state)
    {
        default:
        case BUTTONS_STATE_RELEASED:
        {
            break;
        }
        case BUTTONS_STATE_PRESSED:
        case BUTTONS_STATE_PRESSED_LONG:
        {
            if(display_power_state() == true)
            {
                menu++;
            }
            if(menu >= DISPLAY_MENU_ID_LAST)
            {
                menu = DISPLAY_MENU_ID_CLOCK;
            }
            display_set_menu(menu);
            break;
        }
    }

    return;
}

static void buttons_cb_view_left(buttons_id_t id)
{
    buttons_data_t *button = &buttons_data[id];
    display_menu_id_t menu = display_get_menu();

    switch(button->state)
    {
        default:
        case BUTTONS_STATE_RELEASED:
        {
            break;
        }
        case BUTTONS_STATE_PRESSED:
        case BUTTONS_STATE_PRESSED_LONG:
        {
            if(display_power_state() == true)
            {
                menu--;
            }
            if(menu == (display_menu_id_t)0)
            {
                menu = (display_menu_id_t)(DISPLAY_MENU_ID_LAST - 1);
            }
            display_set_menu(menu);
            break;
        }
    }

    return;
}

/**
 **********************************************************************************************************************
 * @file        display.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2018-02-12
 * @brief       This is C source file template.
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
#include <stdio.h>
#include <string.h>

#include "display/display.h"
#include "display/ssd1306.h"
#include "display/display_menu.h"
#include "display/display_popup.h"

#include "debug.h"

#include "cmsis_os2.h"
#include "bsp.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define DISPLAY_TIMEOUT         20000   //!< Display ON timeout in milliseconds.
#define DISPLAY_DIM_ON_DELAY    1       //!< Dim ON step delay in milliseconds.
#define DISPLAY_DIM_OFF_DELAY   1       //!< Dim OFF step delay in milliseconds.
#define DISPLAY_FIRST_MENU      1       //!< If 1 after wakeup displays first menu, else - last viewed.

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Display thread ID. */
osThreadId_t display_thread_id;
/** Display thread attributes. */
const osThreadAttr_t display_thread_attr =
{
    .name = "DISPLAY",
    .stack_size = 1024,
    .priority = osPriorityNormal,
};
/** Display timeout timer ID. */
osTimerId_t display_timer_id;
/** Indication timer attributes. */
const osTimerAttr_t display_timer_attr =
{
    .name = "DISPLAY_TIMER",
};
/** Display power control flag. */
volatile bool display_power_cntrl = true;
/** Display power state flag. */
volatile bool display_power_state_flag = false;
/** Current menu ID. See @ref display_menu_id_t. */
volatile display_menu_id_t display_curr_menu_id = (display_menu_id_t)0;
/** Last displayed menu ID. See @ref display_menu_id_t. */
volatile display_menu_id_t display_last_menu_id = (display_menu_id_t)0;
/** Display popup data. */
volatile display_popup_t display_popup_data = {0};
/** Display contrast. */
uint8_t display_contrast = 0;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
extern display_menu_t display_menu_list[DISPLAY_MENU_ID_LAST];

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Power ON display.
 *
 * @return  State of power ON.
 * @retval  0   failed.
 * @retval  1   success.
 */
static bool display_power_on(void);

/**
 * @brief   Power OFF display.
 *
 * @retval  State off power OFF
 * @retval  0   aborted.
 * @retval  1   success.
 */
static bool display_power_off(bool dim);

/**
 * @brief   Wakeup display from sleep.
 *
 * @return  State of wakeup.
 * @retval  0   failec to wakeup.
 * @retval  1   Wakeup successful.
 */
static bool display_wakeup(void);

/**
 * @brief   Put display to sleep.
 *
 * @param   dim Dim flag. If true will dim off, if false will shut instantly.
 *
 * @return  State of sleep.
 * @retval  0   No sleeping, failed.
 * @retval  1   Sleeping, success.
 */
static bool display_sleep(bool dim);

/**
 * @brief   Dim on display.
 */
static void display_dim_on(void);

/**
 * @brief   Dim off display.
 */
static void display_dim_off(void);

/**
 * @brief   Display delay function.
 *
 * @param   id  Menu ID. See @ref display_menu_id_t.
 */
static void display_delay(display_menu_id_t id);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool display_init(void)
{
    display_menu_init(DISPLAY_MENU_ID_WELCOME, 0, display_menu_cb_welcome);
    display_menu_init(DISPLAY_MENU_ID_CLOCK, 100, display_menu_cb_clock);
    display_menu_init(DISPLAY_MENU_ID_MOTOR, 100, display_menu_cb_motor);
    display_menu_init(DISPLAY_MENU_ID_RADIO, 100, display_menu_cb_radio);
    display_menu_init(DISPLAY_MENU_ID_INFO, 1000, display_menu_cb_info);

    display_set_menu(DISPLAY_MENU_ID_WELCOME);

    // Create display timeout timer.
    if((display_timer_id = osTimerNew(&display_timeout_handle, osTimerOnce, NULL, &display_timer_attr)) == NULL)
    {
        return false;
    }

    // Create display thread.
    if((display_thread_id = osThreadNew(&display_thread, NULL, &display_thread_attr)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}

void display_turn_off(void)
{
    __disable_irq();
    display_power_cntrl = false;
    __enable_irq();

    return;
}

void display_turn_on(void)
{
    __disable_irq();
    display_power_cntrl = true;
    __enable_irq();

    return;
}

bool display_power_state(void)
{
    return display_power_state_flag && display_power_cntrl ? true : false;
}

display_menu_id_t display_get_menu(void)
{
    return display_curr_menu_id;
}

void display_set_menu(display_menu_id_t id)
{
    if(display_power_cntrl == true)
    {
        osTimerStart(display_timer_id, DISPLAY_TIMEOUT);
    }

    if(display_curr_menu_id == id && display_power_state_flag)
    {
        __disable_irq();
        display_power_cntrl = true;
        __enable_irq();
        return;
    }

    __disable_irq();
    display_curr_menu_id = id;
    display_menu_list[id].enable = true;
    display_menu_list[id].init = false;
    display_power_cntrl = true;
    __enable_irq();

    return;
}

void display_set_popup(uint8_t *text, uint32_t timeout)
{
    if(display_power_cntrl == true)
    {
        osTimerStart(display_timer_id, DISPLAY_TIMEOUT);
    }

    __disable_irq();
    display_popup_data.text = text;
    display_popup_data.timeout = timeout == 0 ? UINT32_MAX : timeout;
    display_popup_data.active = true;
    display_power_cntrl = true;
    __enable_irq();

    return;
}

void display_clear_popup(void)
{
    __disable_irq();
    display_popup_data.timeout = 0;
    display_popup_data.active = false;
    __enable_irq();

    return;
}

void display_thread(void *arguments)
{
    display_menu_id_t menu_id = DISPLAY_MENU_ID_WELCOME;
    display_popup_t popup_data = {0};

    osDelay(10);

    // Delay while need to power ON.
    while(!display_power_cntrl)
    {
        osDelay(100);
    }
    // Power ON.
    display_power_on();
    display_power_cntrl = true;

    while(1)
    {
#if 0
        // Delay while need to power ON.
        while(!display_power_cntrl)
        {
            osDelay(100);
        }
        // Power ON.
        display_power_on();
#endif
        // While is power ON.
        while(display_power_cntrl)
        {
            // Draw pop-up.
            if(display_popup_data.active)
            {
                osTimerStop(display_timer_id);
                memcpy(&popup_data, (display_popup_t *)&display_popup_data, sizeof(display_popup_t));
                display_popup_data.active = false;
                display_popup_view(&popup_data);
                while(!display_popup_data.active)
                {
                    osDelay(1);
                    if(popup_data.timeout > 0)
                    {
                        popup_data.timeout--;
                    }
                    else
                    {
                        popup_data.timeout = 0;
                        break;
                    }
                }
                osTimerStart(display_timer_id, DISPLAY_TIMEOUT);
                ssd1306_fill(SSD1306_COLOR_BLACK);
                display_menu_list[menu_id].init = false;
                display_menu_list[menu_id].cb(menu_id);
                display_menu_list[menu_id].init = true;
            }
            // Draw menu: header, content.
            menu_id = display_curr_menu_id;
            display_last_menu_id = menu_id;
            if(display_menu_list[menu_id].enable == true)
            {
                if(display_menu_list[menu_id].init == false)
                {
                    ssd1306_fill(SSD1306_COLOR_BLACK);
                }
                if(display_menu_list[menu_id].cb != NULL)
                {
                    display_menu_list[menu_id].cb(menu_id);
                }
                if(display_menu_list[menu_id].period)
                {
                    display_delay(menu_id);
                }
                else
                {
                    display_menu_list[menu_id].enable = false;
                }
                display_menu_list[menu_id].init = true;
            }
            else
            {
                display_delay(menu_id);
            }
        }
#if 0
        // Power OFF.
        if(display_power_off(true))
        {
            display_menu_list[menu_id].init = false;
        }
#else
        // Sleep.
        if(display_sleep(true))
        {
            display_menu_list[menu_id].init = false;
        }
#endif
        // Delay while need to power ON.
        while(!display_power_cntrl)
        {
            osDelay(100);
        }
        // Power ON.
        display_wakeup();
    }
}

void display_timeout_handle(void *arguments)
{
    __disable_irq();
    display_power_cntrl = false;
    __enable_irq();

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static bool display_power_on(void)
{
    if(display_power_state_flag)
    {
        return true;
    }
    //ssp_1_init();
    //gpio_output_low(GPIO_ID_OLED_POWER);
    gpio_output_high(GPIO_ID_DISPLAY_SELECT);
    gpio_output_high(GPIO_ID_DISPLAY_DC);
    gpio_output_high(GPIO_ID_DISPLAY_RESTART);
    osDelay(1);
    gpio_output_low(GPIO_ID_DISPLAY_RESTART);
    osDelay(1);
    gpio_output_high(GPIO_ID_DISPLAY_RESTART);
    if(ssd1306_init() == false)
    {
        DEBUG_DISPLAY("SSD1306 init failed.");
        display_power_off(false);
        return false;
    }
    osTimerStart(display_timer_id, DISPLAY_TIMEOUT);
    display_contrast = 0;
    ssd1306_set_contrast(display_contrast);

    display_menu_list[display_last_menu_id].cb(display_last_menu_id);
    display_menu_list[display_last_menu_id].init = true;

    display_dim_on();
    DEBUG_DISPLAY("Display ON.");
    display_power_state_flag = true;

    return true;
}

static bool display_power_off(bool dim)
{
    if(dim)
    {
        display_dim_off();
    }
    if(display_power_cntrl)
    {
        display_dim_on();
        osTimerStart(display_timer_id, DISPLAY_TIMEOUT);
        display_power_state_flag = true;
        return false;
    }
    ssd1306_off();
    // If we de'initialize SSP1, somehow SSP0 stops working.
    // ssp_1_deinit();
    // gpio_output_high(GPIO_ID_OLED_POWER);
    gpio_output_low(GPIO_ID_DISPLAY_RESTART);
    gpio_output_low(GPIO_ID_DISPLAY_SELECT);
    gpio_output_low(GPIO_ID_DISPLAY_DC);
    DEBUG_DISPLAY("Display OFF.");
    display_power_state_flag = false;

    return true;
}

static bool display_wakeup(void)
{
    if(display_power_state_flag)
    {
        return true;
    }
    ssd1306_on();
    ssd1306_set_contrast(0);
    display_contrast = 0;

    osTimerStart(display_timer_id, DISPLAY_TIMEOUT);
#if DISPLAY_FIRST_MENU
    // Always display first menu after wakeup.
    __disable_irq();
    //display_curr_menu_id = (display_menu_id_t)1;
    display_menu_list[display_curr_menu_id].enable = true;
    display_menu_list[display_curr_menu_id].init = false;
    __enable_irq();
    display_menu_list[display_curr_menu_id].cb(display_curr_menu_id);
#else
    // Always display last menu after wakeup.
    display_menu_list[display_last_menu_id].cb(display_last_menu_id);
    display_menu_list[display_last_menu_id].init = true;
#endif
    display_dim_on();
    DEBUG_DISPLAY("Display ON.");
    display_power_state_flag = true;

    return true;
}

static bool display_sleep(bool dim)
{
    if(dim)
    {
        display_dim_off();
    }
    if(display_power_cntrl)
    {
        display_dim_on();
        osTimerStart(display_timer_id, DISPLAY_TIMEOUT);
        display_power_state_flag = true;
        return false;
    }
    ssd1306_off();
    DEBUG_DISPLAY("Display sleep.");
    display_power_state_flag = false;

    return true;
}

static void display_dim_on(void)
{
    uint8_t c = display_contrast;

    while(c++ < 0xFF)
    {
        ssd1306_set_contrast(c);
        osDelay(DISPLAY_DIM_ON_DELAY);
    }
    display_contrast = 0xFF;

    return;
}

static void display_dim_off(void)
{
    uint8_t c = display_contrast;

    while(c--)
    {
        ssd1306_set_contrast(c);
        osDelay(DISPLAY_DIM_OFF_DELAY);
        if(display_power_cntrl == true)
        {
            display_contrast = c;
            return;
        }
    }
    display_contrast = 0;

    return;
}

static void display_delay(display_menu_id_t menu)
{
    static uint32_t c = 0;
    uint32_t delay = display_menu_list[menu].period == 0 ? 10 : display_menu_list[menu].period;

    while(delay--)
    {
        osDelay(1);
        c++;
        if(c > 100)
        {
            c = 0;
        }
        if(menu != display_curr_menu_id || display_popup_data.active)
        {
            break;
        }
    }

    return;
}

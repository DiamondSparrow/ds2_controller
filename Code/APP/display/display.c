/**
 **********************************************************************************************************************
 * @file         display.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-09-01
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

#include "display/display.h"
#include "display/ssd1306.h"

#include "cmsis_os.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
/** Define application thread */
osThreadDef(display_thread, osPriorityNormal, 1, 1024);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef void (*display_cb_t)(display_menu_id_t id);

typedef struct
{
    uint32_t period;
    display_cb_t cb;
    bool enable;
    bool init;
} display_menu_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Display thread ID. */
osThreadId display_thread_id;

display_menu_id_t display_menu_id = DISPLAY_MENU_ID_WELCOME;
display_menu_t display_menus[DISPLAY_MENU_ID_LAST] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void display_menu_init(display_menu_id_t id, uint32_t period, display_cb_t cb);

static void display_meniu_cb_welcome(display_menu_id_t id);
static void display_meniu_cb_clock(display_menu_id_t id);
static void display_meniu_cb_light(display_menu_id_t id);
static void display_meniu_cb_temperature(display_menu_id_t id);
static void display_meniu_cb_humidity(display_menu_id_t id);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool display_init(void)
{
    if(ssd1306_init() == false)
    {
        return false;
    }

    display_menu_init(DISPLAY_MENU_ID_WELCOME, 0, display_meniu_cb_welcome);
    display_menu_init(DISPLAY_MENU_ID_CLOCK, 0, display_meniu_cb_clock);
    display_menu_init(DISPLAY_MENU_ID_LIGHT, 0, display_meniu_cb_light);
    display_menu_init(DISPLAY_MENU_ID_TEMPERATURE, 0, display_meniu_cb_temperature);
    display_menu_init(DISPLAY_MENU_ID_HUMIDITY, 0, display_meniu_cb_humidity);

    display_menu_set(DISPLAY_MENU_ID_WELCOME);

    // Create display thread.
    if((display_thread_id = osThreadCreate(osThread(display_thread), NULL)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}

void display_thread(void const *arg)
{
    display_menu_id_t id = DISPLAY_MENU_ID_WELCOME;

    osDelay(500);
    ssd1306_update_screen();
    osDelay(10);

    while(1)
    {
        id = display_menu_id;
        if(display_menus[id].enable == true)
        {
            if(display_menus[id].init == false)
            {
                ssd1306_fill(SSD1306_COLOR_BLACK);
                ssd1306_update_screen();
                osDelay(10);
            }
            if(display_menus[id].cb != NULL)
            {
                display_menus[id].cb(id);
            }
            if(display_menus[id].period)
            {
                osDelay(display_menus[id].period);
            }
            else
            {
                display_menus[id].enable = false;
            }
            display_menus[id].init = true;
        }
        else
        {
            osDelay(10);
        }
    }
}

void display_menu_set(display_menu_id_t id)
{
    //__disable_irq();
    display_menu_id = id;
    display_menus[id].enable = true;
    display_menus[id].init = false;
    //__enable_irq();

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void display_menu_init(display_menu_id_t id, uint32_t period, display_cb_t cb)
{
    display_menus[id].period = period;
    display_menus[id].cb = cb;
    display_menus[id].enable = false;
    display_menus[id].init = false;

    return;
}

static void display_meniu_cb_welcome(display_menu_id_t id)
{
    /*
    ssd1306_draw_rectangle(2, 11, 128, 64, SSD1306_COLOR_BLACK);
    ssd1306_update_screen();
    osDelay(10);
*/
    ssd1306_goto_xy(37, 29);
    ssd1306_puts((uint8_t *)"Hi ;)", &fonts_11x18, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    osDelay(10);

    return;
}

static void display_meniu_cb_clock(display_menu_id_t id)
{
    ssd1306_goto_xy(46, 16);
    ssd1306_puts((uint8_t *)"Clock", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    osDelay(1);
    ssd1306_goto_xy(20, 29);
    ssd1306_puts((uint8_t *)"22:45:00", &fonts_11x18, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    osDelay(1);
    ssd1306_goto_xy(29, 50);
    ssd1306_puts((uint8_t *)"1988-07-19", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    osDelay(10);

    return;
}

static void display_meniu_cb_light(display_menu_id_t id)
{
    if(display_menus[id].init == false)
    {
        ssd1306_goto_xy(46, 16);
        ssd1306_puts((uint8_t *)"Light", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(1);
        ssd1306_goto_xy(58, 29);
        ssd1306_puts((uint8_t *)"?", &fonts_11x18, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(1);
        ssd1306_goto_xy(56, 50);
        ssd1306_puts((uint8_t *)"lx.", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(10);
    }

    return;
}

static void display_meniu_cb_temperature(display_menu_id_t id)
{
    if(display_menus[id].init == false)
    {
        ssd1306_goto_xy(25, 16);
        ssd1306_puts((uint8_t *)"Temperature", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(1);
        ssd1306_goto_xy(58, 29);
        ssd1306_puts((uint8_t *)"?", &fonts_11x18, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(1);
        ssd1306_goto_xy(46, 50);
        ssd1306_puts((uint8_t *)"degC.", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(10);
    }

    return;
}

static void display_meniu_cb_humidity(display_menu_id_t id)
{
    if(display_menus[id].init == false)
    {
        ssd1306_goto_xy(36, 16);
        ssd1306_puts((uint8_t *)"Humidity", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(1);
        ssd1306_goto_xy(58, 29);
        ssd1306_puts((uint8_t *)"?", &fonts_11x18, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(1);
        ssd1306_goto_xy(50, 50);
        ssd1306_puts((uint8_t *)"bar.", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_update_screen();
        osDelay(10);
    }

    return;
}

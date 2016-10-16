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
#include <stdio.h>
#include <string.h>

#include "display/display.h"
#include "display/ssd1306.h"

#include "sensors/sensors.h"
#include "sensors/joystick.h"
#include "motor/motor.h"

#include "cmsis_os.h"
#include "bsp.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define DISPLAY_LINE_X      3
#define DISPLAY_LINE_Y_1    18
#define DISPLAY_LINE_Y_2    29
#define DISPLAY_LINE_Y_3    40
#define DISPLAY_LINE_Y_4    51

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

volatile display_menu_id_t display_menu_id = DISPLAY_MENU_ID_WELCOME;
volatile display_menu_t display_menus[DISPLAY_MENU_ID_LAST] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void display_menu_init(display_menu_id_t id, uint32_t period, display_cb_t cb);

static void display_menu_header(display_menu_id_t id, uint8_t *str);

static void display_meniu_cb_welcome(display_menu_id_t id);
static void display_meniu_cb_clock(display_menu_id_t id);
#if DISPLAY_EXTRA
static void display_meniu_cb_light(display_menu_id_t id);
static void display_meniu_cb_temperature(display_menu_id_t id);
static void display_meniu_cb_humidity(display_menu_id_t id);
#endif // DISPLAY_EXTRA
static void display_meniu_cb_joystick(display_menu_id_t id);
static void display_meniu_cb_motor(display_menu_id_t id);
static void display_meniu_cb_info(display_menu_id_t id);

static void display_delay(display_menu_id_t id);
static void display_contrast_control(void);

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
    display_menu_init(DISPLAY_MENU_ID_CLOCK, 1000, display_meniu_cb_clock);
#if DISPLAY_EXTRA
    display_menu_init(DISPLAY_MENU_ID_LIGHT, 100, display_meniu_cb_light);
    display_menu_init(DISPLAY_MENU_ID_TEMPERATURE, 100, display_meniu_cb_temperature);
    display_menu_init(DISPLAY_MENU_ID_HUMIDITY, 100, display_meniu_cb_humidity);
#endif // DISPLAY_EXTRA
    display_menu_init(DISPLAY_MENU_ID_JOYSTICK, 50, display_meniu_cb_joystick);
    display_menu_init(DISPLAY_MENU_ID_MOTOR, 50, display_meniu_cb_motor);
    display_menu_init(DISPLAY_MENU_ID_INFO, 1000, display_meniu_cb_info);

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
            }
            if(display_menus[id].cb != NULL)
            {
                display_menus[id].cb(id);
            }
            if(display_menus[id].period)
            {
                display_delay(id);
            }
            else
            {
                display_menus[id].enable = false;
            }
            display_menus[id].init = true;
        }
        else
        {
            display_delay(id);
        }
    }
}

void display_menu_set(display_menu_id_t id)
{
    __disable_irq();
    display_menu_id = id;
    display_menus[id].enable = true;
    display_menus[id].init = false;
    __enable_irq();

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

static void display_menu_header(display_menu_id_t id, uint8_t *str)
{
    uint8_t tmp[24] = {0};

    if(display_menus[id].init == false)
    {
        //ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_COLOR_WHITE);
        snprintf((char *)tmp, 24, "< %-13.13s >", str);

        ssd1306_draw_filled_rectangle(0, 0, SSD1306_WIDTH, 13, SSD1306_COLOR_WHITE);
        ssd1306_goto_xy(4, 3);
        ssd1306_puts((uint8_t *)tmp, &fonts_7x10, SSD1306_COLOR_BLACK);

        ssd1306_update_screen();
    }

    return;
}

static void display_meniu_cb_welcome(display_menu_id_t id)
{
    //ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_COLOR_WHITE);

    ssd1306_goto_xy(40, 20);
    ssd1306_puts((uint8_t *)"DS-2", &fonts_11x18, SSD1306_COLOR_WHITE);
    ssd1306_goto_xy(25, 40);
    ssd1306_puts((uint8_t *)"Controller", &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

static void display_meniu_cb_clock(display_menu_id_t id)
{
    uint8_t tmp[16] = {0};
    uint32_t timestamp = rtc_get();
    struct tm clock = {0};

    ConvertRtcTime(timestamp, &clock);

    display_menu_header(id, "Clock");

    ssd1306_goto_xy(20, 23);
    snprintf((char *)tmp, sizeof(tmp), "%02d:%02d:%02d", clock.tm_hour, clock.tm_min, clock.tm_sec);
    ssd1306_puts((uint8_t *)tmp, &fonts_11x18, SSD1306_COLOR_WHITE);
    ssd1306_goto_xy(29, 44);
    snprintf((char *)tmp, sizeof(tmp), "%04d-%02d-%02d", clock.tm_year + TM_YEAR_BASE, clock.tm_mon + 1, clock.tm_mday);
    ssd1306_puts((uint8_t *)tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}
#if DISPLAY_EXTRA
static void display_meniu_cb_light(display_menu_id_t id)
{
    uint8_t tmp[16] = {0};
    uint8_t offset_x = 0;
    uint16_t value = sensors_data.light.value_lp;

    if(display_menus[id].init == false)
    {
        ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_COLOR_WHITE);
        ssd1306_goto_xy(46, 10);
        ssd1306_puts((uint8_t *)"Light", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_goto_xy(56, 44);
        ssd1306_puts((uint8_t *)"lx.", &fonts_7x10, SSD1306_COLOR_WHITE);
    }

    snprintf((char *)tmp, 18, "%d", value);
    offset_x = (128 - (strlen((char *)tmp)  * 11)) / 2;
    ssd1306_goto_xy(3, 23);
    ssd1306_puts((uint8_t *)"            ", &fonts_11x18, SSD1306_COLOR_WHITE);
    snprintf((char *)tmp, sizeof(tmp), "%d", value);
    ssd1306_goto_xy(offset_x, 23);
    ssd1306_puts((uint8_t *)tmp, &fonts_11x18, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

static void display_meniu_cb_temperature(display_menu_id_t id)
{
    uint8_t tmp[16] = {0};
    uint8_t offset_x = 0;
    uint16_t value = sensors_data.temperature.value;

    if(display_menus[id].init == false)
    {
        ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_COLOR_WHITE);
        ssd1306_goto_xy(25, 10);
        ssd1306_puts((uint8_t *)"Temperature", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_goto_xy(48, 44);
        ssd1306_puts((uint8_t *)"degC.", &fonts_7x10, SSD1306_COLOR_WHITE);
    }

    snprintf((char *)tmp, 18, "%d", value);
    offset_x = (128 - (strlen((char *)tmp)  * 11)) / 2;
    ssd1306_goto_xy(3, 23);
    ssd1306_puts((uint8_t *)"            ", &fonts_11x18, SSD1306_COLOR_WHITE);
    snprintf((char *)tmp, sizeof(tmp), "%d", value);
    ssd1306_goto_xy(offset_x, 23);
    ssd1306_puts((uint8_t *)tmp, &fonts_11x18, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

static void display_meniu_cb_humidity(display_menu_id_t id)
{
    uint8_t tmp[16] = {0};
    uint8_t offset_x = 0;
    uint16_t value = sensors_data.humidity.value;

    if(display_menus[id].init == false)
    {
        ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_COLOR_WHITE);
        ssd1306_goto_xy(36, 10);
        ssd1306_puts((uint8_t *)"Humidity", &fonts_7x10, SSD1306_COLOR_WHITE);
        ssd1306_goto_xy(60, 44);
        ssd1306_puts((uint8_t *)"%", &fonts_7x10, SSD1306_COLOR_WHITE);
    }


    snprintf((char *)tmp, 18, "%d", value);
    offset_x = (128 - (strlen((char *)tmp)  * 11)) / 2;
    ssd1306_goto_xy(3, 23);
    ssd1306_puts((uint8_t *)"            ", &fonts_11x18, SSD1306_COLOR_WHITE);
    snprintf((char *)tmp, sizeof(tmp), "%d", value);
    ssd1306_goto_xy(offset_x, 23);
    ssd1306_puts((uint8_t *)tmp, &fonts_11x18, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}
#endif // DISPLAY_EXTRA

static void display_meniu_cb_joystick(display_menu_id_t id)
{
    uint8_t tmp[18] = {0};
    int32_t magn = 0;
    int32_t dir = 0;

    display_menu_header(id, "Joystick");

    snprintf((char *)tmp, 18, "X:  %d       ", joystick_get_x(JOYSTICK_ID_1));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "Y:  %d       ", joystick_get_y(JOYSTICK_ID_1));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    joystick_get_vector(JOYSTICK_ID_1, &magn, &dir);
    snprintf((char *)tmp, 18, "V:  %d %d      ", magn, dir);
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_3);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "SW: %01d   ", joystick_get_sw(JOYSTICK_ID_1));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_4);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

static void display_meniu_cb_motor(display_menu_id_t id)
{
    uint8_t tmp[18] = {0};

    display_menu_header(id, "Motor");

    snprintf((char *)tmp, 18, "L.S: %d / %d   ", motor_get_speed_current(MOTOR_ID_LEFT), motor_get_speed_target(MOTOR_ID_LEFT));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "L.C: %d mA.    ", motor_get_current(MOTOR_ID_LEFT));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "R.S: %d / %d   ", motor_get_speed_current(MOTOR_ID_RIGHT), motor_get_speed_target(MOTOR_ID_RIGHT));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_3);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "R.C: %d mA.    ", motor_get_current(MOTOR_ID_RIGHT));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_4);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

static void display_meniu_cb_info(display_menu_id_t id)
{
    uint8_t tmp[18] = {0};

    display_menu_header(id, "Info");

    snprintf((char *)tmp, 18, "Ver: 1.1-a1");
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "Clk: %ld MHz.", SystemCoreClock / 1000000);
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "Tmp: %.02f degC.", adc_get_temperature());
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_LINE_Y_3);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

static void display_delay(display_menu_id_t id)
{
    static uint32_t c = 0;
    uint32_t delay = display_menus[id].period;

    while(delay--)
    {
        osDelay(1);
        c++;
        if(c > 100)
        {
            c = 0;
            display_contrast_control();
        }
        if(id != display_menu_id)
        {
            break;
        }
    }

    return;
}

static void display_contrast_control(void)
{
    uint16_t ligh_level = 128;

    if(sensors_data.light.state == true)
    {
        ligh_level = (sensors_data.light.value_lp * 256 / 512);
        ligh_level = ligh_level > 255 ? 255 : ligh_level;
        //ligh_level = sensors_data.light.value_lp > 255 ? 255 : sensors_data.light.value_lp;
    }
    ssd1306_set_contrast(ligh_level);

    return;
}

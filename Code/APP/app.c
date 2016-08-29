/**
 **********************************************************************************************************************
 * @file         app.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         Apr 6, 2016
 * @brief        Application C source file.
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
#include "cmsis_os.h"

#include "app.h"
#include "debug.h"
#include "indication.h"
#include "bsp.h"

#include "cli/cli_app.h"
#include "display/ssd1306.h"
#include "servo/servo.h"
#include "ultrasonic/ultrasonic.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
/** Define application thread */
osThreadDef(app_thread, osPriorityNormal, 1, 256);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Application thread ID. */
osThreadId app_thread_id;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void app_delay_us(volatile uint32_t us);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
/**
 * @brief   Main function.
 *
 * @return  None.
 */
int main(void)
{
    SystemCoreClockUpdate();
    bsp_init();
    indication_set_blocking(INDICATION_BOOT);
    DEBUG_BOOT("");
    DEBUG_BOOT("     .-'/ ,_  \'-.   ");
    DEBUG_BOOT("    /  (  ( >  )  \\ ");
    DEBUG_BOOT("    \\   '-' '-'   / ");
    DEBUG_BOOT("    '-..__ __..-'    ");
    DEBUG_BOOT("          /_\\       ");
    DEBUG_BOOT(" # DS-2 Controller #");
    DEBUG_BOOT(" * Booting.");
    DEBUG_BOOT("Build ....... %s %s", __DATE__, __TIME__);
    DEBUG_BOOT("Core Clock .. %ld MHz.", SystemCoreClock);
    
    // Setup and initialize peripherals.
    DEBUG_BOOT("BSP ......... ok.");

    // initialize RTOS kernel.
    if(osKernelInitialize() != osOK)
    {
        // Invoke application error function.
        app_error();
    }
    DEBUG_BOOT("Kernel ...... ok.");

    // Create application thread.
    if((app_thread_id = osThreadCreate(osThread(app_thread), NULL)) == NULL)
    {
        // Failed to create a thread
        app_error();
    }
    DEBUG_BOOT("APP ......... ok.");


    // Start kernel.
    if(osKernelStart() != osOK)
    {
        // app_error application error function.
        app_error();
    }

    return 0;
}

void app_thread(void const *arg)
{
    debug_init();
    DEBUG_INIT(" * Initializing.");
    indication_set_blocking(INDICATION_INIT);
    indication_init();
    DEBUG_INIT("Indication .. ok.");
    cli_app_init();
    DEBUG_INIT("CLI APP ..... ok.");
    servo_init();
    DEBUG_INIT("Servo ....... ok.");
    ultrasonic_init();
    DEBUG_INIT("Ultrasonic .. ok.");
    ssd1306_init();
    DEBUG_INIT("Display ..... ok.");

    DEBUG(" * Running.");
    indication_set(INDICATION_STANDBY);
    DEBUG("State: standby.");

    ssd1306_draw_rectangle(2, 1, 128, 64, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    ssd1306_draw_filled_rectangle(2, 1, 128, 10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();

    ssd1306_goto_xy(3, 13);
    ssd1306_puts("ABCDEFGHIJKLMNOPRS", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    ssd1306_goto_xy(3, 23);
    ssd1306_puts("ABCDEFGHIJKLMNOPRS", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    ssd1306_goto_xy(3, 33);
    ssd1306_puts("ABCDEFGHIJKLMNOPRS", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    ssd1306_goto_xy(3, 43);
    ssd1306_puts("ABCDEFGHIJKLMNOPRS", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();
    ssd1306_goto_xy(3, 53);
    ssd1306_puts("ABCDEFGHIJKLMNOPRS", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_update_screen();

    osDelay(5000);
    ssd1306_power_off();

    while(1)
    {
        osDelay(100);
    }
}

void app_error(void)
{
    indication_set_blocking(INDICATION_RED);
    debug_send_blocking((uint8_t *)"ERROR!\r", 7);

    while(1)
    {
        __nop();
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void app_delay_us(volatile uint32_t us)
{
    /* Go to clock cycles */
    us *= (SystemCoreClock / 1000000) / 8;

    /* Wait till done */
    while(us--)
    {
        __nop();
    }

    return;
}

void HardFault_Handler(void)
{
    debug_send_blocking((uint8_t *)"HARD FAULT!\r", 13);

    NVIC_SystemReset();

    return;
}

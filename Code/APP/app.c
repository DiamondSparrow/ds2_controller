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
#include <stdio.h>
#include <string.h>

#include "chip.h"
#include "cmsis_os2.h"

#include "app.h"
#include "buttons.h"
#include "debug.h"
#include "indication.h"
#include "bsp.h"

#include "cli/cli_app.h"
#include "display/display.h"
#include "display/ssd1306.h"
#include "motor/motor.h"
#include "motor/servo.h"
#include "sensors/am2301.h"
#include "sensors/sensors.h"
#include "sensors/joystick.h"
#include "sensors/ultrasonic.h"
#include "radio/radio.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Application thread attributes. */
const osThreadAttr_t app_thread_attr =
{
    .name = "APP",
    .stack_size = 1024,
    .priority = osPriorityNormal,
};

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Application thread ID. */
osThreadId_t app_thread_id;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Application main thread.
 *
 * @param   arguments   Pointer to thread arguments.
 */
static void app_thread(void *arguments);

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
    DEBUG_BOOT("%-15.15s %s %s", "Build:", __DATE__, __TIME__);
    DEBUG_BOOT("%-15.15s %ld Hz.", "Core Clock:", SystemCoreClock);
    
    // Setup and initialize peripherals.
    DEBUG_BOOT("%-15.15s ok.", "BSP:");

    // Initialize and start Event Recorder
    // EventRecorderInitialize(EventRecordError, 1U);

    // Initialize RTOS kernel.
    if(osKernelInitialize() != osOK)
    {
        DEBUG_BOOT("%-15.15s err.", "RTOS Kernel:");
        // Invoke error function.
        app_error();
    }
    DEBUG_BOOT("%-15.15s ok.", "RTOS Kernel:");

    // Initialize application thread.
    if((app_thread_id = osThreadNew(&app_thread, NULL, &app_thread_attr)) == NULL)
    {
        DEBUG_BOOT("%-15.15s err.", "APP:");
        // Invoke error function.
        app_error();
    }
    DEBUG_BOOT("%-15.15s ok.", "APP:");

    // Start kernel, if not ready.
    if(osKernelGetState() == osKernelReady)
    {
        if(osKernelStart() != osOK)
        {
            DEBUG_BOOT("%-15.15s err.", "OS Start:");
            // Invoke error function.
            app_error();
        }
    }

    return 0;
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
static void app_thread(void *arguments)
{
    uint8_t ret = 0;

    debug_init();
    DEBUG_INIT(" * Initializing.");

    indication_set_blocking(INDICATION_INIT);
    indication_init();
    DEBUG_INIT("%-15.15s ok.", "Indication:");
    cli_app_init();
    DEBUG_INIT("%-15.15s ok.", "CLI APP:");
    //ret = sensors_init();
    //DEBUG_INIT("%-15.15s %s.", "Sensors:", ret == false ? "err" : "ok");
    ret = buttons_init();
    DEBUG_INIT("%-15.15s %s.", "Buttons:", ret == false ? "err" : "ok");
    ret = display_init();
    DEBUG_INIT("%-15.15s %s.", "Display:", ret == false ? "err" : "ok");
    ret = motor_init();
    DEBUG_INIT("%-15.15s %s.", "Motor:", ret == false ? "err" : "ok");
    ret = servo_init();
    DEBUG_INIT("%-15.15s %s.", "Servo:", ret == false ? "err" : "ok");
    ret = radio_init();
    DEBUG_INIT("%-15.15s %s.", "Radio:", ret == false ? "err" : "ok");

    rtc_get_from_build();

    DEBUG(" * Running.");
    indication_set(INDICATION_STANDBY);
    DEBUG("State: standby.");

    display_set_menu(DISPLAY_MENU_ID_MOTOR);
    osDelay(3000);
    motor_test_all();

    while(1)
    {
        osDelay(100);
    }
}


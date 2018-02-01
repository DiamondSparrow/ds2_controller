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
#include "debug.h"
#include "indication.h"
#include "bsp.h"

#include "cli/cli_app.h"
#include "display/display.h"
#include "display/ssd1306.h"
#include "motor/motor.h"
#include "sensors/am2301.h"
#include "sensors/sensors.h"
#include "sensors/joystick.h"
#include "sensors/ultrasonic.h"
#include "servo/servo.h"
#include "radio/nrf24l01.h"

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
#define RADIO_MODE  0   // 1 - RX, 0 - TX
static void app_thread(void *arguments)
{
    //display_menu_id_t menu_id = DISPLAY_MENU_ID_WELCOME;
    uint8_t ret = 0;
    //uint8_t c = 0;
    //uint8_t d = 0;
    //bool sw_left = false;
    //bool sw_right = false;
#if RADIO_MODE
    uint8_t data[32] = {0};
#endif

    debug_init();
    DEBUG_INIT(" * Initializing.");
    indication_set_blocking(INDICATION_INIT);
    indication_init();

    DEBUG_INIT("%-15.15s ok.", "Indication:");
    cli_app_init();
    DEBUG_INIT("%-15.15s ok.", "CLI APP:");
    //servo_init();
    //DEBUG_INIT("%-15.15s ok.", "Servo:");
    //ultrasonic_init();
    //DEBUG_INIT("%-15.15s ok.", "Ultrasonic:");
    //ret = joystick_init();
    //DEBUG_INIT("%-15.15s %s.", "Joystick:", ret == false ? "err" : "ok");
    //ret = sensors_init();
    //DEBUG_INIT("%-15.15s %s.", "Sensors:", ret == false ? "err" : "ok");
    //ret = motor_init();
    //DEBUG_INIT("%-15.15s %s.", "Motor:", ret == false ? "err" : "ok");
    //ret = display_init();
    //DEBUG_INIT("%-15.15s %s.", "Display:", ret == false ? "err" : "ok");

    nrf24l01_init(1, 4);
    //nrf24l01_set_my_address((uint8_t []){0xE7,0xE7,0xE7,0xE7,0xE});
    nrf24l01_set_my_address((uint8_t []){0xD7,0xD7,0xD7,0xD7,0xD7});
    //nrf24l01_set_tx_address((uint8_t []){0xD7,0xD7,0xD7,0xD7,0xD7});
    nrf24l01_set_tx_address((uint8_t []){0xE7,0xE7,0xE7,0xE7,0xE});

    DEBUG(" * Running.");
    indication_set(INDICATION_STANDBY);
    DEBUG("State: standby.");
    
    rtc_get_from_build();

    osDelay(1000);

    //menu_id = DISPLAY_MENU_ID_CLOCK;
    //display_menu_set(menu_id);
    //motor_test_ramp(MOTOR_ID_LEFT, 10);
    //motor_test_ramp(MOTOR_ID_RIGHT, 10);
#if RADIO_MODE
    //nrf24l01_power_up_rx();
#else
    nrf24l01_transmit((uint8_t []){0x01, 0x02, 0x03, 0x04});
#endif

    while(1)
    {
#if RADIO_MODE
        osDelay(10);
        if(nrf24l01_data_ready())
        {
            nrf24l01_get_data(data);
            DEBUG("Data: %02X,%02X,%02X,%02X", data[0], data[1], data[2], data[3]);
            memset(data, 0, sizeof(data));
            //nrf24l01_power_up_rx();
        }
#else
        osDelay(100);
        ret = nrf24l01_get_tx_status();
        if(ret == NRF24L01_TX_STATUS_OK || ret == NRF24L01_TX_STATUS_LOST)
        {
            DEBUG_INIT("NRF24L01 TX status %02X", ret);
            nrf24l01_transmit((uint8_t []){0x01, 0x02, 0x03, 0x04});
        }
#endif
        
        /*
        d = 20;
        c = 0;
        while(d--)
        {
            if(gpio_input_get(GPIO_SW_LEFT) == false)
            {
                c++;
            }
            osDelay(1);
        }
        if(c >= 10)
        {
            if(sw_left == false)
            {
                menu_id--;
                if(menu_id == 0)
                {
                    menu_id = (display_menu_id_t)(DISPLAY_MENU_ID_LAST - 1);
                }
                display_menu_set(menu_id);
            }
            sw_left = true;
        }
        else
        {
            sw_left = false;
        }
        osDelay(10);
        d = 20;
        c = 0;
        while(d--)
        {
            if(gpio_input_get(GPIO_SW_RIGHT) == false)
            {
                c++;
            }
            osDelay(1);
        }
        if(c >= 10)
        {
            if(sw_right == false)
            {
                menu_id++;
                if(menu_id == DISPLAY_MENU_ID_LAST)
                {
                    menu_id = DISPLAY_MENU_ID_CLOCK;
                }
                display_menu_set(menu_id);
            }
            sw_right = true;
        }
        else
        {
            sw_right = false;
        }
        */
    }
}

void HardFault_Handler(void)
{
    debug_send_blocking((uint8_t *)"HARD FAULT!\r", 13);

    NVIC_SystemReset();

    return;
}

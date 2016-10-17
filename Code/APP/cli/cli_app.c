/**
 **********************************************************************************************************************
 * @file         cli_app.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-06
 * @brief        Command Line Interface C source file.
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
#include <string.h>


#include "cmsis_os.h"

#include "cli_app.h"
#include "cli_cmd.h"
#include "cli.h"

#include "debug.h"
#include "bsp.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define CLI_APP_RX_SIZE     128
#define CLI_APP_TX_SIZE     128

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
/** Define CLI application thread */
osThreadDef(cli_app_thread, osPriorityNormal, 1, 512);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Application thread ID. */
osThreadId cli_app_thread_id;
uint8_t cli_app_rx_data[CLI_APP_RX_SIZE] = {0};
uint8_t cli_app_tx_data[CLI_APP_RX_SIZE] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static uint32_t cli_receive_line(uint8_t *data, uint32_t size);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool cli_app_init(void)
{
    cli_init();

    // Create application thread.
    if((cli_app_thread_id = osThreadCreate(osThread(cli_app_thread), NULL)) == NULL)
    {
        return false;
    }

    return true;
}

void cli_app_thread(void const *arg)
{
    uint32_t rx_count = 0;

    cli_cmd_register();

    while(1)
    {
        osDelay(10);
        memset(cli_app_rx_data, 0, CLI_APP_TX_SIZE);
        if((rx_count = cli_receive_line(cli_app_rx_data, CLI_APP_RX_SIZE)) < 3)
        {
            continue;
        }
        cli_app_rx_data[rx_count] = 0x00;
#if CLI_APP_ECHO
        DEBUG("%s", cli_app_rx_data);
#endif
        memset(cli_app_tx_data, 0, CLI_APP_TX_SIZE);
        if(cli_process_cmd(cli_app_rx_data, cli_app_tx_data, sizeof(cli_app_tx_data)) == true)
        {
            DEBUG("%s", cli_app_tx_data);
        }
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static uint32_t cli_receive_line(uint8_t *data, uint32_t size)
{
    uint32_t c = 0;
    uint8_t byte = 0;

    while(1)
    {
        if(uart_0_read_rb(&byte, 1) == 1)
        {
            if(byte == '\r')
            {
                break;
            }
            data[c] = byte;
            c++;
            if(c >= size)
            {
                break;
            }
        }
        else
        {
            osDelay(1);
        }
    }

    return c;
}

/**
 **********************************************************************************************************************
 * @file         debug.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-10
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
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "bsp.h"
#include "cmsis_os.h"

#include "debug.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
osSemaphoreDef(debug_lock);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
osSemaphoreId debug_lock_id;
uint8_t debug_buffer[DEBUG_BUFFER_SIZE] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool debug_init(void)
{
    if((debug_lock_id = osSemaphoreCreate(osSemaphore(debug_lock), 1)) == NULL)
    {
        return false;
    }

    return true;
}

void debug_send(const char *fmt, ...)
{
    uint16_t i = 0;
    va_list args;

    va_start(args, fmt);
    i = vsnprintf((char*)debug_buffer, DEBUG_BUFFER_SIZE - 1, fmt, args);
    debug_buffer[i] = 0x00;     // Make sure its zero terminated
    uart_0_send(debug_buffer, i);
    va_end(args);

    return;
}

void debug_send_os(const char *fmt, ...)
{
    uint16_t i = 0;
    va_list args;

    if (osSemaphoreWait(debug_lock_id, DEBUG_LOCK_TIMEOUT) >= 0)
    {
        va_start(args, fmt);
        i = vsnprintf((char*)debug_buffer, DEBUG_BUFFER_SIZE - 1, fmt, args);
        debug_buffer[i] = 0x00;     // Make sure its zero terminated
        uart_0_send(debug_buffer, i);
        va_end(args);
        osSemaphoreRelease(debug_lock_id);
    }

    return;
}

void debug_send_blocking(uint8_t *data, uint32_t size)
{
    uart_0_send(data, size);

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/


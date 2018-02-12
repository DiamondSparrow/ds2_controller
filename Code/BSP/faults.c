/**
 **********************************************************************************************************************
 * @file        faults.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2018-02-12
 * @brief       Faults C source file.
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

#include "faults.h"

#include "debug.h"

#include "chip.h"
#include "rtx_os.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define FAULTS_RESET_ON_ERROR       1   //!< If 1 will reset after fault, 0 - loop until someone reset, e.g. watchdog.
#define FAULTS_DEBUG_BUFFER_SIZE    64  //!< Faults debug buffer size in bytes.

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** NMI fault debug message. */
const uint8_t faults_nmi_msg[] = {"\r\nNMI!\r\n"};
/** Hard fault debug message. */
const uint8_t faults_hardfault_msg[] = {"\r\nHARDFAULT!\r\n"};
/** Faults debug buffer. */
uint8_t faults_debug_buffer[FAULTS_DEBUG_BUFFER_SIZE + 1] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
/**
 * @brief   NMI Handler.
 */
void NMI_Handler(void)
{
    debug_send_blocking((uint8_t *)faults_nmi_msg, sizeof(faults_nmi_msg));

#if FAULTS_RESET_ON_ERROR
    NVIC_SystemReset();
#else
    while(1)
    {
        __NOP();
    }
#endif

    return;
}

/**
 * @brief   Hard fault handler.
 */
void HardFault_Handler(void)
{
    debug_send_blocking((uint8_t *)faults_hardfault_msg, sizeof(faults_hardfault_msg));

#if FAULTS_RESET_ON_ERROR
    NVIC_SystemReset();
#else
    while(1)
    {
        __NOP();
    }
#endif

    return;
}

// OS Error Callback function
uint32_t osRtxErrorNotify(uint32_t code, void *object_id)
{
    uint32_t size = 0;

    switch(code)
    {
        case osRtxErrorStackUnderflow:
            // Stack underflow detected for thread (thread_id=object_id)
            size = snprintf((char *)faults_debug_buffer, FAULTS_DEBUG_BUFFER_SIZE, "OS_ERROR_STACK_UNDERFLOW: %d\r\n", (uint32_t)object_id);
            debug_send_blocking(faults_debug_buffer, size);
            break;
        case osRtxErrorISRQueueOverflow:
            // ISR Queue overflow detected when inserting object (object_id)
            size = snprintf((char *)faults_debug_buffer, FAULTS_DEBUG_BUFFER_SIZE, "OS_ERROR_ISR_QUEUE_OVERFLOW: %d\r\n", (uint32_t)object_id);
            debug_send_blocking(faults_debug_buffer, size);
            break;
        case osRtxErrorTimerQueueOverflow:
            // User Timer Callback Queue overflow detected for timer (timer_id=object_id)
            size = snprintf((char *)faults_debug_buffer, FAULTS_DEBUG_BUFFER_SIZE, "OS_ERROR_TIMER_QUEUE_OVERFLOW: %d\r\n", (uint32_t)object_id);
            debug_send_blocking(faults_debug_buffer, size);
            break;
        case osRtxErrorClibSpace:
            // Standard C/C++ library libspace not available: increase OS_THREAD_LIBSPACE_NUM
            size = snprintf((char *)faults_debug_buffer, FAULTS_DEBUG_BUFFER_SIZE, "OS_ERROR_CLIB_SPACE\r\n");
            debug_send_blocking(faults_debug_buffer, size);
            break;
        case osRtxErrorClibMutex:
            // Standard C/C++ library mutex initialization failed
            size = snprintf((char *)faults_debug_buffer, FAULTS_DEBUG_BUFFER_SIZE, "OS_ERROR_CLIB_MUTEX\r\n");
            debug_send_blocking(faults_debug_buffer, size);
            break;
        default:
            break;
    }

#if FAULTS_RESET_ON_ERROR
    NVIC_SystemReset();
#else
    while(1)
    {
        __NOP();
    }
#endif

    return 0U;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

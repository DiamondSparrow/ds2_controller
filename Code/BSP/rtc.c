/**
 **********************************************************************************************************************
 * @file         rtc.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-09-29
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

#include "rtc.h"

#include "chip.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void rtc_init(void)
{
    /* Enable the RTC oscillator, oscillator rate can be determined by calling Chip_Clock_GetRTCOscRate()   */
    Chip_Clock_EnableRTCOsc();

    /* Initialize RTC driver (enables RTC clocking) */
    Chip_RTC_Init(LPC_RTC);

    if(Chip_RTC_GetCount(LPC_RTC) == 0)
    {
        /* RTC reset */
        Chip_RTC_Reset(LPC_RTC);
        /* Start RTC at a count of 0 when RTC is disabled. If the RTC is enabled, you
           need to disable it before setting the initial RTC count. */
        Chip_RTC_Disable(LPC_RTC);
        Chip_RTC_SetCount(LPC_RTC, 0);
    }

    Chip_RTC_Enable(LPC_RTC);

    return;
}

void rtc_set(uint32_t timestamp)
{
    __disable_irq();
    Chip_RTC_Disable(LPC_RTC);
    Chip_RTC_SetCount(LPC_RTC, timestamp);
    Chip_RTC_Enable(LPC_RTC);
    __enable_irq();

    return;
}

uint32_t rtc_get(void)
{
    uint32_t rtc = 0;

    __disable_irq();
    rtc = Chip_RTC_GetCount(LPC_RTC);
    __enable_irq();

    return rtc;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

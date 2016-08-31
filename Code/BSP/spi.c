/**
 **********************************************************************************************************************
 * @file         spi.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-08-30
 * @brief        SPI C source file.
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
#include <string.h>

#include "chip.h"

#include "spi.h"

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
void spi_init(void)
{
    SPI_CFG_T spiCfg;
    SPI_DELAY_CONFIG_T spiDelayCfg;

    /* Enable the clock to the Switch Matrix */
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
    /*
     * Initialize SPI0 pins connect
     * SCK0: PINASSIGN3[15:8]: Select P0.0
     * MOSI0: PINASSIGN3[23:16]: Select P0.16
     * MISO0: PINASSIGN3[31:24] : Select P0.10
     * SSEL0: PINASSIGN4[7:0]: Select P0.9
     */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 16, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));

    Chip_SWM_MovablePinAssign(SWM_SPI0_SCK_IO, 0);      /* P0.0 */
    Chip_SWM_MovablePinAssign(SWM_SPI0_MOSI_IO, 16);    /* P0.16 */
    Chip_SWM_MovablePinAssign(SWM_SPI0_MISO_IO, 10);    /* P0.10 */
    Chip_SWM_MovablePinAssign(SWM_SPI0_SSELSN_0_IO, 9); /* P0.9 */

    /* Disable the clock to the Switch Matrix to save power */
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    /* Initialize SPI Block */
    Chip_SPI_Init(LPC_SPI0);
    /* Set SPI Config register */
    spiCfg.ClkDiv       = 0xFFFF; /* Set Clock divider to maximum */
    spiCfg.Mode         = SPI_MODE_MASTER; /* Enable Master Mode */
    spiCfg.ClockMode    = SPI_CLOCK_MODE0; /* Enable Mode 0 */
    spiCfg.DataOrder    = SPI_DATA_MSB_FIRST; /* Transmit MSB first */
    /* Slave select polarity is active low */
    spiCfg.SSELPol      = (SPI_CFG_SPOL0_LO | SPI_CFG_SPOL1_LO | SPI_CFG_SPOL2_LO | SPI_CFG_SPOL3_LO);
    Chip_SPI_SetConfig(LPC_SPI0, &spiCfg);
    /* Set Delay register */
    spiDelayCfg.PreDelay        = 2;
    spiDelayCfg.PostDelay       = 2;
    spiDelayCfg.FrameDelay      = 2;
    spiDelayCfg.TransferDelay   = 2;
    Chip_SPI_DelayConfig(LPC_SPI0, &spiDelayCfg);
    /* Enable Loopback mode for this example */
    Chip_SPI_EnableLoopBack(LPC_SPI0);
    /* Enable SPI0 */
    Chip_SPI_Enable(LPC_SPI0);

    return;
}
/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

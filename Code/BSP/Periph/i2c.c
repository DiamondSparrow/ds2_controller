/**
 **********************************************************************************************************************
 * @file         i2c.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-08-29
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
#include <string.h>

#include "chip.h"

#include "i2c.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/* I2C clock is set to 1.8MHz */
#define I2C_CLK_DIVIDER         (40)
/* 100KHz I2C bit-rate */
#define I2C_BITRATE             (400000)

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/* I2CM transfer record */
static I2CM_XFER_T  i2c_xfer_rec = {0};
static uint8_t i2c_buffer[256 + 1] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void i2c_init(void)
{
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN);
    Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
    Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);

    /* Enable I2C clock and reset I2C peripheral - the boot ROM does not do this */
    Chip_I2C_Init(LPC_I2C0);

    /* Setup clock rate for I2C */
    Chip_I2C_SetClockDiv(LPC_I2C0, I2C_CLK_DIVIDER);

    /* Setup I2CM transfer rate */
    Chip_I2CM_SetBusSpeed(LPC_I2C0, I2C_BITRATE);

    /* Enable Master Mode */
    Chip_I2CM_Enable(LPC_I2C0);

    /* Disable the interrupt for the I2C */
    NVIC_DisableIRQ(I2C0_IRQn);

    return;
}

void i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t buffer[2] = {0};

    buffer[0] = reg;
    buffer[1] = data;

    i2c_tx_rx(addr, buffer, 2, NULL, 0);

    return;
}

void i2c_write_reg_multi(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size)
{
    i2c_buffer[0] = reg;
    memcpy(&i2c_buffer[1], data, size);

    i2c_tx_rx(addr, i2c_buffer, size + 1, NULL, 0);

    return;
}


bool i2c_tx_rx(uint8_t addr, uint8_t *tx_buff, uint16_t tx_size, uint8_t *rx_buff, uint16_t rx_size)
{
    /* Setup I2C transfer record */
    i2c_xfer_rec.slaveAddr  = addr;
    i2c_xfer_rec.status     = 0;
    i2c_xfer_rec.txSz       = tx_size;
    i2c_xfer_rec.rxSz       = rx_size;
    i2c_xfer_rec.txBuff     = tx_buff;
    i2c_xfer_rec.rxBuff     = rx_buff;

    Chip_I2CM_XferBlocking(LPC_I2C0, &i2c_xfer_rec);
    if(i2c_xfer_rec.status == I2CM_STATUS_OK)
    {
        return true;
    }

    return false;
}
/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/


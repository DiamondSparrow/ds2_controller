/**
 **********************************************************************************************************************
 * @file         bh1750.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-08-30
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

#include "bh1750.h"

#include "i2c.h"
#include "cmsis_os.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/**< BH1750 I2C address. */
#define BH1750_I2C_ADDR_HIGH                0x5C
#define BH1750_I2C_ADDR_LOW                 0x23
#define BH1750_I2C_ADDR                     (BH1750_I2C_ADDR_LOW)

/**< No active state. */
#define BH1750_REG_POWER_DOWN               0x00
/**< Waiting for measurement command. */
#define BH1750_REG_POWER_ON                 0x01
/**< Reset Data register value. Reset command is not acceptable in Power Down mode. */
#define BH1750_REG_RESET                    0x07
/**< Start measurement at 1 lx resolution. Measurement Time is typically 120 ms. */
#define BH1750_REG_CONTHIGH_RES_MODE        0x10
/**< Start measurement at 0.5 lx resolution. Measurement Time is typically 120 ms. */
#define BH1750_REG_CONT_HIGH_RES_MODE_2     0x11
/**< Start measurement at 4 lx resolution. Measurement Time is typically 16 ms. */
#define BH1750_REG_CONT_LOW_RES_MODE        0x13
/**< Start measurement at 1 lx resolution. Measurement Time is typically 120 ms.
 * It is automatically set to Power Down mode after measurement. */
#define BH1750_REG_ONE_TIME_HIGH_RES_MODE   0x20
/**< Start measurement at 0.5 lx resolution. Measurement Time is typically 120ms.
 * It is automatically set to Power Down mode after measurement. */
#define BH1750_REG_ONE_TIME_HIGH_RES_MODE_2 0x21
/**< Start measurement at 4 lx resolution. Measurement Time is typically 16 ms.
 * It is automatically set to Power Down mode after measurement. */
#define BH1750_REG_ONE_TIME_LOW_RES_MODE    0x23

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
static inline bool bh1750_io_write(uint8_t data);
static inline bool bh1750_io_read(uint8_t *data, uint8_t size);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool bh1750_init(bh1750_mode_t mode)
{
    if(bh1750_io_write(BH1750_REG_RESET) == false)
    {
        return false;
    }
    osDelay(10);

    if(bh1750_io_write(mode) == false)
    {
        return false;
    }

    return true;
}

uint16_t bh1750_read_level(void)
{
    uint8_t value[2] = {0};

    if(bh1750_io_read(value, 2) == false)
    {
        return UINT16_MAX;
    }

    return (uint16_t)((value[0] << 8) | value[1]);
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static inline bool bh1750_io_write(uint8_t data)
{
    if(i2c_tx_rx(BH1750_I2C_ADDR, &data, 1, NULL, 0) == false)
    {
        return false;
    }

    return true;
}

static inline bool bh1750_io_read(uint8_t *data, uint8_t size)
{
    if(i2c_tx_rx(BH1750_I2C_ADDR, NULL, 0, data, size) == false)
    {
        return false;
    }

    return true;
}


/**
 **********************************************************************************************************************
 * @file        i2c.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-08-29
 * @brief       I2C C header file.
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

#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
    
/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
void i2c_init(void);
/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */
/**
 * @brief   Writes single byte to slave register.
 *
 * @param   addr    Salve address.
 * @param   reg     Register to write to.
 * @param   data    Data to be written.
 */
void i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data);
void i2c_write_reg_multi(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size);
bool i2c_tx_rx(uint8_t dev_dddr, uint8_t *tx_buff, uint16_t tx_size, uint8_t *rx_buff, uint16_t rx_size);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */

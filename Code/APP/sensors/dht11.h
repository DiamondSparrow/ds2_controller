/**
 **********************************************************************************************************************
 * @file        dht11.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-10-03
 * @brief       This is C header file template.
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

#ifndef DHT11_H_
#define DHT11_H_

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
#define DHT11_PORT             1
#define DHT11_PIN              8

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief  DHT11 main data structure
 */
typedef struct
{
    int16_t temperature;    /**< Temperature in tenths of degrees. If real temperature is 27.3°C, this variable's value is 273. */
    uint16_t humidity;      /**< Humidity in tenths of percent. If real humidity is 55.5%, this variable's value is 555. */
} dht11_data_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief  Initializes DHT11 sensor.
 */
void dht11_init(void);

/**
 * @brief   Reads data from sensor
 *
 * @param   data    Pointer to data structure to store data into. See @ref dht11_data_t.

 * @return  State of read.
 * @retval  0   failed.
 * @retval  1   success.
 */
bool dht11_read(dht11_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* DHT11_H_ */

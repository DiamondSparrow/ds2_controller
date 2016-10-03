/**
 **********************************************************************************************************************
 * @file        am2301.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-09-01
 * @brief       AM2301 (DHT21) temperature and humidity sensor C header file.
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

#ifndef AM2301_H_
#define AM2301_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
#define AM2301_PORT             1
#define AM2301_PIN              8

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**

 * AM2301 return codes.
 */
typedef enum
{
    AM2301_OK                   = 0,    //!< Data OK.
    AM2301_ERROR                = 1,    //!< An error occurred.
    AM2301_CONNECTION_ERROR     = 2,    //!< Device is not connected.
    AM2301_WAITHIGH_ERROR       = 3,    //!< Wait high pulse timeout.
    AM2301_WAITLOW_ERROR        = 4,    //!< Wait low pulse timeout.
    AM2301_WAITHIGH_LOOP_ERROR  = 5,    //!< Loop error for high pulse.
    AM2301_WAITLOW_LOOP_ERROR   = 6,    //!< Loop error for low pulse.
    AM2301_PARITY_ERROR         = 7,    //!< Data read fail.
} am2301_ret_t;

/**
 * @brief  AM2301 main data structure
 */
typedef struct
{
    int16_t temperature;    /**< Temperature in tenths of degrees. If real temperature is 27.3°C, this variable's value is 273. */
    uint16_t humidity;      /**< Humidity in tenths of percent. If real humidity is 55.5%, this variable's value is 555. */
} am2301_data_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/
/**
 * @brief  Initializes AM2301 sensor
 *
 * @return  State of initialization. See @ref am2301_ret_t.
 */
am2301_ret_t am2301_init(void);

/**
 * @brief   Reads data from sensor
 *
 * @param   data    Pointer to data structure to store data into. See @ref am2301_data_t.

 * @return  State of read. See @ref am2301_ret_t.
 */
am2301_ret_t am2301_read(am2301_data_t *data);

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* AM2301_H_ */

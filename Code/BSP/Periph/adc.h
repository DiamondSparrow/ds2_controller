/**
 **********************************************************************************************************************
 * @file        adc.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-10-12
 * @brief       ADC C header file.
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

#ifndef ADC_H_
#define ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief   ADC id list.
 */
typedef enum
{
    ADC_ID_TEMPERATURE,         //!< Temperature.
    ADC_ID_MOTOR_LEFT_CURR,     //!< Left motor current
    ADC_ID_MOTOR_RIGHT_CURR,    //!< Right motor current.
    ADC_ID_JS_X,
    ADC_ID_JS_Y,
    ADC_ID_LAST,                //!< Last should stay last.
} adc_id_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize ADC.
 */
void adc_init(void);

/**
 * @brief   Get raw value (12 bits).
 *
 * @param   id  ADC ID of which value to get. See @ref adc_id_t.
 *
 * @return  Channel 12 bit value. If data invalid - UINT32_MAX (0xFFFFFFFF).
 */
uint32_t adc_get_value_raw(adc_id_t id);

/**
 * @brief   Get channel value in millivolts.
 *
 * @param   id  ADC ID of which value to get. See @ref adc_id_t.
 *
 * @return  Channel value in mV. If data invalid - UINT32_MAX (0xFFFFFFFF).
 */
uint32_t adc_get_value_volt(adc_id_t id);

/**
 * @brief   Get internal temperature sensor value in degC.
 *
 * @return  Temperature in degC.
 */
float adc_get_temperature(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_H_ */

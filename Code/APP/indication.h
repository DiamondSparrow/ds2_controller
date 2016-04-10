/**
 **********************************************************************************************************************
 * @file        indication.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        Apr 5, 2016
 * @brief       Indication control C header file.
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

#ifndef INDICATION_H_
#define INDICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
#define INDICATION_LED_RED      0       //!< Read LED PIN configuration.
#define INDICATION_LED_GREEN    1       //!< Green LED PIN configuration.
#define INDICATION_LED_BLUE     2       //!< Blue LED PIN configuration.
#define INDICATION_PERIOD       1000UL  //!< Indication control period in ms.

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief   Indications enumerator.
 */
typedef enum
{
    INDICATION_NA,     //!< Indication not available - OFF.
    INDICATION_RED,    //!< Red LED is ON only.
    INDICATION_GREEN,  //!< Green LED is ON only.
    INDICATION_BLUE,   //!< Blue LED is ON only.
    INDICATION_BOOT,   //!< Indication for boot mode.
    INDICATION_INIT,   //!< Indication for initialization mode.
    INDICATION_STANDBY,//!< Indication for standby mode.
    INDICATION_IDLE,   //!< Indication for idle mode.
    INDICATION_FAULT,  //!< Indication for fault mode.
} indication_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize indication control module.
 *
 * @return  State of initialization.
 * @retval  false   failed.
 * @retval  true    success.
 */
bool indication_init(void);

/**
 * @brief   Set indication.
 *
 * @param   indication  Indication to set. See @ref indication_t.
 */
void indication_set(indication_t indication);

/**
 * @brief   Set indication immediately.
 *
 * @param   indication  Indication to set. See @ref indication_t.
 */
void indication_set_blocking(indication_t indication);

/**
 * @brief   Indication timer handler.
 *
 * @param   arg Pointer to timer arguments.
 */
void indication_handle(void const *arg);

#ifdef __cplusplus
}
#endif

#endif /* INDICATION_H_ */

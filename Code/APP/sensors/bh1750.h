/**
 **********************************************************************************************************************
 * @file        bh1750.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-08-30
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

#ifndef BH1750_H_
#define BH1750_H_

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
typedef enum
{
    BH1750_MODE_CONT_HIGH_RES       = 0x10, /**< Start measurement at 1 lx resolution.
                                                 Measurement Time is typically 120 ms. */
    BH1750_MODE_CONT_HIGH_RES_2     = 0x11, /**< Start measurement at 0.5 lx resolution.
                                                 Measurement Time is typically 120 ms. */
    BH1750_MODE_CONT_LOW_RES        = 0x13, /**< Start measurement at 4 lx resolution.
                                                 Measurement Time is typically 16 ms. */
    BH1750_MODE_ONE_TIME_HIGH_RES   = 0x20, /**< Start measurement at 1 lx resolution.
                                                 Measurement Time is typically 120 ms.
                                                 It is automatically set to Power Down mode after measurement. */
    BH1750_MODE_ONE_TIME_HIGH_RES_2 = 0x21, /**< Start measurement at 0.5 lx resolution.
                                                 Measurement Time is typically 120ms.
                                                 It is automatically set to Power Down mode after measurement. */
    BH1750_MODE_ONE_TIME_LOW_RES    = 0x23, /**< Start measurement at 4 lx resolution.
                                                 Measurement Time is typically 16 ms.
                                                 It is automatically set to Power Down mode after measurement. */
} bh1750_mode_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize light sensor BH1750.
 *
 * @param   mode    Mode to initialize.
 *
 * @return  State of initialization.
 * @retval  0   failed.
 * @retval  1   success.
 */
bool bh1750_init(bh1750_mode_t mode);

/**
 * @brief   Read BH1750 light level.
 *
 * @return  Light level in lx.
 */
uint16_t bh1750_read_level(void);


#ifdef __cplusplus
}
#endif

#endif /* BH1750_H_ */

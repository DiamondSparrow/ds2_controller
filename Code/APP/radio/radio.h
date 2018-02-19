/**
 **********************************************************************************************************************
 * @file        radio.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2018-02-06
 * @brief       Radio C header file.
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

#ifndef RADIO_H_
#define RADIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief Radio data structure.
 */
typedef struct
{
    uint32_t tx_counter;        /**< Transmit packet counter. */
    uint32_t tx_lost_counter;   /**< Lost transmit packet counter. */
    uint32_t rx_counter;        /**< Received packet counter. */
    uint32_t rtr_acumulator;    /**< Accumulated retransmitted packet counter. */
    uint32_t rtr_current;       /**< Current packer retransmissions count. */
    uint32_t rtr;               /**< Retransmissions quality indicator from 0 to 15. 0 - no lost, 15 - all lost. */
    uint32_t quality;           /**< Retransmissions quality in percentage, from 0 - 100 %. */
} radio_data_t;

/**********************************************************************************************************************
 * Prototypes of exported constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool radio_init(void);
void radio_thread(void *arguments);

#ifdef __cplusplus
}
#endif

#endif /* RADIO_H_ */

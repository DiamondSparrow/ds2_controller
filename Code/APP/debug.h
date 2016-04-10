/**
 **********************************************************************************************************************
 * @file        debug.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-04-10
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

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
#define DEBUG_BUFFER_SIZE   255
#define DEBUG_LOCK_TIMEOUT  1000

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
#define DEBUG_BOOT(F, ...)  debug_send(F "\r\n", ##__VA_ARGS__)
#define DEBUG_INIT(F, ...)  debug_send_os(F "\r\n", ##__VA_ARGS__)
#define DEBUG(F, ...)       debug_send_os(F "\r\n", ##__VA_ARGS__)

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool debug_init(void);
void debug_send(const char *fmt, ...);
void debug_send_os(const char *fmt, ...);
void debug_send_blocking(uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */

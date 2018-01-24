/**
 **********************************************************************************************************************
 * @file        cli_app.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-04-06
 * @brief       Command Line Interface application C header file.
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

#ifndef CLI_APP_H_
#define CLI_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
#define CLI_APP_ECHO    1   //!< Control Echo: 0 - diasble, 1 - enable.

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
/**
 * @brief   Initialize Command Line Interface application.
 *
 * @return  State of initialization.
 * @retval  0   failed.
 * @retval  1   success.
 */
bool cli_app_init(void);

/**
 * @brief   CLI application thread.
 *
 * @param   arguments   Pointer to thread arguments.
 */
void cli_app_thread(void *arguments);

#ifdef __cplusplus
}
#endif

#endif /* CLI_APP_H_ */

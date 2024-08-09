/**
 **********************************************************************************************************************
 * @file        display.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2018-02-12
 * @brief       Display C header file.
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

#ifndef DISPLAY_H_
#define DISPLAY_H_

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

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef enum
{
    DISPLAY_MENU_ID_WELCOME,
    DISPLAY_MENU_ID_CLOCK,
    DISPLAY_MENU_ID_MOTOR,
    DISPLAY_MENU_ID_RADIO,
    DISPLAY_MENU_ID_INFO,
    DISPLAY_MENU_ID_LAST,
} display_menu_id_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize display.
 *
 * @return  State of initialization.
 * @retval  0   failed.
 * @retval  1   success.
 */
bool display_init(void);

/**
 * @brief   Turn off display.
 */
void display_turn_off(void);

/**
 * @brief   Turn on display.
 */
void display_turn_on(void);

/**
 * @brief   Get display power state.
 *
 * @return  Display power state.
 * @retval  0   Display is off.
 * @retval  1   Display is on.
 */
bool display_power_state(void);

/**
 * @brief   Get current menu id.
 *
 * @return  Current menu id.
 */
display_menu_id_t display_get_menu(void);

/**
 * @brief   Set display menu to view.
 *
 * @param   id  Menu ID to display. See @ref display_menu_id_t.
 */
void display_set_menu(display_menu_id_t id);

/**
 * @brief   Set pop-up message on screen.
 *
 * @param   text    Pop-up message text.
 * @param   timeout Pop-up show timeout in milliseconds.
 */
void display_set_popup(uint8_t *text, uint32_t timeout);

/**
 * @brief   Clear pop'op message.
 */
void display_clear_popup(void);

/**
 * @brief   Display thread.
 *
 * @param   arguments   Pointer to thread arguments.
 */
void display_thread(void *arguments);

/**
 * @brief   Display timeout timer handler.
 *
 * @param   arguments   Pointer to timer arguments.
 */
void display_timeout_handle(void *arguments);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H_ */

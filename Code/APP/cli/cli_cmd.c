/**
 **********************************************************************************************************************
 * @file         cli_cmd.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-10
 * @brief        This is C source file template.
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

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cli_cmd.h"
#include "cli.h"

#include "debug.h"
#include "bsp.h"
#include "common.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static const cli_cmd_t cli_cmd_help =
{
    (const uint8_t *)"help",
    (const uint8_t *)"help  Lists all the registered commands.",
    cli_cmd_help_cb,
    0,
};
const cli_cmd_t cli_cmd_info =
{
    (const uint8_t *)"info",
    (const uint8_t *)"info  Shows device information.",
    cli_cmd_info_cb,
    0,
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
extern cli_cmd_t cli_reg_cmd[CLI_CMD_MAX_SIZE];

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
void cli_cmd_register(void)
{
    cli_register_cmd((const cli_cmd_t *)&cli_cmd_help);
    cli_register_cmd((const cli_cmd_t *)&cli_cmd_info);

    return;
}

bool cli_cmd_help_cb(uint8_t *data, size_t size, const uint8_t *cmd)
{
    uint8_t i = 0;

    UNUSED_VARIABLE(cmd);

    /* Return the next command help string, before moving the pointer on to
     the next command in the list. */
    for(i = 0; i < CLI_CMD_MAX_SIZE; i++)
    {
        if(strlen((char *)cli_reg_cmd[i].help) > 2)
        {
            DEBUG("%s", cli_reg_cmd[i].help);
        }
    }

    return false;
}

bool cli_cmd_info_cb(uint8_t *data, size_t size, const uint8_t *cmd)
{
    UNUSED_VARIABLE(cmd);

    DEBUG("Device ...... DS-2 Controller");
    DEBUG("Build ....... %s %s", __DATE__, __TIME__);
    DEBUG("Core Clock .. %ld MHz.", SystemCoreClock);


    return false;
}

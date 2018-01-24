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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cli_cmd.h"
#include "cli.h"

#include "debug.h"
#include "servo/servo.h"
#include "common.h"
#include "bsp.h"

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
/** The definition of the list of commands. Commands that are registered are added to this list. */
const cli_cmd_t cli_cmd_list[CLI_CMD_COUNT] =
{
    {
        (const uint8_t *)"help",
        (const uint8_t *)"help      Lists all the registered commands.",
        cli_cmd_help_cb,
        0,
    },
    {
        (const uint8_t *)"info",
        (const uint8_t *)"info      Shows device information.",
        cli_cmd_info_cb,
        0,
    },
    {
        (const uint8_t *)"servo",
        (const uint8_t *)"servo     Control servo:$servo %action $angle.",
        cli_cmd_servo_cb,
        3,
    },
    {
        (const uint8_t *)"pointer",
        (const uint8_t *)"pointer   Control pointer: $pan %$tilt $angle.",
        cli_cmd_pointer_cb,
        2,
    },
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
bool cli_cmd_help_cb(uint8_t *data, size_t size, const uint8_t *cmd)
{
    uint8_t i = 0;

    UNUSED_VARIABLE(cmd);

    /* Return the next command help string, before moving the pointer on to
     the next command in the list. */
    for(i = 0; i < CLI_CMD_COUNT; i++)
    {
        if(strlen((char *)cli_cmd_list[i].help) > 2)
        {
            DEBUG("%s", cli_cmd_list[i].help);
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

bool cli_cmd_servo_cb(uint8_t *data, size_t size, const uint8_t *cmd)
{
    uint8_t ptr_size = 0;
    uint8_t *ptr = NULL;
    servo_id_t servo_id = SERVO_ID_LAST;
    int8_t angle = 0;

    // Check $servo parameter.
    if((ptr = (uint8_t *)cli_get_parameter(cmd, 1, &ptr_size)) == NULL)
    {
        return false;
    }
    if(memcmp(ptr, "pan", ptr_size) == 0)
    {
        servo_id = SERVO_ID_PAN;
    }
    if(memcmp(ptr, "tilt", ptr_size) == 0)
    {
        servo_id = SERVO_ID_TILT;
    }

    if(servo_id == SERVO_ID_LAST)
    {
        return false;
    }

    // Check $action parameter
    if((ptr = (uint8_t *)cli_get_parameter(cmd, 2, &ptr_size)) == NULL)
    {
        return false;
    }
    if(memcmp(ptr, "test", ptr_size) == 0)
    {
        DEBUG("Testing servo %d...", servo_id);
        servo_test(servo_id);
        DEBUG("Testing servo %d is done.", servo_id);
        return false;
    }

    if(memcmp(ptr, "set", ptr_size) != 0)
    {
        return false;
    }

    // Check $angle parameter
    if((ptr = (uint8_t *)cli_get_parameter(cmd, 3, &ptr_size)) == NULL)
    {
        return false;
    }
    angle = atoi((char *)ptr);
    if(servo_set(servo_id, angle) == true)
    {
        DEBUG("Servo %d angle set to %d deg.", servo_id, angle);
    }
    else
    {
        DEBUG("Servo %d angle set to %d deg.failed.", servo_id, angle);
    }

    return false;
}

bool cli_cmd_pointer_cb(uint8_t *data, size_t size, const uint8_t *cmd)
{
    int8_t pan = 0;
    int8_t tilt = 0;
    uint8_t ptr_size = 0;
    uint8_t *ptr = NULL;

    if((ptr = (uint8_t *)cli_get_parameter(cmd, 1, &ptr_size)) == NULL)
    {
        return false;
    }
    pan = atoi((char *)ptr);
    if((ptr = (uint8_t *)cli_get_parameter(cmd, 2, &ptr_size)) == NULL)
    {
        return false;
    }
    tilt = atoi((char *)ptr);
    servo_set(SERVO_ID_PAN, pan);
    servo_set(SERVO_ID_TILT, tilt);
    DEBUG("Pointer %d %d.", pan, tilt);

    return false;
}

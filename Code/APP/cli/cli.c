/**
 **********************************************************************************************************************
 * @file         cli.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         Apr 6, 2016
 * @brief        Command Line Interface C source file.
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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "cli.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define CLI_MSG_ENTER_HELP      "Enter \"help\" to view a list of available commands."
#define CLI_MSG_CMD_INCORRECT   "Incorrect command parameter(s)."
#define CLI_MSG_CMD_UNKNOWN     "Command not recognized."

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
cli_cmd_t cli_reg_cmd[CLI_CMD_MAX_SIZE] = {0};
/** Commands register index. */
static uint8_t cli_reg_cmd_index = 0;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**
 * @brief   Get the number of parameters that follow the command name.
 *
 * @param   cmd     Pointer to command string.
 *
 * @return  Number of parameters that follow the command name.
 */
static int8_t cli_get_prm_count(const uint8_t *cmd);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void cli_init(void)
{
    cli_reg_cmd_index = 0;
    memset(cli_reg_cmd, 0, sizeof(cli_cmd_t) * CLI_CMD_MAX_SIZE);

    return;
}

bool cli_register_cmd(const cli_cmd_t *cmd)
{
    if(cli_reg_cmd_index >= CLI_CMD_MAX_SIZE)
    {
        return false;
    }

    memcpy(&cli_reg_cmd[cli_reg_cmd_index], cmd, sizeof(cli_cmd_t));
    cli_reg_cmd_index++;

    return true;
}

bool cli_process_cmd(const uint8_t * const input, uint8_t *buffer, size_t size)
{
    uint8_t i = 0;
    uint8_t res = 0;
    bool ret = true;
    const uint8_t *reg_cmd_str = NULL;
    size_t reg_cmd_str_len = 0;

    /* Search for the command string in the list of registered commands. */
    for(i = 0; i < CLI_CMD_MAX_SIZE; i++)
    {
        reg_cmd_str = cli_reg_cmd[i].cmd;
        reg_cmd_str_len = strlen((char *)reg_cmd_str);

        /* To ensure the string lengths match exactly, so as not to pick up
         a sub-string of a longer command, check the byte after the expected
         end of the string is either the end of the string or a space before
         a parameter. */
        if((input[reg_cmd_str_len] == ' ') || (input[reg_cmd_str_len] == 0x00))
        {
            if(memcmp(input, reg_cmd_str, reg_cmd_str_len) == 0)
            {
                res = 1;
                /* The command has been found.  Check it has the expected
                 number of parameters.  If cExpectedNumberOfParameters is -1,
                 then there could be a variable number of parameters and no
                 check is made. */
                if(cli_reg_cmd[i].prm_count >= 0)
                {
                    if(cli_get_prm_count(input) != cli_reg_cmd[i].prm_count)
                    {
                        res = 2;
                    }
                }
                break;
            }
        }
    }


    if(res == 2)
    {
        /* The command was found, but the number of parameters with the command
         was incorrect. */
        memcpy(buffer, CLI_MSG_CMD_INCORRECT " " CLI_MSG_ENTER_HELP, size);
        ret = true;
    }
    else if(res == 1)
    {
        /* Call the callback function that is registered to this command. */
        if(cli_reg_cmd[i].callback != NULL)
        {
            ret = cli_reg_cmd[i].callback(buffer, size, input);
        }
    }
    else
    {
        /* The command was not found. */
        memcpy(buffer, CLI_MSG_CMD_UNKNOWN " " CLI_MSG_ENTER_HELP, size);
        ret = true;
    }

    return ret;
}

const uint8_t *cli_get_parameter(const uint8_t *cmd, uint8_t prm_index, uint8_t *size)
{
    uint8_t prm_found = 0;
    const uint8_t *ret = NULL;

    *size = 0;

    while(prm_found < prm_index)
    {
        /* Index the character pointer past the current word.  If this is the start
         of the command string then the first word is the command itself. */
        while(((*cmd) != 0x00) && ((*cmd) != ' '))
        {
            cmd++;
        }

        /* Find the start of the next string. */
        while(((*cmd) != 0x00) && ((*cmd) == ' '))
        {
            cmd++;
        }

        /* Was a string found? */
        if(*cmd != 0x00)
        {
            /* Is this the start of the required parameter? */
            prm_found++;

            if(prm_found == prm_index)
            {
                /* How long is the parameter? */
                ret = cmd;
                while(((*cmd) != 0x00) && ((*cmd) != ' '))
                {
                    (*size)++;
                    cmd++;
                }

                if(*size == 0)
                {
                    ret = NULL;
                }

                break;
            }
        }
        else
        {
            break;
        }
    }

    return ret;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static int8_t cli_get_prm_count(const uint8_t *cmd)
{
    int8_t prm_count = 0;
    bool space_flag = false;

    /* Count the number of space delimited words in pcCommandString. */
    while(*cmd != 0x00)
    {
        if((*cmd) == ' ')
        {
            if(space_flag != true)
            {
                prm_count++;
                space_flag = true;
            }
        }
        else
        {
            space_flag = false;
        }
        cmd++;
    }

    /* If the command string ended with spaces, then there will have been too
     many parameters counted. */
    if(space_flag == true)
    {
        prm_count--;
    }

    /* The value returned is one less than the number of space delimited words,
     as the first word should be the command itself. */
    return prm_count;
}

/**
 **********************************************************************************************************************
 * @file        sensors.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-09-01
 * @brief       Sensors C header file.
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

#ifndef SENSORS_H_
#define SENSORS_H_

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
typedef struct
{
    struct
    {
        bool state;
        uint16_t value;
        uint16_t value_lp;
    } light;
    struct
    {
        bool state;
        int16_t value;
    } temperature;
    struct
    {
        bool state;
        uint16_t value;
    } humidity;
} sensors_data_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/
extern volatile sensors_data_t sensors_data;

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool sensors_init(void);
void sensors_thread(void *arguments);

#ifdef __cplusplus
}
#endif

#endif /* SENSORS_H_ */

/**
 **********************************************************************************************************************
 * @file        motor.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-09-12
 * @brief       Motor control C header file.
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

#ifndef MOTOR_H_
#define MOTOR_H_

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
    MOTOR_ID_LEFT,
    MOTOR_ID_RIGHT,
    MOTOR_ID_LAST,
} motor_id_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool motor_init(void);
void motor_thread(void *arguments);

void motor_ramp_set(uint16_t ramp);
void motor_forward(motor_id_t motor, uint8_t speed);
void motor_backward(motor_id_t motor, uint8_t speed);
void motor_brake(motor_id_t motor);
void motor_neutral(motor_id_t motor);
void motor_test(motor_id_t motor, uint8_t ramp);
void motor_test_ramp(motor_id_t motor, uint8_t ramp);

int16_t motor_get_speed_target(motor_id_t motor);
int16_t motor_get_speed_current(motor_id_t motor);
uint16_t motor_get_current(motor_id_t motor);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H_ */

/**
 **********************************************************************************************************************
 * @file         filters.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-10-04
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
#include <stdint.h>

#include "filters.h"

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
filters_kalman_t filter_kalman_init(double proc_noise_cov, double meas_noise_cov, double est_error, double value)
{
    filters_kalman_t result = {0};

    result.process_noise_cov        = proc_noise_cov;
    result.measurement_noise_cov    = meas_noise_cov;
    result.estimation_error         = est_error;
    result.value                    = value;

    return result;
}

double filters_kalman_update(filters_kalman_t *data, double input)
{
    // Prediction update. Omit x = x.
    data->estimation_error = data->estimation_error + data->process_noise_cov;

    // Measurement update
    data->gain = data->estimation_error / (data->estimation_error + data->measurement_noise_cov);
    data->value = data->value + data->gain * (input - data->value);
    data->estimation_error = (1 - data->gain) * data->estimation_error;

    return data->value;
}

double filter_low_pass(filters_low_pass_t *data, double input, double cut_off)
{
    data->input = input;
    data->cut_off = cut_off;
    data->output = data->output + (data->cut_off * (data->input - data->output));

    return data->output;
}

double filter_high_pass(filters_high_pass_t *data, double input, double cut_off)
{
    data->input = input;
    data->cut_off = cut_off;
    data->output = data->input - (data->output + data->cut_off * (data->input - data->output));

    return data->output;
}

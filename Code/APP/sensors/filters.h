/**
 **********************************************************************************************************************
 * @file        filters.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-10-04
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

#ifndef FILTERS_H_
#define FILTERS_H_

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
/**
 * @brief   Kalman filter data.
 */
typedef struct
{
    double process_noise_cov;       //!< Process noise covariance.
    double measurement_noise_cov;   //!< Measurement noise covariance.
    double value;                   //!< Current input value.
    double estimation_error;        //!< Estimation error covariance.
    double gain;                    //!< Kalman gain.
} filters_kalman_t;

/**
 * @brief   Low Pass filter data.
 */
typedef struct
{
    double input;   //!< Input value.
    double output;  //!< Output value.
    double cut_off; //!< Filter cut off.
} filters_low_pass_t;

/**
 * @brief   High Pass filter data.
 */
typedef struct
{
    double input;   //!< Input value.
    double output;  //!< Output value.
    double cut_off; //!< Filter cut off.
} filters_high_pass_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize kalman filter.
 *
 * @param   proc_noise_cov  Process noise covariance.
 * @param   meas_noise_cov  Measurement noise covariance.
 * @param   est_error       Estimation error covariance.
 * @param   value           Initial value.
 *
 * @return  Initialized kalman data structure. See @ref filters_kalman_t.
 */
filters_kalman_t filter_kalman_init(double proc_noise_cov, double meas_noise_cov, double est_error, double value);

/**
 * @brief   Update kalman filter.
 * @note    Before using you must initialize kalman filter. See @ref filter_kalman_init.
 *
 * @param   data    Kalman filter data. See @ref filters_kalman_t.
 * @param   input   Input value for kalman filter.
 *
 * @return  Kalman filter output value.
 */
double filters_kalman_update(filters_kalman_t *data, double input);

/**
 * @brief   Low pass filter.
 *
 * @param   data    Low pass filter data. See @ref filters_low_pass_t.
 * @param   input   Input data for low pass filter.
 * @param   cut_off Cut off value.
 *
 * @return  Output value of low pass filter.
 */
double filter_low_pass(filters_low_pass_t *data, double input, double cut_off);


/**
 * @brief   High pass filter.
 *
 * @param   data    High pass filter data. See @ref filters_high_pass_t.
 * @param   input   Input data for low pass filter.
 * @param   cut_off Cut off value.
 *
 * @return  Output value of high pass filter.
 */
double filter_high_pass(filters_high_pass_t *data, double input, double cut_off);

#ifdef __cplusplus
}
#endif

#endif /* FILTERS_H_ */

/**
 **********************************************************************************************************************
 * @file        common.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        Apr 6, 2016
 * @brief       Common stuff C header file.
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

#ifndef COMMON_H_
#define COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
#define UNUSED_VARIABLE(X)  ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
/** Swaps the upper byte with the lower byte in a 16 bit variable */
#ifndef SWAP
#define SWAP(x) ((((x)&0xFF)<<8)|(((x)>>8)&0xFF))
#endif  // SWAP
/** The upper 8 bits of a 16 bit value */
#ifndef MSB
#define MSB(a) (((a) & 0xFF00) >> 8)
#endif  // MSB
/** The lower 8 bits (of a 16 bit value) */
#ifndef LSB
#define LSB(a) ((a) & 0xFF)
#endif  // LSB
/** Leaves the minimum of the two arguments */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif  // MIN
/** Leaves the maximum of the two arguments */
#ifndef MAX
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif  // MAX

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H_ */

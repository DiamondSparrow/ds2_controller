/**
 **********************************************************************************************************************
 * @file        fonts.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-08-29
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

#ifndef FONTS_H_
#define FONTS_H_

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
 * @brief   Font structure used on my LCD libraries
 */
typedef struct
{
    uint8_t font_width;     /*!< Font width in pixels */
    uint8_t font_height;    /*!< Font height in pixels */
    const uint16_t *data;   /*!< Pointer to data font data array */
} fonts_t;

/**
 * @brief   String length and height
 */
typedef struct
{
    uint16_t length;    /*!< String length in units of pixels */
    uint16_t height;    /*!< String height in units of pixels */
} fonts_size_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/
extern fonts_t fonts_7x10;
extern fonts_t fonts_11x18;
extern fonts_t fonts_16x26;

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Calculates string length and height in units of pixels depending on string and font used.
 *
 * @param   str         String to be checked for length and height.
 * @param   font_size   Pointer to empty @ref fonts_size_t structure where informations will be saved.
 * @param   font        Pointer to @ref font_size font used for calculations.
 *
 * @retval  Pointer to string used for length and height.
 */
uint8_t *fonts_get_string_size(uint8_t *str, fonts_size_t *font_size, fonts_t *font);

#ifdef __cplusplus
}
#endif

#endif /* FONTS_H_ */

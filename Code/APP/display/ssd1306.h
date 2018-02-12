/**
 **********************************************************************************************************************
 * @file        ssd1306.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2018-02-12
 * @brief       SSD1306 OLED display header file.
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

#ifndef SSD1306_H_
#define SSD1306_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "fonts.h"
/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
/** I2C address */
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        0x78
//#define SSD1306_I2C_ADDR      0x7A
#endif
/** SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif
/** SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

#define SSD1306_DRV_MODE        0 //!< Driver mode: 0 - SPI, 1 - I2C

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief  SSD1306 color enumeration.
 */
typedef enum
{
    SSD1306_COLOR_BLACK = 0x01, /*!< Black color, no pixel */
    SSD1306_COLOR_WHITE = 0x00, /*!< Pixel is set. Color depends on display */
} ssd1306_color_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initializes SSD1306 display.
 *
 * @return  Initialization status.
 * @retval  0   Failed, Display was not detected on I2C port.
 * @retval  1   Success, display initialized OK and ready to use.
 */
bool ssd1306_init(void);

/**
 * @brief   Power on SSD1306.
 */
void ssd1306_on(void);

/**
 * @brief   Power off SSD1306.
 */
void ssd1306_off(void);

/**
 * @brief   Turn on inversion in SSD1306.
 */
void ssd1306_invert_on(void);

/**
 * @brief   Turn off inversion in SSD1306.
 */
void ssd1306_invert_off(void);

/**
 * @brief   Toggles pixels inversion inside internal RAM.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 */
void ssd1306_invert_toggle(void);

/**
 * @brief   Flip the horizontal orientation of the screen.
 */
void ss1306_orientation_flip_h(void);
/**
 * @brief   Flip the vertical orientation of the screen.
 */
void ss1306_orientation_flip_v(void);

/**
 * @brief   Set contrast.
 *
 * @param   contrast    Contrast value to set.
 */
void ssd1306_set_contrast(uint8_t contrast);

/**
 * @brief   Updates buffer from internal RAM to display.
 *
 * @note    This function must be called each time you do some changes to display, to update buffer from RAM to display.
 */
void ssd1306_update_screen(void);

/**
 * @brief   Fills entire LCD with desired color
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   color   Color to be used for screen fill. See @ref ssd1306_color_t.
 */
void ssd1306_fill(ssd1306_color_t color);

/**
 * @brief   Draws pixel at desired location
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   x   X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1.
 * @param   y   Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1.
 * @param   c   Color to be used for screen fill. See @ref ssd1306_color_t enumeration.
 */
void ssd1306_draw_pixel(uint16_t x, uint16_t y, ssd1306_color_t c);

/**
 * @brief   Sets cursor pointer to desired location for strings.
 *
 * @param   x   X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1.
 * @param   y   Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1.
 */
void ssd1306_goto_xy(uint16_t x, uint16_t y);

/**
 * @brief   Puts character to internal RAM
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   ch      Character to be written.
 * @param   font    Pointer to @ref fonts_t structure with used font.
 * @param   color   Color used for drawing. TSee @ref ssd1306_color_t.
 *
 * @retval  Character written.
 */
uint8_t ssd1306_putc(uint8_t ch, fonts_t *font, ssd1306_color_t color);

/**
 * @brief   Puts string to internal RAM.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   str     String to be written.
 * @param   font    Pointer to @ref fonts_t structure with used font.
 * @param   color   Color used for drawing. See @ref ssd1306_color_t.
 *
 * @retval  Zero on success or character value when function failed.
 */
uint8_t ssd1306_puts(uint8_t *str, fonts_t *font, ssd1306_color_t color);

/**
 * @brief   Draws line on display.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   x0  Line X start point. Valid input is 0 to SSD1306_WIDTH - 1.
 * @param   y0  Line Y start point. Valid input is 0 to SSD1306_HEIGHT - 1.
 * @param   x1  Line X end point. Valid input is 0 to SSD1306_WIDTH - 1.
 * @param   y1  Line Y end point. Valid input is 0 to SSD1306_HEIGHT - 1.
 * @param   c   Color to be used. See @ref ssd1306_color_t.
 */
void ssd1306_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, ssd1306_color_t c);

/**
 * @brief   Draws rectangle on display.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   x   Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1.
 * @param   y   Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1.
 * @param   w   Rectangle width in units of pixels.
 * @param   h   Rectangle height in units of pixels.
 * @param   c   Color to be used. See @ref ssd1306_color_t.
 */
void ssd1306_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c);

/**
 * @brief   Draws filled rectangle on display.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated LCD screen.
 *
 * @param   x   Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1.
 * @param   y   Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1.
 * @param   w   Rectangle width in units of pixels.
 * @param   h   Rectangle height in units of pixels.
 * @param   c   Color to be used. See @ref ssd1306_color_t.
 */
void ssd1306_draw_filled_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c);

/**
 * @brief   Draws triangle on display.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   x1  First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param   y1  First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param   x2  Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param   y2  Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param   x3  Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param   y3  Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param   c   Color to be used. See @ref ssd1306_color_t.
 */
void ssd1306_draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t c);

/**
 * @brief   Draws filled triangle on display.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   x1  First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param   y1  First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param   x2  Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param   y2  Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param   x3  Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param   y3  Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param   c   Color to be used. See @ref ssd1306_color_t.
 */
void ssd1306_draw_filled_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t c);

/**
 * @brief   Draws circle to buffer.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   x   X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param   y   Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param   r   Circle radius in units of pixels
 * @param   c   Color to be used. See @ref ssd1306_color_t.
 */
void ssd1306_draw_circle(int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c);

/**
 * @brief   Draws filled circle to buffer.
 *
 * @note    @ref ssd1306_update_screen() must be called after that in order to see updated display.
 *
 * @param   x   X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1.
 * @param   y   Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1.
 * @param   r   Circle radius in units of pixels.
 * @param   c   Color to be used. See@ref ssd1306_color_t.
 */
void ssd1306_draw_filled_circle(int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c);

#ifdef __cplusplus
}
#endif

#endif /* SSD1306_H_ */

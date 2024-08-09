/**
 **********************************************************************************************************************
 * @file        display_popup.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2018-02-12
 * @brief       Display pop-up C source file.
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
#include <stdio.h>
#include <string.h>

#include "display/display_popup.h"
#include "display/display.h"
#include "display/ssd1306.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/*
 *
 * +----------------A*----------------+
 * |                Y1                |
 * |    +-----------B*-----------+    |
 * |    |           Y2           |    |
 * |    |    +--------------+    |    |
 * | X1 | X2 |     TEXT     | X3 | X4 |
 * |    |    +--------------+    |    |
 * |    |           Y3           |    |
 * |    +------------------------+    |
 * |                Y4                |
 * +----------------------------------+
 */

#define DISPLAY_POPUP_BORDER_SIZE   1
#define DISPLAY_POPUP_BORDER_X1     3
#define DISPLAY_POPUP_BORDER_X2     10
#define DISPLAY_POPUP_BORDER_X3     10
#define DISPLAY_POPUP_BORDER_X4     3
#define DISPLAY_POPUP_BORDER_Y1     2
#define DISPLAY_POPUP_BORDER_Y2     10
#define DISPLAY_POPUP_BORDER_Y3     10
#define DISPLAY_POPUP_BORDER_Y4     2

#define DISPLAY_POPUP_BORDER_AX     (DISPLAY_POPUP_BORDER_X1 + DISPLAY_POPUP_BORDER_X2 + \
                                     DISPLAY_POPUP_BORDER_X3 + DISPLAY_POPUP_BORDER_X4 + \
                                     DISPLAY_POPUP_BORDER_SIZE + DISPLAY_POPUP_BORDER_SIZE)
#define DISPLAY_POPUP_BORDER_AY     (DISPLAY_POPUP_BORDER_Y1 + DISPLAY_POPUP_BORDER_Y2 + \
                                     DISPLAY_POPUP_BORDER_Y3 + DISPLAY_POPUP_BORDER_Y4 + \
                                     DISPLAY_POPUP_BORDER_SIZE + DISPLAY_POPUP_BORDER_SIZE)


#define DISPLAY_POPUP_BORDER_BX     (DISPLAY_POPUP_BORDER_X2 + DISPLAY_POPUP_BORDER_X3 + \
                                     DISPLAY_POPUP_BORDER_SIZE + DISPLAY_POPUP_BORDER_SIZE)
#define DISPLAY_POPUP_BORDER_BY     (DISPLAY_POPUP_BORDER_Y2 + DISPLAY_POPUP_BORDER_Y3 + \
                                     DISPLAY_POPUP_BORDER_SIZE + DISPLAY_POPUP_BORDER_SIZE)

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
 * Private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void display_popup_view(display_popup_t *data)
{
    uint8_t txt[16] = {0};
    uint8_t txt_len = 0;

    txt_len = snprintf((char *)txt, sizeof(txt), "%s", data->text);
    
    ssd1306_draw_filled_rectangle(
        ((SSD1306_WIDTH - (DISPLAY_POPUP_BORDER_AX + (fonts_7x10.font_width * txt_len))) / 2),
        ((SSD1306_HEIGHT - (DISPLAY_POPUP_BORDER_AY + fonts_7x10.font_height)) / 2),
        (DISPLAY_POPUP_BORDER_AX + (fonts_7x10.font_width  * txt_len)),
        (DISPLAY_POPUP_BORDER_AY + fonts_7x10.font_height),
        SSD1306_COLOR_BLACK);
    ssd1306_draw_filled_rectangle(
        ((SSD1306_WIDTH - (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len))) / 2),
        ((SSD1306_HEIGHT - (DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height)) / 2),
        (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len)),
        (DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height),
        SSD1306_COLOR_WHITE);

    ssd1306_draw_pixel(((SSD1306_WIDTH - (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len))) / 2),
        ((SSD1306_HEIGHT - (DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height)) / 2),
        SSD1306_COLOR_BLACK);

    ssd1306_draw_pixel(
        ((SSD1306_WIDTH - (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len))) / 2) +
            (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len)),
        ((DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height) / 2),
        SSD1306_COLOR_BLACK);

    ssd1306_draw_pixel(((SSD1306_WIDTH - (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len))) / 2),
        ((SSD1306_HEIGHT - (DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height)) / 2) + DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height,
        SSD1306_COLOR_BLACK);

    ssd1306_draw_pixel(
        ((SSD1306_WIDTH - (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len))) / 2) +
                    (DISPLAY_POPUP_BORDER_BX + (fonts_7x10.font_width * txt_len)),
        ((SSD1306_HEIGHT - (DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height)) / 2) + DISPLAY_POPUP_BORDER_BY + fonts_7x10.font_height,
        SSD1306_COLOR_BLACK);

    ssd1306_goto_xy(
        ((SSD1306_WIDTH - (fonts_7x10.font_width * txt_len)) / 2),
        ((SSD1306_HEIGHT - fonts_7x10.font_height) / 2) + 1);
    ssd1306_puts(txt, &fonts_7x10, SSD1306_COLOR_BLACK);

    ssd1306_update_screen();

    return;
}

/**
 **********************************************************************************************************************
 * @file         ssd1306.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-08-29
 * @brief        SSD1306 OLED display C source file.
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

#include "ssd1306.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

#include "cmsis_os.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/** Write command. */
//#define SSD1306_WRITECOMMAND(command)       i2c_write_reg((SSD1306_I2C_ADDR >> 1), 0x00, (command))
/** Write data. */
//#define SSD1306_WRITEDATA(data)             i2c_write_reg((SSD1306_I2C_ADDR >> 1), 0x40, (data))
/** Absolute value. */
#define ABS(x)   ((x) > 0 ? (x) : -(x))


/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/* SSD1306 data buffer */
static uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8 + 1];

typedef struct {
    uint16_t current_x;
    uint16_t current_y;
    uint8_t inverted;
    uint8_t initialized;
    bool orientation_h;
    bool orientation_v;
} ssd1306_t;

static ssd1306_t ssd1306_data = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void ssd1306_write_cmd(uint8_t command);
static void ssd1306_write_data(uint8_t *data, uint16_t size);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool ssd1306_init(void)
{
    /* Init LCD */
    ssd1306_write_cmd(0xAE); //display off
    ssd1306_write_cmd(0x20); //Set Memory Addressing Mode
    ssd1306_write_cmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    ssd1306_write_cmd(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
    ssd1306_write_cmd(0xC8); //Set COM Output Scan Direction
    ssd1306_write_cmd(0x00); //---set low column address
    ssd1306_write_cmd(0x10); //---set high column address
    ssd1306_write_cmd(0x40); //--set start line address
    ssd1306_write_cmd(0x81); //--set contrast control register
    ssd1306_write_cmd(0xFF);
    ssd1306_write_cmd(0xA1); //--set segment re-map 0 to 127
    ssd1306_write_cmd(0xA6); //--set normal display
    ssd1306_write_cmd(0xA8); //--set multiplex ratio(1 to 64)
    ssd1306_write_cmd(0x3F); //
    ssd1306_write_cmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    ssd1306_write_cmd(0xD3); //-set display offset
    ssd1306_write_cmd(0x00); //-not offset
    ssd1306_write_cmd(0xD5); //--set display clock divide ratio/oscillator frequency
    ssd1306_write_cmd(0xF0); //--set divide ratio
    ssd1306_write_cmd(0xD9); //--set pre-charge period
    ssd1306_write_cmd(0x22); //
    ssd1306_write_cmd(0xDA); //--set com pins hardware configuration
    ssd1306_write_cmd(0x12);
    ssd1306_write_cmd(0xDB); //--set vcomh
    ssd1306_write_cmd(0x20); //0x20,0.77xVcc
    ssd1306_write_cmd(0x8D); //--set DC-DC enable
    ssd1306_write_cmd(0x14); //
    ssd1306_write_cmd(0xAF); //--turn on SSD1306 panel

    /* Clear screen */
    ssd1306_fill(SSD1306_COLOR_BLACK);

    /* Update screen */
    ssd1306_update_screen();

    /* Set default values */
    ssd1306_data.current_x = 0;
    ssd1306_data.current_y = 0;

    /* Initialized OK */
    ssd1306_data.initialized = 1;

    /* Return OK */
    return true;
}

void ssd1306_display_on(void)
{
    ssd1306_write_cmd(0x8D);
    ssd1306_write_cmd(0x14);
    ssd1306_write_cmd(0xAF);

    return;
}

void ssd1306_display_off(void)
{
    ssd1306_write_cmd(0x8D);
    ssd1306_write_cmd(0x10);
    ssd1306_write_cmd(0xAE);

    return;
}

void ssd1306_display_invert(void)
{
    ssd1306_write_cmd(0xA7);

    return;
}

void ssd1306_display_normal(void)
{
    ssd1306_write_cmd(0xA6);

    return;
}

void ssd1306_set_contrast(uint8_t contrast)
{
    ssd1306_write_cmd(0x81);
    ssd1306_write_cmd(contrast);

    return;
}

void ss1306_orientation_flip_h(void)
{
    ssd1306_data.orientation_h = ssd1306_data.orientation_h == true ? false : true;
    if(ssd1306_data.orientation_h)
    {
        ssd1306_write_cmd(0xC0 | 0x08);
    }
    else
    {
        ssd1306_write_cmd(0xC0 | 0x00);
    }

    return;
}

void ss1306_orientation_flip_v(void)
{
    ssd1306_data.orientation_v = ~ssd1306_data.orientation_v == true ? false : true;
    if(ssd1306_data.orientation_v)
    {
        ssd1306_write_cmd(0xA0 | 0x01);
    }
    else
    {
        ssd1306_write_cmd(0xA0 | 0x00);
    }
    ssd1306_write_cmd(0xC8);

    return;
}

void ssd1306_update_screen(void)
{
    uint8_t y = 0;

    for(y = 0; y < SSD1306_HEIGHT / 8; y++)
    {
        ssd1306_write_cmd(0xB0 + y);
        ssd1306_write_cmd(0x02);
        ssd1306_write_cmd(0x10);

        ssd1306_write_data(&ssd1306_buffer[SSD1306_WIDTH * y], SSD1306_WIDTH);
    }
    osDelay(1);

    return;
}

void ssd1306_toggle_invert(void)
{
    uint16_t i;

    /* Toggle invert */
    ssd1306_data.inverted = !ssd1306_data.inverted;

    /* Do memory toggle */
    for(i = 0; i < sizeof(ssd1306_buffer); i++)
    {
        ssd1306_buffer[i] = ~ssd1306_buffer[i];
    }

    return;
}

void ssd1306_fill(ssd1306_color_t color)
{
    /* Set memory */
    memset(ssd1306_buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(ssd1306_buffer));

    return;
}

void ssd1306_draw_pixel(uint16_t x, uint16_t y, ssd1306_color_t c)
{
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        /* Error */
        return;
    }

    /* Check if pixels are inverted */
    if(ssd1306_data.inverted)
    {
        c = (ssd1306_color_t)!c;
    }

    /* Set color */
    if(c == SSD1306_COLOR_WHITE)
    {
        ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    }
    else
    {
        ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }

    return;
}

void ssd1306_goto_xy(uint16_t x, uint16_t y)
{
    /* Set write pointers */
    ssd1306_data.current_x = x;
    ssd1306_data.current_y = y;

    return;
}

uint8_t ssd1306_putc(uint8_t ch, fonts_t *font, ssd1306_color_t color)
{
    uint32_t i = 0;
    uint32_t b = 0;
    uint32_t j = 0;

    /* Check available space in LCD */
    if(SSD1306_WIDTH <= (ssd1306_data.current_x + font->font_width)
        || SSD1306_HEIGHT <= (ssd1306_data.current_y + font->font_height))
    {
        /* Error */
        return 0;
    }

    /* Go through font */
    for(i = 0; i < font->font_height; i++)
    {
        b = font->data[(ch - 32) * font->font_height + i];
        for(j = 0; j < font->font_width; j++)
        {
            if((b << j) & 0x8000)
            {
                ssd1306_draw_pixel(ssd1306_data.current_x + j, (ssd1306_data.current_y + i), (ssd1306_color_t)color);
            }
            else
            {
                ssd1306_draw_pixel(ssd1306_data.current_x + j, (ssd1306_data.current_y + i), (ssd1306_color_t)!color);
            }
        }
    }

    /* Increase pointer */
    ssd1306_data.current_x += font->font_width;

    /* Return character written */
    return ch;
}

uint8_t ssd1306_puts(uint8_t *str, fonts_t *font, ssd1306_color_t color)
{
    /* Write characters */
    while(*str)
    {
        /* Write character by character */
        if(ssd1306_putc(*str, font, color) != *str)
        {
            /* Return error */
            return *str;
        }

        /* Increase string pointer */
        str++;
    }

    /* Everything OK, zero should be returned */
    return *str;
}

void ssd1306_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, ssd1306_color_t c)
{
    int16_t dx = 0;
    int16_t dy =0;
    int16_t sx = 0;
    int16_t sy = 0;
    int16_t err = 0;
    int16_t e2 = 0;
    int16_t i = 0;
    int16_t tmp = 0;

    /* Check for overflow */
    if(x0 >= SSD1306_WIDTH)
    {
        x0 = SSD1306_WIDTH - 1;
    }
    if(x1 >= SSD1306_WIDTH)
    {
        x1 = SSD1306_WIDTH - 1;
    }
    if(y0 >= SSD1306_HEIGHT)
    {
        y0 = SSD1306_HEIGHT - 1;
    }
    if(y1 >= SSD1306_HEIGHT)
    {
        y1 = SSD1306_HEIGHT - 1;
    }

    dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if(dx == 0)
    {
        if(y1 < y0)
        {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if(x1 < x0)
        {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Vertical line */
        for(i = y0; i <= y1; i++)
        {
            ssd1306_draw_pixel(x0, i, c);
        }

        /* Return from function */
        return;
    }

    if(dy == 0)
    {
        if(y1 < y0)
        {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if(x1 < x0)
        {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Horizontal line */
        for(i = x0; i <= x1; i++)
        {
            ssd1306_draw_pixel(i, y0, c);
        }

        /* Return from function */
        return;
    }

    while(1)
    {
        ssd1306_draw_pixel(x0, y0, c);
        if(x0 == x1 && y0 == y1)
        {
            break;
        }
        e2 = err;
        if(e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if(e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }

    return;
}

void ssd1306_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c)
{
    /* Check input parameters */
    if(x >= SSD1306_WIDTH ||  y >= SSD1306_HEIGHT)
    {
        /* Return error */
        return;
    }

    /* Check width and height */
    if((x + w) >= SSD1306_WIDTH)
    {
        w = SSD1306_WIDTH - x;
    }
    if((y + h) >= SSD1306_HEIGHT)
    {
        h = SSD1306_HEIGHT - y;
    }

    /* Draw 4 lines */
    ssd1306_draw_line(x, y, x + w, y, c);           /* Top line */
    ssd1306_draw_line(x, y + h, x + w, y + h, c);   /* Bottom line */
    ssd1306_draw_line(x, y, x, y + h, c);           /* Left line */
    ssd1306_draw_line(x + w, y, x + w, y + h, c);   /* Right line */

    return;
}

void ssd1306_draw_filled_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t c)
{
    uint8_t i = 0;

    /* Check input parameters */
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        /* Return error */
        return;
    }

    /* Check width and height */
    if((x + w) >= SSD1306_WIDTH)
    {
        w = SSD1306_WIDTH - x;
    }
    if((y + h) >= SSD1306_HEIGHT)
    {
        h = SSD1306_HEIGHT - y;
    }

    /* Draw lines */
    for(i = 0; i <= h; i++)
    {
        /* Draw lines */
        ssd1306_draw_line(x, y + i, x + w, y + i, c);
    }

    return;
}

void ssd1306_draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t c)
{
    /* Draw lines */
    ssd1306_draw_line(x1, y1, x2, y2, c);
    ssd1306_draw_line(x2, y2, x3, y3, c);
    ssd1306_draw_line(x3, y3, x1, y1, c);

    return;
}

void ssd1306_draw_filled_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t c)
{
    int16_t deltax = 0;
    int16_t deltay = 0;
    int16_t x = 0;
    int16_t y = 0;
    int16_t xinc1 = 0;
    int16_t xinc2 = 0;
    int16_t yinc1 = 0;
    int16_t yinc2 = 0;
    int16_t den = 0;
    int16_t num = 0;
    int16_t numadd = 0;
    int16_t numpixels = 0;
    int16_t curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if(x2 >= x1)
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if(y2 >= y1)
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if(deltax >= deltay)
    {
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    }
    else
    {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for(curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        ssd1306_draw_line(x, y, x3, y3, c);

        num += numadd;
        if(num >= den)
        {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }

    return;
}

void ssd1306_draw_circle(int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ssd1306_draw_pixel(x0, y0 + r, c);
    ssd1306_draw_pixel(x0, y0 - r, c);
    ssd1306_draw_pixel(x0 + r, y0, c);
    ssd1306_draw_pixel(x0 - r, y0, c);

    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ssd1306_draw_pixel(x0 + x, y0 + y, c);
        ssd1306_draw_pixel(x0 - x, y0 + y, c);
        ssd1306_draw_pixel(x0 + x, y0 - y, c);
        ssd1306_draw_pixel(x0 - x, y0 - y, c);

        ssd1306_draw_pixel(x0 + y, y0 + x, c);
        ssd1306_draw_pixel(x0 - y, y0 + x, c);
        ssd1306_draw_pixel(x0 + y, y0 - x, c);
        ssd1306_draw_pixel(x0 - y, y0 - x, c);
    }

    return;
}

void ssd1306_draw_filled_circle(int16_t x0, int16_t y0, int16_t r, ssd1306_color_t c)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ssd1306_draw_pixel(x0, y0 + r, c);
    ssd1306_draw_pixel(x0, y0 - r, c);
    ssd1306_draw_pixel(x0 + r, y0, c);
    ssd1306_draw_pixel(x0 - r, y0, c);
    ssd1306_draw_line(x0 - r, y0, x0 + r, y0, c);

    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ssd1306_draw_line(x0 - x, y0 + y, x0 + x, y0 + y, c);
        ssd1306_draw_line(x0 + x, y0 - y, x0 - x, y0 - y, c);

        ssd1306_draw_line(x0 + y, y0 + x, x0 - y, y0 + x, c);
        ssd1306_draw_line(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void ssd1306_write_cmd(uint8_t command)
{
#if SSD1306_DRV_MODE
    i2c_write_reg((SSD1306_I2C_ADDR >> 1), 0x00, (command));
#else
    gpio_output_high(GPIO_DISPLAY_SELECT);
    gpio_output_low(GPIO_DISPLAY_DC);
    gpio_output_low(GPIO_DISPLAY_SELECT);
    spi_0_write_buffer(&command, 1);
    gpio_output_high(GPIO_DISPLAY_SELECT);
#endif

    return;
}

static void ssd1306_write_data(uint8_t *data, uint16_t size)
{
#if SSD1306_DRV_MODE
    i2c_write_reg_multi((SSD1306_I2C_ADDR >> 1), 0x40, data, size);
#else
    gpio_output_high(GPIO_DISPLAY_SELECT);
    gpio_output_high(GPIO_DISPLAY_DC);
    gpio_output_low(GPIO_DISPLAY_SELECT);
    spi_0_write_buffer(data, size);
    gpio_output_high(GPIO_DISPLAY_SELECT);
#endif

    return;
}

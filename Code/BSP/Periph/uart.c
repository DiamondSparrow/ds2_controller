/**
 **********************************************************************************************************************
 * @file         uart.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-10
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

#include "chip.h"

#include "uart.h"

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
/* Transmit and receive ring buffers */
__IO RINGBUFF_T uart0_tx_rb;
__IO RINGBUFF_T uart0_rx_rb;
uint8_t uart_0_rx_buffer[UART_0_RX_BUFFER_SIZE];
uint8_t uart_0_tx_buffer[UART_0_TX_BUFFER_SIZE];

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void uart_0_init(void)
{
    // Enable the clock to the Switch Matrix.
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    /* Disables pullups/pulldowns and enable digitial mode */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 13, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));

    /* UART signal muxing via SWM */
    Chip_SWM_MovablePortPinAssign(SWM_UART0_RXD_I, 0, 13);
    Chip_SWM_MovablePortPinAssign(SWM_UART0_TXD_O, 0, 18);

    // Disable the clock to the Switch Matrix to save power .
    Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    /* Use main clock rate as base for UART baud rate divider */
    Chip_Clock_SetUARTBaseClockRate(Chip_Clock_GetMainClockRate(), false);

    /* Setup UART */
    Chip_UART_Init(LPC_USART0);
    Chip_UART_ConfigData(LPC_USART0, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
    Chip_UART_SetBaud(LPC_USART0, UART_0_BAUDRATE);
    Chip_UART_Enable(LPC_USART0);
    Chip_UART_TXEnable(LPC_USART0);

    /* Before using the ring buffers, initialize them using the ring buffer init function */
    RingBuffer_Init((RINGBUFF_T *)&uart0_rx_rb, uart_0_rx_buffer, 1, UART_0_RX_BUFFER_SIZE);
    RingBuffer_Init((RINGBUFF_T *)&uart0_tx_rb, uart_0_tx_buffer, 1, UART_0_TX_BUFFER_SIZE);

    /* Enable receive data and line status interrupt */
    Chip_UART_IntEnable(LPC_USART0, UART_INTEN_RXRDY);
    Chip_UART_IntDisable(LPC_USART0, UART_INTEN_TXRDY); /* May not be needed */

    /* Enable UART interrupt */
    NVIC_EnableIRQ(UART0_IRQn);

    return;
}

void uart_0_send(uint8_t *data, uint32_t size)
{
    Chip_UART_SendBlocking(LPC_USART0, data, size);

    return;
}

void uart_0_send_rb(uint8_t *data, uint32_t size)
{
    Chip_UART_SendRB(LPC_USART0, (RINGBUFF_T *)&uart0_tx_rb, data, size);

    return;
}

uint32_t uart_0_read_rb(uint8_t *data, uint32_t size)
{
    return Chip_UART_ReadRB(LPC_USART0, (RINGBUFF_T *)&uart0_rx_rb, data, size);
}

void UART0_IRQHandler(void)
{
    Chip_UART_IRQRBHandler(LPC_USART0, (RINGBUFF_T *)&uart0_rx_rb, (RINGBUFF_T *)&uart0_tx_rb);

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/

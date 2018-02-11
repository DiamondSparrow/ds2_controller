/**
 **********************************************************************************************************************
 * @file         adc.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-10-12
 * @brief        ADC C source file.
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

#include "adc.h"

#include "chip.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define ADC_REFERENCE               3300
#define ADC_RESOLUTION              4096
#define ADC_AVG_COUNT               10
#define ADC_TEMP_SENSOR_LLS_SLOPE   (-2.29) //!< Temperature sensor Linear-Least-Square slope (mV/degC).
#define ADC_TEMP_SENSOR_LLS_0       (577.3) //!< Temperature sensor Linear-Least-Square slope LLS intercept at 0 degC.

#define ADC_1_ENABLE                0

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct
{
    LPC_ADC_T *adc;
    uint8_t ch;
    uint8_t port;
    uint8_t pin;
    CHIP_SWM_PIN_FIXED_T sw_pin;
    uint32_t value;
    struct
    {
        uint32_t counter;
        uint32_t value;
        uint32_t accumulator;
    } avg;
} adc_data_t;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
adc_data_t adc_data[ADC_ID_LAST] =
{
    {.adc = LPC_ADC0, .ch = 0,   .port = 0xFF,   .pin = 0xFF,    .sw_pin = SWM_FIXED_ADC0_0, .value = 0, {0, 0, 0}},
    {.adc = LPC_ADC0, .ch = 2,   .port = 0,      .pin = 6,       .sw_pin = SWM_FIXED_ADC0_2, .value = 0, {0, 0, 0}},
    {.adc = LPC_ADC0, .ch = 3,   .port = 0,      .pin = 5,       .sw_pin = SWM_FIXED_ADC0_3, .value = 0, {0, 0, 0}},
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void adc_0_init(void);
#if ADC_1_ENABLE
static void adc_1_init(void);
#endif

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void adc_init(void)
{
    adc_0_init();
#if ADC_1_ENABLE
    adc_1_init();
#endif

    return;
}

uint32_t adc_get_value_raw(adc_id_t id)
{
#if ADC_AVG_COUNT > 1
    return (adc_data[id].avg.value);
#else
    uint32_t value = adc_data[ch].value;

    if(!(value & ADC_DR_OVERRUN) && (value & ADC_SEQ_GDAT_DATAVALID))
    {
        return ADC_DR_RESULT(value);
    }

    return UINT32_MAX;
#endif
}

uint32_t adc_get_value_volt(adc_id_t id)
{
    uint32_t value = adc_get_value_raw(id);

    if(value != UINT32_MAX)
    {
        return (value * ADC_REFERENCE) / ADC_RESOLUTION;
    }

    return UINT32_MAX;
}

float adc_get_temperature(void)
{
    uint32_t value = adc_get_value_volt(ADC_ID_TEMPERATURE);

    if(value != UINT32_MAX)
    {
        return (float)(((float)value  - (float)ADC_TEMP_SENSOR_LLS_0 ) / (float)ADC_TEMP_SENSOR_LLS_SLOPE);
    }

    return 0;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void adc_0_init(void)
{
    uint8_t i = 0;

    /* Setup ADC for 12-bit mode and normal power */
    Chip_ADC_Init(LPC_ADC0, 0);

    /* Setup for maximum ADC clock rate */
    Chip_ADC_SetClockRate(LPC_ADC0, ADC_MAX_SAMPLE_RATE);

    /*
     * For ADC0, seqeucner A will be used without threshold events.
     * It will be triggered manually by the sysTick interrupt and
     * only monitor the internal temperature sensor.
     */
    Chip_ADC_SetupSequencer(LPC_ADC0, ADC_SEQA_IDX, (ADC_SEQ_CTRL_CHANSEL(0) | ADC_SEQ_CTRL_CHANSEL(2) | ADC_SEQ_CTRL_CHANSEL(3) | ADC_SEQ_CTRL_BURST | ADC_SEQ_CTRL_MODE_EOS));

    /* Power up the internal temperature sensor */
    Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_TS_PD);

    /* For ADC0, select temperature sensor for channel 0 on ADC0 */
    Chip_ADC_SetADC0Input(LPC_ADC0, ADC_INSEL_TS);

    /* Use higher voltage trim for both ADCs */
    Chip_ADC_SetTrim(LPC_ADC0, ADC_TRIM_VRANGE_HIGHV);

    for(i = 0; i < ADC_ID_LAST; i++)
    {
        if(adc_data[i].adc != LPC_ADC0)
        {
            continue;
        }
        if(adc_data[i].port != 0xFF && adc_data[i].pin != 0xFF)
        {
            /* Disables pullups/pulldowns and disable digital mode */
            Chip_IOCON_PinMuxSet(LPC_IOCON, adc_data[i].port, adc_data[i].pin, (IOCON_MODE_INACT | IOCON_ADMODE_EN));
            /* Assign ADC1_0 to PIO1_1 via SWM (fixed pin) */
            Chip_SWM_EnableFixedPin(adc_data[i].sw_pin);
        }
    }

    /* Need to do a calibration after initialization and trim */
    Chip_ADC_StartCalibration(LPC_ADC0);
    while (!(Chip_ADC_IsCalibrationDone(LPC_ADC0))) {}

    /* Clear all pending interrupts */
    Chip_ADC_ClearFlags(LPC_ADC0, Chip_ADC_GetFlags(LPC_ADC0));

    /* Enable sequence A completion interrupts for ADC0 */
    Chip_ADC_EnableInt(LPC_ADC0, ADC_INTEN_SEQA_ENABLE);

    /* Enable related ADC NVIC interrupts */
    NVIC_EnableIRQ(ADC0_SEQA_IRQn);

    /* Enable sequencers */
    Chip_ADC_EnableSequencer(LPC_ADC0, ADC_SEQA_IDX);

    return;
}

/**
 * @brief   Handle interrupt from ADC0 sequencer A
 */
void ADC0A_IRQHandler(void)
{
    uint8_t i = 0;
    uint32_t pending;

    /* Get pending interrupts */
    pending = Chip_ADC_GetFlags(LPC_ADC0);

    /* Sequence A completion interrupt */
    if (pending & ADC_FLAGS_SEQA_INT_MASK)
    {
        for(i = 0; i < ADC_ID_LAST; i++)
        {
            if(adc_data[i].adc != LPC_ADC0)
            {
                continue;
            }
            adc_data[i].value = Chip_ADC_GetDataReg(LPC_ADC0, adc_data[i].ch);
#if ADC_AVG_COUNT > 1
            if(adc_data[i].value & ADC_SEQ_GDAT_DATAVALID)
            {
                adc_data[i].avg.accumulator += ADC_DR_RESULT(adc_data[i].value);
                adc_data[i].avg.counter++;
                if(adc_data[i].avg.counter >= ADC_AVG_COUNT)
                {
                    adc_data[i].avg.value = adc_data[i].avg.accumulator / ADC_AVG_COUNT;
                    adc_data[i].avg.accumulator = 0;
                    adc_data[i].avg.counter = 0;
                }
            }
#endif
        }
    }

    /* Clear Sequence A completion interrupt */
    Chip_ADC_ClearFlags(LPC_ADC0, ADC_FLAGS_SEQA_INT_MASK);

    return;
}

#if ADC_1_ENABLE
static void adc_1_init(void)
{
    uint8_t i = 0;

    /* Setup ADC for 12-bit mode and normal power */
    Chip_ADC_Init(LPC_ADC1, 0);

    /* Setup for maximum ADC clock rate */
    Chip_ADC_SetClockRate(LPC_ADC1, ADC_MAX_SAMPLE_RATE);

    /*
     * For ADC0, seqeucner A will be used without threshold events.
     * It will be triggered manually by the sysTick interrupt and
     * only monitor the internal temperature sensor.
     */
    Chip_ADC_SetupSequencer(LPC_ADC1, ADC_SEQA_IDX, (ADC_SEQ_CTRL_CHANSEL(1) | ADC_SEQ_CTRL_CHANSEL(4) | ADC_SEQ_CTRL_BURST | ADC_SEQ_CTRL_MODE_EOS));

    /* Use higher voltage trim for both ADCs */
    //Chip_ADC_SetTrim(LPC_ADC1, ADC_TRIM_VRANGE_HIGHV);

    for(i = 0; i < ADC_ID_LAST; i++)
    {
        if(adc_data[i].adc != LPC_ADC1)
        {
            continue;
        }
        if(adc_data[i].port != 0xFF && adc_data[i].pin != 0xFF)
        {
            /* Disables pullups/pulldowns and disable digital mode */
            Chip_IOCON_PinMuxSet(LPC_IOCON, adc_data[i].port, adc_data[i].pin, (IOCON_MODE_INACT | IOCON_ADMODE_EN));
            /* Assign ADC1_0 to PIO1_1 via SWM (fixed pin) */
            Chip_SWM_EnableFixedPin(adc_data[i].sw_pin);
        }
    }

    /* Need to do a calibration after initialization and trim */
    Chip_ADC_StartCalibration(LPC_ADC1);
    while (!(Chip_ADC_IsCalibrationDone(LPC_ADC1))) {}

    /* Clear all pending interrupts */
    Chip_ADC_ClearFlags(LPC_ADC1, Chip_ADC_GetFlags(LPC_ADC1));

    /* Enable sequence A completion interrupts for ADC0 */
    Chip_ADC_EnableInt(LPC_ADC1, ADC_INTEN_SEQA_ENABLE);

    /* Enable related ADC NVIC interrupts */
    NVIC_EnableIRQ(ADC1_SEQA_IRQn);

    /* Enable sequencers */
    Chip_ADC_EnableSequencer(LPC_ADC1, ADC_SEQA_IDX);

    return;
}


/**
 * @brief   Handle interrupt from ADC1 sequencer A
 */
void ADC1A_IRQHandler(void)
{
    uint8_t i = 0;
    uint32_t pending;

    /* Get pending interrupts */
    pending = Chip_ADC_GetFlags(LPC_ADC1);

    /* Sequence A completion interrupt */
    if (pending & ADC_FLAGS_SEQA_INT_MASK)
    {
        for(i = 0; i < ADC_ID_LAST; i++)
        {
            if(adc_data[i].adc != LPC_ADC1)
            {
                continue;
            }
            adc_data[i].value = Chip_ADC_GetDataReg(LPC_ADC1, adc_data[i].ch);
#if ADC_AVG_COUNT > 1
            if(adc_data[i].value & ADC_SEQ_GDAT_DATAVALID)
            {
                adc_data[i].avg.accumulator += ADC_DR_RESULT(adc_data[i].value);
                adc_data[i].avg.counter++;
                if(adc_data[i].avg.counter >= ADC_AVG_COUNT)
                {
                    adc_data[i].avg.value = adc_data[i].avg.accumulator / ADC_AVG_COUNT;
                    adc_data[i].avg.accumulator = 0;
                    adc_data[i].avg.counter = 0;
                }
            }
            else
            {
                adc_data[i].avg.accumulator = 0;
                adc_data[i].avg.counter = 0;
            }
#endif
        }
    }

    /* Clear Sequence A completion interrupt */
    Chip_ADC_ClearFlags(LPC_ADC1, ADC_FLAGS_SEQA_INT_MASK);

    return;
}
#endif // ADC_1_ENABLE


#include "zerynth.h"
#include "zerynth_debug.h"

// static VSysTimer colorSensing_timer;
static uint32_t colorSensing_counter = 0;
static uint8_t  colorSensing_counting = 1;
typedef enum {
    COLOR_RED   = 0,
    COLOR_GREEN = 1,
    COLOR_BLUE  = 2,
    COLOR_CLEAR = 3,
    COLOR_MAX   = 4,
} colorSensing_colors_t;

typedef enum {
    SCALE_2    = 0,
    SCALE_20   = 1,
    SCALE_100  = 2,
    POWER_DOWN = 3,
    FREQ_LIMIT = 4,
} colorSensing_freqScaling_t;

typedef struct 
{
    void*   port;
    int     pad;
} colorSensing_controlPin_t;

typedef struct
{
    int s0;
    int s1;
    int s2;
    int s3;
    int out;
    int led;
    int oe;
} colorSensing_pins_t;

typedef struct
{
    uint8_t  freqScale;
    uint32_t timerPeriod;
    uint8_t  ledOn;
} colorSensing_options_t;

static colorSensing_controlPin_t colorSensing_controlPin;
static colorSensing_pins_t  colorSensing_pins;
static colorSensing_colors_t colorSensing_color_element = COLOR_CLEAR;
static vhal_pcnt_unit_t colorSensing_unit = 0;
static uint32_t colorSensing_color[4] = {0, 0, 0, 0};
static colorSensing_options_t colorSensing_options;


static inline void ColorSensing_StartCount()
{
    vhalPinFastSet(colorSensing_controlPin.port, colorSensing_controlPin.pad);
    colorSensing_counting = 1;
}

static inline void ColorSensing_StopCount()
{
    vhalPinFastClear(colorSensing_controlPin.port, colorSensing_controlPin.pad);
    colorSensing_counting = 0;
}

void ColorSensing_FrequencyScaling(colorSensing_freqScaling_t freq)
{
    vhal_error_t err = -1;
    switch (freq)
    {
        case SCALE_100:
            err = vhalPinWrite(colorSensing_pins.s0, 1);
            err = vhalPinWrite(colorSensing_pins.s1, 1);
            break;
        case SCALE_20:
            err = vhalPinWrite(colorSensing_pins.s0, 1);
            err = vhalPinWrite(colorSensing_pins.s1, 0);
            break;
        case SCALE_2:
            err = vhalPinWrite(colorSensing_pins.s0, 0);
            err = vhalPinWrite(colorSensing_pins.s1, 1);
            break;
        case POWER_DOWN:
            err = vhalPinWrite(colorSensing_pins.s0, 0);
            err = vhalPinWrite(colorSensing_pins.s1, 0);
            // Total power down
            err = vhalPinWrite(colorSensing_pins.oe, 1);
            break;
        default:
            // Should never come here
            break;
    }
}

void ColorSensing_PrepareColor(uint32_t color)
{
    vhal_error_t err = -1;
    switch (color)
    {
        case COLOR_RED:
            err = vhalPinWrite(colorSensing_pins.s2, 0);
            err = vhalPinWrite(colorSensing_pins.s3, 0);
            break;
        case COLOR_BLUE:
            err = vhalPinWrite(colorSensing_pins.s2, 0);
            err = vhalPinWrite(colorSensing_pins.s3, 1);
            break;
        case COLOR_GREEN:
            err = vhalPinWrite(colorSensing_pins.s2, 1);
            err = vhalPinWrite(colorSensing_pins.s3, 1);
            break;
        default:
            err = vhalPinWrite(colorSensing_pins.s2, 1);
            err = vhalPinWrite(colorSensing_pins.s3, 0);
            break;
    }
}


uint32_t ColorSensing_GetColorSegment(uint32_t colorElement)
{
    vhal_error_t err = 0;
    uint16_t count = 0;
    ColorSensing_PrepareColor(colorElement);
    err = vhalPcntClear(colorSensing_unit);
    err = vhalPcntResume(colorSensing_unit);

    // ColorSensing_StartCount();
    // ZDEBUG("Timer set to %d\n", colorSensing_timerPeriod);
    vosThSleep(TIME_U(colorSensing_options.timerPeriod, MILLIS));
    // vosTimerOneShot(colorSensing_timer, TIME_U(colorSensing_timerPeriod, MILLIS), ColorSensing_StopCount, NULL);
    // while (colorSensing_counting) 
    //     ;
    err = vhalPcntCount(colorSensing_unit, &count);
    // err = vhalPcntClear(colorSensing_unit);

    return count;
}

void ColorSensing_GetColor()
{
    ColorSensing_FrequencyScaling(colorSensing_options.freqScale);
    if (colorSensing_options.ledOn)
        vhalPinWrite(colorSensing_pins.led, 1);
    for (int col = COLOR_RED; col < COLOR_MAX; col++)
    {
        colorSensing_color[col] = ColorSensing_GetColorSegment(col);
    }
    vhalPinWrite(colorSensing_pins.led, 0);
    ColorSensing_FrequencyScaling(POWER_DOWN);
}

void ColorSensing_SetFrequencyScale(uint8_t scale)
{
    if ((scale < FREQ_LIMIT) && (scale > 0))
    {
        colorSensing_options.freqScale = scale;
    }
}

void ColorSensing_SetMeasureTiming(uint32_t time_ms)
{
    if (time_ms)
    {
        colorSensing_options.timerPeriod = time_ms;
    }
}

void ColorSensing_SetLedLight(uint8_t ledOn)
{
    colorSensing_options.ledOn = ledOn;
}

C_NATIVE(_init_color_sensor) {
    C_NATIVE_UNWARN();
    // s0, s1, s2, s3, out, led, oe, ctrl, timerTime
    PYC_CHECK_NUM_ARGS(9);
    PYC_CHECK_ARG_INTEGER(0);
    PYC_CHECK_ARG_INTEGER(1);
    PYC_CHECK_ARG_INTEGER(2);
    PYC_CHECK_ARG_INTEGER(3);
    PYC_CHECK_ARG_INTEGER(4);
    PYC_CHECK_ARG_INTEGER(5);
    PYC_CHECK_ARG_INTEGER(6);
    PYC_CHECK_ARG_INTEGER(7);
    PYC_CHECK_ARG_INTEGER(8);

    colorSensing_pins.s0  = PYC_ARG_INT(0);
    colorSensing_pins.s1  = PYC_ARG_INT(1);
    colorSensing_pins.s2  = PYC_ARG_INT(2);
    colorSensing_pins.s3  = PYC_ARG_INT(3);
    colorSensing_pins.out = PYC_ARG_INT(4);
    colorSensing_pins.led = PYC_ARG_INT(5);
    colorSensing_pins.oe  = PYC_ARG_INT(6);
    
    ZDEBUG("start\n");

    vhal_error_t err = -1;
    err = vhalPinSetMode(colorSensing_pins.s0, PINMODE_OUTPUT_PUSHPULL, EXT_INTR_DISABLE);
    err = vhalPinSetMode(colorSensing_pins.s1, PINMODE_OUTPUT_PUSHPULL, EXT_INTR_DISABLE);
    err = vhalPinSetMode(colorSensing_pins.s2, PINMODE_OUTPUT_PUSHPULL, EXT_INTR_DISABLE);
    err = vhalPinSetMode(colorSensing_pins.s3, PINMODE_OUTPUT_PUSHPULL, EXT_INTR_DISABLE);
    err = vhalPinSetMode(colorSensing_pins.oe, PINMODE_OUTPUT_PUSHPULL, EXT_INTR_DISABLE);
    err = vhalPinSetMode(colorSensing_pins.led, PINMODE_OUTPUT_PUSHPULL, EXT_INTR_DISABLE);
    err = vhalPinSetMode(colorSensing_pins.out, PINMODE_INPUT_PULLNONE, EXT_INTR_DISABLE);
   
    ZDEBUG("OK\n");

    int controlPin = PYC_ARG_INT(7);
    err = vhalPinSetMode(controlPin, PINMODE_OUTPUT_PUSHPULL, EXT_INTR_DISABLE);
    colorSensing_controlPin.port = vhalPinGetPort(controlPin);
    colorSensing_controlPin.pad = vhalPinGetPad(controlPin);

    ZDEBUG("Input, ctrl pins: %d, %d\n", colorSensing_pins.out, controlPin);

    vhal_pcnt_config_t config = 
    {
        .pulse_gpio_num = colorSensing_pins.out, 
        .ctrl_gpio_num = controlPin,
        .lctrl_mode = VHAL_PCNT_MODE_DISABLE,
        .hctrl_mode = VHAL_PCNT_MODE_KEEP,
        .pos_mode = VHAL_PCNT_COUNT_INC,
        .neg_mode = VHAL_PCNT_COUNT_DIS,
        .counter_h_lim = 10000,
        .counter_l_lim = -10000,
        .unit = VHAL_PCNT_UNIT_0,
        .channel = VHAL_PCNT_CHANNEL_0,
    };

    colorSensing_options.freqScale = SCALE_100;
    colorSensing_options.timerPeriod = 100;
    colorSensing_options.ledOn = 1;

    err = vhalPcntInit(&config);
    colorSensing_options.timerPeriod = PYC_ARG_INT(8);
    ZDEBUG("Timer set to %d\n", colorSensing_options.timerPeriod);

    // colorSensing_timer = vosTimerCreate();
    
    ZDEBUG("Init DONE\n");

    return VHAL_OK;    
}

C_NATIVE(_get_color) 
{
    C_NATIVE_UNWARN();
    PYC_CHECK_NUM_ARGS(1);
    PYC_CHECK_ARG_INTEGER(0);

    int segment = PYC_ARG_INT(0);
    if (segment == 3)
    {
        // ZDEBUG("Citaj sve  ");
        ColorSensing_GetColor();
    } else {
        ZDEBUG("Citaj jedan %d   ", segment);
        ColorSensing_FrequencyScaling(SCALE_100);
        colorSensing_color[0] = 0;
        colorSensing_color[1] = 0;
        colorSensing_color[2] = 0;
        colorSensing_color[segment] = ColorSensing_GetColorSegment(segment);
        ColorSensing_FrequencyScaling(POWER_DOWN);
    }

    // ZDEBUG("Color is %x, %x, %x\n", colorSensing_color[0], colorSensing_color[1], colorSensing_color[2]);

    PTuple *color = ptuple_new(4, NULL);;
    PTUPLE_SET_ITEM(color, 0, PSMALLINT_NEW(colorSensing_color[COLOR_RED]));
    PTUPLE_SET_ITEM(color, 1, PSMALLINT_NEW(colorSensing_color[COLOR_GREEN]));
    PTUPLE_SET_ITEM(color, 2, PSMALLINT_NEW(colorSensing_color[COLOR_BLUE]));
    PTUPLE_SET_ITEM(color, 3, PSMALLINT_NEW(colorSensing_color[COLOR_CLEAR]));

    *res = MAKE_RESULT(color);

    return VHAL_OK;    
}

C_NATIVE(_set_options) 
{
    C_NATIVE_UNWARN();
    PYC_CHECK_NUM_ARGS(3);
    PYC_CHECK_ARG_INTEGER(0);
    PYC_CHECK_ARG_INTEGER(1);
    PYC_CHECK_ARG_INTEGER(2);

    uint8_t  freq_scale = PYC_ARG_INT(0);
    uint32_t time_ms = PYC_ARG_INT(1);
    uint8_t  led_on = PYC_ARG_INT(2);

    // ZDEBUG("Test %d, %d\n", freq_scale, time_ms);

    ColorSensing_SetFrequencyScale(freq_scale);
    ColorSensing_SetMeasureTiming(time_ms);
    ColorSensing_SetLedLight(led_on);

    return VHAL_OK;
}

C_NATIVE(_get_options) 
{
    C_NATIVE_UNWARN();

    PTuple *options = ptuple_new(3, NULL);;
    PTUPLE_SET_ITEM(options, 0, PSMALLINT_NEW(colorSensing_options.freqScale));
    PTUPLE_SET_ITEM(options, 1, PSMALLINT_NEW(colorSensing_options.timerPeriod));
    PTUPLE_SET_ITEM(options, 2, PSMALLINT_NEW(colorSensing_options.ledOn));
    
    *res = MAKE_RESULT(options);

    return VHAL_OK;
}


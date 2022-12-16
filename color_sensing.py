@native_c("_get_color", ["color_sensing.c"], [])
def __get_color(color):
    pass

@native_c("_init_color_sensor", ["color_sensing.c"], [])
def __init(s0, s1, s2, s3, out, led, oe, ctrl, timePeriod):
    pass

@native_c("_set_options", ["color_sensing.c"], [])
def __set_options(freqScale, timerPeriod, ledOn):
    pass

@native_c("_get_options", ["color_sensing.c"], [])
def __get_options():
    pass

FREQ_SCALE_2    = 0
FREQ_SCALE_20   = 1
FREQ_SCALE_100  = 2
FREQ_POWER_DOWN = 3

class ColorSensor():
    def __init__(self, s0, s1, s2, s3, out, led, oe, controlPin, timePeriod):
        __init(s0, s1, s2, s3, out, led, oe, controlPin, timePeriod)
        
    def get_color(self):
        col = __get_color(3)
        return col
    
    def get_color_segment(self, seg):
        color = __get_color(seg)
        return color
    
    def set_options(self, freqScale, timerPeriod, ledOn):
        __set_options(freqScale, timerPeriod, ledOn)
        # if freqScale in [FREQ_SCALE_2, FREQ_SCALE_20, FREQ_SCALE_100, FREQ_POWER_DOWN]:
        #     else:
        #     raise AttributeError
        
    def get_options():
        options = __get_options()
        return options
    
    def calibrate(self, numRepetitions):
        el1 = 0
        el2 = 0
        el3 = 0
        for i in range(numRepetitions):
            a, b, c = self.get_color()
            el1 += a
            el2 += b
            el3 += c
        return (int(el1/numRepetitions), int(el2/numRepetitions), int(el3/numRepetitions))
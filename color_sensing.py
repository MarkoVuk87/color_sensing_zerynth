@native_c("_get_color", ["color_sensing.c"], [])
def __get_color(color):
    pass

@native_c("_init_color_sensor", ["color_sensing.c"], [])
def __init(s0, s1, s2, s3, out, led, oe, ctrl, timePeriod):
    pass

class ColorSensor():
    def __init__(self, s0, s1, s2, s3, out, led, oe, controlPin, timePeriod):
        __init(s0, s1, s2, s3, out, led, oe, controlPin, timePeriod)
        
    def get_color(self):
        col = __get_color(3)
        return col
    
    def get_color_segment(self, seg):
        color = __get_color(seg)
        return color
    
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
        pass
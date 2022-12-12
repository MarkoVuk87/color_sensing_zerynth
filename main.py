###############################################################################
# Hello Zerynth
###############################################################################

import gpio
import timers
import color_sensing
import serial

counter = 0

COL_CLEAR = 0
COL_RED   = 2
COL_BLUE  = 1
COL_GREEN = 3

S0 = D18
S1 = D19
S2 = D26
S3 = D25
OUT = D35
LED = D15
OE = D16

# color = (0, 0, 0)
curr_color = COL_CLEAR

def print_color(color):
    print(color)

def color_in():
    counter += 1

def prepare_clear():
    gpio.high(S2) 
    gpio.low(S3)
    
def prepare_red():
    gpio.low(S2) 
    gpio.low(S3)     

def prepare_green():
    gpio.high(S2) 
    gpio.high(S3) 

def prepare_blue():
    gpio.low(S2) 
    gpio.high(S3)  
    
def switch_col():
    print("Int")
    global counter
    global curr_color
    curr_color += 1
    
    if (curr_color == COL_BLUE):
        print("Blue", counter)
        # color[COL_BLUE] = counter
        prepare_green()
    elif (curr_color == COL_RED):
        print("Red", counter)
        # color[COL_RED] = counter
        prepare_blue()
    elif (curr_color == COL_GREEN):
        print("green", counter)
        # color[COL_GREEN] = counter
    else:
        print_color(color)
        curr_color = 0
        prepare_red()
    counter = 0

sleep(3000)

try:
    print("Init")
    colorSensor = color_sensing.ColorSensor(S0, S1, S2, S3, OUT, LED, OE, D17, 100)
except Exception as e:
    print(e)

sleep(1000)
print("Start recording color")

# x, y, z = colorSensor.calibrate(10)
x,y,z = 0,0,0    
# print("Color calibrated:", x, y, z)                

while True:
    try:
        color = colorSensor.get_color()
        print("color: RGB(L)", color[0] - x, color[1] - y, color[2] - z, color[3])
        
        # for i in range(3):
        #     color = colorSensor.get_color_segment(i)
        #     print("seg: RGB", color[0] - x, color[1] - y, color[2] - z)
        #     sleep(1000)            
        # # sleep(1000)
    except Exception as e:
        print(e)
        break
print("END")
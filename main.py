###############################################################################
# Hello Zerynth
###############################################################################

import gpio
import timers

global counter
counter = 0

COL_RED   = 1
COL_BLUE  = 2
COL_GREEN = 3

S2 = D26
S3 = D25
OUT = D35
LED = D15

color = (0, 0, 0)
curr_color = 0

def print_color(color):
    print(color)

def color_in():
    counter += 1
    
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
    global counter, curr_color
    curr_color += 1 
    if (curr_color == COL_RED):
        print("Red")
        color[COL_RED] = counter
        prepare_blue()
    elif (curr_color == COL_BLUE):
        print("Blue")
        color[COL_BLUE] = counter
        prepare_green()
    elif (curr_color == COL_GREEN):
        print("green")
        color[COL_GREEN] = counter
    else:
        print_color(color)
        curr_color = 0
        prepare_red()
    counter = 0

sleep(3000)
print("Init")

timer_col = timers.Timer()

gpio.mode(LED, OUTPUT)  # Set mode for D10
gpio.mode(S2, OUTPUT)   # Set mode for S2
gpio.mode(S3, OUTPUT)   # Set mode for S3
gpio.high(LED) 

prepare_red()
gpio.on_rise(OUT, color_in)

print("Start loop")
timer_col.interval(100, switch_col)
# loop forever
while True:
    sleep(1000)

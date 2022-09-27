###############################################################################
# Hello Zerynth
###############################################################################

# Welcome to the first and simplest Zerynth example.
# Let's just loop forever by printing something to the standard output,
# in this case, the USB serial port (open the device console to view the output)

import gpio

global counter
counter = 0

S2 = D26
S3 = D25

def color_in():
    counter += 1

gpio.mode(D15, OUTPUT)  # Set mode for D10
gpio.mode(S2, OUTPUT)  # Set mode for S2
gpio.mode(S3, OUTPUT)  # Set mode for S2
gpio.high(D15) 

gpio.on_rise(D35, color_in)

def prepare_red():
    gpio.low(S2) 
    gpio.low(S3)     

# loop forever
while True:
    
    prepare_red()
    sleep(1000)
    print("RED=", counter)
    counter = 0
    
    print
    gpio.high(S2) 
    gpio.high(S3) 
    sleep(1000)
    print("GREEN=", counter)
    counter = 0

    gpio.low(S2) 
    gpio.high(S3) 
    sleep(1000)
    print("BLUE=", counter)
    counter = 0

###############################################################################
# Hello Zerynth
###############################################################################

import gpio
import timers
import color_sensing
import serial

S0 = D18
S1 = D19
S2 = D26
S3 = D25
OUT = D35
LED = D15
OE = D16

STATUS_OK = 0


# create a serial stream linked to SERIAL0 port
ser = serial.serial()
mainStatus = STATUS_OK

def printOut(text):
    print(text, stream=ser)

def Main_SendColor():
    color = colorSensor.get_color()
    # print("color: RGB(L)", color[0] - x, color[1] - y, color[2] - z, color[3])
    return ("R" + str(color[0]) + ",G" + str(color[1]) + ",B" + str(color[2]) + ",I" + str(color[3]) + ";")

def Main_ParseCommand(command):
    commandList = command.split(',')
    printOut(commandList[0])
    if (commandList[0] == 'STATUS'):
        printOut("STATUS," +  str(mainStatus))
    elif (commandList[0] == 'GET_COLOR'):
        if (commandList[1] not in [-1, 0]):
            for i in range(int(commandList[1])):
                printOut(Main_SendColor())
        else:
            printOut('ERROR')
    elif (commandList[0] == 'SET_OPTIONS'):
        try:
            colorSensor.set_options(int(commandList[1]), int(commandList[2]), int(commandList[3]))
        except Exception as e:
            printOut(e)
    elif (commandList[0] == 'GET_OPTIONS'):
        try:
            printOut(str(colorSensor.get_options()))
        except Exception as e:
            print(e)
    else:
        printOut("Neka glupost")

try:
    printOut("Init")
    colorSensor = color_sensing.ColorSensor(S0, S1, S2, S3, OUT, LED, OE, D17, 100)
except Exception as e:
    printOut(e)

sleep(1000)
# temp = colorSensor.get_options()
# print("get options", temp)

while True:
    try:
        # print("Here")
        if (ser.available() > 0):
            read_buffer = ser.readline(end='\r', with_end=False, timeout=3000)
            Main_ParseCommand(read_buffer)
            # ser.write(read_buffer + '\n')
        # sleep(1000)
        
        # color = colorSensor.get_color()
        # print("color: RGB(L)", color[0] - x, color[1] - y, color[2] - z, color[3])
        
        # prepare_to_send = "R" + str(color[0]) + ",G" + str(color[1]) + ",B" + str(color[2]) + ",I" + str(color[3]) + ";"
        
        # print(prepare_to_send)
        
        # try:
        #     print(prepare_to_send, stream=ser)
        # except Exception as e:
        #     print(e)
        
        # if (ser.available() > 0):
        #     ser.write(prepare_to_send)
        
        # for i in range(3):
        #     color = colorSensor.get_color_segment(i)
        #     print("seg: RGB", color[0] - x, color[1] - y, color[2] - z)
        #     sleep(1000)            
        # # sleep(1000)
    except Exception as e:
        print(e)
print("END")
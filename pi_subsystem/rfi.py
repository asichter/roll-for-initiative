###########################################################
# Main Raspberry Pi code for Roll For Initiative Project
# Course: ECE 47700
# Team: 09
# Author: Alexandra E. Sichterman
# Last Modified: 04/06/2022
###########################################################

# Import Statments
import serial
import sys
sys.path.append("/home/asichter/Desktop/roll-for-initiative/pi_subsystem")
import diceRecognition
import pdb
import time

# Signal PiCamera to take a picture
def trigger_picam():
    return diceRecognition.process()

# Process Picture
def process():
    return

class RFI:
    def __init__(self):
        test_start = time.time()
        while(time.time() - test_start < 6):
            try:
                self.ser = serial.Serial(
                    port="/dev/ttyAMA0",
                    baudrate=115200,
                    stopbits=serial.STOPBITS_ONE,
                    bytesize=serial.EIGHTBITS,
                    timeout=1,
                    writeTimeout=0)
                self.ser.close()
                print("AMA0")
                break
            except serial.SerialException:
                print("Error opening serial connection ttyAMA0")
            try:
                self.ser = serial.Serial(
                    port="/dev/ttyS0",
                    baudrate=115200,
                    stopbits=serial.STOPBITS_ONE,
                    bytesize=serial.EIGHTBITS,
                    timeout=1,
                    writeTimeout=0)
                self.ser.close()
                print("s0")
                break
            except serial.SerialException:
                print("Error opening serial connection ttyS0")
        


    # Export Number Array
    def export(self, dice_array):
        ser = self.ser
        #ser.open()
        print('exporting\n')

        if(ser.isOpen() == False):                                                                                                                         
            ser.open() 
        for num in dice_array:
            #pdb.set_trace()
            #print('here1')
            if ser.isOpen():
                #print('here2')
                try:
                    #ser.write(serial.to_bytes(num))
                    #ser.write(num.encode('utf-8'))
                    # ser.write("H".encode('utf-8'))
                    # output = "H"
                    ascii_out = chr(int(num+"\n"))
                    print(ord(ascii_out))
                    # test_str = "this is a test!"
                    ser.write(ascii_out.encode()) 
                    time.sleep(0.05)
                except serial.SerialTimeoutException:
                    print("TIMEOUT: write took too long")
        if ser.isOpen():
            ser.close()
        return

def main():
    # SETUP PORT
    #pdb.set_trace()
    rfi = RFI()

    if(rfi.ser.isOpen() == False):                                                                                                                         
        rfi.ser.open() 
    rfi.export(["65"])  
    while True:
        if(rfi.ser.isOpen() == False):                                                                                                                         
            rfi.ser.open()                                                    
        # Look for USART signal from microcontroller                                                                              
        recv_data = rfi.ser.readline()
        print(recv_data)
        if recv_data: 
            print("recv")                                                                                                                                                                                                                                                                                                                                                                                                                                  
            dice_array = diceRecognition.process()
            print(dice_array)
            dice_array = [str(die) for die in dice_array]
            rfi.export(dice_array)

        # rfi.ser.close()
        # break

if __name__ == "__main__":
    main()

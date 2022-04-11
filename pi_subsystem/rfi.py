###########################################################
# Main Raspberry Pi code for Roll For Initiative Project
# Course: ECE 47700
# Team: 09
# Author: Alexandra E. Sichterman
# Last Modified: 04/06/2022
###########################################################

# Import Statments
import serial
import diceRecognition

# Signal PiCamera to take a picture
def trigger_picam():
    return diceRecognition.process()

# Process Picture
def process():
    return

class RFI:
    def __init__(self):
        try:
            self.ser = serial.Serial(
                port="dev/ttys0",
                baudrate=115200,
                parity=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=1)
            self.ser.close()
        except serial.SerialException:
            print("Error opening serial connection")
        


    # Export Number Array
    def export(self, dice_array):
        ser = self.ser
        ser.open()

        for num in dice_array:
            if ser.is_open():
                try:
                    ser.write(serial.to_bytes(num))
                except serial.SerialTimeoutException:
                    print("TIMEOUT: write took too long")

        ser.close()
        return

def main():
    # SETUP PORT
    rfi = RFI()
    

    recv_data = False
    while True:
        rfi.ser.open()
        # Look for USART signal from microcontroller
        recv_data = rfi.ser.readline()
        if recv_data:
            rfi.ser.close()
            dice_pictures = trigger_picam()
            dice_array = process(dice_pictures)
            break
        # Signal PiCamera to take a picture
        # Process picture
        # Export number array
        rfi.export(["1"])
        break

if __name__ == "__main__":
    main()
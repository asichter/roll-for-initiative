import cv2 as cv
import diceRecognition
import os

for image in os.listdir("/home/pi/Desktop/crop/"):
    img = cv.imread("/home/pi/Desktop/crop/" + image)
    img = diceRecognition.resize(img,size=(64,64))
    diceRecognition.show_roll(img)
    #img = diceRecognition.rotate(img)
    diceRecognition.show_roll(img)
    img = diceRecognition.convert_to_RGB(img)
    #img = diceRecognition.binary_threshold(img)
    cv.imwrite("/home/pi/Desktop/test-resize/resized.jpg",img)
    os.system('python3 OCR.py')
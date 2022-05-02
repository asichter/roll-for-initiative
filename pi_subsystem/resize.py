from diceRec import *
#import os
#import cv2 as cv

for img in os.listdir("training/"):
    image = cv.imread("training/" + img)
    image = resize(image)
    cv.imwrite("training2/" + img,image)

for img in os.listdir("testing/"):
    image = cv.imread("testing/" + img)
    image = resize(image)
    cv.imwrite("testing2/" + img,image)

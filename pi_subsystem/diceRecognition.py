import os
from platform import java_ver
from picamera import PiCamera
from time import sleep
import cv2 as cv
import numpy as np
import argparse
import imutils


# for debugging
def show_roll(roll):
    cv.namedWindow('ROLL',cv.WINDOW_NORMAL)
    cv.imshow('ROLL',roll)
    cv.waitKey(0)

def get_roll():
    roll_img = cv.imread("/home/pi/Desktop/temp/temp.jpg")
    return roll_img

def get_contours(roll):
    _,contours,_ = cv.findContours(roll,mode=cv.RETR_EXTERNAL,method=cv.CHAIN_APPROX_NONE)         
    return contours

# for debugging
def draw_contours(roll,contours):
    contourRoll = roll.copy()
    cv.drawContours(image=contourRoll,contours=contours,contourIdx=-1,color=(0,255,0),thickness=5)
    return contourRoll

def take_picture():
    camera = PiCamera()
    #camera.start_preview()
    camera.capture("/home/pi/Desktop/temp/temp.jpg")
    #camera.stop_preview()
    camera.close()

def convert_grayscale(roll):
    grayscale_roll = cv.cvtColor(roll,cv.COLOR_BGR2GRAY)
    return grayscale_roll

def binary_threshold(roll):
    ret,thresh = cv.threshold(roll,127,255,0)
    return thresh

def convert_to_RGB(roll):
    converted_roll = cv.cvtColor(roll,cv.COLOR_BGR2RGB)
    return converted_roll

def resize(img,size=(64,64)):
    h, w = img.shape[:2]
    c = img.shape[2] if len(img.shape)>2 else 1

    if h == w: 
        return cv.resize(img, size, cv.INTER_AREA)

    if h > w:
        dif = h
    else:
        dif = w
    #dif = h if h > w else w

    #interpolation = cv.INTER_AREA if dif > (size[0]+size[1])//2 else 
    #                cv.INTER_CUBIC

    if dif > (size[0] + size[1]) // 2:
        interpolation = cv.INTER_AREA
    else:
        interpolation = cv.INTER_CUBIC

    x_pos = (dif - w)//2
    y_pos = (dif - h)//2

    if len(img.shape) == 2:
        mask = np.zeros((dif, dif), dtype=img.dtype)
        mask[y_pos:y_pos+h, x_pos:x_pos+w] = img[:h, :w]
    else:
        mask = np.zeros((dif, dif, c), dtype=img.dtype)
        mask[y_pos:y_pos+h, x_pos:x_pos+w, :] = img[:h, :w, :]

    return cv.resize(mask, size, interpolation)

def rotate(image):

    pass

def process():
    take_picture()
    roll = get_roll()
    process_roll = convert_grayscale(roll)
    thresh = binary_threshold(process_roll)
    contours = get_contours(thresh)
    roll_countours = draw_contours(roll, contours)
    
    dice_pictures = []
    for c in contours:
        x,y,w,h = cv.boundingRect(cnt)
        crop = roll[y:y+h,x:x+w]
        cropped = convert_to_RGB(crop)
        dice_pictures.append(cropped)

    return dice_pictures


if __name__ == "__main__":
    #sleep(1)
    #parser = argparse.ArgumentParser()
    #parser.add_argument('PicName')
    #args = parser.parse_args()
    take_picture()
    roll = get_roll()
    process_roll = convert_grayscale(roll)
    thresh = binary_threshold(process_roll)
    contours = get_contours(thresh)
    i = 0
    contourRoll = draw_contours(roll,contours)
    show_roll(contourRoll)
    for cnt in contours:
        x,y,w,h = cv.boundingRect(cnt)
        crop = roll[y:y+h,x:x+w]
        cropped = convert_to_RGB(crop)
        cropped = resize(cropped,size=(64,64))
        cropped = binary_threshold(cropped)
        highest =  [cropped,0]
        show_roll(cropped)
        for i in range(0,9):
            cropped = imutils.rotate(cropped,45)
            show_roll(cropped)
            cv.imwrite("/home/pi/Desktop/crop/cropped.jpg".format(i),cropped)
            os.system('python3 OCR.py')
        #cv.imwrite("/home/pi/Desktop/crop/cropped.jpg".format(i),cropped)
        #os.system('python3 OCR.py')
        #cv.imwrite("/home/pi/Desktop/knn-training2/{name}.jpg".format(name=args.PicName),crop)
        #cv.imwrite("/home/pi/Desktop/{name}.jpg".format(name=args.PicName),crop)





    
import os
from platform import java_ver
from time import sleep
import cv2 as cv
import numpy as np
import argparse
from img2vec_pytorch import Img2Vec
from PIL import Image
import pickle


# for debugging
def show_roll(roll):
    cv.namedWindow('ROLL',cv.WINDOW_NORMAL)
    cv.imshow('ROLL',roll)
    cv.waitKey(0)

def get_roll():
    roll_img = cv.imread("/home/asichter/Desktop/temp/temp.jpg")
    return roll_img

def get_contours(roll):
    contours,_ = cv.findContours(roll,mode=cv.RETR_EXTERNAL,method=cv.CHAIN_APPROX_NONE)         
    return contours

# for debugging
def draw_contours(roll,contours):
    contourRoll = roll.copy()
    cv.drawContours(image=contourRoll,contours=contours,contourIdx=-1,color=(0,255,0),thickness=5)
    return contourRoll

def take_picture():
    os.system("libcamera-jpeg -o /home/asichter/Desktop/temp/temp.jpg --nopreview --shutter 10000 > /dev/null 2>&1")

def convert_grayscale(roll):
    grayscale_roll = cv.cvtColor(roll,cv.COLOR_RGB2GRAY)
    return grayscale_roll

def binary_threshold(roll):
    ret,thresh = cv.threshold(roll,127,255,cv.THRESH_BINARY)
    return thresh

def convert_to_RGB(roll):
    converted_roll = cv.cvtColor(roll,cv.COLOR_BGR2RGB)
    return converted_roll

def resize(img,size=(100,100)):
    h, w = img.shape[:2]
    c = img.shape[2] if len(img.shape)>2 else 1

    if h == w: 
        return cv.resize(img, size, cv.INTER_AREA)

    if h > w:
        dif = h
    else:
        dif = w

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

def process():
    os.system("rm /home/asichter/Desktop/crop/*.*")
    loaded_model = pickle.load(open('diceRec3','rb'))
    roll = take_picture()
    roll = get_roll()
    #show_roll(roll)
    process_roll = convert_grayscale(roll)
    thresh = binary_threshold(process_roll)
    contours = get_contours(thresh)
    contourRoll = draw_contours(roll,contours)
    save_image(contourRoll,"contours")
    i = 0
    values = []
    for cnt in contours:
        x,y,w,h = cv.boundingRect(cnt)
        crop = roll[y:y+h,x:x+w]
        cropped = convert_to_RGB(crop)
        if cropped.shape[0] < 100 and cropped.shape[1] < 100:
            continue
        cropped = resize(cropped)
        cropped  = binary_threshold(cropped)
        save_image(cropped,i) 
        image = Image.open("/home/asichter/Desktop/crop/cropped" + str(i) + ".jpg")
        test = convert_vec(image)
        values.append(loaded_model.predict(test)[0])
        print("crop" + str(i) + ".jpg : " + str(loaded_model.predict(test)))
        i += 1
    return values

def convert_vec(image):
    test = []
    img2vec = Img2Vec(cuda=False)
    vec = img2vec.get_vec(image, tensor=True).detach().numpy()
    vec = vec.reshape(512)
    test.append(vec)
    test = np.array(test)
    return test

def save_image(img,i):
    cv.imwrite("/home/asichter/Desktop/crop/cropped" + str(i) + ".jpg",img)




if __name__ == "__main__":
    os.system("rm /home/asichter/Desktop/crop/*.*")
    loaded_model = pickle.load(open('diceRec3','rb'))
    take_picture()
    roll = get_roll()
    save_image(roll,"raw")
    process_roll = convert_grayscale(roll)
    thresh = binary_threshold(process_roll)
    contours = get_contours(thresh)
    i = 0
    contourRoll = draw_contours(roll,contours)
    save_image(contourRoll,"contours")
    values = []
    for cnt in contours:
        x,y,w,h = cv.boundingRect(cnt)
        crop = roll[y:y+h,x:x+w]
        cropped = convert_to_RGB(crop)
        if cropped.shape[0] < 100 and cropped.shape[1] < 100:
            continue
        cropped = resize(cropped)
        cropped  = binary_threshold(cropped)
        save_image(cropped,i) 
        image = Image.open("/home/asichter/Desktop/crop/cropped" + str(i) + ".jpg")
        test = convert_vec(image)
        #values.append(loaded_model.predict(test))
        print("crop" + str(i) + ".jpg : " + str(loaded_model.predict(test)))
        i += 1
    #print(values)




    

    

import os
import diceRecognition
import cv2 as cv



if __name__ == "__main__":
    #TODO:
    # Convert to grayscale
    # Resize image
    # Change dimensionality
    # 
    dir = "/home/pi/Desktop/knn-training/"
    for img in os.listdir(dir):
        print(img)
        img = cv.imread(dir + img)

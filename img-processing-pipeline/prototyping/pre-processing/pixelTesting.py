# Messing around with da pixels in an image
# because I thought I was cool and thought
# I could do normalization by myself
# Also takes a few seconds to run 
import cv2 as cv

test_img = cv.imread("../Dice Testing Pictures\ideal.JPG")

rows,cols,_ = test_img.shape
print((rows))
print((cols))
b = 0
g = 0
r = 0
for i in range(rows):
    for j in range(cols):
        k = test_img[i,j]
        if k[0] > b:
            b = k[0]
        if k[1] > g:
            g = k[0]
        if k[2] > r:
            r = k[0]
print(b,g,r)
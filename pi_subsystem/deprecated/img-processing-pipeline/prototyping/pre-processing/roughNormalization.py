# Very very rudimentary and broken
# preprocessing pipeline.
# Normalizes, grayscales, and then
# (horribly) applies Canny edge 
# detection
import cv2 as cv
import numpy as np
import os
  

for img in os.listdir("../Dice Testing Pictures/"):
    print(img)
    test_img = cv.imread("../Dice Testing Pictures/" + img)
    result_img = np.zeros((3024,4032)) # output img
    output_img = cv.normalize(test_img, result_img,100,190,cv.NORM_MINMAX)
    cv.namedWindow(img,cv.WINDOW_NORMAL) # windows to display images
    cv.imshow(img,test_img) # input image
    cv.waitKey(0)
    cv.namedWindow("normalize" + img,cv.WINDOW_NORMAL)
    cv.imshow("normalize" + img,output_img) # normalized image
    cv.waitKey(0)
    cv.cvtColor(output_img, cv.COLOR_RGB2GRAY)
    cv.namedWindow("grayscale" + img,cv.WINDOW_NORMAL) 
    cv.imshow("grayscale" + img,output_img) # grayscale image
    cv.waitKey(0)
    edges = cv.Canny(output_img,0,100) # apply the Canny algorithm - this horribly disfigures the image - read below
    cv.namedWindow("canny"+img,cv.WINDOW_NORMAL)
    cv.imshow("canny" + img,edges)  # display Canny (cover your eyes)
    cv.waitKey(0)
    contours = cv.findContours(edges,cv.RETR_EXTERNAL,cv.CHAIN_APPROX_NONE)
    cv.drawContours(test_img,contours[0],-1,(0,0,255),thickness = 2)
    cv.namedWindow("drawContours" + img,cv.WINDOW_NORMAL)
    cv.imshow("drawContours" + img,test_img)
    cv.waitKey(0)

# Canny disfigures the image because we don't know the minVal and maxVal for hysterisis
# threshold, which will be largely dependent on our mat/tray and full protyotype, so
# we will have tweak it then. Also I maybe (definitely) don't full understand
# how to use Canny yet.



'''
ideal_img = cv.imread("Dice Testing Pictures\ideal.JPG")
input_img = cv.imread("Dice Testing Pictures\glare.JPG")


# make window for display
#cv.namedWindow("ideal", cv.WINDOW_NORMAL)
#cv.namedWindow("input",cv.WINDOW_NORMAL)

#cv.imshow("input", input_img)
#cv.imshow("ideal",ideal_img)
cv.waitKey(0)

result_img = np.zeros((3024,4032))
output_img = cv.normalize(input_img, result_img,0,155,cv.NORM_MINMAX)


# display image
print("output image")
cv.namedWindow("output", cv.WINDOW_NORMAL)
cv.namedWindow("input", cv.WINDOW_NORMAL)
cv.imshow("output", output_img)
cv.imshow("input",input_img)
cv.waitKey(0)'''


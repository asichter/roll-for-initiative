# THIS DOESN'T WORK LOL, just a reference for how our pre-processing (without OCR) could look like
import cv2

def calculate_value(roi):
    params = cv2.SimpleBlobDetector_Params()
    params.filterByInertia = True
    params.minInertiaRatio = 0.5

    detector = cv2.SimpleBlobDetector_create(params)
    key_points = detector.detect(roi)
    return len(key_points)


im = cv2.imread("dice2.jpg")
cv2.namedWindow("main", cv2.WINDOW_AUTOSIZE)
cv2.imshow('main', im)
cv2.waitKey(0)

im_clone = im.copy()
im_clone1 = im.copy()
im_clone = cv2.cvtColor(im_clone, cv2.COLOR_RGB2GRAY)
im_clone = cv2.GaussianBlur(im_clone, (5, 5), 0)
ret, im_thresh = cv2.threshold(im_clone, 190, 255, cv2.THRESH_BINARY)
cv2.imshow('main', im_thresh)
cv2.waitKey(0)

im_canny = cv2.Canny(im_thresh, 2, 2*2, 3)
cv2.imshow('main', im_canny)
cv2.waitKey(0)


im_contours, contours = cv2.findContours(im_canny, cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)

for i in range(len(contours)):
    contours_area = cv2.contourArea(contours[i])
    x, y, w, h = cv2.boundingRect(contours[i])
    im = cv2.rectangle(im, (x, y), (x + w, y + h), (255, 0, ), 5)
    cv2.imshow('main',im)
    cv2.waitKey(0)
    dice_roi = im_clone1[y: (y + h), x: (x + w)]
    number = calculate_value(dice_roi)
    im = cv2.putText(im,"Value: " + str(number),(x,y+h),cv2.FONT_HERSHEY_COMPLEX_SMALL,5,(0,255,0),5)

cv2.drawContours(im, contours, -1, (0, 0, 255), 5)

cv2.imshow('main', im)
cv2.waitKey(0)
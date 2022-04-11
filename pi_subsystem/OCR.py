import os
import pytesseract
from PIL import Image
#import imutils 

custom_config = '--oem 3 --psm 6 outputbase digits'
custom_config1 = '--psm 13 --oem 3 -c tessedit_char_whitelist=0123456789'
custom_config2 = '--psm '
custom_config3 = '-l eng --oem 3 --psm 12'
custom_config4 = '--oem 1 --psm 9'
custom_config5 = '--psm 10 tessedit_char_whitelist=0123456789'




if __name__ == "__main__":
    ocr_img = Image.open("/home/pi/Desktop/crop/cropped.jpg")
    text = pytesseract.image_to_data(ocr_img,config = custom_config5)
    print(text)
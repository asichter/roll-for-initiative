from diceRec import *

for img in os.listdir("trainingv3/"):
    print(img)
    image = cv2.imread("trainingv3/" + img)
    image = resize(image)
    image = binary_threshold(image)
    cv2.imwrite("training5/" + img,image)

for img in os.listdir("testingv3/"):
    image = cv2.imread("testingv3/" + img)
    image = resize(image)
    image = binary_threshold(image)
    cv2.imwrite("testing5/" + img,image)
from img2vec_pytorch import Img2Vec
from PIL import Image
import os
import cv2
import pickle
from sklearn import neighbors
import numpy as np
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
import argparse

'''def grayscale(img):
    return cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)'''

'''def resize(img,size=(100,100)):
    h, w = img.shape[:2]
    c = img.shape[2] if len(img.shape)>2 else 1

    if h == w: 
        return cv2.resize(img, size, cv2.INTER_AREA)

    #dif = h if h > w else w
    if h > w:
        dif = h
    else:
        dif = w

    if dif > (size[0]+size[1])//2:
        interpolation = cv2.INTER_AREA
    else:
        interpolation = cv2.INTER_CUBIC
    #interpolation = cv2.INTER_AREA if dif > (size[0]+size[1])//2 else 
    #                cv2.INTER_CUBIC

    x_pos = (dif - w)//2
    y_pos = (dif - h)//2

    if len(img.shape) == 2:
        mask = np.zeros((dif, dif), dtype=img.dtype)
        mask[y_pos:y_pos+h, x_pos:x_pos+w] = img[:h, :w]
    else:
        mask = np.zeros((dif, dif, c), dtype=img.dtype)
        mask[y_pos:y_pos+h, x_pos:x_pos+w, :] = img[:h, :w, :]

    return cv2.resize(mask, size, interpolation)'''


def label(img):
    return img.split("_")[0]





if __name__ == "__main__":
    trainingData = []
    trainingLabel = []
    for i,img in enumerate(os.listdir("training/")):
        #print(img)
        #print(i)
        image = Image.open("training/" + img)
        img2vec = Img2Vec(cuda=False)
        vec = img2vec.get_vec(image, tensor=True).detach().numpy()
        vec = vec.reshape(512)
        #print(vec.shape)
        trainingData.append(vec)
        imgLabel = label(img)
        trainingLabel.append(imgLabel)
    testingData = []
    testingLabel = []
    for img in os.listdir("testing/"):
        #print(img)
        image = Image.open("testing/" + img)
        img2vec = Img2Vec(cuda=False)
        vec = img2vec.get_vec(image, tensor=True).detach().numpy()
        vec = vec.reshape(512)
        testingData.append(vec)
        testingLabel.append(label(img))
    X_train = np.array(trainingData)
    Y_train = np.array(trainingLabel)
    print("X-train after reshape: " + str(X_train.shape))
    knn = neighbors.KNeighborsClassifier(n_neighbors=1)
    knn.fit(X_train,Y_train)
    recog = open('diceRec','wb')
    pickle.dump(knn,recog)

    X_test = np.array(testingData)
    Y_test = np.array(testingLabel)
    print("X-test :" + str(X_test.shape))
    result = knn.predict(X_test)
    print(result)
    score = knn.score(X_test,Y_test)
    print("Score: " + str(score*100))

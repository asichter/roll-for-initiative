import diceRec
import os
from img2vec_pytorch import Img2Vec
from PIL import Image
import pickle
import numpy as np

testingData = []
testingLabel = []
loaded_model = pickle.load(open('diceRec','rb'))
#image = Image.open("testing/1_1.jpg")
#image.reshape(512)
#print(loaded_model.predict(image))
i = 0
for img in os.listdir("testing/"):
    print(img)
    test = []
    image = Image.open("testing/" + img)
    img2vec = Img2Vec(cuda=False)
    vec = img2vec.get_vec(image, tensor=True).detach().numpy()
    vec = vec.reshape(512)
    test.append(vec)
    test = np.array(test)
    print(loaded_model.predict(test))
    i += 1
    if i == 1:
        break
    #image = grayscale(image)
    #image = resize(image)
    #image = PCA(3)
    #image.flatten()
    #image.reshape(-1,image.size)
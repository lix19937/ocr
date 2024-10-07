import numpy as np
import tensorflow as tf
import cv2
from scipy.misc import imread, imresize
from PIL import Image, ImageEnhance, ImageOps, ImageFile



def resize_image(image):
    """
        Resize an image to the "good" input size
        固定高度为32像素，宽度为160,图像为RGB模式，所有图像的像素值都归一化到0-1之间.
    """
    img=cv2.imdecode(np.fromfile(image, dtype=np.uint8), 1)

    img=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    cp1 = np.random.uniform(0.01, 0.08)
    cp2 = np.random.uniform(0.2, 0.17)
    cp3 = np.random.uniform(0.93, 0.99)
    cp4 = np.random.uniform(0.9, 0.99)
    x0 = int(img.shape[1] * cp1)
    y0 = int(img.shape[0] * cp2)
    x1 = int(img.shape[1] * cp3)
    y1 = int(img.shape[0] * cp4)
    halfnm = np.random.randint(0, 2)
    if (halfnm == 1):
        ##random crop image
        if (img.shape[0] != 32):
            img = img[y0:y1, x0:x1]
        random_factor = np.random.randint(1, 4)
        img=cv2.blur(img,(random_factor,random_factor))
    img = cv2.resize(img, (160, 32))
    img=np.asarray(img,dtype=np.float32)
    img=img/255.0
    return img, 37


def sparse_tuple_from(sequences, dtype=np.int32):
    """
        Inspired (copied) from https://github.com/igormq/ctc_tensorflow_example/blob/master/utils.py
    """
    indices = []
    values = []
    for n, seq in enumerate(sequences):
        # print('n=',n)
        # print('seq=',seq)
        indices.extend(zip([n]*len(seq), [i for i in range(len(seq))]))
        values.extend(seq)
    indices = np.asarray(indices, dtype=np.int64)
    values = np.asarray(values, dtype=dtype)
    print('lensequences=',len(sequences))
    shape = np.asarray([len(sequences), np.asarray(indices).max(0)[1]+1], dtype=np.int64)
    return indices, values, shape

def label_to_array(label, dict):
    index=[]
    number = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
    for x in label.strip():
        if dict.get(x)!=None:
            index.append(int(dict[x]))
            continue
        if x in number:
            index.append(int(dict[int(x)]))
    return index

print('utils testolkkkkk!!')
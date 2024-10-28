#!/usr/bin/env python

import cv2 as cv
import numpy as np
import os 
import itertools as it
from loguru import logger 

train_data_path = "svm_data/train/"
test_data_path  = "svm_data/test/"

SZ = 20
bin_n = 16 # Number of bins
num_classes = 10
h_cnt = 100 # png include  num chars in  horizontal direction
v_cnt = 50  # png include  num chars in  vertical direction  
h_start = 50
USE_FILES_PATH = False

# https://blog.csdn.net/sinat_21720047/article/details/95997085
# https://github.com/spmallick/learnopencv/tree/master/digits-classification
# https://learnopencv.com/support-vector-machines-svm/
# https://docs.opencv.org/4.9.0/dd/d3b/tutorial_py_svm_opencv.html 
# https://learnopencv.com/histogram-of-oriented-gradients/

affine_flags = cv.WARP_INVERSE_MAP|cv.INTER_LINEAR

def deskew(img):
    m = cv.moments(img)
    if abs(m['mu02']) < 1e-2:
        return img.copy()
    
    skew = m['mu11']/m['mu02']
    M = np.float32([[1, skew, -0.5*SZ*skew], [0, 1, 0]])
    img = cv.warpAffine(img, M, (SZ, SZ), flags=affine_flags)
    return img


def hogv1(img):
    gx = cv.Sobel(img, cv.CV_32F, 1, 0)
    gy = cv.Sobel(img, cv.CV_32F, 0, 1)
    mag, ang = cv.cartToPolar(gx, gy)
    bins = np.int32(bin_n*ang/(2*np.pi))    # quantizing binvalues in (0...16)
    bin_cells = bins[:10,:10], bins[10:,:10], bins[:10,10:], bins[10:,10:]
    mag_cells = mag[:10,:10], mag[10:,:10], mag[:10,10:], mag[10:,10:]
    hists = [np.bincount(b.ravel(), m.ravel(), bin_n) for b, m in zip(bin_cells, mag_cells)]
    hist = np.hstack(hists)     # hist is a 64 bit vector
    return hist  # numpy 


def hogv2(img) : 
    winSize = (20,20)
    blockSize = (8,8)
    blockStride = (4,4)
    cellSize = (8,8)
    nbins = 9
    derivAperture = 1
    winSigma = -1.
    histogramNormType = 0
    L2HysThreshold = 0.2
    gammaCorrection = 1
    nlevels = 64
    signedGradient = True
    hog = cv.HOGDescriptor(winSize, blockSize, blockStride, cellSize, nbins, derivAperture, winSigma, histogramNormType, L2HysThreshold, gammaCorrection, nlevels, signedGradient)
    desc = hog.compute(img)
    return desc  # numpy 

myhog=hogv2

def grouper(n, iterable, fillvalue=None):
    '''grouper(3, 'ABCDEFG', 'x') --> ABC DEF Gxx'''
    args = [iter(iterable)] * n
    output = it.zip_longest(fillvalue=fillvalue, *args)
    return output

def mosaic(w, imgs):
    '''Make a grid from images.

    w    -- number of grid columns
    imgs -- images (must have same size and format)
    '''
    imgs = iter(imgs)
    img0 = next(imgs)
    pad = np.zeros_like(img0)
    imgs = it.chain([img0], imgs)
    rows = grouper(w, imgs, pad)
    return np.vstack(map(np.hstack, rows))

def read_classify_dataset(file_dir: str):
    samples = []
    labels = []
    dimgs = []
    postfix = set(['png','jpg'])
    for root, __, files in os.walk(file_dir):
        if root == "./": 
            continue
        for filename in files:
            abspath = os.path.join(root, filename)
            if abspath.split('.')[-1] in postfix:  
                try:                    
                    img = cv.imread(abspath, 0)
                    if img is None:
                        raise Exception("imread {} failed".format(abspath))
                    dimg = deskew(img)
                    himg = myhog(dimg)
                    samples.append(himg)
                    label = root.split('/')[-1]
                    labels.append(np.int64(label))
                    dimgs.append(dimg)
                except:
                    pass  

    return dimgs, samples, labels

def evaluate(testdata, predictions, labels):
    accuracy = (labels == predictions).mean()
    logger.info('Percentage Accuracy: %.2f %%' % (accuracy*100))

    confusion = np.zeros((num_classes, num_classes), np.int32)
    for i, j in zip(labels, predictions):
        confusion[int(i), int(j)] += 1
    logger.info("confusion matrix:\n{}".format(confusion))

    vis = []
    for img, flag in zip(testdata, predictions == labels):
        img = cv.cvtColor(img, cv.COLOR_GRAY2BGR)
        if not flag:
            img[...,:2] = 0
        
        vis.append(img)
    return mosaic(25, vis)


logger.info("load train data ...")

###### read from file path 
if USE_FILES_PATH:
    __, hogdata, labels = read_classify_dataset(train_data_path)
    trainData = np.float32(hogdata)  
    trainData = trainData.reshape(-1, trainData.shape[-1])
    responses = np.array(labels)[:, None]            # (2500, 1)
else:
    img = cv.imread(cv.samples.findFile('digits.png'), 0)
    if img is None:
        raise Exception("we need the digits.png image from samples/data here !")

    cells = [np.hsplit(row, h_cnt) for row in np.vsplit(img, v_cnt)]

    train_cells = [ i[:h_start] for i in cells ]
    test_cells  = [ i[h_start:] for i in cells ]
    deskewed  = [list(map(deskew, row)) for row in train_cells]
    hogdata   = [list(map(myhog, row)) for row in deskewed]
    trainData = np.float32(hogdata)  
    trainData = trainData.reshape(-1, trainData.shape[-1])
    responses = np.repeat(np.arange(num_classes), 5*h_start)[:, None]

######     Now training      ########################
logger.info("training ...")

svm = cv.ml.SVM_create()
# svm.setKernel(cv.ml.SVM_RBF)
svm.setKernel(cv.ml.SVM_LINEAR)  # 98.04%
svm.setType(cv.ml.SVM_C_SVC)
svm.setC(0.1)
svm.setGamma(5.383)

svm.trainAuto(trainData, cv.ml.ROW_SAMPLE, responses)
svm.save('svm_data.dat')
logger.info("training done")

######     Now testing      ########################
logger.info("load test data ...")

###### read from file path 
if USE_FILES_PATH:
    deskewed, hogdata, labels = read_classify_dataset(test_data_path)
    testData = np.float32(hogdata)  
    testData = testData.reshape(-1, testData.shape[-1])
    responses = np.array(labels)[:, None]                # (2500, 1)
else:
    deskewed = [list(map(deskew, row)) for row in test_cells]
    hogdata  = [list(map(myhog, row)) for row in deskewed]
    testData = np.float32(hogdata)  
    testData = testData.reshape(-1, testData.shape[-1])
    responses = np.repeat(np.arange(num_classes), 5*(h_cnt -h_start))[:, None]


###############################
predictions = svm.predict(testData)[1]

#######   Check Accuracy   ########################
# mask = (predictions==responses)
# correct = np.count_nonzero(mask)
# print('Percentage Accuracy: %.2f %%' % (correct*100.0/ predictions.size))

deskewed = np.array(deskewed).reshape(predictions.shape[0], SZ, SZ) 

vis = evaluate(deskewed, predictions, responses)
cv.imwrite("digits-classification.png", vis)
cv.imshow("Vis", vis)
cv.waitKey(0)
cv.destroyAllWindows()


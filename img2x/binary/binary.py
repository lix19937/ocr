# https://www.geeksforgeeks.org/python-thresholding-techniques-using-opencv-set-1-simple-thresholding/?ref=lbp    

import cv2  
import numpy as np  
  
# path to input image is specified and   
# image is loaded with imread command  
image1 = cv2.imread('input1.jpg')  
  
# cv2.cvtColor is applied over the 
# image input with applied parameters 
# to convert the image in grayscale  
img = cv2.cvtColor(image1, cv2.COLOR_BGR2GRAY) 
  
ret, thresh1 = cv2.threshold(img, 120, 255, cv2.THRESH_BINARY) 
  
# the window showing output images 
# with the corresponding thresholding  
# techniques applied to the input images 
cv2.imshow('Binary Threshold', thresh1) 
    
# De-allocate any associated memory usage   
if cv2.waitKey(0) & 0xff == 27:  
    cv2.destroyAllWindows()  

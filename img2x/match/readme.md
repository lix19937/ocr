
## matchTemplate   

###  6种匹配方法的选择         
```
methods = ['cv.TM_CCOEFF', 'cv.TM_CCOEFF_NORMED', 'cv.TM_CCORR',
           'cv.TM_CCORR_NORMED', 'cv.TM_SQDIFF', 'cv.TM_SQDIFF_NORMED']


### 多尺度的模板匹配算法    
matchTemplate()只能检测和模板图像大小一样的目标，对于不同大小的目标，或者稍有差异的目标检测不行。
针对这样的情况，通过对模板图像和待检测图像提取轮廓图像(减少干扰)，改变待检测图像的大小来实现多尺度的模板匹配算法





```

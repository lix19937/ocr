
## 基于倾斜角度 旋转    

```py
cv2.getRotationMatrix2D(center, angle, scale)
```
其中，center 指旋转的中心坐标；angle 代表旋转角度，比如，45代表逆时针旋转45°；scale 代表缩放程度，1.0表示不缩放。  

```py
cv2.warpAffine(src, M, dsize)
```
其中，src是待处理的图片；M是仿射矩阵，这里用getRotationMatrix2D的返回值；dsize 表示输出图片的大小。    


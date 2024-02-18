
## 说明   

|项目| 详细 |  备注|    
|--- | ----| ----|     
|全图尺寸 宽 x 高 | 1080 x 1920|-|     
|`编号:` | 作为模板匹配基准 opencv `matchTemplate` | y_offset <=360 内搜索 |     
|文本定位 | 直接使用文本块区域检测模型进行文本块定位| ----|      
|文本筛选 | 选取`编号`，手写区域符号| ----|       

注意：matchTemplate api 对缩放敏感，要求模板图像和待搜索图像的目标图像尽可能保持相同的宽高   ！！！    

|样式1| 样式2|    
|--- | ----|    
|![sample.png](sample.png)|![sample1.png](sample1.png)|    

```
pip --version   

pip install opencv-python 
```

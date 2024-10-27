## adaboost detect  

 
http://docs.opencv.org/2.4/doc/user_guide/ug_traincascade.html     
opencv_traincascade 程序使用TBB来处理多线程。如果希望使用多核并行运算加速，请使用TBB来编译OpenCV。

正样本通常只要是80%-90%就好，负样本一般是正样本2-3倍       
vec文件中包含的数目应该大于numPos，通常numPos选取正样本的80%至90%来训练   


```
with -npos 7000 -nneg 2973
vec-file has to contain >= (numPos + (numStages-1) * (1 - minHitRate) * numPos) + S
7000 >= (numPos + (20-1) * (1 - 0.999) * numPos) + 2973
(7000 - 2973)/(1 + 19*0.001) >= numPos
numPos <= 4027/1.019
numPos <= 3951 ~~ 3950
and used:
-npos 3950 -nneg 2973
It works.
```

http://blog.csdn.net/wuxiaoyao12/article/details/39227189
http://jingyan.baidu.com/article/4dc40848f50689c8d946f197.html

```  
1,dos下  进入pos目录下   dir  /b  > pos.txt   删除末行  替换名字 jpg   jpg  1 0 0 w  h      
2,dos下  进入neg目录下   dir  /b  > neg.txt   删除末行    
3,生成pos.vec  正样本描叙文件     
opencv_createsamples.exe -vec pos.vec -info pos\pos.txt -bg neg\neg.txt -w 40 -h 40 -num 1327

4a,用opencv_haartraining 训练   默认模式是GAB  haar特征    
opencv_haartraining.exe -data classifier -vec pos.vec -bg neg\neg.txt -w 40 -h 40 -mem 2048 -mode ALL -minhitrate 0.9999 -npos 1127  -nneg 6000  -nstages 18  -nsplits 2   -bt RAB

opencv_traincascade 支持 Haar[Viola2001] 和 LBP[Liao2007] (Local Binary Patterns) 两种特征，并易于增加其他的特征。与Haar特征相比，LBP特征是整数特征，因此训练和检测过程都会比Haar特征快几倍。LBP和Haar特征用于检测的准确率，是依赖训练过程中的训练数据的质量和训练参数。训练一个与基于Haar特征同样准确度的LBP的分类器是可能的。“
用opencv_traincascade训练   

4b, opencv_traincascade.exe  -data classifier -vec pos.vec  -bg neg\neg.txt -w 30 -h 30  -mem 2048 -mode ALL  -minHitRate 0.9999 -maxFalseAlarmRate 0.5 -numPos 565  -numNeg 687  -numStages 16  -featureType HAAR

5,[可选] 手动生成xml
opencv249_prj.exe --size="30x30" D:\test_vs\test_etc\opencv249_prj\classifier D:\test_vs\test_etc\opencv249_prj\classifier.xml
```

当出现 “opencv_haartraining 已停止工作”  检查负样本尺寸   一定要大于正样本尺寸！！！

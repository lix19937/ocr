
+ 依赖环境搭建    

  + docker 导出
    ```bash   
     docker export 2abb3aa0771b > ocr_v1_export_backup.tar   
    ```
    2abb3aa0771b 为容器名    
    
  + docker 导入
    ```bash
     docker import ocr_v1_export_backup.tar mytest:v1
     docker run -it -v   /home/gwm/workspaces/J6_start_3027/:/open_explorer  --shm-size 128G   mytest:v1  /bin/bash

    ```
    
+  源码工程说明     
    +  目录
        ```
        root@da75400919b1:/open_explorer/YOLOX-main# tree -L 1
        
        ├── YOLOX_outputs     # ckpt 文件  
        ├── exps              # 网络配置文件   
        ├── input             # 输入
        ├── km                # 核心代码
        ├── lix_answer_dnn    # answer 网络训练
        ├── lix_answer_dnn_unittest # answer 网络预测单元测试   
        ├── output            # 输出  
        └── yolox             # yolox 代码    
      
        ```

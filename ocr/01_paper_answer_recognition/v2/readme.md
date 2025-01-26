
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

  +  

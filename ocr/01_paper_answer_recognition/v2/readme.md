
+ 依赖环境搭建    

  + docker 导出
    ```bash
    (torch210) lix@SHJS-PF4ZKYLL:~/workspaces/python$ docker ps -a
    CONTAINER ID   IMAGE                                                COMMAND                   CREATED        STATUS                     PORTS     NAMES
    2abb3aa0771b   ocr_docker_v1                                        "/bin/bash"               11 days ago    Up 2 days                            recursing_antonelli
    

     docker export 2abb3aa0771b > ocr_v1_export_backup.tar   
    ```
    2abb3aa0771b 为容器名    
    
  + docker 导入
    ```bash
     docker import ocr_v1_export_backup.tar mytest:v1
     docker run -it -v   ${spec_local_path}/:/open_explorer  --shm-size 128G   mytest:v1  /bin/bash
    ```
  + 验证
    ```bash   
      (torch210) lix@SHJS-PF4ZKYLL:~/workspaces/python$ docker ps -a
      CONTAINER ID   IMAGE                                                COMMAND                   CREATED        STATUS                     PORTS     NAMES
      da75400919b1   mytest:v1                                            "/bin/bash"               2 days ago     Up 2 days                            serene_fermi
      2abb3aa0771b   ocr_docker_v1                                        "/bin/bash"               11 days ago    Up 2 days                            recursing_antonelli
    ```    
+  源码工程说明     
    +  目录
        ```
        root@da75400919b1:/open_explorer/YOLOX-main# tree -L 1
        
        ├── YOLOX_outputs           # ckpt 文件  
        ├── exps                    # 网络配置文件   
        ├── input                   # 输入
        ├── km                      # 核心代码，具体内容见下文   
        ├── lix_answer_dnn          # answer 网络训练
        ├── lix_answer_dnn_unittest # answer 网络预测单元测试   
        ├── output                  # 输出  
        └── yolox                   # yolox 代码    
      
        ```

        ```
        root@da75400919b1:/open_explorer/YOLOX-main/km# tree -L 1
        ├── _EN_2021sep.onnx       # 英文字符识别模型文件    
        ├── __init__.py
        ├── crnn.py                # 英文字符识别网络定义   
        ├── deploy                 # 数字识别模型测试输入目录  
        ├── doc                    # 字体
        ├── inference              # 网络模型文件
        ├── inference_results      # 数字识别模型测试输出目录
        ├── model.pth              # answer 模型文件   
        ├── ocr_answer.py          # answer 定位识别数据流
        ├── ocr_pipeline.py        # 总数据流
        ├── ocr_uniform.py         # 文本定位规范化
        ├── paddleocr.py           # answer 网络定义 
        ├── ppocr                  # answer 网络定义内部实现    
        ├── predict_system.py      # answer 定位识别中间层  
        ├── text_crnn_test.py      # crnn 单元测试  
        ├── text_recog.py          # crnn 模型封装  
        └── tools                  # 测试脚本 （可忽略）
        ```
        
    + 运行命令
      ```bash
      python3 ./km/ocr_pipeline.py    
      ```  

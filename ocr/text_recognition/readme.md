```
    Text recognition model taken from here: https://github.com/meijieru/crnn.pytorch
    How to convert from pb to onnx:
    Using classes from here: https://github.com/meijieru/crnn.pytorch/blob/master/models/crnn.py
    import torch
    import models.crnn as crnn
    model = CRNN(32, 1, 37, 256)
    model.load_state_dict(torch.load('crnn.pth'))
    dummy_input = torch.randn(1, 1, 32, 100)
    torch.onnx.export(model, dummy_input, "crnn.onnx", verbose=True)
```

https://github.com/meijieru/crnn.pytorch   


+ crnn    
https://github.com/opencv/opencv_zoo/blob/main/models/text_recognition_crnn/demo.py   

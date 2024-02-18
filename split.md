竖直长图，给定高度，切分    
竖直长图，给定数目，切分   

https://www.axialis.com/tutorials/create-transparent-image-with-photoshop.html    
https://answers.opencv.org/question/73016/how-to-overlay-an-png-image-with-alpha-channel-to-another-png/    

```
std::vector<cv::Mat> channels;
cv::split(jpgImage, channels);///  jpgImage (BGR) --> channels  
channels.push_back(mask);
cv::Mat bgraImage;
cv::merge(channels, bgrAImage);
```


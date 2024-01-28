// 支持自动设置阈值  

//#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
 
using namespace std;

cv::Mat SrcImage, GrayImage, BinaryImage;

void on_trackbar(int pos, void*){
	//转化为二值图
	threshold(GrayImage, BinaryImage, pos, 255, CV_THRESH_BINARY);
	cv::imshow("二值图", BinaryImage);
}

void to_gray(){
	// 创建与原图同类型和同大小的矩阵
	GrayImage.create(SrcImage.size(), SrcImage.type());
  
	// 将原图转换为灰度图像
	cvtColor(SrcImage, GrayImage, CV_BGR2GRAY);

	cv::imshow("灰度图", GrayImage);
}

void creat_trackbar(){
	int nThreshold = 0;
	cv::createTrackbar("二值图阈值", "二值图", &nThreshold, 254, on_trackbar);
}

int main(){
	SrcImage = cv::imread("2.bmp");
	cv::imshow("原图", SrcImage);
  
	to_gray();
	on_trackbar(1, 0);
  
	creat_trackbar();
  
	waitKey();
  return 0;
}

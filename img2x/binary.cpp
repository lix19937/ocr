// 支持自动设置阈值  

//#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
 
cv::Mat src_img, gray_img, binary_img;

void on_trackbar(int pos, void*){
	cv::threshold(gray_img, binary_img, pos, 255, CV_THRESH_BINARY);
	// cv::threshold(gray_img, binary_img, pos, 255, CV_THRESH_OTSU);
	// cv::threshold(gray_img, binary_img, pos, 255, CV_THRESH_TRIANGLE);

	cv::imshow("binary", binary_img);
}

void to_gray(){
	// 创建与原图同类型和同大小的矩阵
	gray_img.create(src_img.size(), src_img.type());
  
	cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);

	cv::imshow("gray", gray_img);
}

void creat_trackbar(){
	int nThreshold = 0;
	cv::createTrackbar("thres", "binary", &nThreshold, 254, on_trackbar);
}

int main(){
	src_img = cv::imread("2.bmp");
	cv::imshow("raw", src_img);
	
	to_gray();
	on_trackbar(1, 0);
	
	creat_trackbar();
	
	waitKey();
	return 0;
}

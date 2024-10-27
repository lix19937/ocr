
#include <string>
#include "cv.h"    
#include "highgui.h"  

using namespace cv;

int main(){

	std::string in_file_name = "D:/adaboost_train_2_ju_all_svm_train/test_etc/opencv249_prj/test_modify_bri/Œ¢–≈Õº∆¨_20170830154032.png";
	Mat srcimg = imread(in_file_name, 0);
	if (srcimg.empty()){
		std::cout << "imread failed!" << std::endl;
		return 1;
	}
	
	for (size_t j = 0; j < srcimg.rows; j++){
		for (size_t i = 0; i < srcimg.cols; i++){
			if (srcimg.at<uchar>(j, i) < 245){ ////245  
				size_t tmp = srcimg.at<uchar>(j, i) + 64; ///60
				if (tmp > 255) tmp = 255;
				srcimg.at<uchar>(j, i) = tmp;
			}
		}
	}
	imwrite("C:/Users/Administrator/Desktop/2.png", srcimg);

	return 0;
}
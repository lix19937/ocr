
#include "opencv2/opencv.hpp"    
#include  <iostream>    
#include  <vector>  

#if (defined _WIN32 || defined WINCE || defined __CYGWIN__)
  #define OPENCV_VERSION "343"
  #pragma comment(lib, "opencv_core" OPENCV_VERSION ".lib")
  #pragma comment(lib, "opencv_imgproc" OPENCV_VERSION ".lib")
  #pragma comment(lib, "opencv_highgui" OPENCV_VERSION ".lib")
  #pragma comment(lib, "opencv_imgcodecs" OPENCV_VERSION ".lib")
  #pragma comment(lib, "opencv_objdetect" OPENCV_VERSION ".lib")
#endif

void modifybrightness(cv::Mat &src){
	if (src.channels() > 1){
		cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
	}

	cv::Scalar avgbrightness = mean(src);
	cv::convertScaleAbs(src, src, 128. / avgbrightness.val[0], 0);///128
}

int adaboostcascade(const std::string &input_file){
  std::vector<cv::String> imgnames;
  if (input_file[input_file.size() - 1] == '/' || input_file[input_file.size() - 1] == '\\') {
    cv::glob(input_file + "*g", imgnames);
  }
  else {
    cv::glob(input_file + "/*g", imgnames);
  }

	cv::CascadeClassifier cascade("conf/model.xml");

	int y_offset = 25;
	auto x_offset_ratio = 0.14;
	for (const auto &it: imgnames){
		auto srcimg = imread(it, 1);
		if (srcimg.empty()){
			std::cout << "imread failed.  fname:" <<it<< std::endl;
			continue;
		}

		cv::Mat src;
    int new_h = 2480 * (srcimg.rows / double(srcimg.cols));
		cv::resize(srcimg, src, cv::Size(2480, new_h));

		//detect
		std::vector<cv::Rect> objects;
    cv::Rect re(x_offset_ratio* src.cols, y_offset, src.cols*(1 - 2 * x_offset_ratio), 0.35*src.rows );
		cv::Mat roi = src(re);

		////modifybrightness(roi);
		cascade.detectMultiScale(roi, objects, 1.1, 1, 1, cv::Size(34, 34 ), cv::Size(72, 70));/// 117 29
		cv::rectangle(src, re, cv::Scalar(0, 255, 0), 2, 8, 0);
    cv::putText(src, "roi area", cv::Point(re.x, re.y +25), 2, 2, cv::Scalar(0,0,255));

		std::cout << "beat num:" << objects.size() << std::endl;
		for (int i = 0; i < objects.size(); ++i){
			rectangle(roi, objects[i], cv::Scalar(255, 0, 0), 3, 8, 0);
      cv::putText(roi, "+", cv::Point(objects[i].x + objects[i].width/2, objects[i].y + objects[i].height/2), 2,1, cv::Scalar(255, 0, 255));
		}

    cv::resize(src, src, cv::Size(1400, 1400./src.cols*src.rows));
    cv::imshow(it, src);
    cv::waitKey();
    cv::destroyAllWindows();
	}

	return 0;
}


int main(int argc, char** argv){
	const std::string input_file = "samples/";
	adaboostcascade(input_file);

	return 0;
}
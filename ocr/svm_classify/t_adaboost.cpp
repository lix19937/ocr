
#include "opencv2/core/core.hpp"    
#include "opencv2/highgui/highgui.hpp"    
#include "opencv2/imgproc/imgproc.hpp"    
#include "opencv2/ml/ml.hpp"    
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include  <direct.h>  
#include  <stdio.h>  
#include  <Windows.h>
#include  <iostream>    
#include  <vector>  

using namespace cv;
using namespace std;

#define _IMG_UNI_OFF
#define _SINGLE_WORD_UNI_OFF

static void readimgnamefromfile(const char* fileName, std::vector<std::string> &imgnames);
static long long icnt = 0;

void modifybrightness(cv::Mat &src){
	if (src.channels() > 1){
		cvtColor(src, src, COLOR_BGR2GRAY);
	}

	Scalar avgbrightness = mean(src);
	convertScaleAbs(src, src, 255 / avgbrightness.val[0], 0);/// or 300
}

void modifybrightness2(cv::Mat &src){
	if (src.channels() > 1){
		cvtColor(src, src, COLOR_BGR2GRAY);
	}

	Scalar avgbrightness = mean(src);
	convertScaleAbs(src, src, 128 / avgbrightness.val[0], 0);///128
}

void writegrayimg(const int i, const std::string &input_path, const std::string &output_path_pos, int hw){
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << std::endl;
		return;
	}
	//imwrite(output_path_pos + std::to_string(i) + ".jpg", src);  return;

	//modifybrightness2(src);
	Mat uniformtmp;
	if (src.cols == src.rows)
		resize(src, uniformtmp, Size(40, 40), 0, 0, CV_INTER_AREA);////////////////// 44 44        身份证
	else if (src.cols <= 32 || src.rows <= 32){
		resize(src, uniformtmp, Size(40, 40), 0, 0, CV_INTER_AREA);////////////////// 44 44        身份证
	}
	else 
		uniformtmp = src;

	imwrite(output_path_pos + std::to_string(i) + ".jpg", uniformtmp);  return;

	//resize(src, uniformtmp, Size(32, 32), 0, 0, CV_INTER_AREA);////single word


	resize(src, uniformtmp, Size(65, 45), 0, 0, CV_INTER_AREA);////////////////// 47  30        600*848  银行卡  

	imwrite(output_path_pos + std::to_string(i) + ".jpg", uniformtmp);  return;
}

void negcreate(){
	const std::string input_path = "D:/adaboost_train_8_sfz/samples2/";//"D:/adaboost_train_8_sfz/test_etc/opencv249_prj/neg33/";
	const std::string output_path = "D:/adaboost_train_8_sfz/test_etc/opencv249_prj/neg_3/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	for (int i = 0; i < imgnames.size(); ++i){
		writegrayimg(i, input_path + imgnames[i], output_path, 40);
	}	
}

void writegrayimganduni(const int i, const std::string &input_path, const std::string &output_path_pos, int hw){
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << std::endl;
		return;
	}
	Mat uniformtmp;
	modifybrightness2(uniformtmp);
	resize(src, uniformtmp, Size(hw, hw), 0, 0, CV_INTER_AREA);
	//imwrite(output_path_pos + std::to_string(i) + ".jpg", uniformtmp);
	return;


	if (src.cols < hw || src.rows < hw){
		Mat uniformtmp;
		modifybrightness2(uniformtmp);
		resize(src, uniformtmp, Size(hw, hw), 0, 0, CV_INTER_AREA);
		//imwrite(output_path_pos + std::to_string(i) + ".jpg", uniformtmp);
	}
	else{
		//imwrite(output_path_pos + std::to_string(i) + ".jpg", src);
	}
}

void graysamplescreate(){
	const std::string input_path = "C:/Users/Administrator/Desktop/tmp/";
	const std::string output_path = "C:/Users/Administrator/Desktop/1234/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	for (int i = 0; i < imgnames.size(); i += 1){
		std::string path = input_path + imgnames[i];

		Mat src = imread(path, 0);
		if (src.empty()){
			std::cerr << "imread faild, i:" << i << std::endl;
			continue;
		}
		Mat uniformtmp;
		modifybrightness2(src);
		//imwrite(output_path + "00" + std::to_string(i) + ".jpg", src);
	}
}

void svmsamplescreate(){
	const std::string input_path ="C:/Users/Administrator/Desktop/7/";
	const std::string output_path = "D:/adaboost_train_ju/test_etc/data3/7/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	int j = 125;
	for (int i = 250; i < imgnames.size(); i += 1, ++j){
		writegrayimganduni(j, input_path + imgnames[i], output_path, 32);

	
		if (j== 215) break;
	}
}

int adaboostcascade(const std::string &input_file){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	CascadeClassifier cascade("adaboost_gray_sfz_588.xml");

	int y_offset = 20;
	double x_offset_ratio = 0.1;
	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}

		Mat src;
		int new_h = 826 * (srcimg.rows / double(srcimg.cols));///// w = 1652/2 = 826   身份证
		resize(srcimg, src, Size(826, new_h));

		//detect
		vector<Rect> objects;
		Rect re = Rect(x_offset_ratio* src.cols, y_offset, src.cols*(1 - 2 * x_offset_ratio), src.rows - y_offset);
		Mat roi = src( re);

		modifybrightness2(roi);
		/////detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		cascade.detectMultiScale(roi, objects, 1.1, 1, 1, Size(109, 27 ), Size(129, 33));/// 117 29
		rectangle(src, re, Scalar(255), 2, 8, 0);

		std::cout << "beat num:" << objects.size() << std::endl;
		if (objects.empty()){
			//imwrite("d:/123/" + imgnames[idx], src);
		}

		rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 2, 8, 0);
		for (int i = 0; i < objects.size(); ++i){
			rectangle(roi, objects[i], Scalar(0), 3, 8, 0);
		}

		namedWindow("result", 0);
		resizeWindow("result", src.cols >> 1, src.rows>>1);
		imshow("result", src);
		waitKey();
	}

	return 0;
}

void coloruniform(){
	const std::string input_path = "d:/bj/";
	const std::string tem_path = "D:/bj_out/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames); 

	for (int i = 0, tmpi = 0; i < imgnames.size(); ++i){
		Mat src = imread(input_path + imgnames[i]);
		if (src.empty()){
			std::cerr << "imread faild, i:" << i << ", fname:" << input_path << std::endl;
			continue;
		}

		vector<Mat > rgbch;
		split(src, rgbch);
		////imshow("b", rgbch[0]); //imshow("g", rgbch[1]); //imshow("r", rgbch[2]);
		modifybrightness(rgbch[0]);
		modifybrightness(rgbch[1]);
		modifybrightness(rgbch[2]);
		////imshow("bb", rgbch[0]); //imshow("gb", rgbch[1]); //imshow("rb", rgbch[2]); 	waitKey();
		Mat dst;
		merge(rgbch, src);
		////imshow("22", dst);		waitKey();
		//imwrite(tem_path + std::to_string(i) + ".jpg", src);
	}
}

bool pospreprecess(const int i, const std::string &input_path, const std::string &tem_path, const std::string &output_path_pos, const int &unisize, Mat &uniformmat){
	Mat src = imread(input_path , 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << std::endl;
		return false;
	}
	////imwrite(tem_path + "s1_src" + to_string(i) + ".jpg", src);
	Mat  sample_mat = src.clone();

	//gray2bin
	threshold(sample_mat, sample_mat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	////imwrite(tem_path + "s2_threshold" + to_string(i) + ".jpg", sample_mat);

	//search top bot lines 
	medianBlur(sample_mat, sample_mat, 3);
	////imwrite(tem_path + "s3_medianBlur" + to_string(i) + ".jpg", sample_mat);

	int t_idx = 0, b_idx = sample_mat.rows - 1, l_idx = 0, r_idx = sample_mat.cols - 1;
	for (; t_idx < sample_mat.rows >> 1; ++t_idx){//y    top
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(t_idx)[j] == 0){
				goto out_t;
			}
		}
	}		out_t:;

	for (; b_idx>sample_mat.rows >> 1; --b_idx){//y  bottom
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(b_idx)[j] == 0){
				goto out_b;
			}
		}
	}	out_b:;

	for (; l_idx < sample_mat.cols >> 1; ++l_idx){ //  x   left 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[l_idx] == 0){
				goto out_l;
			}
		}
	}	out_l:;

	for (; r_idx > sample_mat.cols >> 1; --r_idx){ //  x   right 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[r_idx] == 0){
				goto out_r;
			}
		}
	}	out_r:;

	Rect re = Rect(l_idx - 1, t_idx - 1, r_idx - l_idx + 2, b_idx - t_idx + 2);
	if (re.x < 0){
		re.x = l_idx;
	}
	if (re.y < 0){
		re.y = t_idx;
	}
	if (re.br().x > sample_mat.cols){
		re.width = sample_mat.cols - re.x;
	}
	if (re.br().y > sample_mat.rows){
		re.height = sample_mat.rows - re.y;
	}

	//rectangle(sample_mat, re.tl(), re.br(), Scalar(128, 128, 128), 1, 8, 0);
	////imwrite(tem_path + "s4_uniform" + to_string(i) + ".jpg", sample_mat);		 

	// rect roi 
	//rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 1, 8, 0);
	////imwrite(tem_path + "s5_uniform" + to_string(i) + ".jpg", src(re));	 

	double whratio = re.width / (re.height + 0.0);
	std::cout << "cnt:" << i << ", w/h :" << whratio << std::endl;
	Mat srcb = src(re);
	//modifybrightness(srcb);
	uniformmat = Mat(unisize, unisize, srcb.type(), Scalar(255));
	if (whratio < 1) {
		Mat uniformtmp;
		resize(srcb, uniformtmp, Size(unisize * whratio, unisize), 0, 0, CV_INTER_LINEAR);
		Mat roi = uniformmat(Rect((uniformmat.cols - uniformtmp.cols) >> 1, 0, uniformtmp.cols, uniformtmp.rows));
		uniformtmp.copyTo(roi);
	}
	else if (whratio <= 1.1) {
		resize(srcb, uniformmat, Size(unisize, unisize), 0, 0, CV_INTER_LINEAR);
	}
	else {
		std::cout << "whratio is not suppout at present." << std::endl;
		return false;
	}
	////imwrite(tem_path + "s6_uniformmat" + to_string(i) + ".jpg", uniformmat);

	//modifybrightness(uniformmat);
	////imwrite(tem_path + "s7_uniformmat_bri" + to_string(i) + ".jpg", uniformmat);

	//imwrite(output_path_pos +  std::to_string(i) + ".jpg", uniformmat);
	return true;
}

bool pospreprecess2(const int i, const std::string &input_path, const std::string &tem_path, const std::string &output_path_pos, const int &unisize, Mat &uniformmat){
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << std::endl;
		return false;
	}
	////imwrite(tem_path + "s1_src" + to_string(i) + ".jpg", src);
	Mat  sample_mat = src.clone();

	//gray2bin
	threshold(sample_mat, sample_mat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	////imwrite(tem_path + "s2_threshold" + to_string(i) + ".jpg", sample_mat);

	//search top bot lines 
	medianBlur(sample_mat, sample_mat, 5);
	////imwrite(tem_path + "s3_medianBlur" + to_string(i) + ".jpg", sample_mat);

	int t_idx = 0, b_idx = sample_mat.rows - 1, l_idx = 0, r_idx = sample_mat.cols - 1;
	for (; t_idx < sample_mat.rows >> 1; ++t_idx){//y    top
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(t_idx)[j] == 0){
				goto out_t;
			}
		}
	}	out_t:;

	for (; b_idx>sample_mat.rows >> 1; --b_idx){//y  bottom
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(b_idx)[j] == 0){
				goto out_b;
			}
		}
	}	out_b:;

	for (; l_idx < sample_mat.cols >> 1; ++l_idx){ //  x   left 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[l_idx] == 0){
				goto out_l;
			}
		}
	}	out_l:;

	for (; r_idx > sample_mat.cols >> 1; --r_idx){ //  x   right 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[r_idx] == 0){
				goto out_r;
			}
		}
	}	out_r:;

	Rect re = Rect(l_idx - 1, t_idx - 1, r_idx - l_idx + 2, b_idx - t_idx + 2);
	if (re.x < 0){
		re.x = l_idx;
	}
	if (re.y < 0){
		re.y = t_idx;
	}
	if (re.br().x > sample_mat.cols){
		re.width = sample_mat.cols - re.x;
	}
	if (re.br().y > sample_mat.rows){
		re.height = sample_mat.rows - re.y;
	}

	//rectangle(sample_mat, re.tl(), re.br(), Scalar(128, 128, 128), 1, 8, 0);
	////imwrite(tem_path + "s4_uniform" + to_string(i) + ".jpg", sample_mat);	 

	// rect roi 
	//rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 1, 8, 0);
	////imwrite(tem_path + "s5_uniform" + to_string(i) + ".jpg", src(re));	 

	uniformmat = Mat(unisize, unisize, sample_mat.type(), Scalar(255));
	////////////////////////////	////////////////////////////
	const int comsize = re.width > re.height ? re.width :re.height;
	Mat uniformmattmp = Mat(comsize, comsize, sample_mat.type(), Scalar(255) );
	Mat roi = uniformmattmp(Rect((comsize - re.width) >> 1, (comsize - re.height) >> 1, re.width, re.height) );
	src(re).copyTo(roi);

	resize(uniformmattmp, uniformmat, Size(unisize, unisize), 0, 0, CV_INTER_LINEAR);
	/////////////////////////////////	////////////////////////////

#ifdef  use_old
	resize(src(re), uniformmat, Size(unisize, unisize), 0, 0, CV_INTER_LINEAR);
	//threshold(uniformmat, uniformmat, 0, 255, CV_THRESH_OTSU| CV_THRESH_BINARY);
#endif

 	modifybrightness(uniformmat);
	//threshold(uniformmat, uniformmat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	////imshow("after bri", uniformmat);
	//waitKey();
	//imwrite(output_path_pos + std::to_string(i) + ".jpg", uniformmat);
	return true;
}

///for sfz pos
bool pospreprecess3(const int i, const std::string &input_path, const std::string &tem_path, const std::string &output_path_pos, const int &unisize, Mat &uniformmat){
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << std::endl;
		return false;
	}
	////imwrite(tem_path + "s1_src" + to_string(i) + ".jpg", src);
	Mat  sample_mat = src.clone();

	//gray2bin
	threshold(sample_mat, sample_mat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	////imwrite(tem_path + "s2_threshold" + to_string(i) + ".jpg", sample_mat);

	//search top bot lines 
	medianBlur(sample_mat, sample_mat, 5);
	////imwrite(tem_path + "s3_medianBlur" + to_string(i) + ".jpg", sample_mat);

	int t_idx = 0, b_idx = sample_mat.rows - 1, l_idx = 0, r_idx = sample_mat.cols - 1;
	for (; t_idx < sample_mat.rows >> 1; ++t_idx){//y    top
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(t_idx)[j] == 0){
				goto out_t;
			}
		}
	}	out_t:;

	for (; b_idx>sample_mat.rows >> 1; --b_idx){//y  bottom
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(b_idx)[j] == 0){
				goto out_b;
			}
		}
	}	out_b:;

	for (; l_idx < sample_mat.cols >> 1; ++l_idx){ //  x   left 
	//	for (int i = 0; i < sample_mat.rows; ++i){ ////top -> bot
		for (int i = sample_mat.rows-1; i >=0; --i){ ////bot->top
			if (sample_mat.ptr<uchar>(i)[l_idx] == 0){
				goto out_l;
			}
		}
	}	out_l:;

	for (; r_idx > sample_mat.cols >> 1; --r_idx){ //  x   right 
		//	for (int i = 0; i < sample_mat.rows; ++i){ ////top -> bot
		for (int i = sample_mat.rows - 1; i >= 0; --i){ ////bot->top
			if (sample_mat.ptr<uchar>(i)[r_idx] == 0){
				goto out_r;
			}
		}
	}	out_r:;

	Rect re = Rect(l_idx - 1, t_idx - 1, r_idx - l_idx + 2, b_idx - t_idx + 2);
	if (re.x < 0){
		re.x = l_idx;
	}
	if (re.y < 0){
		re.y = t_idx;
	}
	if (re.br().x > sample_mat.cols){
		re.width = sample_mat.cols - re.x;
	}
	if (re.br().y > sample_mat.rows){
		re.height = sample_mat.rows - re.y;
	}

	/* the follow just for debug */
	//rectangle(sample_mat, re.tl(), re.br(), Scalar(128, 128, 128), 1, 8, 0);
	////imwrite(tem_path + "s4_uniform" + to_string(i) + ".jpg", sample_mat);	 

	// rect roi 
	//rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 1, 8, 0);
	////imwrite(tem_path + "s5_uniform" + to_string(i) + ".jpg", src(re));	 

#ifdef _IMG_UNI
	#ifdef _SINGLE_WORD_UNI_ 
		uniformmat = Mat(unisize, unisize, sample_mat.type(), Scalar(255));	   
		resize(src(re), uniformmat, Size(unisize, unisize), 0, 0, /* CV_INTER_LINEAR*/ CV_INTER_AREA);
	#else
		uniformmat = Mat(unisize, unisize, sample_mat.type(), Scalar(255));	   
		resize(src(re), uniformmat, Size(unisize, unisize), 0, 0, /* CV_INTER_LINEAR*/ CV_INTER_AREA);
	#endif
	//modifybrightness2(uniformmat);
	imwrite(output_path_pos + "0" + std::to_string(i) + ".jpg", uniformmat);
#else
	Mat uni_t = src(re);
	modifybrightness2(uni_t);
	//imwrite(output_path_pos + "0" + std::to_string(i) + ".jpg", uni_t);
#endif

	return true;
}
   
void poscreate(){
	const std::string input_path = /*"D:/adaboost_train_8_sfz/test_etc/opencv249_prj/pos/";*/ "D:/715-work/sfz_背面/";
//	const std::string output_path_pos = "D:/test_vs/test_etc/opencv249_prj/pos/";
	const std::string tem_path = "D:/adaboost_train_8_sfz/test_etc/opencv249_prj/pos_3/";// "D:/715-work/sfz_背面_处理/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	for (int i = 0, tmpi = 0; i < imgnames.size(); ++i){
		Mat uniformmat;
		if (pospreprecess3(tmpi, input_path + imgnames[i], tem_path, tem_path, 32/*归一化尺寸*/, uniformmat)){
			++tmpi;
		}
	}
}

static std::string WChar2Ansi(LPCWSTR pwszSrc){
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}

void readimgnamefromfile(const char* fileName, std::vector<std::string> &imgnames){
	imgnames.clear();
	WIN32_FIND_DATA file;
	int i = 0;
	char tempFilePath[MAX_PATH + 1];
	char tempFileName[256];
	// 转换输入文件名
	sprintf_s(tempFilePath, "%s/*", fileName);
	// 多字节转换
	WCHAR   wstr[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, tempFilePath, -1, wstr, sizeof(wstr));
	// 查找该文件待操作文件的相关属性读取到WIN32_FIND_DATA
	HANDLE handle = FindFirstFile(wstr, &file);
	if (handle != INVALID_HANDLE_VALUE) {
		FindNextFile(handle, &file);
		FindNextFile(handle, &file);
		// 循环遍历得到文件夹的所有文件名    
		do {
			sprintf(tempFileName, "%s", fileName);
			imgnames.push_back(WChar2Ansi(file.cFileName));
			imgnames[i].insert(0, tempFileName);
			size_t pos = imgnames[i].find_last_of("/");
			if (pos != std::string::npos){
				imgnames[i] = imgnames[i].substr(pos + 1);
			}
			//std::cout << "vec :" << imgnames[i] << std::endl;
			i++;
		} while (FindNextFile(handle, &file));
	}
	FindClose(handle);
}

//// for zhu yuan
bool pospreprecess4(const int i, const std::string &input_path, const std::string &tem_path, const std::string &output_path_pos, const int &unisize, Mat &uniformmat){
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << ", input_path:" << input_path << std::endl;
		return false;
	}
	//imwrite(tem_path + "s1_src" + to_string(i) + ".jpg", src);
	Mat  sample_mat = src.clone();

//	modifybrightness2(sample_mat);

	//gray2bin
	threshold(sample_mat, sample_mat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	//imwrite(tem_path + "s2_threshold" + to_string(i) + ".jpg", sample_mat);

	//search top bot lines 
	medianBlur(sample_mat, sample_mat, 3);
	//imwrite(tem_path + "s3_medianBlur" + to_string(i) + ".jpg", sample_mat);

	int t_idx = 0, b_idx = sample_mat.rows - 1, l_idx = 0, r_idx = sample_mat.cols - 1;
	for (; t_idx < sample_mat.rows >> 1; ++t_idx){//y    top
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(t_idx)[j] == 0){
				goto out_t;
			}
		}
	}	out_t:;

	for (; b_idx>sample_mat.rows >> 1; --b_idx){//y  bottom
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(b_idx)[j] == 0){
				goto out_b;
			}
		}
	}	out_b:;

	for (; l_idx < sample_mat.cols >> 1; ++l_idx){ //  x   left 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[l_idx] == 0){
				goto out_l;
			}
		}
	}	out_l:;

	for (; r_idx > sample_mat.cols >> 1; --r_idx){ //  x   right 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[r_idx] == 0){
				goto out_r;
			}
		}
	}	out_r:;

	Rect re = Rect(l_idx - 1, t_idx - 1, r_idx - l_idx + 2, b_idx - t_idx + 2);
	if (re.x < 0){
		re.x = l_idx;
	}
	if (re.y < 0){
		re.y = t_idx;
	}
	if (re.br().x > sample_mat.cols){
		re.width = sample_mat.cols - re.x;
	}
	if (re.br().y > sample_mat.rows){
		re.height = sample_mat.rows - re.y;
	}

	//rectangle(sample_mat, re.tl(), re.br(), Scalar(128, 128, 128), 1, 8, 0);
	//imwrite(tem_path + "s4_uniform" + to_string(i) + ".jpg", sample_mat);	 

	// rect roi 
	//rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 1, 8, 0);
	//imwrite(tem_path + "s5_uniform" + to_string(i) + ".jpg", src(re));	 

	int uni_row = unisize*1;
	uniformmat = Mat(uni_row, unisize, sample_mat.type(), Scalar(255));
	resize(src(re), uniformmat, Size(unisize, uni_row), 0, 0, CV_INTER_AREA);
	modifybrightness2(uniformmat);

	//threshold(uniformmat, uniformmat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	//imshow("after bri", uniformmat);
	//waitKey();
	std::string nosuf;
	size_t pos = input_path.find_last_of('/');
	if (pos != std::string::npos){
		nosuf = input_path.substr(pos + 1);
	}

	imwrite(output_path_pos +/* nosuf */   std::to_string(i) + ".jpg", uniformmat);
	return true;
}

//// for zhu yuan adaboost pos
void ada_zy_poscreate(){
	const int unisize = 64; //  1 :  5 or  1: 4   64*32

//	const std::string input_path = "D:/create_chars/genaratechar/TrapezoidCorrection/zy_cut_by_hand/"; //"D:/715-work/lp_single/"; // "D:/adaboost_train_10_lp/test_etc/opencv249_prj/t_ada_create_auto_1234/";//"D:/715-work/lp_single/"; 
//	const std::string tem_path = "D:/adaboost_train_12_zy/pos/"; //"D:/adaboost_train_10_lp/test_etc/opencv249_prj/pos/";

	const std::string input_path = "D:/create_chars/genaratechar/TrapezoidCorrection/zy_cut_from_adaboost/"; //"D:/715-work/lp_single/"; // "D:/adaboost_train_10_lp/test_etc/opencv249_prj/t_ada_create_auto_1234/";//"D:/715-work/lp_single/"; 
	const std::string tem_path = "D:/create_chars/genaratechar/TrapezoidCorrection/zy_cut_uni_2599/"; //"D:/adaboost_train_10_lp/test_etc/opencv249_prj/pos/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	//normalization
	for (int i = 0 ; i < imgnames.size(); ++i){
		Mat uniformmat;
		if (!pospreprecess4(i, input_path + imgnames[i], tem_path, tem_path, unisize, uniformmat)){
			std::cout << "+++++++" << std::endl;
		}
	}
}

//// for zhu yuan resize ada pos
int zy_splitkerl(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);
	long long jcnt = 0;

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		Mat src;
		resize(srcimg, src, Size(64, 32), 0, 0, 3);
		imwrite(out_path + std::to_string(1232 + jcnt++) + ".jpg"/* imgnames[idx]*/, src);
	}

	return 0;
}

//// for zhu yuan 
void writegrayimg_split(const int i, const std::string &input_path, const std::string &output_path_pos){
	const int uni_w = 100, uni_h = uni_w >> 1;
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << std::endl;
		return;
	}

	if (src.rows <= uni_w || src.cols <= uni_h){
		std::cerr << "!!!!!  i:" << i << std::endl;
		imwrite(output_path_pos + "0" + std::to_string(icnt++) + ".jpg", src);
		return;
	}

	Mat srcimg;
	if (src.cols > 1248){
		int new_h = 1248 * (src.rows / double(src.cols));///// w = 1248   zhu yuan fei yong 
		resize(src, srcimg, Size(1248, new_h));
	}
	else{
		srcimg = src;
	}

	for (int ri = 0; ri < srcimg.rows - uni_h; ri += uni_h + 10){
		for (int ci = 0; ci < srcimg.cols - uni_w; ci += uni_w + 10){
			Mat ss = srcimg(Rect(ci, ri, uni_w, uni_h));
			imwrite(output_path_pos + "0"+std::to_string(icnt++) + ".jpg", ss);
		}
	}
}

////for zhu yuan resize ada neg
void zy_ada_neg(){
	const std::string input_path = "C:/Users/Administrator/Desktop/新建文件夹/";
	const std::string output_path = "D:/adaboost_train_12_zy/neg/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	for (int i = 0; i < imgnames.size(); i++){
		writegrayimg_split(i, input_path + imgnames[i], output_path);
	}
}

////for zhu yuan adaboost
int zy_adaboostcascade(const std::string &input_file){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	CascadeClassifier cascade("D:/adaboost_train_12_zy/classifier_zy.xml");

	int y_offset = 15;
	double x_offset_ratio = 0.1;
	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}

		std::cout << "imread. i:" << idx << ", fname:" << imgnames[idx] << std::endl;

		Mat src;
		int new_h = 1248*1.5 * (srcimg.rows / double(srcimg.cols));/////  
		resize(srcimg, src, Size(1248*1.5, new_h));

		//detect
		vector<Rect> objects;
		Rect re = Rect(x_offset_ratio* src.cols, y_offset, src.cols*(1 - 2 * x_offset_ratio), src.rows*0.9 - y_offset);
		Mat roi = src(re);

		modifybrightness2(roi);
		/////detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		cascade.detectMultiScale(roi, objects, 1.1, 1, 1, Size(64*0.5, 32*0.5), Size(64*2, 32*2));/// 117 29
		rectangle(src, re, Scalar(255), 2, 8, 0);

		std::cout << "beat num:" << objects.size() << std::endl;
		if (objects.empty()){
			//imwrite("d:/123/" + imgnames[idx], src);
		}

		rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 2, 8, 0);
		for (int i = 0; i < objects.size(); ++i){
			rectangle(roi, objects[i], Scalar(0), 2, 8, 0); continue;
			Mat dst;
			resize(roi(objects[i]), dst, Size(64, 32), 0, 0, 3);
			imwrite("D:/adaboost_train_2_ju_all_svm_train/test_etc/svm_2_classes_zy_train/data3/00/" + std::to_string(1599+icnt++) + ".jpg", dst);
		}

		//continue;
		namedWindow("result", 0);
		resizeWindow("result", src.cols >> 1, src.rows >> 1);
		imshow("result", src);
		waitKey();
	}

	return 0;
}

//// for jun resize ada pos
int jun_splitkerl(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);
	long long jcnt = 0;

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		Mat src;
		//modifybrightness2(src);
		resize(srcimg, src, Size(32, 32), 0, 0, 2);
		modifybrightness2(src);
		imwrite(out_path + std::to_string(219*5+ jcnt++) + ".jpg"/* imgnames[idx]*/, src);
	}

	return 0;
}

//// for jun resize ada neg
int jun_ada_neg(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);
	long long jcnt = 0;

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		//std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		Mat src;
		
		if (srcimg.cols < 32 || srcimg.rows < 32)
			resize(srcimg, src, Size(32, 32), 0, 0, 3);
		else src = srcimg;

		imwrite(out_path + std::to_string( jcnt++) + ".jpg"/* imgnames[idx]*/, src);
	}

	return 0;
}

////for zhu yuan adaboost
int jun_adaboostcascade(const std::string &input_file){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	CascadeClassifier cascade("D:/adaboost_train_13_jun/classifier_jun_0920.xml");

	int y_offset = 20;
	double x_offset_ratio = 0.1;
	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}

		//std::cout << "imread. i:" << idx << ", fname:" << imgnames[idx] << std::endl;

		Mat src;
		int new_h = 2480 * 1 * (srcimg.rows / double(srcimg.cols));/////  
		resize(srcimg, src, Size(2480 * 1, new_h));

		//detect
		vector<Rect> objects;
		Rect re = Rect(x_offset_ratio* src.cols, y_offset, src.cols*(1 - 2 * x_offset_ratio), src.rows*0.4 - y_offset);
		Mat roi = src(re);

		modifybrightness2(roi);
		/////detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		cascade.detectMultiScale(roi, objects, 1.1, 1, 1, Size(34, 34), Size(72, 70) );/// 117 29
		rectangle(src, re, Scalar(255), 2, 8, 0);

		//std::cout << "beat num:" << objects.size() << std::endl;
		if (objects.empty()){
			//imwrite("d:/123/" + imgnames[idx], src);
		}

		rectangle(src, re.tl(), re.br(), Scalar(255), 2, 8, 0);
		for (int i = 0; i < objects.size(); ++i){
			//rectangle(roi, objects[i], Scalar(0), 2, 8, 0); continue;
			Mat dst;
			resize(roi(objects[i]), dst, Size(32, 32), 0, 0, 3);
			imwrite("D:/adaboost_train_2_ju_all_svm_train/test_etc/svm_2_classes_jun_train/data3/0/" + std::to_string(icnt++) + ".jpg", dst);
		}

		continue;
		namedWindow("result", 0);
		resizeWindow("result", src.cols >> 1, src.rows >> 1);
		imshow("result", src);
		waitKey();
	}

	return 0;
}

////for fa svm pos
void svm_fa_poscreate(){
	const int unisize = 32; //  1 :  5 or  1: 4   64*32

	const std::string input_path = "D:/adaboost_train_2_ju_all_svm_train/test_etc/svm_2_classes_fa_train/data3/neg/";
	const std::string tem_path = "D:/adaboost_train_2_ju_all_svm_train/test_etc/svm_2_classes_fa_train/data3/0/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	//normalization
	for (int i = 0; i < imgnames.size(); ++i){
		Mat uniformmat;
		if (!pospreprecess4(i, input_path + imgnames[i], tem_path, tem_path, unisize, uniformmat)){
			std::cout << "+++++++" << std::endl;
		}
	}
}

int main(int argc, char** argv){
//	svm_fa_poscreate(); return 0;
	jun_adaboostcascade("D:/715-work/EaiImageAnalysis/test_dll/out/b解放军/"); return 0;

//	jun_ada_neg("D:/adaboost_train_13_jun/pos_tmp/", "D:/adaboost_train_13_jun/pos/"); return 0;

//	jun_splitkerl("D:/adaboost_train_13_jun/jun_by_hand/", "D:/adaboost_train_13_jun/pos/"); return 0;

//	ada_zy_poscreate(); return 0;
//	zy_adaboostcascade("D:/影像分拣/4-住院费用清单/"/* "D:/adaboost_train_12_zy/neg_samples/"*/); return 0;

//	zy_ada_neg(); return 0;

	//zy_splitkerl("D:/create_chars/genaratechar/TrapezoidCorrection/out/", "D:/create_chars/genaratechar/TrapezoidCorrection/zy_cut_uni/"); return 0;
//	ada_zy_poscreate(); return 0;
	//graysamplescreate(); return 0;
	//svmsamplescreate(); return 0;
	//negcreate();		 	return 0;
	//coloruniform();
 //   poscreate();    return 0;


	//const std::string input_file = "D:/分拣/samples/";//"D:/分拣/2-票据彩色/ ";//"D:/bj/";//"D:/test_vs/test_etc/opencv249_prj/sample_class/";
	//adaboostcascade(input_file);

	return 0;
}
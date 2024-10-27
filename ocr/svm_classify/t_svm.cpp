
#include "cv.h"    
#include "highgui.h"  
#include <ml.h>  
#include <opencv2/ml/ml.hpp>
#include "mysvm.h"

#include <iostream>    
#include <fstream>    
#include <string>    
#include <vector>
#include <io.h>
#include <stdlib.h> 


#include  <direct.h>  
#include  <stdio.h>  
#include  <Windows.h>
#include  <stdlib.h>

using namespace cv;
using namespace std;

//Size winSize = Size(64, 48); // yhk          !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!change  1

//Size winSize = Size(128, 32); //sfz

Size winSize = Size(32, 32); // 32 32

#define hog_feature_nums  ( (size_t)9* \
(16 / 8)* \
(16 / 8)* \
((winSize.width - 16) / 8 + 1)* \
((winSize.height - 16) / 8 + 1) )

string filepath = "..\\data_all\\";
int classes = 3;//31;  //图像类别数31     票  据   上   京   央   诊  住  
int size = 32; //32;  //图像大小32
int train_samples =  1000; // 50;  //每一类的样本数
string num = "012345678";//"0123456789,-";
char word[] = "XX票据上京央诊住苏";//"零壹贰叁肆伍陆柒捌玖元负万仟佰拾整角分圆";

//////////// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!change  2

//const std::string svm_file = "../svm_8_classes_train/ADABOOST_SVM_RBF_GRAY20_0820.XML";
//const std::string samples_path = "../svm_8_classes_train/data3/";

//const std::string svm_file = "../svm_2_classes_shang_train/ADABOOST_SVM_RBF_SHANG_GRAY2_0825.XML";
//const std::string samples_path = "../svm_2_classes_shang_train/data3/";

//const std::string svm_file = "../svm_2_classes_ju_train/ADABOOST_SVM_RBF_JU_GRAY2_0808.XML";
//const std::string samples_path = "../svm_2_classes_ju_train/data3/";

//const std::string svm_file = "../svm_2_classes_zhen_train/ADABOOST_SVM_RBF_ZHEN_GRAY2_0809.XML";
//const std::string samples_path = "../svm_2_classes_zhen_train/data3/";

//const std::string svm_file = "../svm_2_classes_bankc_train/ADABOOST_SVM_RBF_BANKC_GRAY2_0817.XML";
//const std::string samples_path = "../svm_2_classes_bankc_train/data3/";

//const std::string svm_file = "../svm_2_classes_idc_train/ADABOOST_SVM_RBF_IDC_GRAY2_0819.XML";
//const std::string samples_path = "../svm_2_classes_idc_train/data3/";

const std::string svm_file = "../svm_2_classes_shen_train/ADABOOST_SVM_RBF_SHEN_GRAY2_0907.XML";
const std::string samples_path = "../svm_2_classes_shen_train/data3/";

//const std::string svm_file = "../svm_2_classes_pei_train/ADABOOST_SVM_RBF_PEI_GRAY2_0904.XML";
//const std::string samples_path = "../svm_2_classes_pei_train/data3/";

//char word_test[] =  "XX票据上京央诊住苏";//"零壹贰叁肆伍陆柒捌玖拾元圆仟佰角分整万NN"; //NN代表噪声图像

////////!!!!!change  3
//char word_test[] = "XX银";// 
//char word_test[] = "XX身";// 

//char word_test[] = "XX赔";// 

char word_test[] = "XX申";

//获取文件夹下的所有文件名
void getFiles(string path, vector<string>& files)
{
	long   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void getdata(Mat &trainData, Mat &trainClasses){
	Mat src_image;
	Mat prs_image;
	Mat data;
	string file;
	int size = 32;  //32 ////////////////////////

	unsigned long n;
	HOGDescriptor *hog = new HOGDescriptor(Size(size, size)  /*winSize*/ , Size(16, 16), Size(8, 8), Size(8, 8), 9);
	for (int i = 0; i < classes; i++){
		for (int j = 0; j < train_samples; j++){
			file = filepath + to_string(i + 1) + "_" + to_string(j + 1) + ".png";
			src_image = imread(file, 0);
			if (!src_image.data){
				printf("Error:can not open the image %s\n", file);
				continue;
			}
			//threshold(src_image, src_image, 0, 255, CV_THRESH_OTSU);
			Mat img;
			resize(src_image, img, Size(size, size), 0, 0, CV_INTER_AREA);
			trainClasses.at<float>(i*train_samples+j, 0) = i;
			vector<float>descriptors;
			hog->compute(img, descriptors, Size(1, 1), Size(0, 0));
			if (i == 0 && j == 0){
				trainData = Mat::zeros(train_samples*classes, descriptors.size(), CV_32FC1);
			}
			n = 0;
			for (vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++){
				trainData.at<float>(i*train_samples + j, n) = *iter;
				n++;
			}
		}
	}
}

// LPCWSTR转string
std::string WChar2Ansi(LPCWSTR pwszSrc){
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
			//size_t pos = imgnames[i].find_last_of("/");
			//if (pos != std::string::npos){
			//	imgnames[i] = imgnames[i].substr(pos + 1);
			//}
			//std::cout << "vec :" << imgnames[i] << std::endl;
			i++;
		} while (FindNextFile(handle, &file));
	}
	FindClose(handle);
}

//读取发票文件裁下来的字符训练集
void getdata_(Mat &trainData, Mat &trainClasses,int samplenum,int classnum){
	Mat src_image;
	Mat prs_image;
	Mat data;
	string file;
	//int size = 32;/////////////////对于汉字 

	unsigned long n;
	trainData = Mat::zeros(samplenum, hog_feature_nums/*324*/, CV_32FC1);////////
	HOGDescriptor *hog = new HOGDescriptor(winSize /*Size(size, size)*//*winSize*/, Size(16, 16)/*blockSize*/, Size(8, 8)/*blockStride*/, Size(8, 8)/*cellSize*/, 9/*nbins*/);
	int count = 0;
	for (int i = 0; i <classnum; ++i){
		vector<string> files;
		string path_test = samples_path + to_string(i);
		//getFiles(path_test, files);
		readimgnamefromfile((path_test +"/"  ).c_str(), files);
		for (int j = 0; j < files.size(); ++j){
			src_image = imread(files[j], 0);
			if (!src_image.data){
				cout << "Error:can not open the image " << files[j] << endl;
				continue;
			}
			//threshold(src_image, src_image, 0, 255, CV_THRESH_OTSU);
			Mat img;
			resize(src_image, img, winSize /*Size(size, size)*/, 0, 0, CV_INTER_AREA);
			trainClasses.at<float>(count, 0) = i;
			vector<float>descriptors;
			hog->compute(img, descriptors, Size(1, 1)/*winStride*/, Size(0, 0) /*padding*/);		
			n = 0;
			for (vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++){
				trainData.at<float>(count, n) = *iter;
				n++;
			}
			count++;
		}
	}
}

void test_number(mysvm &svm){
	Mat test_img;
	int size = 32;
	unsigned long n;
	test_img = imread("..\\data_char\\18\\1509.jpg", 0);
	if (!test_img.data){
		printf("Error:can not open the image \n");
	}
	//threshold(test_img, test_img, 0, 255, CV_THRESH_OTSU);
	Mat img;
	resize(test_img, img, Size(size, size), 0, 0, CV_INTER_AREA);
	HOGDescriptor *hog = new HOGDescriptor(Size(size, size), cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);
	vector<float> descriptors;
	hog->compute(img, descriptors, Size(1, 1), Size(0, 0));
	Mat SVMtestMat = Mat::zeros(1, descriptors.size(), CV_32FC1);
	n = 0;
	for (vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++)
	{
		SVMtestMat.at<float>(0, n) = *iter;
		n++;
	}
	float *data = new float[descriptors.size()];
	for (int i = 0; i < descriptors.size(); i++){
		data[i] = SVMtestMat.at<float>(0, i);
	}
	double sum_ret;
	int ret = svm.predict(data, descriptors.size(),false, sum_ret);
	//if (ret <= 11){
	//	printf("%c \n", num[ret]);
	//}
	//else{
	//	int t = (ret - 12);
	//	cout << word[t * 2] << word[t * 2 + 1] << endl;;
	//}
	/*cout << ret << endl;*/
	cout << word_test[ret * 2] << word_test[ret * 2 + 1]<<"："<<sum_ret<<endl;
	delete[] data;
}

void test(mysvm &svm, Mat &img, const std::string &flg){
	unsigned long n;
	if (!img.data){
		printf("Error:can not open the image \n");
		return;
	}

	//threshold(img, img, 0, 255, CV_THRESH_OTSU);
	resize(img.clone(), img, winSize, 0, 0, CV_INTER_AREA);
	HOGDescriptor *hog = new HOGDescriptor(winSize, cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);
	vector<float> descriptors;
	hog->compute(img, descriptors, Size(1, 1), Size(0, 0));
	Mat SVMtestMat = Mat::zeros(1, descriptors.size(), CV_32FC1);
	n = 0;
	for (vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++){
		SVMtestMat.at<float>(0, n) = *iter;
		++n;
	}
	float *data = new float[descriptors.size()];
	for (int i = 0; i < descriptors.size(); i++){
		data[i] = SVMtestMat.at<float>(0, i);
	}
	double sum_ret;
	float retf = svm.predict(data, descriptors.size(), false, sum_ret);
	int ret = retf;
	if (1 + 2 * ret >= sizeof(word_test) / sizeof(word_test[0]) || ret < 0){
		std::cerr << "error ,  ret:" << ret << ", retf:" << retf << std::endl;
		ret = 0;
	}
	// if (sum_ret <= 0.6)		
	cout << word_test[ret * 2] << word_test[ret * 2 + 1] << ":" << sum_ret << ", fn:" << flg << std::endl;

	char tm[2] = { word_test[ret * 2], word_test[ret * 2 + 1] };
	std::string  word; word.assign(tm, 2);
	if (word_test[ret * 2] == 'X'){
		//cout << flg << ", p:"<< sum_ret << std::endl;
	}
#ifdef zhen_svm_
	if (word == "诊") { 
		cout << "+++++++++" << flg << std::endl; 
		std::string tt;
		size_t pos = flg.find_last_of("/");
		if (pos != std::string::npos){
			tt = flg.substr(pos + 1);
		//	imwrite("D:/zhen_may_pos/" + tt, img);
			remove(flg.c_str());
		}	
	}
#else   
	if (word != "赔") {
		cout << flg << ", ———————————p:" << sum_ret << std::endl;
		std::string tt;
		size_t pos = flg.find_last_of("/");
		if (pos != std::string::npos){
			tt = flg.substr(pos + 1);
		 	imwrite("C:/Users/Administrator/Desktop/新建文件夹 (2)/" + tt, img);
		//	remove(flg.c_str());
		}
	}
#endif
}

bool findvalidrows(vector<int> &yblank, int &rowbegin, int &rowend)
{
	bool foundit = false;
	for (size_t i = 0; i < yblank.size() - 1; i++){
		if ((yblank[i + 1] - yblank[i] > 30)&&(yblank[i + 1] - yblank[i] < 50)){
			foundit = true;
			rowbegin = yblank[i]; rowend = yblank[i + 1];
		}//valid
	}
	return foundit;
}

void widthcut(Mat &img, vector<int>&xblank){
	for (size_t i = 0; i < img.cols; i++){
		int count = 0;
		for (size_t j = 0; j < img.rows; j++){
			if (img.at<uchar>(j, i) == 255){
				count++;
			}
		}
		if (xblank.size()>1)
		{
			if (count >= img.rows - 3){
				xblank.push_back(i);
			}
		}
		else
		{
			if (count >= img.rows - 5)xblank.push_back(i);
		}
	}
}

void heightcut(Mat &img, vector<int> &yblank){
	int count = 0;
	int max = 0;
	yblank.push_back(0);
	for (size_t i = 0; i < img.rows; i++){
		count = 0;
		for (size_t j = 0; j < img.cols; j++){
			if (img.at<uchar>(i, j) == 255){
				count += 1;
			}
		}
		if (count >= img.cols - 20){
			yblank.push_back(i);
		}
	}
	yblank.push_back(img.rows);
}

bool heightcut_deep(Mat &img, int &rowbegin, int &rowend)
{
	////Mat gcimg;
	////threshold(gc.clone(), gcimg, 0, 255, CV_THRESH_OTSU);
	//imshow("instead gcimg", gc);
	//
	//Mat gcimg_part = gc(Rect(gc.cols / 5, 0, gc.cols / 3, gc.rows)).clone();
	//vector<int> eblank;
	//heightcut(gcimg_part, eblank);
	//if (findvalidrows(eblank, rowbegin, rowend))
	//{
	//	img = gc.clone();
	//	return true;
	//}
	//else
	//{
		//
		Mat img_half = img(Rect(0, 0, img.cols / 2, img.rows)).clone();
		vector<int> cblank;
		heightcut(img_half, cblank);
		if (!findvalidrows(cblank, rowbegin, rowend))
		{
			Mat img_part = img(Rect(img.cols / 5, 0, img.cols / 3, img.rows)).clone();
			vector<int> dblank;
			heightcut(img_part, dblank);
			if (!findvalidrows(dblank, rowbegin, rowend))
			{
				return false;
			}//end if not found again
			return true;
		}//end if not found
		//
		return true;
	//}//end if/else
}

void cut_image(vector<string> &files, vector<Mat> &result_img){
	Mat src_img = imread(files[0], 0);
	Mat img;
	threshold(src_img, img, 0, 255, CV_THRESH_OTSU);
	vector<int> yblank;
	heightcut(img, yblank);
	for (size_t i = 0; i < yblank.size() - 1; i++){
		if (yblank[i + 1] - yblank[i] <= 30){
			continue;
		}
		if (yblank[i + 1] - yblank[i] >= 50){
			yblank.insert(yblank.begin() + i + 1, yblank[i] + 40);
		}
		vector<int> xblank;
		Mat element = getStructuringElement(MORPH_RECT, Size(1, 1));
		Mat img_ = img(Rect(0, yblank[i], img.cols, yblank[i + 1] - yblank[i]));
		morphologyEx(img_, img_, MORPH_OPEN, element); //防止字的黑点太稀疏
		widthcut(img_, xblank);
		if (xblank.size() == 0) {
			return;
		}
		for (size_t j = 0; j < xblank.size() - 1; j++){
			if (xblank[j + 1] - xblank[j] <= 10){
				continue;
			}
			if (xblank[j + 1] - xblank[j] >= 45){
				int tmp = (xblank[j + 1] - xblank[j]) / 38;
				for (int i = 0; i < tmp; i++){
					xblank.insert(xblank.begin() + j + 1, xblank[j] + 38);
				}
			}
			Mat ret = src_img(Rect(xblank[j], yblank[i], xblank[j + 1] - xblank[j], yblank[i + 1] - yblank[i]));
			result_img.push_back(ret);
			/*	rectangle(src_img, Point(xblank[j], yblank[i]), Point(xblank[j + 1], yblank[i + 1]), Scalar(0, 0, 0), 1, 8);*/
		}
	}
}

//cut_image函数重载，差别是传入的参数不同，函数内容基本相同
void cut_image(Mat &img, vector<Mat> &result_img){
	Mat dst = img.clone();
	double threshreturn = threshold(img.clone(), img, 0, 255, CV_THRESH_OTSU);
	//
	//imshow("otsu cut_img", img);
	//
	vector<int> yblank;
	heightcut(img, yblank);
	if (yblank.size() == 0) {return;}
	int rowbegin = 0, rowend = img.rows;
	bool foundit = findvalidrows(yblank, rowbegin, rowend);
	if (!foundit)
	{ 
		if (threshreturn >120)threshreturn = 120;
		threshold(dst.clone(), img, threshreturn, 255, CV_THRESH_BINARY);
		foundit = heightcut_deep(img, rowbegin, rowend); 
	}
	if (!foundit)
	{
		for (size_t i = 0; i < yblank.size() - 1; i++){
			if (yblank[i + 1] - yblank[i] <= 30){
				continue;
			}
			if (yblank[i + 1] - yblank[i] >= 50){
				//yblank.insert(yblank.begin() + i + 1, yblank[i] + 40);
				rowbegin = yblank[i]; rowend = yblank[i] + 40;
				foundit = true;
			}
		}
	}

	//
	if (foundit){
		vector<int> xblank;
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));//better than size(1,1)
		Mat img_ = img(Rect(0, rowbegin, img.cols, rowend - rowbegin));
		morphologyEx(img_, img_, MORPH_OPEN, element); //防止字的黑点太稀疏
		imshow("morphexopen img", img_);
		widthcut(img_, xblank);
		if (xblank.size() == 0) {
			return;
		}
		int widthminlow = (rowend - rowbegin)*0.7,widthminhigh = (rowend - rowbegin)*0.8;
		if (widthminlow < 20)widthminlow = 20;
		if (widthminhigh < 20)widthminhigh = 20;
		for (size_t j = 0; j < xblank.size() - 1; j++){
			if (xblank[j + 1] - xblank[j] <= widthminhigh){
				int tmpblackpixelnum = sum(255-img_(Rect(xblank[j], 0, xblank[j + 1] - xblank[j], rowend-rowbegin))).val[0]/255;
				if (tmpblackpixelnum < 10)
				{
					continue;
				}
				else
				{
					bool havenewmerge;
					int fullwhiteconcolnum = 0;
					do
					{
						havenewmerge = false;
						if (j + 2 < xblank.size())
						{
							int tmpblackpixelnum2 = sum(255 - img_(Rect(xblank[j + 1], 0, xblank[j + 2] - xblank[j + 1], rowend-rowbegin))).val[0] / 255;
							if (tmpblackpixelnum2 == 0)fullwhiteconcolnum += xblank[j + 2] - xblank[j + 1];
							if (tmpblackpixelnum2 > 0)fullwhiteconcolnum = 0;
							//if (((fullwhiteconcolnum<5)||(xblank[j+1]-xblank[j]<widthmin)) && (xblank[j + 2] - xblank[j] < 45))
							if ((fullwhiteconcolnum<5) && (xblank[j + 2] - xblank[j] < 45))
							{
								xblank.erase(xblank.begin() + j + 1);
								havenewmerge = true;
							}
						}
					} while ((xblank[j + 1] - xblank[j] <= widthminhigh)&&(havenewmerge));
					if (xblank[j + 1] - xblank[j] <= widthminlow)continue;
				}//end if/else
			}
			//一个字的宽度大约是38像素
			if (xblank[j + 1] - xblank[j] >= 45){
				int tmp = round(((double)(xblank[j + 1] - xblank[j])) / 38.0);
				int gap = (xblank[j + 1] - xblank[j]) / tmp;
				if (gap > 38 * 1.5)gap = 38;
				xblank.insert(xblank.begin() + j + 1, xblank[j] + gap);//only need cut one character, others remains for next loop
			}
			Mat ret = dst(Rect(xblank[j], rowbegin, xblank[j + 1] - xblank[j], rowend-rowbegin));
			result_img.push_back(ret);
			rectangle(dst, Point(xblank[j], rowbegin), Point(xblank[j + 1], rowend), Scalar(0, 0, 0), 1, 8);
		}//end for j
	}//end for i
	imshow("rectangle dst", dst);
}

//去除横贯文字的直线
Mat erase_redline(vector<string> &files, vector<Mat> &result_img, int index){
	Mat src_img = imread(files[index]);
	//Mat color_img = src_img.clone();//to be continued
	cvtColor(src_img.clone(), src_img, CV_BGR2GRAY);
	//imshow("1", src_img);
	Mat dst;
	dst.create(src_img.rows, src_img.cols, CV_8UC1);
	dst.setTo(0);
	Mat canny_img;
	//Canny(src_img, canny_img, 80, 100, 3);
	Canny(src_img, canny_img, 32, 80, 3);
	//imshow("canny edge", canny_img);
	vector<Vec4i> lines;
	//HoughLinesP(canny_img, lines, 1, CV_PI / 180, 80, 20, 30);
	HoughLinesP(canny_img, lines, 1, CV_PI / 360.0, 180, 100, 30);
	if (lines.size() == 0)
	{
		//cut_image(src_img, result_img);
		return src_img;
	}
	for (size_t i = 0; i < lines.size(); i++){
		Vec4i l = lines[i];
		line(dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);
	}
	//imshow("line dst", dst);
	Mat img;
	img.create(src_img.rows, src_img.cols, CV_8UC1);
	threshold(src_img, img, 0, 255, CV_THRESH_OTSU);
	for (int row = 0; row < img.rows; row++){
		for (int col = 0; col < img.cols; col++){
			if ((dst.at<uchar>(row, col)) != 0){
				int up = 0, down = 0;
				int white = 0;
				for (int i = row; i >= 0; i--)
				{
					if ((img.at<uchar>(i, col)) == 0){
						up++;
						white = 0;
					}
					else
						white++;
					if (white>2){
						break;
					}
				}
				white = 0;
				for (int i = row; i < img.rows; i++){
					if ((img.at<uchar>(i, col)) == 0){
						down++;
						white = 0;
					}
					else
						white++;
					if (white>2){
						break;
					}
				}
				if (up + down < 8)
				{
					for (int i = -up; i <= down && (row + i<img.rows) && (row + i >= 0); i++){
						src_img.at<uchar>(row + i, col) = 136; //136是因为这个值和背景色接近
					}
				}
			}
		}
	}
	//cut_image(src_img, result_img);
	return src_img;
}

//尝试去除颜色，失败
void erase_color(vector<string> &files){
	Mat src_img = imread(files[3]);
	Mat img;
	cvtColor(src_img, img, CV_BGR2HSV);
	vector<Mat> channels;
	//Mat ret = img(Rect(195, 45, 20, 15));
	split(src_img, channels);
	for (int i = 1; i < img.rows - 1; i++){
		for (int j = 1; j < img.cols - 1; j++){
			if ((channels[2].at<uchar>(i, j) <= 105) && (channels[2].at<uchar>(i, j) >= 90))
			{
				/*img.at<Vec3b>(i, j)[0] = 22;
				img.at<Vec3b>(i, j)[1] = 25;
				img.at<Vec3b>(i, j)[2] = 153;*/
				channels[0].at<uchar>(i, j) = 255;
				channels[1].at<uchar>(i, j) = 255;
				channels[2].at<uchar>(i, j) = 255;
			}
		}
	}
	merge(channels, src_img);
	/*cvtColor(src_img, src_img, CV_BGR2GRAY);
	threshold(src_img, src_img, 120, 255, CV_THRESH_OTSU);*/
	imshow("1", src_img);
}

///  train   
void train_main(int samplenum,int classnum){
	Mat trainData;
	Mat trainClasses;
	trainClasses.create(samplenum, 1, CV_32FC1);// how many samples
	printf("\ngetdata...\n");

	getdata_(trainData, trainClasses,samplenum,classnum);
	//
	CvSVM svm;
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::RBF;//c=12.5 p=0 gamma=0.506250 for hjdx
	params.C = 2.500000;  // 损失函数
	params.p = 0;  //设置e-SVR中损失函数值  
	params.gamma = 0.506250; // 0.506250; 针对多项式、rbf、sigmoid核函数设置参数 
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 2000, 5e-3);////SVM训练终止条件  
	//params.term_crit = cvTermCriteria(CV_TERMCRIT_EPS, 100, 5e-3);
	//params.class_weights         classWeights; // C-SVC权重  
	/*
	class_weights:C_SVC中的可选权重，赋给指定的类，乘以C以后变成class_weightssi*C。这些权重影响不同类别的错误分类惩罚项。权重越大，某一类的误分类数据的惩罚项就越大。
    term_crit:SVM的训练过程的终止条件，解决部分受约束二次最优问题。可以指定公差和/或最大迭代次数。
	*/
	//对不用的参数step设为0  
	CvParamGrid nuGrid = CvParamGrid(1, 1, 0.0);
	CvParamGrid coeffGrid = CvParamGrid(1, 1, 0.0);
	CvParamGrid degreeGrid = CvParamGrid(1, 1, 0.0);

	printf("\nbegin training...\n");
	time_t start, stop;
	start = time(NULL);

	//svm.train(trainData, trainClasses, Mat(), Mat(), params);
	//
	 svm.train_auto(trainData, trainClasses, Mat(), Mat(), params, 10, svm.get_default_grid(CvSVM::C), svm.get_default_grid(CvSVM::GAMMA), svm.get_default_grid(CvSVM::P), nuGrid, coeffGrid, degreeGrid);//true is two-class probelm
	 
	 stop = time(NULL);
	 printf("Use Time:%ld(s)\t %f\n", (stop - start), (stop - start)/3600.0);	
	 svm.save(svm_file.c_str());
	 printf("end training.\n");

	//end training
	//output optimal params
	CvSVMParams params_re = svm.get_params();
	float C = params_re.C;
	float P = params_re.p;
	float gamma = params_re.gamma;
	
	//////Parms: C = 312.500000, P = 0.000000,gamma = 0.002250  8 classes for all word
	//////Parms: C = 12.500000, P = 0.000000,gamma = 0.506250   2 class  for  shang  0804
	//////Parms: C = 12.500000, P = 0.000000,gamma = 0.506250    2 class  for  zhen  0807
	//////Parms: C = 312.500000, P = 0.000000,gamma = 0.033750   2 class  for  ju  0808
	//////Parms: C = 12.500000, P = 0.000000,gamma = 0.033750    2 class  for  bankc  0817
	//////Parms: C = 312.500000, P = 0.000000,gamma = 0.002250  2 class  for idc  0818
	//////Parms: C = 12.500000, P = 0.000000,gamma = 0.033750
	//////Parms: C = 2.500000, P = 0.000000,gamma = 0.506250    9 classes
	//////Parms: C = 12.500000, P = 0.000000,gamma = 0.506250   2 class  for  shang  0822
	//////Parms: C = 2.500000, P = 0.000000,gamma = 0.506250    2 class  for  shang  0825
	printf("\nParms: C = %f, P = %f,gamma = %f \n", C, P, gamma);//c=12.5 p=0 gamma=0.506250 for hjdx train_auto
	//
}

static void modifybrightness(Mat &src){
	if (src.channels() > 1){
		cvtColor(src, src, COLOR_BGR2GRAY);
	}

	Scalar avgbrightness = mean(src);
	convertScaleAbs(src, src, 128 / avgbrightness.val[0], 0);///128
}

#define TRAIN_ONf
int main9994(){
	char   buffer[256];
	_getcwd(buffer, 256);
	printf("The   current   directory   is:   %s ", buffer);

#ifdef TRAIN_ON
	//训练SVM模型
	/*
	int samplenum =230 + 223 + 222+  +217 + 216*3 + 1419;// 8;   data3
	//int samplenum = 409 + 407 + 406 + 216 * 2 + +414 + 400+ 1434;// 8;   data4
	int classnum = 8; //20;
	*/

	//int samplenum = 230 + 223 + 223 + +217 + 216 * 3 +223 + 1594;// 8;   data3
	//int classnum = 9; //20;

	//int samplenum = 1519 + 7583;// 2;   data3   SHANG
	//int classnum = 2; 

	//int samplenum = 1600 + 7635;// 2;   data3   SHANG 0822
	//int classnum = 2; 

	//int samplenum = 2355 + 7338;// 2;   data3   SHANG 0824
	//int classnum = 2;

	//int samplenum = 1915 + 7997;// 2;   data3   ZHEN 
	//int classnum = 2; 

	//int samplenum = 2753 + 11540;// 2;   data3    JU
	//int classnum = 2;

	//int samplenum = 1717 + 6813;// 2;   data3    BANKC
	//int classnum = 2;

	//int samplenum = 2851 + 6870;// 2;   data3    IDC
	//int classnum = 2;

	int samplenum = 2685 + 10000;// 2;   data3   pei 0904
	int classnum = 2;

	train_main(samplenum, classnum);

	//以上用来训练SVM模型,如果模型已经训练好，可以直接读取模型来进行测试。
#else
	mysvm svm;
	svm.clear();

	FileStorage svm_fs(svm_file, FileStorage::READ);
	if (svm_fs.isOpened()){
		svm.load(svm_file.c_str());
	}

	vector<string> files;
	string path_test = "E:/新建文件夹/";
	//string path_test = "D:/adaboost_train_10_lp2/test_etc/opencv249_prj/neg_p/";////pei
	//string path_test = "D:/adaboost_train_2_ju_all_svm_train/test_etc/svm_2_classes_pei_train/data3/0/"; //"D:/adaboost_train_8_sfz/test_etc/opencv249_prj/pos_3/";//"D:/ju_neg_svm/";//"D:/adaboost_train_7_yang/test_etc/opencv249_prj/neg/" ;//"D:/addr/" ;// /* "D:/adaboost_train_6_jing/test_etc/opencv249_prj/neg/";/ */"D:/adaboost_train_6_jing/test_etc/opencv249_prj/pos/";
	//string path_test = "D:\\adaboost_train_ju\\test_etc\\data3\\7/";
	readimgnamefromfile((path_test ).c_str(), files);

	for (int i = 0; i < files.size(); ++i){
		Mat testimg = imread(files[i], 0);////////////////
		//modifybrightness(testimg);
		test(svm, testimg, files[i]);
	}
#endif
	
	system("pause");
	return 0;
}

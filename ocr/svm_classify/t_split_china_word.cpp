
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
#include  <set>
#include <io.h>

#include <fstream> 

using namespace cv;
using namespace std;

long long icnt = 0;
static void readimgnamefromfile(const char* fileName, std::vector<std::string> &imgnames);
static void pospreprecess3(Mat &src, Rect &re, const int &val);

struct _xrect{
	_xrect(int xstart  = 0, int xend= 0) :_xstart(xstart), _xend(xend){
	}

	int _xstart;
	int _xend;

	bool operator < (const _xrect& other) const{
		return this->_xstart < other._xstart;
	}

	bool operator == (const _xrect& other) const{
		return (this->_xstart == other._xstart && this->_xend == other._xend);
	}	

	int getweight() const{
		return _xend - _xstart;
	}

	 _xrect& operator=(const _xrect& other) {
		if (this != &other)	{
			this->_xstart = other._xstart;
			this->_xend = other._xend;
		}
		return *this;
	}
};

struct _rect{
	_rect(int x, int y, int w, int h) :_x(x), _y(y), _w(w), _h(h){
	}
	int _x; 
	int _y; 
	int _w;
	int _h;

	bool operator < (const _rect& other) const{
		return this->_x < other._x;
	}

	bool operator == (const _rect& other) const	{
		return (this->_x == other._x && this->_y == other._y && this->_w == other._w && this->_h == other._h);
	}
	
	int getrightx()   const{
		return _x + _w;
	}
};

static void modifybrightness(Mat &src){
	if (src.channels() > 1){
		cvtColor(src, src, COLOR_BGR2GRAY);
	}

	Scalar avgbrightness = mean(src);
	convertScaleAbs(src, src, 255 / avgbrightness.val[0], 0);/// or 300
}

static void modifybrightness2(Mat &src){
	if (src.channels() > 1){
		cvtColor(src, src, COLOR_BGR2GRAY);
	}

	Scalar avgbrightness = mean(src);
	convertScaleAbs(src, src, 128 / avgbrightness.val[0], 0);/// or 300
}

Mat gethorprojimg(const Mat &image, const int &value, std::vector<int> &hproj){
	const int height = image.rows, width = image.cols;
	int tmp = 0;

	for (int i = 0; i < height; ++i){
		tmp = 0;
		for (int j = 0; j < width; ++j){
			if (image.at<uchar>(i, j) == value){
				++tmp;
			}
		}
		hproj.push_back(tmp);
	}

	Mat projimg(height, width, CV_8U, Scalar(255));
	for (int i = 0; i < height; ++i){
		line(projimg, Point(width - hproj[i], i), Point(width - 1, i), Scalar(0));
	}
	return  projimg;
}

Mat getverprojimg(const Mat &image, const int &value, std::vector<int> &hproj){
	Mat matTmp = image.clone();

	int maxCol = 0, maxNum = 0;
	int minCol = 0, minNum = matTmp.rows;
	int height = matTmp.rows, width = matTmp.cols;
	int tmp = 0;

	for (int col = 0; col < width; ++col){
		tmp = 0;
		for (int row = 0; row < height; ++row){
			if (matTmp.at<uchar>(row, col) == value)	{
				++tmp;
			}
		}
		hproj.push_back( tmp);
		if (tmp > maxNum){
			maxNum = tmp;
			maxCol = col;
		}
		if (tmp < minNum){
			minNum = tmp;
			minCol = col;
		}
	}

	Mat projImg(height, width, CV_8U, Scalar(0));
	for (int col = 0; col < width; ++col)
	{
		line(projImg, Point(col, height - hproj[col]), Point(col, height - 1), Scalar(255));
	}
	return  projImg;
}

void filterpoints(Mat foremat, const int &winsize, const int &countthresh, Mat &afterfilter){
	afterfilter = foremat.clone();
	for (int i = winsize; i < foremat.rows - winsize; i++)//row,y,height
	{
		for (int j = winsize; j < foremat.cols - winsize; j++)//col,x,width
		{
			if (foremat.at<uchar>(i, j) == 0){
				Scalar tempsum =  sum(foremat(Rect(j - winsize, i - winsize, 2 * winsize, 2 * winsize)));
				if (tempsum.val[0] < countthresh) 
					afterfilter.at<uchar>(i, j) = 255;////0
			}
		}
	}
}

void mergeareahori(Mat &src, const int &step){
	const int fc = 0, bc = 255 - fc;
	const int col_end = src.cols - step - 1;
	for (int i = 0; i<src.rows; ++i){
		for (int j = 0; j<col_end; ++j){
			if (src.ptr<uchar>(i)[j] == fc && src.ptr<uchar>(i)[1 + j] == bc && src.ptr<uchar>(i)[step + j] == fc){
				for (int k = 1; k<step; ++k){
					src.ptr<uchar>(i)[k + j] |= fc;
				}

				j += step;
			}
		}
	}
}

int splitkerl2(const std::string &input_file){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx =0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx] );
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}

	 	////imshow("raw", srcimg);

		Mat src;
		medianBlur(srcimg, src, 5);
		////imshow("rawmb", srcimg);

		std::vector<Mat> bgrch3;
		split(src, bgrch3);
		Mat rgdiff = bgrch3[2] - bgrch3[1];
	 	////imshow("rgdiff", rgdiff);
		//////imshow("r", bgrch3[2]);
		//////imshow("g", bgrch3[0]);

		modifybrightness(rgdiff);
	 	////imshow("rgdiff_mbri", rgdiff);

		threshold(rgdiff, rgdiff, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
	 	////imshow("rgdiff_bin", rgdiff);

		//
		Mat  rgdiffdilate; //= rgdiff;
		Mat element = getStructuringElement(0, Size(4, 1));
		erode(rgdiff, rgdiffdilate, element); ///dilate   erode
		 ////imshow("rgdiffbin_dilate", rgdiffdilate);//////////////////////////
	
		Mat gray;
		cvtColor(src, gray, COLOR_BGR2GRAY);
		 ////imshow("gray", gray);

		Mat graybin, grayblurgau;
			
		GaussianBlur(bgrch3[2], grayblurgau, Size(5, 5), 0, 0);		
		////imshow("grayblur_gau", grayblurgau);

		threshold(grayblurgau, graybin, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);		
		////imshow("gray_bin", graybin);
				
		Mat dst = rgdiffdilate & graybin;
		////imshow("last_dst", dst);

		const int offset = 3;
		Mat topre = dst(Rect(0, 0, dst.cols, offset));// x  y  w  h
		topre &= 0;

		Mat botre = dst(Rect(0, dst.rows - offset, dst.cols, offset));// x  y  w  h
		botre &= 0;

		Mat lefre = dst(Rect(0, offset, offset, dst.rows - offset*2));// x  y  w  h
		lefre &= 0;

		Mat rigre = dst(Rect(dst.cols - offset, offset, offset, dst.rows - offset*2));// x  y  w  h
		rigre &= 0;

		vector<vector<Point> > contours;
		findContours(dst.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//
		for (int i = contours.size() - 1; i >= 0; i--){
			Rect tmprect = boundingRect(contours[i]);
			rectangle(dst, tmprect, Scalar(128), 1, 8, 0);
			if (tmprect.height <= 3 || tmprect.width <=3){
				dst(tmprect) &= 0;
			}
		}		
		
		////imshow("last_dst_roi_xxx", dst);
		Mat afterfilter;
		filterpoints(dst, 3, 2*255, afterfilter);
		////imshow("last_dst_fil", afterfilter);

    	Rect re;
		pospreprecess3(afterfilter, re, 255);
		rectangle(srcimg, re, Scalar(255, 0, 0), 2, 8, 0);
		////imshow("last_dst_roi", srcimg);		waitKey();

	    //waitKey();		destroyAllWindows();



		imwrite("tmp_word/" + std::to_string(icnt++) + "dilate.jpg", srcimg(re));  continue;

	
		///////////step 2
		Mat roi = bgrch3[2](re).clone(); //gray   bgrch3[2]
		//////imshow("roi", roi);		

		resize(roi, roi, Size(roi.cols * 2, roi.rows * 2));
		Mat roibin;
		threshold(roi, roibin, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
		//////imshow("equbin", roibin);

		Mat roibinero;
		element = getStructuringElement(0, Size(1, 3));
		dilate(roibin, roibinero, element); ///dilate
		//////imshow("roibinero", roibinero);

		for (int i = 0; i < 6; ++i){
			mergeareahori(roibinero, i);
		}
		//////imshow("roibinerohori", roibinero);

		std::vector<int> projvec;
		Mat hproj = gethorprojimg(roibinero, 255, projvec);
		//////imshow("hproj", hproj);

		for (int i = 0; i < projvec.size(); ++i){///////from top to bot 
			if (projvec[i] >= 0.7*roibinero.cols){//has line   set white
				Rect re(0, i, roibinero.cols,1);
				roibinero(re) &= 0;
			}
		}
		//////imshow("equerodelline", roibinero);

		contours.clear();
		findContours(roibinero.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//
		for (int i = contours.size() - 1; i >= 0; i--){
			Rect tmprect = boundingRect(contours[i]);
			rectangle(roibinero, tmprect, Scalar(128), 1, 8, 0);
			if (tmprect.height <= 3 || tmprect.width <= 3){
				roibinero(tmprect) &= 0;
			}
			else {
			//	roibinero(tmprect) |= 255;///fill  the area 
			}
		}
		
		//////imshow("equerodel", roibinero);

		projvec.clear();
		Mat vproj = getverprojimg(roibinero, 255, projvec);
		//////imshow("vproj", vproj);

		///
		std::vector<_xrect> merge_set;
		bool is_find_x_xtart = false;
		_xrect  xtmp;
		for (int i = 0; i < vproj.cols; ++i){
			if (!is_find_x_xtart && vproj.at<uchar>(vproj.rows - 2, i) == 255){
				xtmp._xstart = i;
				is_find_x_xtart = true;
			}
			else if (is_find_x_xtart && (vproj.at<uchar>(vproj.rows - 2, i) == 0 || i + 1 == vproj.cols)) {
				xtmp._xend = i;
				is_find_x_xtart = false;
				int len = merge_set.size()-1;
				if (len >= 0 && (xtmp._xstart - merge_set[len]._xend >= 38 * 4 && merge_set[len].getweight() < 50)  ){
					merge_set[len] = xtmp;
				}
				else if (len >= 0 && (xtmp._xstart - merge_set[len]._xend <= 3 || merge_set[len].getweight() <= 18 && xtmp.getweight() <50 )){
					merge_set[len]._xend = xtmp._xend;
				}				 
				else{
					merge_set.push_back(xtmp);
				}
			}
		}

		std::string path = "./" + imgnames[idx];
		int ret = _access(path.c_str(), 0);
		if (ret != 0)
		{
			ret = _mkdir(path.c_str());
			if (ret != 0)
			{
				std::cout << "_mkdir error !" << ", new_path:" << path << std::endl;
				return 1;
			}
		}

		std::vector<Rect> res_rect;
		for (int i = 0; i < merge_set.size(); ++i){/////calc each area height  
			line(vproj, Point(merge_set[i]._xstart, 0), Point(merge_set[i]._xstart, vproj.rows), Scalar(128));
			line(vproj, Point(merge_set[i]._xend, 0), Point(merge_set[i]._xend, vproj.rows), Scalar(128));
			//res_rect.push_back();
			Rect re(merge_set[i]._xstart, 0, merge_set[i].getweight(), roibinero.rows);
			imwrite(path+ "/"+std::to_string(i++) + ".jpg", roi(re));
		}
		//////imshow("vprojppp", vproj);

		////
	}

	return 0;
}

int splitkerl(const std::string &input_file){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx]);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}

		//imshow("raw", srcimg);
		waitKey();

		Mat src;
		medianBlur(srcimg, src, 5);
		//imshow("srcblur", src);
		waitKey();

		std::vector<Mat> bgrch3;
		split(src, bgrch3);
		Mat rgdiff = bgrch3[2] - bgrch3[1];
		//imshow("rgdiff", rgdiff);
		waitKey();

		modifybrightness2(rgdiff);
		//imshow("rgdiffmb", rgdiff);
		waitKey();

		threshold(rgdiff, rgdiff, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
		//imshow("rgdiff_bin", rgdiff);
		waitKey();

		//
		Mat  rgdiffdilate; //= rgdiff;
		Mat element = getStructuringElement(0, Size(5, 2));
		dilate(rgdiff, rgdiffdilate, element);
		//imshow("rgdiffbin_dilate", rgdiffdilate);
		waitKey();

		Mat gray;
		cvtColor(src, gray, COLOR_BGR2GRAY);
		//imshow("gray", gray);
		waitKey();

		Mat grayblur;
		medianBlur(gray, grayblur, 5);
		threshold(grayblur, gray, 0, 255, CV_THRESH_BINARY  + CV_THRESH_OTSU);		////////imshow("gray_bin", gray);

		medianBlur(gray, grayblur, 5);		////////imshow("gray_bin_blur", grayblur);

		////top   bot  left  right
		//Mat toproi = grayblur(Rect(0, 0, grayblur.cols, 10));
		//Mat elementt = getStructuringElement(0, Size(7, 7));
		//dilate(toproi, toproi, elementt);

		//Mat botroi = grayblur(Rect(0, gray.rows-10, grayblur.cols,10));
		//dilate(botroi, botroi, elementt);

		//Mat leftroi = grayblur(Rect(0, 10, 10, gray.rows -20));
		//dilate(leftroi, leftroi, elementt);

		//Mat rightroi = grayblur(Rect(grayblur.cols - 10, 10, 10, gray.rows - 20 ));
		//dilate(rightroi, rightroi, elementt);
		//////////imshow("gray_bin_dli", grayblur);

		Mat dst = rgdiffdilate | grayblur;
		//imshow("last_dst", dst);  waitKey();

		for (int i = 0; i < 25; ++i){
			mergeareahori(dst, i);
		}
		//imshow("mergeareahori", dst);  waitKey();


//		Mat afterfilter;
	//	filterpoints(dst, 10, 4 * 255, afterfilter);
	//	//imshow("afterfilter", afterfilter);  
//		waitKey();

		Rect re;
		pospreprecess3(dst, re, 0);
		rectangle(srcimg, re, Scalar(255, 0, 0), 2, 8, 0);
		//imshow("last_dst_roi", srcimg);

		 waitKey();		destroyAllWindows();
		//imwrite("tmp_word/"+std::to_string(icnt++)+".jpg", srcimg(re));
	}

	return 0;
}

//fix right mid pos 
int splitkerl3(const std::string &input_file, const std::string &out_path){
	const int uni_w = 32*12, uni_h = 32;////uniform size  need modify here !

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx]);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}

		////imshow("raw", srcimg);
		Mat src = srcimg;
		//medianBlur(srcimg, src, 3);
		////imshow("srcblur", src);

		std::vector<Mat> bgrch3;
		split(src, bgrch3);// b0    g1    r2
		Mat bdst;
		threshold(bgrch3[0], bdst, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
	//	//imshow("bdst", bdst);

		//
		Mat gdst;
		threshold(bgrch3[1], gdst, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
	//	//imshow("gdst", gdst);

		//Mat gray;
		//cvtColor(src, gray, COLOR_BGR2GRAY);
		////imshow("gray", gray);

		Mat bdst_u;
		//resize(bdst, bdst_u, Size(uni_w, uni_h), 0, 0, 3);
		Mat gdst_u;
		//resize(gdst, gdst_u, Size(uni_w, uni_h), 0, 0, 3);

		////////////////////////////	////////////////////////////
		if (srcimg.rows > uni_h){
			int new_w = uni_h * (srcimg.cols / double(srcimg.rows));
			std::cout << "1 nw:" << new_w << std::endl;
			if (new_w <= uni_w){
				resize(gdst, gdst_u, Size(new_w, uni_h), 0, 0, 3);
				resize(bdst, bdst_u, Size(new_w, uni_h), 0, 0, 3);
			}
			else{
				new_w = (uni_h-4) * (srcimg.cols / double(srcimg.rows));
				std::cout << "2 nw:" << new_w << std::endl;

				resize(gdst, gdst_u, Size(new_w, uni_h), 0, 0, 3);
				resize(bdst, bdst_u, Size(new_w, uni_h), 0, 0, 3);
			}
		}
		else if (srcimg.cols > uni_w) {
			int new_h = uni_w * (srcimg.rows / double(srcimg.cols));
			std::cout << "3 nh:" << new_h << std::endl;
			resize(gdst, gdst_u, Size(uni_w, new_h), 0, 0, 3);
			resize(bdst, bdst_u, Size(uni_w, new_h), 0, 0, 3);
		}
		else{
			gdst_u = gdst;
			bdst_u = bdst;
		}

		if ( !(gdst_u.cols <= uni_w && gdst_u.rows <= uni_h) ){
			std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! w*h:" << gdst_u.cols <<"\t"<< gdst_u.rows << std::endl;
			continue;
		}

		Mat uniformmat = Mat(uni_h, uni_w, bdst.type(), Scalar(0));
		Mat uniformmat_g= uniformmat.clone();
		Mat roi = uniformmat(Rect((uni_w - gdst_u.cols) >> 1, (uni_h - gdst_u.rows) >> 1, gdst_u.cols, gdst_u.rows));////x  y 
		Mat roi_g = uniformmat_g(Rect((uni_w - gdst_u.cols) >> 1, (uni_h - gdst_u.rows) >> 1, gdst_u.cols, gdst_u.rows));////x  y 

		bdst_u.copyTo(roi);  //////////b  branch
		gdst_u.copyTo(roi_g);/////////g 

		std::string name;
		auto it = imgnames[idx].find_last_of('.');
		if (it != std::string::npos){
			name = imgnames[idx].substr(0, it);
		}

		////imshow("bgr", srcimg);
		////imshow("b", uniformmat);
		////imshow("g", uniformmat_g);
		//waitKey();
		//destroyAllWindows();

		imwrite(out_path + imgnames[idx], srcimg);
		imwrite(out_path + name + "__b.jpg", uniformmat);
		imwrite(out_path + name + "__g.jpg", uniformmat_g);
	}

	return 0;
}

//split 32*32 word  yblx  single
int splitkerl4(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}

		////imshow("raw", srcimg);		waitKey();

		Mat srct = srcimg;
		Mat src;
		GaussianBlur(srct, src, Size(3, 3), 0);
		////imshow("src", src);		waitKey();

		modifybrightness2(src);
	//	//imshow("srct", src);		waitKey();
	//	destroyAllWindows();
	//	imwrite(out_path + imgnames[idx], src);

		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		
		for (int ci = 0; ci < src.cols-32; ci += 32){
			Mat each_word = src(Rect(ci, 0, 32, src.rows) );//x y w h
			imwrite(out_path + imgnames[idx]+"_____" + std::to_string(ci / 32) + ".jpg", each_word);
		}

		////imshow("bgr", srcimg);
		////imshow("b", uniformmat);
		////imshow("g", uniformmat_g);
		//waitKey();
		//destroyAllWindows();
	}

	return 0;
}

//for fei ada pos
int splitkerl5(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;

		const int offset = 1;
		Rect re(offset, offset, srcimg.cols - offset, srcimg.rows - offset);//x y w h 
		Mat srct = srcimg(re);
		Mat src;
		resize(srct, src, Size(srcimg.cols, srcimg.rows), 0, 0, 3);
		imwrite(out_path +std::to_string(icnt++)+".jpg"/* imgnames[idx]*/, src);
	}
	//1523

	return 0;
}

//for fei ada pos
int splitkerl6(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		Mat src;
		resize(srcimg, src, Size(32, 32), 0, 0, 3);
		imwrite(out_path + std::to_string( 1547+ icnt++) + ".jpg"/* imgnames[idx]*/, src);
	}
	//1523

	return 0;
}

//for fei ada neg
int splitkerl7(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;

		Mat src;
		int new_w = 34 * (srcimg.cols / double(srcimg.rows));///// w = 1652/2 = 826   身份证
		resize(srcimg, src, Size(new_w,  34), 0, 0, 3);
		imwrite(out_path + std::to_string(icnt++) + ".jpg"/* imgnames[idx]*/, src);
	}
	//1523

	return 0;
}

//for fei ada neg
int adaboostcascade_fei(const std::string &input_file){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	CascadeClassifier cascade("D:/adaboost_train_11_fei/classifier_fei2.xml");
	if (cascade.empty()){
		std::cout << "cascade error!" << std::endl;
		return 1;
	}

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << "imread . i:" << idx << ", fname:" << imgnames[idx] << std::endl;

		Mat src= srcimg;
		int new_w = 32*1 * (srcimg.cols / double(srcimg.rows));///// w = 1652/2 = 826   身份证
		resize(srcimg, src, Size(new_w, 32 * 1));

		Mat coarse = Mat(src.rows * 1.5, src.cols * 1.5, src.type(), Scalar(128));///////////
		Mat mid = coarse(Rect((coarse.cols - src.cols) >> 1, (coarse.rows - src.rows) >> 1, src.cols, src.rows));////x  y 
		src.copyTo(mid);  //////////b  branch

		//detect
		vector<Rect> objects;
		Mat roi = coarse;//.clone();

		modifybrightness2(roi);
		cascade.detectMultiScale(roi, objects, 1.1, 1, 1, Size(10, 14), Size(70, 74));/// 

		std::cout << "beat num:" << objects.size() << std::endl;
		if (objects.empty()){
			//imwrite("d:/123/" + imgnames[idx], src);
			//continue;
		}

		for (int i = 0; i < objects.size(); ++i){
			rectangle(roi, objects[i], Scalar(0), 2, 8, 0);
			std::cout << "w*h:" << objects[i].width << "\t" << objects[i].height << std::endl;
		}

		//namedWindow("result", 0);
		//resizeWindow("result", src.cols , src.rows);
		imshow("result", roi);
		waitKey();
		destroyAllWindows();
	}

	return 0;
}

static void pospreprecess3(Mat &src, Rect &re, const int &val){
	Mat  sample_mat = src.clone();

	int t_idx = 0, b_idx = sample_mat.rows - 1, l_idx = 0, r_idx = sample_mat.cols - 1;
	for (; t_idx < sample_mat.rows >> 1; ++t_idx){//y    top    0 ~ 0.5h 
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(t_idx)[j] == val){
				goto out_t;
			}
		}
	}	out_t:;

	for (; b_idx>sample_mat.rows >> 1; --b_idx){//y  bottom    
		for (int j = 0; j < sample_mat.cols; ++j){
			if (sample_mat.ptr<uchar>(b_idx)[j] == val){
				goto out_b;
			}
		}
	}	out_b:;

	for (; l_idx < sample_mat.cols ; ++l_idx){ //  x   left 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[l_idx] == val){
				goto out_l;
			}
		}
	}	out_l:;

	for (; r_idx > 0 ; --r_idx){ //  x   right 
		for (int i = 0; i < sample_mat.rows; ++i){ ////
			if (sample_mat.ptr<uchar>(i)[r_idx] == val){
				goto out_r;
			}
		}
	}	out_r:;

	// x,y , w , h 
	re = Rect(l_idx - 7, t_idx - 2, r_idx - l_idx + 8, b_idx - t_idx + 4 +2);
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
	if (re.width < 0){
		re.width = 0;
	}
	if (re.height < 0){
		re.height = 0;
	}
	//rectangle(sample_mat, re.tl(), re.br(), Scalar(128, 128, 128), 1, 8, 0);
	//imwrite(tem_path + "s4_uniform" + to_string(i) + ".jpg", sample_mat);	 

	// rect roi 
	//rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 1, 8, 0);
	//imwrite(tem_path + "s5_uniform" + to_string(i) + ".jpg", src(re));	 
}

//cutting edge to uniform
bool fpospreprecess(const int i, const std::string &input_path, const std::string &tem_path, const std::string &output_path_pos, const int &unisize, Mat &uniformmat){
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << ", input_path:" << input_path << std::endl;
		return false;
	}
	//imwrite(tem_path + "s1_src" + to_string(i) + ".jpg", src);
	Mat  sample_mat = src.clone();
	//imshow("1", sample_mat);

	//gray2bin
	threshold(sample_mat, sample_mat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	//imwrite(tem_path + "s2_threshold" + to_string(i) + ".jpg", sample_mat);
	//imshow("2", sample_mat); waitKey();

	//search top bot lines 
	//medianBlur(sample_mat, sample_mat, 3);
	//imwrite(tem_path + "s3_medianBlur" + to_string(i) + ".jpg", sample_mat);
	////imshow("3", sample_mat); waitKey();

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
	//imshow("4", sample_mat); waitKey();
	//imwrite(tem_path + "s4_uniform" + to_string(i) + ".jpg", sample_mat);	 

	// rect roi 
	//rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 1, 8, 0);
	//imwrite(tem_path + "s5_uniform" + to_string(i) + ".jpg", src(re));	 

	uniformmat = Mat(unisize, unisize, sample_mat.type(), Scalar(255));
	resize(src(re), uniformmat, Size(unisize, unisize), 0, 0, CV_INTER_AREA-3);
	//imshow("05", uniformmat);	waitKey();

	modifybrightness2(uniformmat);
	//imshow("5", uniformmat);	waitKey();
	//threshold(uniformmat, uniformmat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	////imshow("after bri", uniformmat);
	//waitKey();
	size_t pos = input_path.find_last_of('/');
	if (pos == std::string::npos){
		system("pause");
	}
	std::string sub_name = input_path.substr(pos+1);
	imwrite(output_path_pos + /*sub_name */std::to_string(i) + ".jpg", uniformmat);
	//destroyAllWindows();
	return true;
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

static void readimgnamefromfile(const char* fileName, std::vector<std::string> &imgnames){
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

void fei_poscreate(){
	const int unisize = 24;  //24 ada ,      32 svm 

	const std::string input_path = "C:/Users/Administrator/Desktop/pei/";//"D:/adaboost_train_11_fei/fei_samples/";  
	const std::string tem_path = "D:/adaboost_train_10_lp2/test_etc/opencv249_prj/pos_p/";//"D:/adaboost_train_11_fei/pos_good/"; 
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	for (int i = 0, tmpi = 2349; i < imgnames.size(); ++i){
		Mat uniformmat;
		if (fpospreprecess(tmpi, input_path + imgnames[i], tem_path, tem_path, unisize, uniformmat)){
			++tmpi;
		}
	}
}

//adaboost pos 
void shen_poscreate(){
	const int unisize = 32;

	const std::string input_path = "D:/adaboost_train_2_ju_all_svm_train/test_etc/svm_2_classes_shen_train/data3/pos_sample/";
	const std::string tem_path = "D:/adaboost_train_2_ju_all_svm_train/test_etc/svm_2_classes_shen_train/data3/1/";
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	for (int i = 0, tmpi = 849; i < imgnames.size(); ++i){
		Mat uniformmat;
		if (fpospreprecess(tmpi, input_path + imgnames[i], tem_path, tem_path, unisize, uniformmat)){
			++tmpi;
		}
	}
}

void precess_idc_img(const std::string &input_file, const std::string &out_path){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;

		Mat src;
		int new_h = 826 * (srcimg.rows / double(srcimg.cols));///// w = 1652/2 = 826   身份证
		resize(srcimg, src, Size(826, new_h), 0, 0, 3);

		Mat srcbin;
		threshold(src, srcbin, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
		imshow("srcbin", srcbin);
		waitKey();
		destroyAllWindows();

		//imwrite(out_path + std::to_string(icnt++) + ".jpg"/* imgnames[idx]*/, src);
	}
}

void replace_loop(std::string& src, const std::string& replace_src/*"\"h_type"*/, const std::string& replace_des/*"\"X_XXXX"*/, std::vector<int> &pos_list){
	size_t replace_src_size = replace_src.size();
	size_t pos = src.find(replace_src);
	while (pos != std::string::npos){
		pos_list.push_back(pos);
		src.replace(pos, replace_src_size, replace_des);
		pos = src.find(replace_src, (pos + replace_des.size()));
	}
}

///read json to txt 
int read_json(){
	ifstream fin("1.txt");

	ostringstream os;
	os << fin.rdbuf();
	//cout << os.str();
	std::string str = os.str();
	std::vector<int> pos_list;

	size_t pos1 = str.find("\"h_type");
	if (pos1 != std::string::npos){
		std::string line = str.substr(0, pos1);
		std::cout << "line1:\n" << line << std::endl;
	}

#if 0
	std::string line2 = str.substr(pos1 + strlen("\"h_type") + 1);  ///  
	//	std::cout << line2.substr(0, 800) << std::endl;

	std::cout << "\n\n" << std::endl;
	size_t pos2 = line2.find("\"h_type"); ////
	if (pos2 != std::string::npos){
		std::string line3 = line2.substr(0, pos2);
		std::cout << line3 << std::endl;
	}
	else{
		std::cout << "fail" << std::endl;
	}

	std::string line2has = str.substr(pos1, pos2 + strlen("\"h_type") + 1);  ///  
	std::cout << "line2:\n" << line2has << std::endl;
#endif

#if 0
	pos_list.push_back(pos1);////one hit start 

	for (int i = 0;; ++i){
		std::cout << "i:" << i << std::endl;
		std::string line2 = str.substr(pos1 + strlen("\"h_type") + 1);  /// no  h_type field
		size_t pos2 = line2.find("\"h_type"); ///xth hit start 
		if (pos2 != std::string::npos){
			std::string line3 = line2.substr(0, pos2);
			pos_list.push_back(pos2 + strlen("\"h_type") + 1);////detla 

			pos1 += pos2;
			//	std::cout << line3 << std::endl;
		}
		else{
			std::cout << "fail" << std::endl;
			break;
		}
	}
#endif
	std::string line2has;
	replace_loop(str, "\"h_type", "\"h_type", pos_list);
	for (int i = 0; i < /* pos_list.size() - */1; ++i){
		line2has = str.substr(pos_list[i], pos_list[i + 1] - pos_list[i]);  ///  no 
		line2has.resize(line2has.size() - 2);
		std::cout << "\nline:" << i << ":\n" << line2has << std::endl;
	}

#if 1
	size_t hpos = line2has.find("\"ticket_id");
	if (hpos != std::string::npos){
		std::cout << "find" << std::endl;
	}
	else{
		std::cout << "not find" << std::endl;
	}

	size_t mpos = line2has.find(",", std::string("\"ticket_id").size() + hpos + std::string(":").size());
	if (mpos != std::string::npos){
		std::cout << "find" << std::endl;
	}
	else{
		std::cout << "not find" << std::endl;
	}

	size_t start = hpos + std::string(":").size() + std::string("\"ticket_id").size() + 1;
	std::string strr = line2has.substr(start, mpos - start);
	std::cout << "ticket_id:\n" << strr << std::endl;
#endif

	//
	std::cout << "\n" << std::endl;
	std::string pname_line2has;
	std::vector<int> ppos_list;
	replace_loop(line2has, "\"p_name", "\"p_name", ppos_list);

	int i = 0;
	for (; i < ppos_list.size() - 1; ++i){
		pname_line2has = line2has.substr(ppos_list[i], ppos_list[i + 1] - ppos_list[i]);  ///  no 
		pname_line2has.resize(pname_line2has.size() - 2);
		std::cout << "\nline:" << i << ":\n" << pname_line2has << std::endl;
	}

	pname_line2has = line2has.substr(ppos_list[i]);  ///  no 
	std::cout << "\nline:" << i << ":\n" << pname_line2has << std::endl;

	return 0;
}

std::set<float> ratio_vec;

bool line_multi_typesprecess(const int i, const std::string &input_path, const std::string &tem_path, const std::string &output_path_pos, const int &unisize, Mat &uniformmat){
	Mat src = imread(input_path, 0);
	if (src.empty()){
		std::cerr << "imread faild, i:" << i << ", input_path:" << input_path << std::endl;
		return false;
	}

	size_t pos = input_path.find_last_of('/');
	if (pos == std::string::npos){
		system("pause");
	}
	std::string sub_name = input_path.substr(pos + 1);

	//imwrite(tem_path + "s1_src" + to_string(i) + ".jpg", src);
	Mat  sample_mat = src.clone();
//	imshow("1", sample_mat);

	//Mat sample_mat = Mat(sample_mat_.size(), sample_mat_.type());
	//GaussianBlur(sample_mat_, sample_mat, Size(3,3), 0);
	//imshow("1g", sample_mat);

	//gray2bin
	threshold(sample_mat, sample_mat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	//imwrite(tem_path + "s2_threshold" + to_string(i) + ".jpg", sample_mat);
//	imshow("2", sample_mat);  //waitKey();

	//search top bot lines 
	medianBlur(sample_mat, sample_mat, 3);
	/////imwrite(tem_path + "s3_medianBlur" + to_string(i) + ".jpg", sample_mat);
//	imshow("3", sample_mat); 
	//waitKey();
	//destroyAllWindows();
	//return true;

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

	Rect re = Rect(l_idx - 1, t_idx - 2, r_idx - l_idx + 3, b_idx - t_idx + 4);
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
	double hw = re.height / (re.width + 0.0);
	std::cout << "h/w:" << hw << ",  new_h:"<<20*hw<< std::endl;

	//Mat tmp = src(re);
	//modifybrightness2(tmp);
	//imwrite(output_path_pos + sub_name /* std::to_string(i)*/ + ".jpg", tmp );
	//return true;

	//rectangle(sample_mat, re.tl(), re.br(), Scalar(128, 128, 128), 1, 8, 0);
	//imshow("4", sample_mat); waitKey();
	//destroyAllWindows();
	//return true;

	//imwrite(tem_path + "s4_uniform" + to_string(i) + ".jpg", sample_mat);	 

	// rect roi 
	//rectangle(src, re.tl(), re.br(), Scalar(255, 255, 255), 1, 8, 0);
	//imwrite(tem_path + "s5_uniform" + to_string(i) + ".jpg", src(re));	 

	uniformmat = Mat(/*unisize, unisize,*/ 30, 20, sample_mat.type(), Scalar(255));
	resize(src(re), uniformmat, Size(/*unisize, unisize*/20, 30), 0, 0, CV_INTER_AREA - 3);

	modifybrightness2(uniformmat);
	imwrite(output_path_pos + /*sub_name*/  std::to_string(i+1050) + ".jpg", uniformmat);
	return true;

	//imshow("05", uniformmat);	waitKey();


	//imshow("5", uniformmat);	waitKey();
	//threshold(uniformmat, uniformmat, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	////imshow("after bri", uniformmat);
	//waitKey();

	imwrite(output_path_pos +  sub_name /* std::to_string(i)*/ + ".jpg", uniformmat);
	//destroyAllWindows();
	return true;
}

//zero 
int split_line_multi_types(){
	const std::string input_path =
		//	"D:/Documents/Downloads/数字_gray_samples/数字/上海住院/5/";
		"D:/Documents/Downloads/EAI3W6/5/";
	//	"D:/Documents/Downloads/EAI3W6/graycharimgfjzf/5/";
	//	"D:/Documents/Downloads/EAI3W6/graycharimgdnzhye/5/";
	//	"D:/adaboost_train_14_zero/0_cut_by_hand/";
	//	"E:/tools/WeChat Files/ljw-001/Files/EAI3W6 1/graycharimgflzf/0/";
	//"E:/tools/WeChat Files/ljw-001/Files/EAI3W6 1/graycharimgfjzf/0/";
	//	"D:/Documents/Downloads/EAI3W6/graycharimgdnzhye/0/";

		//"D:/adaboost_train_2_ju_all_svm_train/test_etc/ypmxrows/"; 
	const std::string out_path =
		"D:/adaboost_train_14_five/ttpos/";
		//"D:/adaboost_train_14_zero/0_cut_by_hand_uni/";
	//"D:/adaboost_train_2_ju_all_svm_train/test_etc/ypmxrows_out/";
	//"D:/adaboost_train_11_fei/pos_good/"; 

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_path.c_str(), imgnames);///////////

	for (int i = 0; i < imgnames.size(); ++i){
		Mat uniformmat;
		if (!line_multi_typesprecess(i, input_path + imgnames[i], out_path, out_path, 32, uniformmat)){
			std::cout << "failed" << std::endl;
		}
	}
	return 0;
}

//for 0 ada neg
int resize_neg_zero( ){
	const std::string input_file =
		"D:/adaboost_train_14_zero/tpos/";
		 //"D:/adaboost_train_14_zero/result/";
	const std::string out_path =
		 "D:/adaboost_train_14_zero/pos/";
		//"D:/adaboost_train_14_zero/t_neg/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		Mat src;
		if ( srcimg.rows < 32){
			int hei = 32;
			int wi = hei*srcimg.cols / (srcimg.rows + 0.0);
			if (wi < 20) wi = 21;
			resize(srcimg, src, Size(wi, hei), 0, 0, 3);
			modifybrightness2(src);
		}
		else if (srcimg.cols < 21){
			int wi = 21;
			int hei = wi*srcimg.rows / (srcimg.cols + 0.0);
			if (hei < 30) hei = 32;
			resize(srcimg, src, Size(wi, hei), 0, 0, 3);
			modifybrightness2(src);
		}
		else{
			src = srcimg;
		}
		
		imwrite(out_path + std::to_string(5823+icnt++) + ".jpg"/* imgnames[idx]*/, src);
	}

	return 0;
}

//for 0 ada neg
int resize_pos_zero(){
	const std::string input_file =
		"D:/adaboost_train_ff_create_chars_vs/genaratechar/TrapezoidCorrection/out_0/";
	//"D:/adaboost_train_14_zero/result/";
	const std::string out_path =
		"D:/adaboost_train_14_zero/0_by_word/";
	//"D:/adaboost_train_14_zero/t_neg/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		Mat src;
		resize(srcimg, src, Size(20, 30), 0, 0, 3);
		modifybrightness2(src);
		
		imwrite(out_path + std::to_string(3876 + icnt++) + ".jpg"/* imgnames[idx]*/, src);
	}

	return 0;
}

//for fei ada neg
int adaboostcascade_zero(const std::string &input_file){
	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	CascadeClassifier cascade("D:/adaboost_train_14_five/classifier_five_1001.xml");

	//CascadeClassifier cascade("D:/adaboost_train_14_zero/classifier_zero_930_new.xml");
	if (cascade.empty()){
		std::cout << "cascade error!" << std::endl;
		return 1;
	}

	for (int idx = 40; idx < imgnames.size(); ++idx){
		Mat srcimg_color = imread(input_file + imgnames[idx], 1);
		if (srcimg_color.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << "imread .--------------- i:" << idx << ", fname:" << imgnames[idx] << std::endl;

		Mat src, grayimg;
		if (srcimg_color.channels() > 1){
			cvtColor(srcimg_color, grayimg, COLOR_BGR2GRAY);
		}
		else{
			grayimg = srcimg_color;
		}

		////
		int new_h = 32 * 1.3;
		double h_rate = (new_h+0.0) / grayimg.rows;
		int new_w = new_h * (grayimg.cols / double(grayimg.rows));////  ob area expand 
		resize(grayimg, src, Size(new_w, new_h));

		double ratio = 1.4;
		Mat coarse = Mat(src.rows * ratio, src.cols * ratio, src.type(), Scalar(128));//// expand img for ob area
		Rect mid_re( (coarse.cols - src.cols) >> 1, (coarse.rows - src.rows) >> 1, src.cols, src.rows ) ;
		Mat mid = coarse(mid_re );////x  y 
		src.copyTo(mid);  //////////b  branch

		////detect
		vector<Rect> objects;
		Mat roi = coarse.clone();

		modifybrightness2(roi);
		cascade.detectMultiScale(roi, objects, 1.1, 1, 1, Size(14, 25), Size(30, 50));/// 

		//std::cout << "beat num:" << objects.size() << std::endl;
		if (objects.empty()){
			//imwrite("d:/123/" + imgnames[idx], src);
			//continue;
		}

		for (int i = 0; i < objects.size(); ++i){
			Rect re = objects[i];
			re.width += 1;
			//imwrite("D:/adaboost_train_14_zero/result/" + imgnames[idx], roi(re)); continue;
			rectangle(roi, re, Scalar(255,0, 0), 1, 8, 0);///
			std::cout << "w*h:" << objects[i].width << "\t" << objects[i].height << std::endl;
			Rect c_re((re.x - mid_re.x) / h_rate, (re.y - mid_re.y) / h_rate, re.width / h_rate, re.height / h_rate);
			rectangle(srcimg_color, c_re, Scalar(255, 0, 0), 1, 8, 0);///
		}
		//continue;
		//namedWindow("result", 0);
		//resizeWindow("result", src.cols , src.rows);
		//imwrite("D:/adaboost_train_14_zero/result/" + imgnames[idx], srcimg_color); continue;
		imshow("craw", srcimg_color);
		imshow("raw", coarse);
		imshow("result", roi);
		waitKey();
		destroyAllWindows();
	}

	return 0;
}

//87*16
//for csyx  ada  
int resize_neg_csyx(){
	const std::string input_file =
		"D:/adaboost_train_ff_create_chars_vs/genaratechar/TrapezoidCorrection/out_cszm/";
		//"D:/715-work/出生医学/";
	//"D:/adaboost_train_14_zero/result/";
	const std::string out_path =
		"D:/adaboost_train_16_csyx/pos/";
	//"D:/adaboost_train_14_zero/t_neg/";

	std::vector<std::string> imgnames;
	readimgnamefromfile(input_file.c_str(), imgnames);

	for (int idx = 0; idx < imgnames.size(); ++idx){
		Mat srcimg = imread(input_file + imgnames[idx], 0);
		if (srcimg.empty()){
			std::cout << "imread failed. i:" << idx << ", fname:" << imgnames[idx] << std::endl;
			continue;
		}
		std::cout << " i:" << idx << ", fname:" << imgnames[idx] << std::endl;
		Mat src;
		resize(srcimg, src, Size(87, 17), 0, 0, 3);
		modifybrightness2(src);
		imwrite(out_path + std::to_string(81+ icnt++) + ".jpg"/* imgnames[idx]*/, src);
	}

	return 0;
}

int main(int argc, char** argv){
	//return split_line_multi_types();
//	return resize_neg_csyx();

//	return resize_pos_zero();

	return adaboostcascade_zero("D:/Documents/Downloads/ypmxrows_full/ypmxrows/"/*"D:/adaboost_train_2_ju_all_svm_train/test_etc/ypmxrows/"*/);

	return resize_neg_zero();

	return split_line_multi_types();


	precess_idc_img("E:/tools/WeChat Files/ljw-001/Files/身份证复印件_2.0/", ""); return 0;

	shen_poscreate(); return 0;
	const std::string input_file ="D:/Documents/Downloads/yblximg/yblximg/";//"D:/分拣/2-票据彩色/ ";//"D:/bj/";//"D:/test_vs/test_etc/opencv249_prj/sample_class/";
	const std::string out_path = "D:/Documents/Downloads/yblximg/out_single/";
	// splitkerl4(input_file, out_path); return 0;

	//const std::string i_file = "D:/Documents/Downloads/largefeechimg/费/";
	//const std::string o_path = "D:/adaboost_train_11_fei/pos/";
	//splitkerl5(i_file, o_path);
	
	  fei_poscreate(); return 0;

	const std::string i_file = "C:/Users/Administrator/Desktop/费/";
	const std::string o_path = "D:/adaboost_train_11_fei/pos/";
//	splitkerl6(i_file, o_path); return 0;

	//const std::string i_file    = "C:/Users/Administrator/Desktop/out_single/";
	//const std::string o_path = "D:/adaboost_train_11_fei/neg/";
	//splitkerl7(i_file, o_path);

	adaboostcascade_fei("D:/Documents/Downloads/yblximg/fei/"); return 0;

	return 0;
}

